// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "IOdysseyViewportDrawingEditorAdapter.h"

#include "PainterEditor/OdysseyPainterEditorSource.h"
#include "TextureEditor/OdysseyTextureEditorSource.h"
#include "OdysseyBrushAssetBase.h"
#include "IOdysseyStylusInputModule.h"

#include "IMeshPaintGeometryAdapter.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "TextureCompiler.h"
#include "FileHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingTool.h"
#include "TexturePaintHelpers.h"

IOdysseyViewportDrawingEditorAdapter::~IOdysseyViewportDrawingEditorAdapter()
{
}

IOdysseyViewportDrawingEditorAdapter::IOdysseyViewportDrawingEditorAdapter(FOdysseyViewportDrawingEditorExtension* iExtension) :
    mTexture(nullptr),
    mExtension(iExtension),
    mState( eState::kIdle ),
    mLastKnownViewport(nullptr),
    mTool(nullptr)
{
}

void
IOdysseyViewportDrawingEditorAdapter::Initialize()
{
    mExtension->GetEditor()->OnSelectedToolChanged().AddRaw(this, &IOdysseyViewportDrawingEditorAdapter::OnSelectedToolChanged);

    UOdysseyStylusInputSubsystem* inputSubsystem = GEditor->GetEditorSubsystem<UOdysseyStylusInputSubsystem>();
    inputSubsystem->AddMessageHandler(*this);

    SetTool(mExtension->GetEditor()->GetSelectedTool());

    mState = eState::kIdleReady;
}

void
IOdysseyViewportDrawingEditorAdapter::Finalize()
{
    mState = eState::kIdle;
    SetTexture(nullptr);
    SetTool(nullptr);
    mExtension->GetEditor()->OnSelectedToolChanged().RemoveAll(this);
    
    UOdysseyStylusInputSubsystem* inputSubsystem = GEditor->GetEditorSubsystem<UOdysseyStylusInputSubsystem>();
    inputSubsystem->RemoveMessageHandler(*this);
}

void
IOdysseyViewportDrawingEditorAdapter::SetTexture(UTexture* iTexture)
{
    mTexture = iTexture;
    mState = mTexture ? eState::kIdleReady : eState::kIdle;
}

UTexture*
IOdysseyViewportDrawingEditorAdapter::GetTexture() const
{
    return mTexture;
}

void
IOdysseyViewportDrawingEditorAdapter::OnSelectedToolChanged()
{
    //If the tool is a drawing tool, we need to prepare the brushInstance to draw in a 3D Context
    SetTool(mExtension->GetEditor()->GetSelectedTool());
}

void
IOdysseyViewportDrawingEditorAdapter::SetTool(UOdysseyPainterEditorTool* iTool)
{
    UOdysseyPainterEditorRasterDrawingTool* drawingTool = GetDrawingTool();
    if (drawingTool)
    {
        drawingTool->OnCreatedBrushInstance().RemoveAll(this);
        drawingTool->OnDestroyBrushInstance().RemoveAll(this);
        UnbindStampBrushInstance(drawingTool->GetBrushInstance());
    }
    
    mTool = TStrongObjectPtr< UOdysseyPainterEditorTool >(iTool);

    drawingTool = GetDrawingTool();
    if (drawingTool)
    {
        drawingTool->OnCreatedBrushInstance().AddRaw(this, &IOdysseyViewportDrawingEditorAdapter::BindStampBrushInstance);
        drawingTool->OnDestroyBrushInstance().AddRaw(this, &IOdysseyViewportDrawingEditorAdapter::UnbindStampBrushInstance);
        BindStampBrushInstance(drawingTool->GetBrushInstance());
    }
}

UOdysseyPainterEditorRasterDrawingTool*
IOdysseyViewportDrawingEditorAdapter::GetDrawingTool()
{
    return Cast<UOdysseyPainterEditorRasterDrawingTool>(mTool.Get());
}

void IOdysseyViewportDrawingEditorAdapter::UnbindStampBrushInstance(UOdysseyBrushAssetBase* iUnbindBrush)
{
    /* if (iUnbindBrush)
        iUnbindBrush->GetStampOverrideDelegate().Unbind();*/
}

void IOdysseyViewportDrawingEditorAdapter::BindStampBrushInstance(UOdysseyBrushAssetBase* iBindBrush)
{
    /* if (iBindBrush)
        iBindBrush->GetStampOverrideDelegate().BindRaw(this, &IOdysseyViewportDrawingEditorAdapter::StampOverride); */
}

void IOdysseyViewportDrawingEditorAdapter::StartPainting()
{
    UOdysseyPainterEditorRasterDrawingTool* rasterDrawingTool = Cast<UOdysseyPainterEditorRasterDrawingTool>(mTool.Get());
    if (rasterDrawingTool)
    {
        UOdysseyBrushAssetBase* brushInstance = rasterDrawingTool->GetBrushInstance();
        if (brushInstance)
            brushInstance->GetStampOverrideDelegate().BindRaw(this, &IOdysseyViewportDrawingEditorAdapter::StampOverride);
    }

    if (mTool)
        mCapturedByEditor = mTool->OnMouseDown(mCurrentStrokeRay.mPoint, EKeys::LeftMouseButton);
}

void IOdysseyViewportDrawingEditorAdapter::Paint()
{
    if (!mTool)
        return;

    mTool->OnMouseDrag(mCurrentStrokeRay.mPoint);
}

void IOdysseyViewportDrawingEditorAdapter::FinishPainting()
{
    if (!mTool)
        return;
    
    mTool->OnMouseUp(mCurrentStrokeRay.mPoint, EKeys::LeftMouseButton);
    mCapturedByEditor = false;

    UOdysseyPainterEditorRasterDrawingTool* rasterDrawingTool = Cast<UOdysseyPainterEditorRasterDrawingTool>(mTool.Get());
    if (rasterDrawingTool)
    {
        UOdysseyBrushAssetBase* brushInstance = rasterDrawingTool->GetBrushInstance();
        if (brushInstance)
            brushInstance->GetStampOverrideDelegate().Unbind();
    }
}

bool
IOdysseyViewportDrawingEditorAdapter::ViewportCoordinatesToTextureCoordinates(FVector2D iPositionInViewport, FEditorViewportClient* iViewportClient, FVector2D* oPositionInTexture)
{
    if ( !mTexture )
        return false;

    const TSharedPtr<IMeshPaintGeometryAdapter>* meshAdapterPtr = mExtension->ComponentToAdapterMap().Find(mExtension->Component());
    if (!meshAdapterPtr)
        return false;

    TSharedPtr<IMeshPaintGeometryAdapter> meshAdapter = *meshAdapterPtr;

    // Compute a world space ray from the screen space mouse coordinates
    /* FSceneViewFamilyContext viewFamily(FSceneViewFamily::ConstructionValues(
        iViewportClient->Viewport,
        iViewportClient->GetScene(),
        iViewportClient->EngineShowFlags)
        .SetRealtimeUpdate(iViewportClient->IsRealtime()));
    FSceneView* view = iViewportClient->CalcSceneView(&viewFamily);

    const FViewportCursorLocation mouseViewportRay(view, iViewportClient, iPositionInViewport.X, iPositionInViewport.Y); */

    FVector rayOrigin;
    FVector rayDirection;
    GetRayParamsFromViewportPosition(iViewportClient, iPositionInViewport.X, iPositionInViewport.Y, &rayOrigin, &rayDirection);

    FHitResult traceHitResult(1.0f);
    const FVector rayEnd(rayOrigin + rayDirection * HALF_WORLD_MAX);

    meshAdapter->LineTraceComponent(traceHitResult, rayOrigin, rayEnd, FCollisionQueryParams(SCENE_QUERY_STAT(Paint), true));

    // Convert trace to UV position
    FVector2D coord;
    if (UGameplayStatics::FindCollisionUV(traceHitResult, mExtension->GetUVIndexUsedByCurrentTexture(), coord))
    {
        oPositionInTexture->X = coord.X * mTexture->GetSurfaceWidth();
        oPositionInTexture->Y = coord.Y * mTexture->GetSurfaceHeight();
        return true;
    }

    return false;
}

bool IOdysseyViewportDrawingEditorAdapter::IsReadyToDraw() 
{
    if( !mTexture )
        return false;

    if (!mExtension->Actor() || !mExtension->Component() || !mExtension->Material() || mExtension->Actor()->IsPendingKill() || mExtension->Component()->IsPendingKill() || mExtension->Material()->IsPendingKill() )
    {
        SetTexture(nullptr);
        return false;
    }

    return mState != eState::kIdle;
}

bool IOdysseyViewportDrawingEditorAdapter::MouseMove(FEditorViewportClient* iViewportClient, FViewport* iViewport, int32 iX, int32 iY)
{
    mOverrideMouseCursor = false;

    if (!IsReadyToDraw())
        return false;

    // Compute a world space ray from the screen space mouse coordinates
    /* FSceneViewFamilyContext viewFamily(FSceneViewFamily::ConstructionValues(
        iViewportClient->Viewport,
        iViewportClient->GetScene(),
        iViewportClient->EngineShowFlags)
        .SetRealtimeUpdate(iViewportClient->IsRealtime()));
    FSceneView* view = iViewportClient->CalcSceneView(&viewFamily);
    const FViewportCursorLocation mouseViewportRay(view, (FEditorViewportClient*)iViewport->GetClient(), iViewport->GetMouseX(), iViewport->GetMouseY()); */

    FVector2D pointPos(iViewport->GetMouseX(), iViewport->GetMouseY());
    bool isTextureBased = mExtension->PaintingAdapterMethod() == EOdysseyViewportDrawingPaintingAdapterMethod::OdysseyTextureBased;
    if (isTextureBased && !ViewportCoordinatesToTextureCoordinates(pointPos, iViewportClient, &pointPos))
        return false;

    mLastStrokeRay = mCurrentStrokeRay;
    
    GetRayParamsFromViewportPosition(iViewportClient, iViewport->GetMouseX(), iViewport->GetMouseY(), &mCurrentStrokeRay.mRayOrigin, &mCurrentStrokeRay.mRayDirection);
    mCurrentStrokeRay.mPoint = FOdysseyPoint::DefaultPoint();
    mCurrentStrokeRay.mPoint.x = pointPos.X;
    mCurrentStrokeRay.mPoint.y = pointPos.Y;
    mCurrentStrokeRay.mPoint.keysDown = mKeysPressed;
    mCurrentStrokeRay.mPoint.ComputeRelativeParameters(mLastStrokeRay.mPoint);

    if( mExtension->GetEditor()->GetSelectedTool() )
    {
        mExtension->GetEditor()->GetSelectedTool()->OnMouseHover( mCurrentStrokeRay.mPoint );
        
        mMouseCursor = mExtension->GetEditor()->GetSelectedTool()->GetMouseCursor();
        mOverrideMouseCursor = true;
    }

    return true;
}

bool IOdysseyViewportDrawingEditorAdapter::InputKey(FEditorViewportClient* iViewportClient, FViewport* iViewport, FKey iKey, EInputEvent iEvent)
{
    if (mLastKnownViewport != iViewport)
        mLastKnownViewport = iViewport;

    /* FString eventStr = TEXT("");
    if (iEvent == IE_Repeat)
        eventStr = TEXT("REPEAT");
    if (iEvent == IE_Pressed)
        eventStr = TEXT("PRESSED");
    if (iEvent == IE_Released)
        eventStr = TEXT("RELEASED");
    UE_LOG(LogTemp, Warning, TEXT("Input Key : %s %s"), *iKey.ToString(), *eventStr); */

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
    if( iEvent == EInputEvent::IE_Pressed )
    {
        //key already pressed, don't send a KeyDown or MouseDown twice
        //Can happen on windows with some touch options
        if (mKeysPressed.Contains(iKey))
            return true;
            
        mKeysPressed.Add( iKey );
    }
    else if( iEvent == EInputEvent::IE_Released ) 
    {
        //key already released, don't send a KeyUp or MouseUp twice
        //Can happen on windows with some touch options
        if (!mKeysPressed.Contains(iKey)) 
            return true;

        mKeysPressed.Remove(iKey);
    }
    else if( iEvent == EInputEvent::IE_DoubleClick )
    {
        UOdysseyPainterEditorTool* selectedTool = mExtension->GetEditor()->GetSelectedTool();
        if (!selectedTool)
            return false;

        bool ignoreDown = selectedTool->OnMouseDoubleClick(mCurrentStrokeRay.mPoint, iKey);
        if (ignoreDown)
            return true;
        
        if (mKeysPressed.Contains(iKey))
            return true;
        
        mKeysPressed.Add( iKey );
    }

    //Cleanup PressedKeys
    for (int i = mKeysPressed.Num() - 1; i >= 0; i--)
    {
        if (!iViewport->KeyState(mKeysPressed[i]))
        {
            mKeysPressed.RemoveAt(i);
        }
    }

    //---

    /* const bool bIsAltDown =  mKeysPressed.Contains( EKeys::LeftAlt ) || mKeysPressed.Contains( EKeys::RightAlt );
    const bool bIsCtrlDown = mKeysPressed.Contains( EKeys::LeftControl ) || mKeysPressed.Contains( EKeys::RightControl );
    
    UE_LOG(LogTemp, Warning, TEXT("bIsAltDown : %s"), bIsAltDown ? TEXT("TRUE") : TEXT("FALSE"));
    //Dolly the camera, we're not painting
    if ( bIsAltDown ) 
        return false; */

    //---

    // Compute a world space ray from the screen space mouse coordinates
    /* FSceneViewFamilyContext viewFamily(FSceneViewFamily::ConstructionValues(
        iViewportClient->Viewport,
        iViewportClient->GetScene(),
        iViewportClient->EngineShowFlags)
        .SetRealtimeUpdate(iViewportClient->IsRealtime()));
    FSceneView* view = iViewportClient->CalcSceneView(&viewFamily);
    const FViewportCursorLocation mouseViewportRay(view,(FEditorViewportClient*)iViewport->GetClient(),iViewport->GetMouseX(),iViewport->GetMouseY()); */

    FVector2D pointPos(iViewport->GetMouseX(), iViewport->GetMouseY());
    bool isTextureBased = mExtension->PaintingAdapterMethod() == EOdysseyViewportDrawingPaintingAdapterMethod::OdysseyTextureBased;

    FVector2D posInTexture = pointPos;
    bool isOutsideTexture = !ViewportCoordinatesToTextureCoordinates(pointPos, iViewportClient, &posInTexture);
    if (isTextureBased)
        pointPos = posInTexture;
    
    //If we didn't initiate the drawing process in a previous event
    //and the current event is a mouse event
    //we filter some mouse events to allow (return false) :
    // - double click on meshes
    // - right click camera movement
    //and disallow (return true) left click camera movement
    bool isMouseEvent = iKey == EKeys::LeftMouseButton || iKey == EKeys::RightMouseButton;
    if (!mIsMouseDown && isMouseEvent && (isOutsideTexture || iEvent == IE_Released)) 
    {
        if (iEvent == EInputEvent::IE_DoubleClick)
            return false;

        if (iKey == EKeys::RightMouseButton)
            return false;

        if (iKey == EKeys::LeftMouseButton)
            return false;

        return false;
    }

    //Init our StrokeRay, having all the basic info to draw 
    FOdysseyRay strokeRay;
    GetRayParamsFromViewportPosition(iViewportClient, iViewport->GetMouseX(), iViewport->GetMouseY(), &strokeRay.mRayOrigin, &strokeRay.mRayDirection);
    strokeRay.mPoint = FOdysseyPoint::DefaultPoint();
    strokeRay.mPoint.x = pointPos.X;
    strokeRay.mPoint.y = pointPos.Y;
    strokeRay.mPoint.keysDown = mKeysPressed;
    strokeRay.mPoint.ComputeRelativeParameters(mCurrentStrokeRay.mPoint);

    if (!mIsMouseDown && (iKey == EKeys::LeftMouseButton || iKey == EKeys::RightMouseButton))
        mMouseButton = iKey;

    if (iKey == EKeys::LeftMouseButton || iKey == EKeys::RightMouseButton)
    {
        if (iEvent == EInputEvent::IE_Pressed || iEvent == EInputEvent::IE_DoubleClick)
        {
            StartStylusInputRecord();
            if (!mIsRecordingStylus)
            {
                MouseDown(strokeRay);
            }
        }
        else if(iEvent == EInputEvent::IE_Released)
        {
            if (mIsRecordingStylus)
            {
                StopStylusInputRecord();
            }
            else
            {
                MouseUp(strokeRay);
            }
            
            if (mIsMouseDown && !mKeysPressed.Contains(mMouseButton))
                mMouseButton = FKey();
        }
    }
    else
    {
        if (iEvent == EInputEvent::IE_Pressed)
        {
            return KeyDown(iKey);
        }
        else if(iEvent == EInputEvent::IE_Released)
        {
            return KeyUp(iKey);
        }
    }

    return mCapturedByEditor;
}

bool IOdysseyViewportDrawingEditorAdapter::CapturedMouseMove(FEditorViewportClient* iViewportClient, FViewport* iViewport, int32 iMouseX, int32 iMouseY)
{
    if (mIsRecordingStylus)
        return mCapturedByEditor;

    // Compute a world space ray from the screen space mouse coordinates
    /* FSceneViewFamilyContext viewFamily(FSceneViewFamily::ConstructionValues(
        iViewportClient->Viewport,
        iViewportClient->GetScene(),
        iViewportClient->EngineShowFlags)
        .SetRealtimeUpdate(iViewportClient->IsRealtime()));
    FSceneView* view = iViewportClient->CalcSceneView(&viewFamily);
    const FViewportCursorLocation mouseViewportRay(view, (FEditorViewportClient*)iViewport->GetClient(), iViewport->GetMouseX(), iViewport->GetMouseY()); */

    FVector2D pointPos(iViewport->GetMouseX(), iViewport->GetMouseY());
    bool isTextureBased = mExtension->PaintingAdapterMethod() == EOdysseyViewportDrawingPaintingAdapterMethod::OdysseyTextureBased;
    bool isOutsideTexture = false;

    FVector2D posInTexture = pointPos;
    isOutsideTexture = !ViewportCoordinatesToTextureCoordinates(pointPos, iViewportClient, &posInTexture);
    if (isTextureBased)
        pointPos = posInTexture;
    
    bool isEventIntercepted = mIsMouseDown;
    if (isOutsideTexture)
        return isEventIntercepted; //don't allow camera to move with left click, but allow with right click

    //Init our StrokeRay, having all the basic info to draw 
    FOdysseyRay strokeRay;
    GetRayParamsFromViewportPosition(iViewportClient, iViewport->GetMouseX(), iViewport->GetMouseY(), &strokeRay.mRayOrigin, &strokeRay.mRayDirection);
    strokeRay.mPoint = FOdysseyPoint::DefaultPoint();
    strokeRay.mPoint.x = pointPos.X;
    strokeRay.mPoint.y = pointPos.Y;
    strokeRay.mPoint.keysDown = mKeysPressed;
    strokeRay.mPoint.ComputeRelativeParameters(mCurrentStrokeRay.mPoint);

    MouseDrag(strokeRay);
    return mCapturedByEditor;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------ Input Functions----

void
IOdysseyViewportDrawingEditorAdapter::MouseDown(const FOdysseyRay& iRay)
{
    if(!IsReadyToDraw())
        return;

    if (mIsMouseDown)
        return;
        
    mIsMouseDown = true;

    mLastStrokeRay = mCurrentStrokeRay;
    mCurrentStrokeRay = iRay;

    if (mState == eState::kIdleReady && mMouseButton == EKeys::LeftMouseButton)
    {
        mState = eState::kDrawing;
        StartPainting();
        return;
    }
}

void
IOdysseyViewportDrawingEditorAdapter::MouseUp(const FOdysseyRay& iRay)
{
    if(!IsReadyToDraw())
        return;

    if (!mIsMouseDown)
        return;

    mLastStrokeRay = mCurrentStrokeRay;
    mCurrentStrokeRay = iRay;

    mIsMouseDown = false;

    if (mState == eState::kDrawing && mMouseButton == EKeys::LeftMouseButton)
    {
        FinishPainting();
        mState = eState::kIdleReady;
    }
}

void
IOdysseyViewportDrawingEditorAdapter::MouseDrag(const FOdysseyRay& iRay)
{
    if(!IsReadyToDraw())
        return;

    if (!mIsMouseDown)
        return;

    mLastStrokeRay = mCurrentStrokeRay;
    mCurrentStrokeRay = iRay;

    bool hasMoved = long(mCurrentStrokeRay.mPoint.x) != long(mLastStrokeRay.mPoint.x) || long(mCurrentStrokeRay.mPoint.y) != long(mLastStrokeRay.mPoint.y);

    if (mState == eState::kDrawing)
    {
        if (!hasMoved)
            return;

        Paint();
    }
}

bool
IOdysseyViewportDrawingEditorAdapter::KeyDown(FKey iKey)
{
    if(!IsReadyToDraw())
        return false;

    UOdysseyPainterEditorTool* selectedTool = mExtension->GetEditor()->GetSelectedTool();
    if (selectedTool)
        return selectedTool->OnKeyDown(iKey);

    return false;
}

bool
IOdysseyViewportDrawingEditorAdapter::KeyUp(FKey iKey)
{
    if(!IsReadyToDraw())
        return false;

    UOdysseyPainterEditorTool* selectedTool = mExtension->GetEditor()->GetSelectedTool();
    if (selectedTool)
        return selectedTool->OnKeyUp(iKey);

    return false;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------ Stylus Functions

void
IOdysseyViewportDrawingEditorAdapter::StartStylusInputRecord()
{
    if (mIsRecordingStylus)
        return;

    auto end_time = std::chrono::steady_clock::now();
    auto delta = std::chrono::duration_cast<std::chrono::milliseconds>( end_time - mStylusLastEventTime).count();
    if(delta > 500 )
        return;
        
    mIsRecordingStylus = true;

    //Down
    ReadStylusInput();

    UOdysseyStylusInputSubsystem* inputSubsystem = GEditor->GetEditorSubsystem<UOdysseyStylusInputSubsystem>();
    inputSubsystem->Flush(); //Get late stylus events
}

void
IOdysseyViewportDrawingEditorAdapter::StopStylusInputRecord()
{
    if (!mIsRecordingStylus)
        return;

    ReadStylusInput();

    UOdysseyStylusInputSubsystem* inputSubsystem = GEditor->GetEditorSubsystem<UOdysseyStylusInputSubsystem>();
    inputSubsystem->Flush(); //Get late stylus events

    //UP
    mIsRecordingStylus = false;
}

void
IOdysseyViewportDrawingEditorAdapter::GetRayParamsFromViewportPosition(FEditorViewportClient* iViewportClient, float iX, float iY, FVector* oOrigin, FVector* oDirection)
{
    // Compute a world space ray from the screen space mouse coordinates
    FSceneViewFamilyContext viewFamily(FSceneViewFamily::ConstructionValues(
        iViewportClient->Viewport,
        iViewportClient->GetScene(),
        iViewportClient->EngineShowFlags)
        .SetRealtimeUpdate(iViewportClient->IsRealtime()));
    FSceneView* view = iViewportClient->CalcSceneView(&viewFamily);

    FVector4 ScreenPos = view->CursorToScreen(iX, iY, 0);

    const FMatrix InvViewMatrix = view->ViewMatrices.GetInvViewMatrix();
    const FMatrix InvProjMatrix = view->ViewMatrices.GetInvProjectionMatrix();

    const double ScreenX = ScreenPos.X;
    const double ScreenY = ScreenPos.Y;

    if (iViewportClient->IsPerspective())
    {
        *oOrigin = view->ViewMatrices.GetViewOrigin();
        *oDirection = InvViewMatrix.TransformVector(FVector(InvProjMatrix.TransformFVector4(FVector4(ScreenX * GNearClippingPlane, ScreenY * GNearClippingPlane, 0.0f, GNearClippingPlane)))).GetSafeNormal();
    }
    else
    {
        *oOrigin = InvViewMatrix.TransformFVector4(InvProjMatrix.TransformFVector4(FVector4(ScreenX, ScreenY, 0.5f, 1.0f)));
        *oDirection = InvViewMatrix.TransformVector(FVector(0, 0, 1)).GetSafeNormal();
    }
}

FOdysseyRay
IOdysseyViewportDrawingEditorAdapter::StylusStateToRay(const FStylusState& iState)
{
    FEditorViewportClient* viewportClient = (FEditorViewportClient*)mLastKnownViewport->GetClient();
    if( !viewportClient )
        return FOdysseyRay();

    TSharedPtr< SViewport > viewportWidget = GCurrentLevelEditingViewportClient->GetEditorViewportWidget()->GetSceneViewport()->GetViewportWidget().Pin();
    if (!viewportWidget)
        return FOdysseyRay();

    //Init our StrokeRay, having all the basic info to draw 
    float scaleDPI = viewportWidget->GetCachedGeometry().GetAccumulatedLayoutTransform().GetScale();
    FVector2D positionInViewport = viewportWidget->GetCachedGeometry().AbsoluteToLocal(iState.GetPosition()) * scaleDPI;

    FVector2D pointPos = positionInViewport;
    bool isTextureBased = mExtension->PaintingAdapterMethod() == EOdysseyViewportDrawingPaintingAdapterMethod::OdysseyTextureBased;

    FVector2D posInTexture = pointPos;
    bool isOutsideTexture = !ViewportCoordinatesToTextureCoordinates(pointPos, viewportClient, &posInTexture);
    if (isTextureBased)
        pointPos = posInTexture;

    if (isOutsideTexture)
        return FOdysseyRay();

    FOdysseyRay strokeRay;
    GetRayParamsFromViewportPosition(viewportClient, positionInViewport.X, positionInViewport.Y, &strokeRay.mRayOrigin, &strokeRay.mRayDirection);
    strokeRay.mPoint = FOdysseyPoint(             pointPos.X
                                                , pointPos.Y
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
    strokeRay.mPoint.ComputeRelativeParameters(mCurrentStrokeRay.mPoint);

    return strokeRay;
}

void
IOdysseyViewportDrawingEditorAdapter::ReadStylusInput()
{
    if (!mIsRecordingStylus)
        return;

    for (int i = mLastStylusEventIndex; i < mStylusStates.Num(); i++ )
    {
        const FStylusState& state = mStylusStates[i];

        FOdysseyRay ray = StylusStateToRay(state);
        
        //Force MouseDown when using the Right Mouse Button to allow hovered mouse clicks
        if (!mStylusIsDown && (state.IsStylusDown() || mMouseButton == EKeys::RightMouseButton ))
        {
            //MouseDown
            MouseDown(ray);
            mStylusIsDown = true;
            mLastStylusEventIndex = i;
        }
        else if (mStylusIsDown && !state.IsStylusDown() && !mKeysPressed.Contains(mMouseButton))
        {
            //MouseUp
            MouseUp(ray);
            mStylusIsDown = false;
            mLastStylusEventIndex = i;
        }
        else if (mStylusIsDown)
        {
            //MouseMove
            MouseDrag(ray);
            mLastStylusEventIndex = i;
        }
    }
}

void
IOdysseyViewportDrawingEditorAdapter::OnStylusStateChanged( const TWeakPtr<SWidget> iWidget, const TArray<FStylusState>& iStates, int32 iIndex )
{
    mStylusLastEventTime = std::chrono::steady_clock::now();

    if ( !IsReadyToDraw() || !mLastKnownViewport )
        return;

    TSharedPtr<SWidget> inWidget = iWidget.Pin();
    if( !inWidget)
        return;

    TSharedPtr< SViewport > viewport = GCurrentLevelEditingViewportClient->GetEditorViewportWidget()->GetSceneViewport()->GetViewportWidget().Pin();
    if( inWidget != viewport )
        return;

    //---

    mStylusStates = iStates;
    mLastStylusEventIndex = 0;

    ReadStylusInput();
}

bool
IOdysseyViewportDrawingEditorAdapter::GetCursor(EMouseCursor::Type& OutCursor) const
{
    OutCursor = mMouseCursor;
    return mOverrideMouseCursor;
}
