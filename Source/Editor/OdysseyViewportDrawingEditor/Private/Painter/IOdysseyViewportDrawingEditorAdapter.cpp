// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "IOdysseyViewportDrawingEditorAdapter.h"

#include "PainterEditor/OdysseyPainterEditorSource.h"
#include "TextureEditor/OdysseyTextureEditorSource.h"
#include "OdysseyBrushAssetBase.h"
#include "IOdysseyStylusInputModule.h"

#include "IMeshPaintGeometryAdapter.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "UObject/SavePackage.h"
#include "TextureCompiler.h"
#include "FileHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingTool.h"
#include "TexturePaintHelpers.h"

#define LOCTEXT_NAMESPACE "IOdysseyViewportDrawingEditorAdapter"

IOdysseyViewportDrawingEditorAdapter::~IOdysseyViewportDrawingEditorAdapter()
{
}

IOdysseyViewportDrawingEditorAdapter::IOdysseyViewportDrawingEditorAdapter(FOdysseyViewportDrawingEditorExtension* iExtension) :
    mTexture(nullptr),
    mExtension(iExtension),
    mState( eState::kIdle ),
    mLastKnownViewport(nullptr),
    mIsCapturedByStylus(false),
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
IOdysseyViewportDrawingEditorAdapter::SetTexture(UTexture2D* iTexture)
{
    mTexture = iTexture;
    mState = mTexture ? eState::kIdleReady : eState::kIdle;
}

UTexture2D*
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
    
    mTool = iTool;

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
    return Cast<UOdysseyPainterEditorRasterDrawingTool>(mTool);
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

void IOdysseyViewportDrawingEditorAdapter::StartPainting()
{
    if (mTool)
        mTool->OnMouseDown(mCurrentStrokeRay.mPoint, EKeys::LeftMouseButton);
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
}

FVector2D IOdysseyViewportDrawingEditorAdapter::ViewportCoordinatesToTextureCoordinates(FVector2D iPositionInViewport, FEditorViewportClient* iViewportClient)
{
    if (!mTexture)
        return FVector2D( 0, 0 );

    const TSharedPtr<IMeshPaintGeometryAdapter>* meshAdapterPtr = mExtension->ComponentToAdapterMap().Find(mExtension->Component());
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
    if (UGameplayStatics::FindCollisionUV(traceHitResult, mExtension->GetUVIndexUsedByCurrentTexture(), coord))
    {
        iPositionInViewport.X = coord.X * mTexture->GetSurfaceWidth();
        iPositionInViewport.Y = coord.Y * mTexture->GetSurfaceHeight();
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

    FVector2D pointPos = mExtension->PaintingAdapterMethod() == EOdysseyViewportDrawingPaintingAdapterMethod::OdysseyTextureBased ? ViewportCoordinatesToTextureCoordinates(FVector2D(iViewport->GetMouseX(), iViewport->GetMouseY()), iViewportClient) : FVector2D(iViewport->GetMouseX(), iViewport->GetMouseY());

    mLastStrokeRay = mCurrentStrokeRay;
    
    mCurrentStrokeRay.mRayOrigin = mouseViewportRay.GetOrigin();
    mCurrentStrokeRay.mRayDirection = mouseViewportRay.GetDirection();
    mCurrentStrokeRay.mPoint = FOdysseyPoint::DefaultPoint();
    mCurrentStrokeRay.mPoint.x = pointPos.X;
    mCurrentStrokeRay.mPoint.y = pointPos.Y;
    mCurrentStrokeRay.mPoint.keysDown = mKeysPressed;
    mCurrentStrokeRay.mPoint.ComputeRelativeParameters(mLastStrokeRay.mPoint);

    if( mExtension->GetEditor()->GetSelectedTool() )
        mExtension->GetEditor()->GetSelectedTool()->OnMouseHover( mCurrentStrokeRay.mPoint );

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

    FVector2D pointPos = mExtension->PaintingAdapterMethod() == EOdysseyViewportDrawingPaintingAdapterMethod::OdysseyTextureBased ? ViewportCoordinatesToTextureCoordinates(FVector2D(iViewport->GetMouseX(), iViewport->GetMouseY()), iViewportClient) : FVector2D(iViewport->GetMouseX(), iViewport->GetMouseY());

    //Init our StrokeRay, having all the basic info to draw 
    FOdysseyRay strokeRay;
    strokeRay.mRayOrigin = mouseViewportRay.GetOrigin();
    strokeRay.mRayDirection = mouseViewportRay.GetDirection();
    strokeRay.mPoint = FOdysseyPoint::DefaultPoint();
    strokeRay.mPoint.x = pointPos.X;
    strokeRay.mPoint.y = pointPos.Y;
    strokeRay.mPoint.keysDown = mKeysPressed;
    strokeRay.mPoint.ComputeRelativeParameters(mCurrentStrokeRay.mPoint);

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
            return true;
        }
    }

    if (mExtension->GetEditor()->GetSelectedTool())
    {
        if (iEvent == EInputEvent::IE_Pressed)
            return mExtension->GetEditor()->GetSelectedTool()->OnKeyDown(iKey);
        else if (iEvent == EInputEvent::IE_Released)
            return mExtension->GetEditor()->GetSelectedTool()->OnKeyUp(iKey);
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

    FVector2D pointPos = mExtension->PaintingAdapterMethod() == EOdysseyViewportDrawingPaintingAdapterMethod::OdysseyTextureBased ? ViewportCoordinatesToTextureCoordinates(FVector2D(iViewport->GetMouseX(), iViewport->GetMouseY()), iViewportClient) : FVector2D(iViewport->GetMouseX(), iViewport->GetMouseY());

    //Init our StrokeRay, having all the basic info to draw 
    FOdysseyRay strokeRay;
    strokeRay.mRayOrigin = mouseViewportRay.GetOrigin();
    strokeRay.mRayDirection = mouseViewportRay.GetDirection();
    strokeRay.mPoint = FOdysseyPoint::DefaultPoint();
    strokeRay.mPoint.x = pointPos.X;
    strokeRay.mPoint.y = pointPos.Y;
    strokeRay.mPoint.keysDown = mKeysPressed;
    strokeRay.mPoint.ComputeRelativeParameters(mCurrentStrokeRay.mPoint);


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

    FVector2D pointPos = mExtension->PaintingAdapterMethod() == EOdysseyViewportDrawingPaintingAdapterMethod::OdysseyTextureBased ? ViewportCoordinatesToTextureCoordinates(positionInViewport, viewportClient) : positionInViewport;

    FOdysseyRay strokeRay;
    strokeRay.mRayOrigin = mouseViewportRay.GetOrigin();
    strokeRay.mRayDirection = mouseViewportRay.GetDirection();
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
        mLastStrokeRay = mCurrentStrokeRay;
        mCurrentStrokeRay = strokeRay;
    }

    mStylusLastEventTime = std::chrono::steady_clock::now();
}

#undef LOCTEXT_NAMESPACE