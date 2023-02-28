// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "IOdysseyViewportDrawingEditorAdapter.h"
#include "IMeshPaintGeometryAdapter.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "UObject/SavePackage.h"

#define LOCTEXT_NAMESPACE "IOdysseyViewportDrawingEditorAdapter"

IOdysseyViewportDrawingEditorAdapter::~IOdysseyViewportDrawingEditorAdapter()
{
    if( mEditor->GetSelectedTool() )
    {
        if (mEditor->GetSelectedTool()->IsA(UOdysseyPainterEditorRasterDrawingTool::StaticClass()))
        {
            UOdysseyPainterEditorRasterDrawingTool* drawingTool = Cast<UOdysseyPainterEditorRasterDrawingTool>(mEditor->GetSelectedTool());
            UnbindStampBrushInstance(drawingTool->GetBrushInstance());
        }
    }

    mEditor->OnSelectedToolChangedDelegate().RemoveAll(this);
    OnToolChange(nullptr);
    mEditor->TargetToPaintWillChangeDelegate().RemoveAll(this);
    mEditor->TargetToPaintChangedDelegate().RemoveAll(this);
    
    UOdysseyStylusInputSubsystem* inputSubsystem = GEditor->GetEditorSubsystem<UOdysseyStylusInputSubsystem>();
    inputSubsystem->RemoveMessageHandler(*this);

    RemoveTextureOverride();
}

IOdysseyViewportDrawingEditorAdapter::IOdysseyViewportDrawingEditorAdapter(TSharedPtr<FOdysseyViewportDrawingEditor> iEditor) :
    mEditor(iEditor),
    mPaintingTexture2DRenderTarget(nullptr),
    mState( eState::kIdle ),
    mLastKnownViewport(nullptr),
    mIsCapturedByStylus(false)
{
    mEditor->TargetToPaintWillChangeDelegate().AddRaw(this, &IOdysseyViewportDrawingEditorAdapter::RemoveTextureOverride);
    mEditor->TargetToPaintChangedDelegate().AddRaw(this, &IOdysseyViewportDrawingEditorAdapter::PrepareAdapterForPainting);
    PrepareAdapterForPainting();

    UOdysseyStylusInputSubsystem* inputSubsystem = GEditor->GetEditorSubsystem<UOdysseyStylusInputSubsystem>();
    inputSubsystem->AddMessageHandler(*this);
}

void IOdysseyViewportDrawingEditorAdapter::PrepareAdapterForPainting()
{
    mEditor->OnSelectedToolChangedDelegate().AddRaw(this, &IOdysseyViewportDrawingEditorAdapter::OnToolChange);
    if (mEditor->GetSelectedTool())
    {
        if (mEditor->GetSelectedTool()->IsA(UOdysseyPainterEditorTool::StaticClass()))
        {
            UOdysseyPainterEditorTool* drawingTool = Cast<UOdysseyPainterEditorTool>(mEditor->GetSelectedTool());
            OnToolChange(drawingTool);
        }
    }
}

bool IOdysseyViewportDrawingEditorAdapter::IsReadyToDraw() const
{
    return !(mState == eState::kIdle);
}

bool IOdysseyViewportDrawingEditorAdapter::MouseMove(FEditorViewportClient* iViewportClient, FViewport* iViewport, int32 iX, int32 iY)
{
    if (!IsReadyToDraw())
        return false;

    // Compute a world space ray from the screen space mouse coordinates
    FSceneViewFamilyContext viewFamily(FSceneViewFamily::ConstructionValues(
        iViewportClient->Viewport,
        iViewportClient->GetScene(),
        iViewportClient->EngineShowFlags)
        .SetRealtimeUpdate(iViewportClient->IsRealtime()));
    FSceneView* view = iViewportClient->CalcSceneView(&viewFamily);
    const FViewportCursorLocation mouseViewportRay(view, (FEditorViewportClient*)iViewport->GetClient(), iViewport->GetMouseX(), iViewport->GetMouseY());

    mLastStrokeRay = mCurrentStrokeRay;
    
    mCurrentStrokeRay.mRayOrigin = mouseViewportRay.GetOrigin();
    mCurrentStrokeRay.mRayDirection = mouseViewportRay.GetDirection();
    mCurrentStrokeRay.mPoint = FOdysseyPoint::DefaultPoint();
    mCurrentStrokeRay.mPoint.x = iViewport->GetMouseX();
    mCurrentStrokeRay.mPoint.y = iViewport->GetMouseY();
    mCurrentStrokeRay.mPoint.keysDown = mKeysPressed;

    return true;
}

bool IOdysseyViewportDrawingEditorAdapter::InputKey(FEditorViewportClient* iViewportClient, FViewport* iViewport, FKey iKey, EInputEvent iEvent)
{
    if (mLastKnownViewport != iViewport)
        mLastKnownViewport = iViewport;

#if PLATFORM_MAC
    if (iEvent == EInputEvent::IE_Pressed && iKey == EKeys::LeftMouseButton)
    {
        //No need to destroy it, it auto destroys itself
        //This is only acceptable because it is a HUGE patch
        //PATCH: Temporary viewportClient created for overriding highPrecision mouse events (useful for mac)
        //TODO: remove this when we have a solution. It hurts to see it here
        FOdysseyViewportDrawingEditorViewportClient* viewportClient = new FOdysseyViewportDrawingEditorViewportClient();
        viewportClient->SetViewport(mLastKnownViewport);
        viewportClient->SetViewportClient(mLastKnownViewport->GetClient());
        mLastKnownViewport->SetViewportClient(viewportClient);
    }
#endif

    for (int i = mKeysPressed.Num() - 1; i >= 0; i--)
    {
        if (!iViewport->KeyState(mKeysPressed[i]))
        {
            mKeysPressed.RemoveAt(i);
        }
    }

    if(iEvent == EInputEvent::IE_Pressed)
    {
        if (!mKeysPressed.Contains(iKey))
            mKeysPressed.Add(iKey);
    } 
    else if(iEvent == EInputEvent::IE_Released)
    {
        mKeysPressed.Remove(iKey);
    }

    const bool bIsAltDown =  mKeysPressed.Contains( EKeys::LeftAlt ) || mKeysPressed.Contains( EKeys::RightAlt );
    const bool bIsCtrlDown = mKeysPressed.Contains( EKeys::LeftControl ) || mKeysPressed.Contains( EKeys::RightControl );
    
    //Dolly the camera, we're not painting
    if ( bIsAltDown ) 
    {
        return false;
    }
    // Allow Ctrl+B to pass through so we can support the finding of a selected static mesh in the content browser.
    else if ( bIsCtrlDown && iKey == EKeys::B && iEvent == IE_Pressed )
    {
        return false;
    }

    auto end_time = std::chrono::steady_clock::now();
    auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - mStylusLastEventTime).count();
   
    //Stylus will handle the event
    if ((mIsCapturedByStylus || delta < 500) && (iKey == EKeys::LeftMouseButton))
        return true;

    // Compute a world space ray from the screen space mouse coordinates
    FSceneViewFamilyContext viewFamily(FSceneViewFamily::ConstructionValues(
        iViewportClient->Viewport,
        iViewportClient->GetScene(),
        iViewportClient->EngineShowFlags)
        .SetRealtimeUpdate(iViewportClient->IsRealtime()));
    FSceneView* view = iViewportClient->CalcSceneView(&viewFamily);
    const FViewportCursorLocation mouseViewportRay(view,(FEditorViewportClient*)iViewport->GetClient(),iViewport->GetMouseX(),iViewport->GetMouseY());

    //Init our StrokeRay, having all the basic info to draw 
    FOdysseyRay strokeRay;
    strokeRay.mRayOrigin = mouseViewportRay.GetOrigin();
    strokeRay.mRayDirection = mouseViewportRay.GetDirection();
    strokeRay.mPoint = FOdysseyPoint::DefaultPoint();
    strokeRay.mPoint.x = iViewport->GetMouseX();
    strokeRay.mPoint.y = iViewport->GetMouseY();
    strokeRay.mPoint.keysDown = mKeysPressed;

    return InputKeyWithStrokeRay(strokeRay, iViewportClient, iViewport, iKey, iEvent);
}

bool IOdysseyViewportDrawingEditorAdapter::InputKeyWithStrokeRay(const FOdysseyRay& iRay, FEditorViewportClient* iViewportClient, FViewport* iViewport, FKey iKey, EInputEvent iEvent)
{
    if(!IsReadyToDraw())
        return false;

    mLastStrokeRay = mCurrentStrokeRay;
    mCurrentStrokeRay = iRay;

    if (mState == eState::kIdleReady)
    {
        if (iKey == EKeys::LeftMouseButton && iEvent == EInputEvent::IE_Pressed)
        {
            mState = eState::kDrawing;
            StartPainting();
            return true;
        }
    }
    else if (mState == eState::kDrawing)
    {
        if (iKey == EKeys::LeftMouseButton && iEvent == EInputEvent::IE_Released)
        {
            FinishPainting();
            mState = eState::kIdleReady;
        }
    }

    return false;
}

bool IOdysseyViewportDrawingEditorAdapter::CapturedMouseMove(FEditorViewportClient* iViewportClient, FViewport* iViewport, int32 iMouseX, int32 iMouseY)
{
    auto end_time = std::chrono::steady_clock::now();
    auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - mStylusLastEventTime).count();

    //Stylus will handle the event
    if (mIsCapturedByStylus || delta < 500)
        return true;

    // Compute a world space ray from the screen space mouse coordinates
    FSceneViewFamilyContext viewFamily(FSceneViewFamily::ConstructionValues(
        iViewportClient->Viewport,
        iViewportClient->GetScene(),
        iViewportClient->EngineShowFlags)
        .SetRealtimeUpdate(iViewportClient->IsRealtime()));
    FSceneView* view = iViewportClient->CalcSceneView(&viewFamily);
    const FViewportCursorLocation mouseViewportRay(view, (FEditorViewportClient*)iViewport->GetClient(), iViewport->GetMouseX(), iViewport->GetMouseY());

    //Init our StrokeRay, having all the basic info to draw 
    FOdysseyRay strokeRay;
    strokeRay.mRayOrigin = mouseViewportRay.GetOrigin();
    strokeRay.mRayDirection = mouseViewportRay.GetDirection();
    strokeRay.mPoint = FOdysseyPoint::DefaultPoint();
    strokeRay.mPoint.x = iMouseX;
    strokeRay.mPoint.y = iMouseY;
    strokeRay.mPoint.keysDown = mKeysPressed;

    return CapturedMouseMoveWithStrokeRay(strokeRay, iViewportClient, iViewport, iMouseX, iMouseY);
}

bool IOdysseyViewportDrawingEditorAdapter::CapturedMouseMoveWithStrokeRay(const FOdysseyRay& iRay, FEditorViewportClient* iViewportClient, FViewport* iViewport, int32 iMouseX, int32 iMouseY)
{
    if (!IsReadyToDraw())
        return false;

    mLastStrokeRay = mCurrentStrokeRay;
    mCurrentStrokeRay = iRay;

    if (mState == eState::kDrawing)
    {
        if (long(mCurrentStrokeRay.mPoint.x) == long(mLastStrokeRay.mPoint.x) && long(mCurrentStrokeRay.mPoint.y) == long(mLastStrokeRay.mPoint.y))
            return true;

        Paint();
    }

    return true;
}

void IOdysseyViewportDrawingEditorAdapter::OnStylusStateChanged(const TWeakPtr<SWidget> iWidget, const FStylusState& iState, int32 iIndex)
{
    if ( !IsReadyToDraw() || !mLastKnownViewport || !iWidget.IsValid() )
        return;

    //We only treat events on the main level Viewport
    if( GCurrentLevelEditingViewportClient->GetEditorViewportWidget()->GetSceneViewport()->GetViewportWidget().Pin().Get() != iWidget.Pin().Get() )
        return;

    FEditorViewportClient* viewportClient = (FEditorViewportClient*)mLastKnownViewport->GetClient();

    if( !viewportClient )
        return;
    
    // Compute a world space ray from the screen space mouse coordinates
    FSceneViewFamilyContext viewFamily(FSceneViewFamily::ConstructionValues(
        viewportClient->Viewport,
        viewportClient->GetScene(),
        viewportClient->EngineShowFlags)
        .SetRealtimeUpdate(viewportClient->IsRealtime()));
    FSceneView* view = viewportClient->CalcSceneView(&viewFamily);
    const FViewportCursorLocation mouseViewportRay(view, viewportClient, mLastKnownViewport->GetMouseX(), mLastKnownViewport->GetMouseY());

    //Init our StrokeRay, having all the basic info to draw 
    float scaleDPI = iWidget.Pin().Get()->GetCachedGeometry().GetAccumulatedLayoutTransform().GetScale();
    FVector2D positionInViewport = iWidget.Pin().Get()->GetCachedGeometry().AbsoluteToLocal(iState.GetPosition()) * scaleDPI;
    FOdysseyRay strokeRay;
    strokeRay.mRayOrigin = mouseViewportRay.GetOrigin();
    strokeRay.mRayDirection = mouseViewportRay.GetDirection();
    strokeRay.mPoint = FOdysseyPoint( positionInViewport.X
                                                , positionInViewport.Y
                                                , iState.GetZ()
                                                , iState.GetPressure()
                                                , iState.GetTimer()
                                                , iState.GetAltitude()
                                                , iState.GetAzimuth()
                                                , iState.GetTwist()
                                                , 0 //iState.GetPitch()
                                                , 0 // iState.GetRoll()
                                                , 0); // iState.GetYaw() );

    strokeRay.mPoint.keysDown = mKeysPressed;

    static bool stylusWasDown = false;
    static bool is_dragging = false;

    bool isDownEvent = !stylusWasDown && iState.IsStylusDown();
    bool isUpEvent = stylusWasDown && !iState.IsStylusDown();
    bool isMoveEvent = stylusWasDown == iState.IsStylusDown();

    stylusWasDown = iState.IsStylusDown();

    if (isDownEvent)
    {
        InputKeyWithStrokeRay( strokeRay, viewportClient, mLastKnownViewport, EKeys::LeftMouseButton, EInputEvent::IE_Pressed );
        mIsCapturedByStylus = true;
    }
    else if (isUpEvent)
    {
        InputKeyWithStrokeRay( strokeRay, viewportClient, mLastKnownViewport, EKeys::LeftMouseButton, EInputEvent::IE_Released );
        mIsCapturedByStylus = false;
    }
    else if( iState.IsStylusDown() )
    {
        CapturedMouseMoveWithStrokeRay( strokeRay, viewportClient, mLastKnownViewport, mLastKnownViewport->GetMouseX(), mLastKnownViewport->GetMouseY() );
    }
    else
    {
        mCurrentStrokeRay = strokeRay;
    }

    mStylusLastEventTime = std::chrono::steady_clock::now();
}

void IOdysseyViewportDrawingEditorAdapter::RemoveTextureOverride()
{
    //if( !mPaintingTexture2DRenderTarget || !mPaintingTexture2DRenderTarget->IsValidLowLevel() )
    //    return;

    if (mEditor->Component() != nullptr && mEditor->Texture() != nullptr)
    {
        FSavePackageArgs packageArgs;
        packageArgs.SaveFlags = mEditor->Texture()->GetFlags();
        UPackage::Save(mEditor->Texture()->GetOutermost(), mEditor->Texture(), *(mEditor->Texture()->GetName()), packageArgs);
        mEditor->Texture()->MipGenSettings = mTextureMipGenSettings;
        mEditor->Texture()->UpdateResource();

        //const ERHIFeatureLevel::Type FeatureLevel = mEditor->Component()->GetWorld()->FeatureLevel;
        //mEditor->Material()->OverrideTexture(mEditor->Texture(), nullptr, FeatureLevel);
        //IMeshPaintGeometryAdapter::DefaultApplyOrRemoveTextureOverride(mEditor->Component(), mEditor->Texture(), nullptr);

        //mPaintingTexture2DRenderTarget->ConditionalBeginDestroy();
        //mPaintingTexture2DRenderTarget = nullptr;
        mState = eState::kIdle;
    }

    if (mPaintingTexture2DRenderTarget && mPaintingTexture2DRenderTarget->IsValidLowLevel())
    {
        mPaintingTexture2DRenderTarget->ConditionalBeginDestroy();
        mPaintingTexture2DRenderTarget = nullptr;
    }
}

void IOdysseyViewportDrawingEditorAdapter::OnToolChange(UOdysseyPainterEditorTool* iNewTool)
{
    if( mEditor->GetSelectedTool() )
    {
        if (mEditor->GetSelectedTool()->IsA(UOdysseyPainterEditorRasterDrawingTool::StaticClass()))
        {
            UOdysseyPainterEditorRasterDrawingTool* drawingTool = Cast<UOdysseyPainterEditorRasterDrawingTool>(mEditor->GetSelectedTool());
            drawingTool->OnCreatedBrushInstance().RemoveAll(this);
            drawingTool->OnDestroyBrushInstance().RemoveAll(this);
            UnbindStampBrushInstance(drawingTool->GetBrushInstance());
        }
    }

    if (!iNewTool)
        return;

    if (iNewTool->IsA(UOdysseyPainterEditorRasterDrawingTool::StaticClass()))
    {
        UOdysseyPainterEditorRasterDrawingTool* drawingTool = Cast<UOdysseyPainterEditorRasterDrawingTool>(iNewTool);
        drawingTool->OnCreatedBrushInstance().AddRaw(this, &IOdysseyViewportDrawingEditorAdapter::BindStampBrushInstance);
        drawingTool->OnDestroyBrushInstance().AddRaw(this, &IOdysseyViewportDrawingEditorAdapter::UnbindStampBrushInstance);
        BindStampBrushInstance(drawingTool->GetBrushInstance());
    }
}

void IOdysseyViewportDrawingEditorAdapter::UnbindStampBrushInstance(UOdysseyBrushAssetBase* iUnbindBrush)
{
    if (iUnbindBrush)
        iUnbindBrush->GetStampOverrideDelegate().Unbind();
}

void IOdysseyViewportDrawingEditorAdapter::BindStampBrushInstance(UOdysseyBrushAssetBase* iBindBrush)
{
    if (iBindBrush)
        iBindBrush->GetStampOverrideDelegate().BindRaw(this, &IOdysseyViewportDrawingEditorAdapter::StampOverride);
}

#undef LOCTEXT_NAMESPACE