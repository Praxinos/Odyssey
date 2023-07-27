// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "IOdysseyViewportDrawingEditorAdapter.h"
#include "IMeshPaintGeometryAdapter.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "UObject/SavePackage.h"
#include "TextureCompiler.h"
#include "FileHelpers.h"
#include "Kismet/GameplayStatics.h"

#define LOCTEXT_NAMESPACE "IOdysseyViewportDrawingEditorAdapter"

IOdysseyViewportDrawingEditorAdapter::~IOdysseyViewportDrawingEditorAdapter()
{
    if( mDrawingTool )
    {
        UnbindStampBrushInstance(mDrawingTool->GetBrushInstance());
    }

    mEditor->OnSelectedToolChangedDelegate().RemoveAll(this);
    OnToolChange();
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
        mDrawingTool = Cast<UOdysseyPainterEditorRasterDrawingTool>(mEditor->GetSelectedTool());
        if( mDrawingTool )
            OnToolChange();
    }
}

FVector2D IOdysseyViewportDrawingEditorAdapter::ViewportCoordinatesToTextureCoordinates(FVector2D iPositionInViewport, FEditorViewportClient* iViewportClient)
{
    const TSharedPtr<IMeshPaintGeometryAdapter>* meshAdapterPtr = mEditor->ComponentToAdapterMap().Find(mEditor->Component());
    if (!meshAdapterPtr)
        return FVector2D( 0, 0 );

    TSharedPtr<IMeshPaintGeometryAdapter> meshAdapter = *meshAdapterPtr;

    // Compute a world space ray from the screen space mouse coordinates
    FSceneViewFamilyContext viewFamily(FSceneViewFamily::ConstructionValues(
        iViewportClient->Viewport,
        iViewportClient->GetScene(),
        iViewportClient->EngineShowFlags)
        .SetRealtimeUpdate(iViewportClient->IsRealtime()));
    FSceneView* view = iViewportClient->CalcSceneView(&viewFamily);

    const FViewportCursorLocation mouseViewportRay(view, iViewportClient, iPositionInViewport.X, iPositionInViewport.Y);

    FHitResult traceHitResult(1.0f);
    const FVector rayEnd(mouseViewportRay.GetOrigin() + mouseViewportRay.GetDirection() * HALF_WORLD_MAX);

    meshAdapter->LineTraceComponent(traceHitResult, mouseViewportRay.GetOrigin(), rayEnd, FCollisionQueryParams(SCENE_QUERY_STAT(Paint), true));

    // Convert trace to UV position
    FVector2D coord;
    if (UGameplayStatics::FindCollisionUV(traceHitResult, mEditor->GetUVIndexUsedByCurrentTexture(), coord))
    {
        iPositionInViewport.X = coord.X * mEditor->Texture()->GetSurfaceWidth();
        iPositionInViewport.Y = coord.Y * mEditor->Texture()->GetSurfaceHeight();
    }

    return iPositionInViewport;
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

    FVector2D texturePos = ViewportCoordinatesToTextureCoordinates(FVector2D(iViewport->GetMouseX(), iViewport->GetMouseY()), iViewportClient);

    mLastStrokeRay = mCurrentStrokeRay;
    
    mCurrentStrokeRay.mRayOrigin = mouseViewportRay.GetOrigin();
    mCurrentStrokeRay.mRayDirection = mouseViewportRay.GetDirection();
    mCurrentStrokeRay.mPoint = FOdysseyPoint::DefaultPoint();
    mCurrentStrokeRay.mPoint.x = texturePos.X;
    mCurrentStrokeRay.mPoint.y = texturePos.Y;
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

    FVector2D texturePos = ViewportCoordinatesToTextureCoordinates(FVector2D(iViewport->GetMouseX(), iViewport->GetMouseY()), iViewportClient);

    //Init our StrokeRay, having all the basic info to draw 
    FOdysseyRay strokeRay;
    strokeRay.mRayOrigin = mouseViewportRay.GetOrigin();
    strokeRay.mRayDirection = mouseViewportRay.GetDirection();
    strokeRay.mPoint = FOdysseyPoint::DefaultPoint();
    strokeRay.mPoint.x = texturePos.X;
    strokeRay.mPoint.y = texturePos.Y;
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

    FVector2D texturePos = ViewportCoordinatesToTextureCoordinates(FVector2D(iViewport->GetMouseX(), iViewport->GetMouseY()), iViewportClient);

    //Init our StrokeRay, having all the basic info to draw 
    FOdysseyRay strokeRay;
    strokeRay.mRayOrigin = mouseViewportRay.GetOrigin();
    strokeRay.mRayDirection = mouseViewportRay.GetDirection();
    strokeRay.mPoint = FOdysseyPoint::DefaultPoint();
    strokeRay.mPoint.x = texturePos.X;
    strokeRay.mPoint.y = texturePos.Y;
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

    FVector2D texturePos = ViewportCoordinatesToTextureCoordinates(positionInViewport, viewportClient);

    FOdysseyRay strokeRay;
    strokeRay.mRayOrigin = mouseViewportRay.GetOrigin();
    strokeRay.mRayDirection = mouseViewportRay.GetDirection();
    strokeRay.mPoint = FOdysseyPoint(             texturePos.X
                                                , texturePos.Y
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
    mState = eState::kIdle;

    if (mEditor->Component() != nullptr && mEditor->Texture() != nullptr)
    {
        mEditor->Texture()->MipGenSettings = mPreviousMipSettings;
        mEditor->Texture()->UpdateResource();
        FTextureCompilingManager::Get().FinishCompilation({ mEditor->Texture() });
        TArray<UPackage*> packages;
        packages.Add(mEditor->Texture()->GetPackage());
        UEditorLoadingAndSavingUtils::SavePackages(packages, true);
    }

    if (!mPaintingTexture2DRenderTarget || !mPaintingTexture2DRenderTarget->IsValidLowLevel())
    {
        return;
    }

    mPaintingTexture2DRenderTarget->ConditionalBeginDestroy();
    mPaintingTexture2DRenderTarget = nullptr;
}

void IOdysseyViewportDrawingEditorAdapter::OnToolChange()
{
    if(mDrawingTool)
    {
        mDrawingTool->OnCreatedBrushInstance().RemoveAll(this);
        mDrawingTool->OnDestroyBrushInstance().RemoveAll(this);
        UnbindStampBrushInstance(mDrawingTool->GetBrushInstance());
    }

    UOdysseyPainterEditorTool* newTool = mEditor->GetSelectedTool();
    if (!newTool )
        return;

    if ( newTool->IsA(UOdysseyPainterEditorRasterDrawingTool::StaticClass()))
    {
        mDrawingTool = Cast<UOdysseyPainterEditorRasterDrawingTool>(newTool);
        mDrawingTool->OnCreatedBrushInstance().AddRaw(this, &IOdysseyViewportDrawingEditorAdapter::BindStampBrushInstance);
        mDrawingTool->OnDestroyBrushInstance().AddRaw(this, &IOdysseyViewportDrawingEditorAdapter::UnbindStampBrushInstance);
        BindStampBrushInstance(mDrawingTool->GetBrushInstance());
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