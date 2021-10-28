// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "IOdysseyViewportDrawingEditorAdapter.h"
#include "IMeshPaintGeometryAdapter.h"

#define LOCTEXT_NAMESPACE "IOdysseyViewportDrawingEditorAdapter"

IOdysseyViewportDrawingEditorAdapter::~IOdysseyViewportDrawingEditorAdapter()
{
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

    UOdysseyStylusInputSubsystem* inputSubsystem = GEditor->GetEditorSubsystem<UOdysseyStylusInputSubsystem>();
    inputSubsystem->AddMessageHandler(*this);
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

    FOdysseyStrokeRay lastRay = mCurrentStrokeRay;
    mCurrentStrokeRay.mRayOrigin = mouseViewportRay.GetOrigin();
    mCurrentStrokeRay.mRayDirection = mouseViewportRay.GetDirection();
    mCurrentStrokeRay.mStrokePoint = FOdysseyStrokePoint::DefaultPoint();
    mCurrentStrokeRay.mStrokePoint.x = iX;
    mCurrentStrokeRay.mStrokePoint.y = iY;
    mCurrentStrokeRay.mStrokePoint.keysDown = mKeysPressed;

    if (mState == eState::kIdleReady)
    {
        //TODO: save current point in viewport ?
        //auto paintengine = mOdysseyPainterEditor->PaintEngine();

        if (long(mCurrentStrokeRay.mStrokePoint.x) == long(lastRay.mStrokePoint.x) && long(mCurrentStrokeRay.mStrokePoint.y) == long(lastRay.mStrokePoint.y))
            return true;

        //paintengine->SetCurrentStrokePoint(mCurrentPointInTexture);
    }

    return true;
}

bool IOdysseyViewportDrawingEditorAdapter::InputKey(FEditorViewportClient* iViewportClient, FViewport* iViewport, FKey iKey, EInputEvent iEvent)
{
    if (mLastKnownViewport != iViewport)
        mLastKnownViewport = iViewport;

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
    FOdysseyStrokeRay strokeRay;
    strokeRay.mRayOrigin = mouseViewportRay.GetOrigin();
    strokeRay.mRayDirection = mouseViewportRay.GetDirection();
    strokeRay.mStrokePoint = FOdysseyStrokePoint::DefaultPoint();
    strokeRay.mStrokePoint.x = iViewport->GetMouseX();
    strokeRay.mStrokePoint.y = iViewport->GetMouseY();
    strokeRay.mStrokePoint.keysDown = mKeysPressed;

    return InputKeyWithStrokeRay(strokeRay, iViewportClient, iViewport, iKey, iEvent);
}

bool IOdysseyViewportDrawingEditorAdapter::InputKeyWithStrokeRay(const FOdysseyStrokeRay& iRay, FEditorViewportClient* iViewportClient, FViewport* iViewport, FKey iKey, EInputEvent iEvent)
{
    //UE_LOG(LogTemp, Display, TEXT("InputKey"));

    if(!IsReadyToDraw())
        return false;

    FOdysseyStrokeRay lastRay = mCurrentStrokeRay;
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
    FOdysseyStrokeRay strokeRay;
    strokeRay.mRayOrigin = mouseViewportRay.GetOrigin();
    strokeRay.mRayDirection = mouseViewportRay.GetDirection();
    strokeRay.mStrokePoint = FOdysseyStrokePoint::DefaultPoint();
    strokeRay.mStrokePoint.x = iMouseX;
    strokeRay.mStrokePoint.y = iMouseY;
    strokeRay.mStrokePoint.keysDown = mKeysPressed;

    return CapturedMouseMoveWithStrokeRay(strokeRay, iViewportClient, iViewport, iMouseX, iMouseY);
}

bool IOdysseyViewportDrawingEditorAdapter::CapturedMouseMoveWithStrokeRay(const FOdysseyStrokeRay& iRay, FEditorViewportClient* iViewportClient, FViewport* iViewport, int32 iMouseX, int32 iMouseY)
{
    if (!IsReadyToDraw())
        return false;

    FOdysseyStrokeRay lastRay = mCurrentStrokeRay;
    mCurrentStrokeRay = iRay;

    if (mState == eState::kDrawing)
    {
        if (long(mCurrentStrokeRay.mStrokePoint.x) == long(lastRay.mStrokePoint.x) && long(mCurrentStrokeRay.mStrokePoint.y) == long(lastRay.mStrokePoint.y))
            return true;

        Paint();
    }

    return true;
}

void IOdysseyViewportDrawingEditorAdapter::OnStylusStateChanged(const TWeakPtr<SWidget> iWidget, const FStylusState& iState, int32 iIndex)
{
    if ( !IsReadyToDraw() || !mLastKnownViewport )
        return;

    //We only treat events on viewports
    if( iWidget.Pin().Get()->GetTypeAsString() != FString("SViewport") )
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
    FOdysseyStrokeRay strokeRay;
    strokeRay.mRayOrigin = mouseViewportRay.GetOrigin();
    strokeRay.mRayDirection = mouseViewportRay.GetDirection();
    strokeRay.mStrokePoint = FOdysseyStrokePoint( mLastKnownViewport->GetMouseX()
                                                , mLastKnownViewport->GetMouseY() 
                                                , iState.GetZ()
                                                , iState.GetPressure()
                                                , iState.GetTimer()
                                                , iState.GetAltitude()
                                                , iState.GetAzimuth()
                                                , iState.GetTwist()
                                                , 0 //iState.GetPitch()
                                                , 0 // iState.GetRoll()
                                                , 0); // iState.GetYaw() );

    strokeRay.mStrokePoint.keysDown = mKeysPressed;

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
        //TODO save current point in viewport ?
    }

    mStylusLastEventTime = std::chrono::steady_clock::now();
}

void IOdysseyViewportDrawingEditorAdapter::RemoveTextureOverride()
{
    if( !mPaintingTexture2DRenderTarget || !mPaintingTexture2DRenderTarget->IsValidLowLevel() )
        return;

    if (mEditor->Component() != nullptr && mEditor->Texture() != nullptr)
    {
        const ERHIFeatureLevel::Type FeatureLevel = mEditor->Component()->GetWorld()->FeatureLevel;
        mEditor->Material()->OverrideTexture(mEditor->Texture(), nullptr, FeatureLevel);
        //IMeshPaintGeometryAdapter::DefaultApplyOrRemoveTextureOverride(mEditor->Component(), mEditor->Texture(), nullptr);

        mPaintingTexture2DRenderTarget->ConditionalBeginDestroy();
        mPaintingTexture2DRenderTarget = nullptr;
        mState = eState::kIdle;
    }
}

#undef LOCTEXT_NAMESPACE