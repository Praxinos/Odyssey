// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "IOdysseyViewportDrawingEditorAdapter.h"

#include "OdysseyPainterEditorSource.h"
#include "OdysseyPainterEditorTextureSource.h"
#include "OdysseyBrushAssetBase.h"
#include "IOdysseyStylusInputModule.h"
#include "Components/SkeletalMeshComponent.h"
#include "Framework/Application/SlateApplication.h"
#include "SceneView.h"
#include "MouseDeltaTracker.h"

#include "IMeshPaintGeometryAdapter.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "TextureCompiler.h"
#include "FileHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "OdysseyPainterEditorRasterDrawingTool.h"
#include "TexturePaintHelpers.h"
#include "OdysseyKeyState.h"
#include "OdysseyPainterEditorCommands.h"
#include "OdysseyViewportDrawingEditorExtension.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyStylusInputSettings.h"
#include "LevelEditorViewport.h"
#include "SEditorViewport.h"
#include "Slate/SceneViewport.h"
#include "StylusInputTabletContext.h"
#include "Tools/ColorPickerTool/OdysseyPainterEditorColorPickerTool.h"

IOdysseyViewportDrawingEditorAdapter::~IOdysseyViewportDrawingEditorAdapter()
{
}

IOdysseyViewportDrawingEditorAdapter::IOdysseyViewportDrawingEditorAdapter(FOdysseyViewportDrawingEditorExtension* iExtension) :
    mTexture(nullptr),
    mExtension(iExtension),
    mAdapterState( eAdapterState::kNotReadyToUse ),
    mLastKnownViewport(nullptr),
    mTool(nullptr),
    mIsMouseDown(false)
{
}

void
IOdysseyViewportDrawingEditorAdapter::Initialize()
{
    TSharedPtr<FOdysseyPainterEditor> editor = mExtension->GetEditor();
    if (!editor)
        return;

    editor->OnCurrentToolChanged().AddRaw(this, &IOdysseyViewportDrawingEditorAdapter::OnCurrentToolChanged);

    /*
    UOdysseyStylusInputSubsystem* inputSubsystem = GEditor->GetEditorSubsystem<UOdysseyStylusInputSubsystem>();
    inputSubsystem->AddMessageHandler(*this);*/

    SetTool(editor->GetCurrentTool());

    mAdapterState = eAdapterState::kReadyToUse;

    TSharedPtr< SViewport > viewportWidget = GCurrentLevelEditingViewportClient->GetEditorViewportWidget()->GetSceneViewport()->GetViewportWidget().Pin();
    if (viewportWidget.IsValid())
        RegisterWindow(viewportWidget.ToSharedRef());
}

void
IOdysseyViewportDrawingEditorAdapter::Finalize()
{
    mAdapterState = eAdapterState::kNotReadyToUse;
    SetTexture(nullptr);
    SetTool(nullptr);

    TSharedPtr<FOdysseyPainterEditor> editor = mExtension->GetEditor();
    if (editor)
        editor->OnCurrentToolChanged().RemoveAll(this);

    /*
    UOdysseyStylusInputSubsystem* inputSubsystem = GEditor->GetEditorSubsystem<UOdysseyStylusInputSubsystem>();
    inputSubsystem->RemoveMessageHandler(*this);*/
}

void
IOdysseyViewportDrawingEditorAdapter::SetTexture(UTexture* iTexture)
{
    mTexture = iTexture;
    mAdapterState = mTexture ? eAdapterState::kReadyToUse : eAdapterState::kNotReadyToUse;
}

UTexture*
IOdysseyViewportDrawingEditorAdapter::GetTexture() const
{
    return mTexture;
}

void
IOdysseyViewportDrawingEditorAdapter::OnCurrentToolChanged()
{
    TSharedPtr<FOdysseyPainterEditor> editor = mExtension->GetEditor();
    if (!editor)
        return;

    //If the tool is a drawing tool, we need to prepare the brushInstance to draw in a 3D Context
    SetTool(editor->GetCurrentTool());
}

void
IOdysseyViewportDrawingEditorAdapter::SetTool(UOdysseyPainterEditorTool* iTool)
{
    mTool = TStrongObjectPtr< UOdysseyPainterEditorTool >(iTool);
}

UOdysseyPainterEditorRasterDrawingTool*
IOdysseyViewportDrawingEditorAdapter::GetDrawingTool()
{
    return Cast<UOdysseyPainterEditorRasterDrawingTool>(mTool.Get());
}

void IOdysseyViewportDrawingEditorAdapter::StartPainting()
{
    mStopDrawing = false;
}

void IOdysseyViewportDrawingEditorAdapter::Paint()
{
    if (!mTool)
        return;

    if( !mStopDrawing )
        mTool->ProcessMouseDrag(mCurrentStrokeRay.mPoint);
}

void IOdysseyViewportDrawingEditorAdapter::FinishPainting()
{
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

    FCollisionQueryParams CollisionParams;
    CollisionParams.bTraceComplex = true;
    CollisionParams.bReturnFaceIndex = true;
    CollisionParams.bReturnPhysicalMaterial = true;

    /*USkeletalMeshComponent* SkeletalMeshComponent = Cast< USkeletalMeshComponent >(mExtension->Component());
    if (SkeletalMeshComponent)
    {
        SkeletalMeshComponent->SetCollisionObjectType(ECC_PhysicsBody);
        SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        SkeletalMeshComponent->bEnablePerPolyCollision = true;
        SkeletalMeshComponent->RecreatePhysicsState();
    }*/

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

    meshAdapter->LineTraceComponent(traceHitResult, rayOrigin, rayEnd, CollisionParams);

    //UE_LOG(LogTemp, Display, TEXT("%d"), traceHitResult.FaceIndex) // is 0 for Skeletal Mesh

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

void IOdysseyViewportDrawingEditorAdapter::Tick(float DeltaTime)
{
    if (mIsRecordingStylus)
        ReadStylusInput(eStylusEventFence::kStylusUp);
}

bool IOdysseyViewportDrawingEditorAdapter::IsReadyToDraw()
{
    if( !mTexture )
        return false;

    if (!IsValid(mExtension->Actor()) || !IsValid(mExtension->Component()) || !IsValid(mExtension->Material()) )
    {
        SetTexture(nullptr);
        return false;
    }

    return mAdapterState != eAdapterState::kNotReadyToUse;
}

bool IOdysseyViewportDrawingEditorAdapter::MouseEnter(FEditorViewportClient* ViewportClient, FViewport* Viewport, int32 x, int32 y)
{
    if (mIsMouseDown)
        return false;

    mIsFocused = true;
    ClearQueue();
    mAdapterState = eAdapterState::kReadyToUse;

    return true;
}

bool IOdysseyViewportDrawingEditorAdapter::MouseLeave(FEditorViewportClient* ViewportClient, FViewport* Viewport)
{
    if (mIsMouseDown)
        return false;

    mIsFocused = false;
    mAdapterState = eAdapterState::kReadyToUse;

    return true;
}

bool IOdysseyViewportDrawingEditorAdapter::MouseMove(FEditorViewportClient* iViewportClient, FViewport* iViewport, int32 iX, int32 iY)
{
    mIsFocused = true;
    mOverrideMouseCursor = false;

    if (!IsReadyToDraw())
        return false;

    //Ensures our huds hit proxies are clickable
    //Be cause sometimes we change the hud values but the proxy map needs to be invalidated
    //As we don't know everytime the hud values changes change, we need to call that here
    //RequestInvalidateHitProxy() is usually called after InputKey() if it returns false
    //But we return true on occasions where the hud needs to be invalidated too.
    iViewportClient->RequestInvalidateHitProxy(iViewport);

    //HUD
    TSharedPtr<FOdysseyHUDElement> hudElement = GetHUDElement(iViewport, iViewport->GetMouseX(), iViewport->GetMouseY());

    if (mHoveredHUDElement != hudElement)
    {
        if(mHoveredHUDElement)
        {
            mHoveredHUDElement->OnMouseLeave();
            mHoveredHUDElement = nullptr;
        }

        if(hudElement)
        {
            mHoveredHUDElement = hudElement;
            mHoveredHUDElement->OnMouseEnter();
        }
    }

    if (mHoveredHUDElement)
    {
        FVector2D viewportPoint(iViewport->GetMouseX(), iViewport->GetMouseY());
        FVector2D hudPoint;
        if (mExtension->ViewportToHUD(iViewportClient, viewportPoint, hudPoint))
        {
            mLastHUDPoint = mCurrentHUDPoint;
            mCurrentHUDPoint = FOdysseyPoint(hudPoint.X, hudPoint.Y);
            mHoveredHUDElement->OnMouseHover(mCurrentHUDPoint);
        }
    }

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

    TSharedPtr<FOdysseyPainterEditor> editor = mExtension->GetEditor();
    if( editor && editor->GetCurrentTool() )
    {
        editor->GetCurrentTool()->ProcessMouseHover( mCurrentStrokeRay.mPoint );

        mMouseCursor = editor->GetCurrentTool()->GetMouseCursor().GetMouseCursorNative();
        mOverrideMouseCursor = true;
    }

    return true;
}

TSharedPtr<FOdysseyHUDElement>
IOdysseyViewportDrawingEditorAdapter::GetHUDElement(FViewport* iViewport, int32 iX, int32 iY)
{
    HOdysseyHUDElementHitProxy* hitproxy = HitProxyCast<HOdysseyHUDElementHitProxy>(iViewport->GetHitProxy(iX, iY));
    if (!hitproxy)
        return nullptr;

    return hitproxy->HUDElement();
}

bool IOdysseyViewportDrawingEditorAdapter::InputKey(FEditorViewportClient* iViewportClient, FViewport* iViewport, FKey iKey, EInputEvent iEvent)
{
    if (mLastKnownViewport != iViewport)
        mLastKnownViewport = iViewport;

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
        //HUD
        TSharedPtr<FOdysseyHUDElement> hudElement = GetHUDElement(iViewport, iViewport->GetMouseX(), iViewport->GetMouseY());
        if (hudElement && hudElement->OnMouseDoubleClick(mCurrentHUDPoint, iKey))
            return true;

        //Tool
        TSharedPtr<FOdysseyPainterEditor> editor = mExtension->GetEditor();
        if (!editor)
            return false;

        UOdysseyPainterEditorTool* selectedTool = editor->GetCurrentTool();
        if (!selectedTool)
            return false;

        if (selectedTool->ProcessMouseDoubleClick(mCurrentStrokeRay.mPoint, iKey))
            return true;

        if (mKeysPressed.Contains(iKey)) //UP
            return true;

        mKeysPressed.Add( iKey ); //DOWN
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

    //HUD
    if (iKey == EKeys::LeftMouseButton || iKey == EKeys::RightMouseButton)
    {
        if (iEvent == EInputEvent::IE_Pressed || iEvent == EInputEvent::IE_DoubleClick)
        {
            mCurrentHUDElement = GetHUDElement(iViewport, iViewport->GetMouseX(), iViewport->GetMouseY());
            if (mCurrentHUDElement)
            {
                FVector2D viewportPoint(iViewport->GetMouseX(), iViewport->GetMouseY());
                FVector2D hudPoint;

                mHUDMouseDownReference = viewportPoint;

                if (mExtension->ViewportToHUD(iViewportClient, viewportPoint, hudPoint))
                {
                    mLastHUDPoint = mCurrentHUDPoint;
                    mCurrentHUDPoint = FOdysseyPoint(hudPoint.X, hudPoint.Y);
                    if (mCurrentHUDElement->OnMouseDown(mCurrentHUDPoint, iKey))
                    {
                        return true;
                    }
                }
            }
        }
        else if(iEvent == EInputEvent::IE_Released)
        {
            if (mCurrentHUDElement)
            {
                FVector2D viewportPoint(iViewport->GetMouseX(), iViewport->GetMouseY());
                FVector2D hudPoint;
                if (mExtension->ViewportToHUD(iViewportClient, viewportPoint, hudPoint))
                {
                    float deltaX = viewportPoint.X - mHUDMouseDownReference.X;
                    float deltaY = viewportPoint.Y - mHUDMouseDownReference.Y;
                    float deltaSquared = deltaX * deltaX + deltaY * deltaY;
                    bool bNoMouseMovement = deltaSquared < MOUSE_CLICK_DRAG_DELTA;

                    mLastHUDPoint = mCurrentHUDPoint;
                    mCurrentHUDPoint.x = hudPoint.X;
                    mCurrentHUDPoint.y = hudPoint.Y;

                    if (bNoMouseMovement)
                        mCurrentHUDElement->OnMouseClick(mCurrentHUDPoint, iKey );
                    bool handled = mCurrentHUDElement->OnMouseUp(mCurrentHUDPoint, iKey);
                    mCurrentHUDElement = nullptr;

                    if( handled )
                        return true;
                }
            }
        }
    }

    //---

    FVector2D pointPos(iViewport->GetMouseX(), iViewport->GetMouseY());
    bool isTextureBased = mExtension->PaintingAdapterMethod() == EOdysseyViewportDrawingPaintingAdapterMethod::OdysseyTextureBased;

    FVector2D posInTexture = pointPos;
    bool isOutsideTexture = !ViewportCoordinatesToTextureCoordinates(pointPos, iViewportClient, &posInTexture);

    if( isOutsideTexture )
        mStopDrawing = true;

    if (isTextureBased)
        pointPos = posInTexture;

    //If we didn't initiate the drawing process in a previous event
    //and the current event is a mouse event
    //we filter some mouse events to allow (return false) :
    // - right click camera movement
    // - Rotation around mesh
    //and disallow (return true) left click camera movement
    bool isMouseEvent = iKey == EKeys::LeftMouseButton || iKey == EKeys::RightMouseButton;
    if (mAdapterState != eAdapterState::kUsedByEditor && !mIsRecordingStylus && isOutsideTexture && isMouseEvent)
    {
        /*GEditor->GetActiveViewport()
        GEngine->GameViewport->SetMouseCaptureMode(EMouseCaptureMode::NoCapture);
        GEngine->GameViewport->SetMouseLockMode(EMouseLockMode::DoNotLock);*/

        if (iKey == EKeys::RightMouseButton ) //Moving camera
            return false;

        if( iKey == EKeys::LeftMouseButton && mKeysPressed.Contains( EKeys::LeftAlt )) //Allow rotation around object
            return false;

        return true;
    }

    //Init our StrokeRay, having all the basic info to draw
    FOdysseyRay strokeRay;
    GetRayParamsFromViewportPosition(iViewportClient, iViewport->GetMouseX(), iViewport->GetMouseY(), &strokeRay.mRayOrigin, &strokeRay.mRayDirection);
    strokeRay.mPoint = FOdysseyPoint::DefaultPoint();
    strokeRay.mPoint.x = pointPos.X;
    strokeRay.mPoint.y = pointPos.Y;
    strokeRay.mPoint.keysDown = mKeysPressed;
    strokeRay.mPoint.ComputeRelativeParameters(mCurrentStrokeRay.mPoint);

    if (iKey == EKeys::LeftMouseButton || iKey == EKeys::RightMouseButton)
    {
        if (iEvent == EInputEvent::IE_Pressed || iEvent == EInputEvent::IE_DoubleClick)
        {
            StartStylusInputRecord(iKey);
            if (!mIsRecordingStylus || iKey == EKeys::RightMouseButton)
            {
                MouseDown(strokeRay, iKey);
            }
            else
            {
                ReadStylusInput();
            }
        }
        else if(iEvent == EInputEvent::IE_Released)
        {
            if (!mIsRecordingStylus)
            {
                MouseUp(strokeRay, iKey);
            }
            else
            {
                ReadStylusInput(eStylusEventFence::kStylusUp);
            }
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

    return ShouldEditorCaptureMouse();
}

bool IOdysseyViewportDrawingEditorAdapter::CapturedMouseMove(FEditorViewportClient* iViewportClient, FViewport* iViewport, int32 iMouseX, int32 iMouseY)
{
    //HUD
    if (mCurrentHUDElement)
    {

        FVector2D viewportPoint(iViewport->GetMouseX(), iViewport->GetMouseY());
        FVector2D hudPoint;
        if (mExtension->ViewportToHUD(iViewportClient, viewportPoint, hudPoint))
        {
            mLastHUDPoint = mCurrentHUDPoint;
            mCurrentHUDPoint.x = hudPoint.X;
            mCurrentHUDPoint.y = hudPoint.Y;
            mCurrentHUDPoint.keysDown = mKeysPressed;
            mCurrentHUDPoint.ComputeRelativeParameters(mLastHUDPoint);
            mCurrentHUDElement->OnMouseDrag(mCurrentHUDPoint);

            return true;
        }
    }

    if (mIsRecordingStylus)
        return true;

    FVector2D pointPos(iViewport->GetMouseX(), iViewport->GetMouseY());
    bool isTextureBased = mExtension->PaintingAdapterMethod() == EOdysseyViewportDrawingPaintingAdapterMethod::OdysseyTextureBased;
    bool isOutsideTexture = false;

    FVector2D posInTexture = pointPos;
    isOutsideTexture = !ViewportCoordinatesToTextureCoordinates(pointPos, iViewportClient, &posInTexture);
    if (isTextureBased)
        pointPos = posInTexture;

    if (isOutsideTexture)
    {
        mStopDrawing = true;
        return ShouldEditorCaptureMouse(); //don't allow camera to move with left click, but allow with right click
    }

    //Init our StrokeRay, having all the basic info to draw
    FOdysseyRay strokeRay;
    GetRayParamsFromViewportPosition(iViewportClient, iViewport->GetMouseX(), iViewport->GetMouseY(), &strokeRay.mRayOrigin, &strokeRay.mRayDirection);
    strokeRay.mPoint = FOdysseyPoint::DefaultPoint();
    strokeRay.mPoint.x = pointPos.X;
    strokeRay.mPoint.y = pointPos.Y;
    strokeRay.mPoint.keysDown = mKeysPressed;
    strokeRay.mPoint.ComputeRelativeParameters(mCurrentStrokeRay.mPoint);

    MouseDrag(strokeRay);
    return ShouldEditorCaptureMouse();
}

bool
IOdysseyViewportDrawingEditorAdapter::HandleClick(FEditorViewportClient* iViewportClient, HHitProxy* iHitProxy, const FViewportClick& iClick)
{
    TSharedPtr<FOdysseyPainterEditor> editor = mExtension->GetEditor();
    if (!editor)
        return false;

    if(!IsReadyToDraw())
        return false;

    mCurrentHUDElement = GetHUDElement(iViewportClient->Viewport, iViewportClient->Viewport->GetMouseX(), iViewportClient->Viewport->GetMouseY());
    if (mCurrentHUDElement)
    {
        FVector2D viewportPoint(iClick.GetClickPos().X, iClick.GetClickPos().Y);
        FVector2D hudPoint;
        if (mExtension->ViewportToHUD(iViewportClient, viewportPoint, hudPoint))
        {
            mLastHUDPoint = mCurrentHUDPoint;
            mCurrentHUDPoint = FOdysseyPoint(hudPoint.X, hudPoint.Y);
            if (mCurrentHUDElement->OnMouseClick(mCurrentHUDPoint, iClick.GetKey()))
            {
                return true;
            }
        }
    }

    FOdysseyRay strokeRay;
    GetRayParamsFromViewportPosition(iViewportClient, iClick.GetClickPos().X, iClick.GetClickPos().Y, &strokeRay.mRayOrigin, &strokeRay.mRayDirection);
    strokeRay.mPoint = FOdysseyPoint::DefaultPoint();
    strokeRay.mPoint.x = iClick.GetClickPos().X;
    strokeRay.mPoint.y = iClick.GetClickPos().Y;
    strokeRay.mPoint.keysDown = mKeysPressed;
    strokeRay.mPoint.ComputeRelativeParameters(mCurrentStrokeRay.mPoint);

    if( !mStopDrawing )
    {
        mLastStrokeRay = mCurrentStrokeRay;
        mCurrentStrokeRay = strokeRay;
    }

    UOdysseyPainterEditorTool* selectedTool = editor->GetCurrentTool();
    if (selectedTool)
        return selectedTool->ProcessMouseClick(mCurrentStrokeRay.mPoint, iClick.GetKey());

    return false;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------ Input Functions----

void
IOdysseyViewportDrawingEditorAdapter::MouseDown(const FOdysseyRay& iRay, const FKey& iMouseButton)
{
    TSharedPtr<FOdysseyPainterEditor> editor = mExtension->GetEditor();
    if (!editor)
        return;

    if(!IsReadyToDraw())
        return;

    if (mIsMouseDown)
        return;

    mIsMouseDown = true;

    mLastStrokeRay = mCurrentStrokeRay;
    mCurrentStrokeRay = iRay;

    if (mAdapterState != eAdapterState::kReadyToUse)
        return;

    if (iMouseButton != EKeys::LeftMouseButton && iMouseButton != EKeys::RightMouseButton)
        return;

    UOdysseyPainterEditorRasterDrawingTool* rasterDrawingTool = Cast<UOdysseyPainterEditorRasterDrawingTool>(mTool.Get());
    if (rasterDrawingTool)
    {
        UOdysseyBrushAssetBase* brushInstance = rasterDrawingTool->GetBrushInstance();
        if (brushInstance)
            brushInstance->GetStampOverrideDelegate().BindRaw(this, &IOdysseyViewportDrawingEditorAdapter::StampOverride);
    }

    if (mTool && mTool->ProcessMouseDown(mCurrentStrokeRay.mPoint, iMouseButton))
    {
        mAdapterState = eAdapterState::kUsedByEditor;
        mMouseButton = iMouseButton;
    }

    StartPainting();
}

void
IOdysseyViewportDrawingEditorAdapter::MouseUp(const FOdysseyRay& iRay, const FKey& iMouseButton)
{
    TSharedPtr<FOdysseyPainterEditor> editor = mExtension->GetEditor();
    if (!editor)
        return;

    if(!IsReadyToDraw())
        return;

    if (!mIsMouseDown)
        return;

    mIsMouseDown = false;

    if( !mStopDrawing )
    {
        mLastStrokeRay = mCurrentStrokeRay;
        mCurrentStrokeRay = iRay;
    }

    if (mAdapterState != eAdapterState::kUsedByEditor)
        return;

    if (iMouseButton != EKeys::LeftMouseButton && iMouseButton != EKeys::RightMouseButton)
        return;

    if (!mTool)
    {
        mAdapterState = eAdapterState::kReadyToUse;
        mMouseButton = FKey();
        return;
    }

    mTool->ProcessMouseUp(mCurrentStrokeRay.mPoint, mMouseButton);
    mStopDrawing = false;
    mAdapterState = eAdapterState::kReadyToUse;
    mMouseButton = FKey();

    UOdysseyPainterEditorRasterDrawingTool* rasterDrawingTool = Cast<UOdysseyPainterEditorRasterDrawingTool>(mTool.Get());
    if (rasterDrawingTool)
    {
        UOdysseyBrushAssetBase* brushInstance = rasterDrawingTool->GetBrushInstance();
        if (brushInstance)
            brushInstance->GetStampOverrideDelegate().Unbind();
    }

    FinishPainting();
}

void
IOdysseyViewportDrawingEditorAdapter::MouseDrag(const FOdysseyRay& iRay)
{
    if(!IsReadyToDraw())
        return;

    if (!mIsMouseDown)
        return;

    if( !mStopDrawing )
    {
        mLastStrokeRay = mCurrentStrokeRay;
        mCurrentStrokeRay = iRay;
    }

    if (mAdapterState != eAdapterState::kUsedByEditor)
        return;

    bool hasMoved = !FMath::IsNearlyEqual(mCurrentStrokeRay.mPoint.x - mLastStrokeRay.mPoint.x, 0.f) || !FMath::IsNearlyEqual(mCurrentStrokeRay.mPoint.y - mLastStrokeRay.mPoint.y, 0.f);
    if (!hasMoved)
        return;

    Paint();
}

bool
IOdysseyViewportDrawingEditorAdapter::KeyDown(FKey iKey)
{
    TSharedPtr<FOdysseyPainterEditor> editor = mExtension->GetEditor();
    if (!editor)
        return false;

    if(!IsReadyToDraw())
        return false;

    UOdysseyPainterEditorTool* selectedTool = editor->GetCurrentTool();
    if (selectedTool)
        return selectedTool->ProcessKeyDown(iKey);

    return false;
}

bool
IOdysseyViewportDrawingEditorAdapter::KeyUp(FKey iKey)
{
    TSharedPtr<FOdysseyPainterEditor> editor = mExtension->GetEditor();
    if (!editor)
        return false;

    if(!IsReadyToDraw())
        return false;

    UOdysseyPainterEditorTool* selectedTool = editor->GetCurrentTool();
    if (selectedTool)
        return selectedTool->ProcessKeyUp(iKey);

    return false;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------ Stylus Functions

void
IOdysseyViewportDrawingEditorAdapter::StartStylusInputRecord(const FKey& iMouseButton)
{
    if (mIsRecordingStylus)
        return;

    auto end_time = std::chrono::steady_clock::now();
    auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - mStylusLastEventTime).count();
    if (delta > 500)
        return;

    mIsRecordingStylus = true;
    mStylusButton = iMouseButton;
}

void
IOdysseyViewportDrawingEditorAdapter::StopStylusInputRecord()
{
    if (!mIsRecordingStylus)
        return;

    ClearQueue();

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

bool
IOdysseyViewportDrawingEditorAdapter::StylusPacketToRay(const UE::StylusInput::FStylusInputPacket& iPacket, FOdysseyRay& ioRay)
{
    FEditorViewportClient* viewportClient = (FEditorViewportClient*)mLastKnownViewport->GetClient();
    if (!viewportClient)
        return false;

    TSharedPtr< SViewport > viewportWidget = GCurrentLevelEditingViewportClient->GetEditorViewportWidget()->GetSceneViewport()->GetViewportWidget().Pin();
    if (!viewportWidget)
        return false;

    TSharedPtr<SWindow> Window = mStylusInputWindow.Pin();
    FVector2D packetPos = FVector2D(iPacket.X, iPacket.Y);

//Fix wrong coordinates with Wintab, Epic should fix it in their code
#if PLATFORM_WINDOWS
    const UOdysseyStylusInputSettings* settings = GetDefault<UOdysseyStylusInputSettings>();
    FName selectedAPI = settings->StylusInputDriver;
    if (selectedAPI == "Wintab")
    {
        TSharedPtr<FGenericWindow> nativeWindow = Window->GetNativeWindow();
        void* osHandle = nativeWindow->GetOSWindowHandle();
        HWND hwnd = static_cast<HWND>(osHandle);
        RECT winRect;
        GetWindowRect(hwnd, &winRect);

        FVector2D slateTopLeft = Window->GetRectInScreen().GetTopLeft();

        FVector2D correction(slateTopLeft.X - winRect.left, slateTopLeft.Y - winRect.top);

        packetPos -= correction;
    }
#endif

    //Init our StrokeRay, having all the basic info to draw
    float scaleDPI = viewportWidget->GetCachedGeometry().GetAccumulatedLayoutTransform().GetScale();
    FVector2D positionInViewport = viewportWidget->GetCachedGeometry().AbsoluteToLocal(packetPos) * scaleDPI;
    positionInViewport += Window->GetRectInScreen().GetTopLeft();

    FVector2D pointPos = positionInViewport;
    bool isTextureBased = mExtension->PaintingAdapterMethod() == EOdysseyViewportDrawingPaintingAdapterMethod::OdysseyTextureBased;

    FVector2D posInTexture = pointPos;
    bool isOutsideTexture = !ViewportCoordinatesToTextureCoordinates(pointPos, viewportClient, &posInTexture);
    if (isTextureBased)
        pointPos = posInTexture;

    if (isOutsideTexture)
    {
        mStopDrawing = true;
        return false;
    }

    const UE::StylusInput::IStylusInputTabletContext* tabletContext = GetTabletContext(mStylusInputInstance, iPacket.TabletContextID);
    GetRayParamsFromViewportPosition(viewportClient, positionInViewport.X, positionInViewport.Y, &ioRay.mRayOrigin, &ioRay.mRayDirection);

#if PLATFORM_WINDOWS
    if (tabletContext)
    {
        UE::StylusInput::ETabletSupportedProperties capabilities = tabletContext->GetSupportedProperties();
        ioRay.mPoint.x = EnumHasAnyFlags(capabilities, UE::StylusInput::ETabletSupportedProperties::X) ? pointPos.X : 0.f;
        ioRay.mPoint.y = EnumHasAnyFlags(capabilities, UE::StylusInput::ETabletSupportedProperties::Y) ? pointPos.Y : 0.f;
        ioRay.mPoint.z = EnumHasAnyFlags(capabilities, UE::StylusInput::ETabletSupportedProperties::Z) ? iPacket.Z : 0.f;
        ioRay.mPoint.pressure = EnumHasAnyFlags(capabilities, UE::StylusInput::ETabletSupportedProperties::NormalPressure) ? iPacket.NormalPressure : 1.f;
        ioRay.mPoint.time = EnumHasAnyFlags(capabilities, UE::StylusInput::ETabletSupportedProperties::TimerTick) ? iPacket.TimerTick : 1.f;
        ioRay.mPoint.altitude = EnumHasAnyFlags(capabilities, UE::StylusInput::ETabletSupportedProperties::AltitudeOrientation) ? iPacket.AltitudeOrientation : 1.f;
        ioRay.mPoint.azimuth = EnumHasAnyFlags(capabilities, UE::StylusInput::ETabletSupportedProperties::AzimuthOrientation) ? iPacket.AzimuthOrientation : 1.f;
        ioRay.mPoint.twist = EnumHasAnyFlags(capabilities, UE::StylusInput::ETabletSupportedProperties::TwistOrientation) ? iPacket.TwistOrientation : 1.f;
        ioRay.mPoint.pitch = EnumHasAnyFlags(capabilities, UE::StylusInput::ETabletSupportedProperties::PitchRotation) ? iPacket.PitchRotation : 1.f;
        ioRay.mPoint.roll = EnumHasAnyFlags(capabilities, UE::StylusInput::ETabletSupportedProperties::RollRotation) ? iPacket.RollRotation : 1.f;
        ioRay.mPoint.yaw = EnumHasAnyFlags(capabilities, UE::StylusInput::ETabletSupportedProperties::YawRotation) ? iPacket.YawRotation : 1.f;
    }
#elif PLATFORM_MAC
    ioRay.mPoint.x = pointPos.X;
    ioRay.mPoint.y = pointPos.Y;
    ioRay.mPoint.z = iPacket.Z;
    ioRay.mPoint.pressure = iPacket.NormalPressure;
    ioRay.mPoint.time = iPacket.TimerTick;
    ioRay.mPoint.altitude = iPacket.AltitudeOrientation;
    ioRay.mPoint.azimuth = iPacket.AzimuthOrientation;
    ioRay.mPoint.twist = iPacket.TwistOrientation;
    ioRay.mPoint.pitch = iPacket.PitchRotation;
    ioRay.mPoint.roll = iPacket.RollRotation;
    ioRay.mPoint.yaw = iPacket.YawRotation;
#endif

    ioRay.mPoint.keysDown = mKeysPressed;
    ioRay.mPoint.ComputeRelativeParameters(mCurrentStrokeRay.mPoint);

    return true;
}

void
IOdysseyViewportDrawingEditorAdapter::ReadStylusInput(eStylusEventFence iUntilEventType)
{
    if (!mIsFocused || mPacketQueue.Num() == 0)
    {
        ClearQueue();
        return;
    }

    UE::StylusInput::FStylusInputPacket packet;

    while (mPacketQueue.Dequeue(packet))
    {
        FOdysseyRay ray;
        StylusPacketToRay(packet, ray);

        //Don't manage MouseDown when using the Right Mouse Button to allow hovered mouse clicks
        if (packet.Type == UE::StylusInput::EPacketType::StylusDown)
        {
            //MouseDown
            mIsStylusDown = true;
            MouseDown(ray, mStylusButton);
            mEventsConsumedSinceLastUp++;

            if (iUntilEventType == eStylusEventFence::kStylusDown)
                return;
        }
        else if (packet.Type == UE::StylusInput::EPacketType::StylusUp)
        {
            //MouseUp
            MouseUp(ray, mStylusButton);
            mIsStylusDown = false;
            mEventsConsumedSinceLastUp = 0;

            if (iUntilEventType == eStylusEventFence::kStylusUp)
            {
                StopStylusInputRecord();
                return;
            }
        }
        //Force Right Mouse Button Drag
        else if (mIsStylusDown)
        {
            //MouseMove
            MouseDrag(ray);
            mEventsConsumedSinceLastUp++;
        }
    }
}


bool
IOdysseyViewportDrawingEditorAdapter::ShouldEditorCaptureMouse() const
{
    if (mAdapterState == eAdapterState::kUsedByEditor)
        return true;

    if (mIsRecordingStylus && mStylusButton != EKeys::RightMouseButton)
        return true;

    return false;
}

void
IOdysseyViewportDrawingEditorAdapter::OnPacket(const UE::StylusInput::FStylusInputPacket& iPacket, UE::StylusInput::IStylusInputInstance* iInstance)
{
    mStylusLastEventTime = std::chrono::steady_clock::now();

// FIX: MOVE WINTAB COORDINATES WHEN MAIN SCREEN IS NOT ON THE (TOP) LEFT OF USER PHYSICAL DESKTOP - AWAITING FOR EPIC PULL REQUEST VALIDATION
#if PLATFORM_WINDOWS
    const UOdysseyStylusInputSettings* settings = GetDefault<UOdysseyStylusInputSettings>();
    FName selectedAPI = settings->StylusInputDriver;
    if (selectedAPI == "Wintab")
    {
        UE::StylusInput::FStylusInputPacket packetCopyWin = iPacket;

        if (iPacket.NormalPressure == 0)
        {
            mCurrentPenStatus = mCurrentPenStatus & ~UE::StylusInput::EPenStatus::CursorIsTouching;
            if (mCurrentPacketType == UE::StylusInput::EPacketType::OnDigitizer)
                mCurrentPacketType = UE::StylusInput::EPacketType::StylusUp;
            else
                mCurrentPacketType = UE::StylusInput::EPacketType::AboveDigitizer;
        }

        if (iPacket.NormalPressure != 0)
        {
            mCurrentPenStatus = mCurrentPenStatus | UE::StylusInput::EPenStatus::CursorIsTouching;
            if (mCurrentPacketType != UE::StylusInput::EPacketType::OnDigitizer && mCurrentPacketType != UE::StylusInput::EPacketType::StylusDown)
                mCurrentPacketType = UE::StylusInput::EPacketType::StylusDown;
            else
                mCurrentPacketType = UE::StylusInput::EPacketType::OnDigitizer;
        }
        else
        {
            mCurrentPenStatus = mCurrentPenStatus & ~UE::StylusInput::EPenStatus::CursorIsTouching;
        }

        packetCopyWin.PenStatus = mCurrentPenStatus;
        packetCopyWin.Type = mCurrentPacketType;

        if (packetCopyWin.Type == UE::StylusInput::EPacketType::StylusDown && mEventsConsumedSinceLastUp == 0)
            ClearQueue();

        mPacketQueue.Enqueue(packetCopyWin);
        return;
    }
#endif
// FIX: HAVE TO MANUALLY HANDLE UP AND DOWN UNTIL EPIC ACCEPT INTERNAL PULL REQUEST
#if PLATFORM_MAC
    UE::StylusInput::FStylusInputPacket packetCopyMac = iPacket;

    if (iPacket.NormalPressure == 0)
    {
        mCurrentPenStatus = mCurrentPenStatus & ~UE::StylusInput::EPenStatus::CursorIsTouching;
        if (mCurrentPacketType == UE::StylusInput::EPacketType::OnDigitizer)
            mCurrentPacketType = UE::StylusInput::EPacketType::StylusUp;
        else
            mCurrentPacketType = UE::StylusInput::EPacketType::AboveDigitizer;
    }

    if (iPacket.NormalPressure != 0)
    {
        mCurrentPenStatus = mCurrentPenStatus | UE::StylusInput::EPenStatus::CursorIsTouching;
        if (mCurrentPacketType != UE::StylusInput::EPacketType::OnDigitizer && mCurrentPacketType != UE::StylusInput::EPacketType::StylusDown)
            mCurrentPacketType = UE::StylusInput::EPacketType::StylusDown;
        else
            mCurrentPacketType = UE::StylusInput::EPacketType::OnDigitizer;
    }
    else
    {
        mCurrentPenStatus = mCurrentPenStatus & ~UE::StylusInput::EPenStatus::CursorIsTouching;
    }

    packetCopyMac.PenStatus = mCurrentPenStatus;
    packetCopyMac.Type = mCurrentPacketType;

    if (packetCopyMac.Type == UE::StylusInput::EPacketType::StylusDown && mEventsConsumedSinceLastUp == 0)
        ClearQueue();

    mPacketQueue.Enqueue(packetCopyMac);
    return;
#else// FIX: HAVE TO MANUALLY HANDLE UP AND DOWN UNTIL EPIC ACCEPT INTERNAL PULL REQUEST

    if (iPacket.Type == UE::StylusInput::EPacketType::StylusDown && mEventsConsumedSinceLastUp == 0)
        ClearQueue();

    mPacketQueue.Enqueue(iPacket);
#endif
}

bool
IOdysseyViewportDrawingEditorAdapter::GetCursor(EMouseCursor::Type& OutCursor) const
{
    OutCursor = mMouseCursor;
    return mOverrideMouseCursor;
}
