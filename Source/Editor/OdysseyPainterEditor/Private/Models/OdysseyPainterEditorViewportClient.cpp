// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Models/OdysseyPainterEditorViewportClient.h"

#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "CubemapUnwrapUtils.h"
#include "Editor/UnrealEdEngine.h"
#include "EngineModule.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureCube.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/TextureRenderTargetCube.h"
#include "Engine/VolumeTexture.h"
#include "ImageUtils.h"
#include "RawMesh.h"
#include "FOdysseySceneViewport.h"
#include "Texture2DPreview.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "UnrealEdGlobals.h"
#include "VolumeTexturePreview.h"
#include "Widgets/Layout/SScrollBar.h"
#include "Widgets/SViewport.h"

#include "IOdysseyStylusInputModule.h"
#include "OdysseyPaintEngine.h"
#include "OdysseyHUDSystem.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSettings.h"
#include "OdysseyStylusInputSettings.h"
#include "Mesh/FOdysseyMeshSelector.h"
#include "OdysseySurface.h"
#include "SOdysseyCursorWidget.h"
#include "SOdysseyViewport.h"
#include "Models/OdysseyPainterEditorCommands.h"
#include "OdysseyKeyState.h"
#include "OdysseyHUDElement.h"

#include <memory>
#include <chrono>
#include <ULIS>

#define LOCTEXT_NAMESPACE "PainterEditor"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorViewportClient
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorViewportClient::~FOdysseyPainterEditorViewportClient( )
{
    InputSubsystem->RemoveMessageHandler( *this );
    DestroyCheckerboardTexture();
}

FOdysseyPainterEditorViewportClient::FOdysseyPainterEditorViewportClient( FOdysseyPainterEditor*                    iOdysseyPainterEditor,
                                                                          TWeakPtr< SOdysseyViewport >              iOdysseyPainterEditorViewport,
                                                                          FOdysseyMeshSelector*                     iMeshSelector)
    : InputSubsystem( nullptr )
    , mOdysseyPainterEditor(iOdysseyPainterEditor)
    , mOdysseyPainterEditorViewportPtr( iOdysseyPainterEditorViewport )
    , mMeshSelector( iMeshSelector )
    , mCheckerboardTexture( NULL )
    , mCurrentMouseCursor( EMouseCursor::Default )
    , mCurrentToolState( eState::kIdle )
    , mNearestNeighbourTexture()
    , mBilinearTexture()
    , mIsMouseDown(false)
{
    check( mOdysseyPainterEditorViewportPtr.IsValid() );

    InputSubsystem = GEditor->GetEditorSubsystem<UOdysseyStylusInputSubsystem>();
    InputSubsystem->AddMessageHandler( *this );

    ENQUEUE_RENDER_COMMAND(InitOdysseyPainterEditorViewportClientTextures)(
        [&](FRHICommandListImmediate& RHICmdList)
        {
            FSamplerStateInitializerRHI SamplerStateInitializerNN(SF_Point , AM_Clamp, AM_Clamp, AM_Clamp);
            mNearestNeighbourTexture.SamplerStateRHI = RHICreateSamplerState(SamplerStateInitializerNN);
            FSamplerStateInitializerRHI SamplerStateInitializerB(SF_Bilinear , AM_Clamp, AM_Clamp, AM_Clamp);
            mBilinearTexture.SamplerStateRHI = RHICreateSamplerState(SamplerStateInitializerB);
        }
    );
    
    FRenderCommandFence fence;
    fence.BeginFence();
    fence.Wait();

    CreateCheckerboardTexture();
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------ Draw Viewport
void
FOdysseyPainterEditorViewportClient::Draw( FViewport* iViewport, FCanvas* ioCanvas )
{
	const UOdysseyPainterEditorSettings& settings = *GetDefault<UOdysseyPainterEditorSettings>();
	ioCanvas->Clear(settings.BackgroundColor);

    UTexture* texture       = mOdysseyPainterEditorViewportPtr.Pin()->GetTexture();
    if (!texture)
        return;

    mNearestNeighbourTexture.TextureRHI = texture->GetResource()->TextureRHI;
    mBilinearTexture.TextureRHI = texture->GetResource()->TextureRHI;

    uint32 width = 0;
    uint32 height = 0;
    mOdysseyPainterEditorViewportPtr.Pin()->ComputeTextureDisplayDimensions( width, height );

    double rotation         = FMath::RadiansToDegrees(mOdysseyPainterEditorViewportPtr.Pin()->GetRotation());
    FVector2D pivotPoint = FVector2D(0.5f, 0.5f);

    //if SViewport->GetPan() == 0,0, it means the center of the texture should be centered in the viewport
    FVector2D pan = mOdysseyPainterEditorViewportPtr.Pin()->GetPan();
    pan += mOdysseyPainterEditorViewportPtr.Pin()->GetViewportCenter() - (FVector2D(width, height) / 2.0f);

    // Fully stream in the texture before drawing it.
    texture->SetForceMipLevelsToBeResident( 30.0f );
    texture->WaitForStreaming();

    // Figure out the size we need
    const float mipLevel = 0; //should be -1, but as we are editing only the first mipmap, then the other mipmaps are not updated and so we cannot use -1 to have automatic mipmap selection.

    TRefCountPtr<FBatchedElementParameters> batchedElementParameters;

    if( GMaxRHIFeatureLevel >= ERHIFeatureLevel::SM5 )
    {
        UTexture2D* texture2D = Cast<UTexture2D>(texture);

        //ODYSSEY: PATCH
        bool isNormalMap = texture->IsNormalMap();
        bool isSingleChannel = texture->CompressionSettings == TC_Grayscale || texture->CompressionSettings == TC_Alpha;
        bool isVirtual = texture->IsCurrentlyVirtualTextured();
        bool isVTSPS = texture2D ? texture2D->IsVirtualTexturedWithSinglePhysicalSpace() : false;
        bool isTextureArray = false;
        float layerIndex = 0.f;
        batchedElementParameters = new FBatchedElementTexture2DPreviewParameters( mipLevel, layerIndex, layerIndex, isNormalMap, isSingleChannel, isVTSPS, isVirtual, isTextureArray, false );
    }

    FVector2D viewport_center( iViewport->GetSizeXY().X / 2, iViewport->GetSizeXY().Y / 2 );

    // Draw background Checker
    {
        FCanvasTileItem tileItem(pan, mCheckerboardTexture->GetResource(), FVector2D(width, height), FVector2D(0, 0), FVector2D( width / mCheckerboardTexture->GetSizeX(), height / mCheckerboardTexture->GetSizeY() ), FLinearColor::White );
        tileItem.BlendMode = SE_BLEND_Opaque;
        tileItem.PivotPoint = pivotPoint;
        tileItem.Rotation.Add( 0, rotation, 0 );
        ioCanvas->DrawItem( tileItem );
    }

    // Draw Drawing Surface
    if( texture->GetResource() != nullptr )
    {
        // PATCH:
        // This is a patch to be able to display a virtual texture
        // As virtual textures resources NEED to be a FVirtualTexture2DResource
        // We cannot use mBilinearTexture and mNearestNeighbourTexture as they are just simple FTexture objects
        // TODO: find a way to force Bilinear and NearestNeighbor filtering with virtual textures without having a lag when switching from one to another.
        FTexture* textureToDisplay = texture->GetResource();
        if (!texture->IsCurrentlyVirtualTextured())
        {
            textureToDisplay = GetZoom() <= 1.5 ? &mBilinearTexture : &mNearestNeighbourTexture;
        }
        // END PATCH:

        FVector2D topLeft = mOdysseyPainterEditorViewportPtr.Pin()->GetFlip();
        FVector2D bottomRight = FVector2D(1, 1) - topLeft;

        FCanvasTileItem tileItem(pan, textureToDisplay, FVector2D(width, height), FLinearColor::White);
        tileItem.BatchedElementParameters = batchedElementParameters;
        uint32 result = (uint32)SE_BLEND_RGBA_MASK_START;
        result += ( 1 << 0 );
        result += ( 1 << 1 );
        result += ( 1 << 2 );
        result += ( 1 << 3 );
        tileItem.BlendMode = (ESimpleElementBlendMode)result;
        tileItem.PivotPoint = pivotPoint;
        tileItem.Rotation.Add( 0, rotation, 0 );
        tileItem.UV0 = topLeft;
        tileItem.UV1 = bottomRight;
        
        if (texture->IsCurrentlyVirtualTextured() && texture->Source.GetNumBlocks() > 1)
        {
            // Adjust UVs to display entire UDIM range, accounting for UE inverted V-axis
            const FIntPoint BlockSize = texture->Source.GetSizeInBlocks();
            tileItem.UV0 = FVector2D(0.0f, 1.0f - (float)BlockSize.Y);
            tileItem.UV1 = FVector2D((float)BlockSize.X, 1.0f);
        }

        ioCanvas->DrawItem( tileItem ); 

        // if we are presenting a virtual texture, make the appropriate tiles resident
        if (texture->IsCurrentlyVirtualTextured())
        {
            FVirtualTexture2DResource* VTResource = static_cast<FVirtualTexture2DResource*>(texture->GetResource());
            const FVector2D ScreenSpaceSize((float)width, (float)height);
            const FVector2D ViewportPositon(-pan.X, -pan.Y);
            const FVector2D UV0 = tileItem.UV0;
            const FVector2D UV1 = tileItem.UV1;

            const ERHIFeatureLevel::Type InFeatureLevel = GMaxRHIFeatureLevel;
            ENQUEUE_RENDER_COMMAND(MakeTilesResident)(
                [InFeatureLevel, VTResource, ScreenSpaceSize, ViewportPositon, ViewportSize = iViewport->GetSizeXY(), UV0, UV1](FRHICommandListImmediate& RHICmdList)
                {
                    // AcquireAllocatedVT() must happen on render thread
                    IAllocatedVirtualTexture* AllocatedVT = VTResource->AcquireAllocatedVT();

                    IRendererModule& RenderModule = GetRendererModule();
                    RenderModule.RequestVirtualTextureTilesForRegion(AllocatedVT, ScreenSpaceSize, ViewportPositon, ViewportSize, UV0, UV1, 0);
                    RenderModule.LoadPendingVirtualTextureTiles(RHICmdList, InFeatureLevel);
                });
        }
    }

    if( mMeshSelector->GetCurrentMesh() )
    {
        int currentLOD = mMeshSelector->GetCurrentLOD();
        int currentUV = mMeshSelector->GetCurrentUVChannel();
        if( currentLOD >= 0 && currentUV >= 0 )
        {
            FRawMesh rawMesh;
            //ODYSSEY: PATCH
            mMeshSelector->GetCurrentMesh()->GetSourceModel( currentLOD ).LoadRawMesh( rawMesh );

            FIndexArrayView indexBuffer = mMeshSelector->GetCurrentMesh()->GetRenderData()->LODResources[currentLOD].IndexBuffer.GetArrayView();
            DrawUVsOntoViewport( iViewport, ioCanvas, currentUV, mMeshSelector->GetCurrentMesh()->GetRenderData()->LODResources[0].VertexBuffers.StaticMeshVertexBuffer, indexBuffer );
        }
    }

    FOdysseyHUDSystem::FDrawHUDParams params;
    params.mCanvas = ioCanvas;
    params.mTextureWidth = texture->GetSurfaceWidth();
    params.mTextureHeight = texture->GetSurfaceHeight();

    TSharedPtr<SOdysseyViewport> viewportWidget = mOdysseyPainterEditorViewportPtr.Pin();
    params.mTextureToHUD = FOdysseyHUDSystem::FDrawHUDParams::FTextureToHUD::CreateLambda(
		[viewportWidget, w = params.mTextureWidth, h = params.mTextureHeight](const FVector2D& iPosition)
		{
            return viewportWidget->ToWorld(iPosition - FVector2D(w / 2.f, h / 2.f));
		}
	);

    mOdysseyPainterEditor->HUDSystem()->DrawHUD(params);
}

EMouseCursor::Type
FOdysseyPainterEditorViewportClient::GetCursor( FViewport* iViewport, int32 iX, int32 iY )
{
    if( mCurrentToolState == eState::kPan || mCurrentToolState == eState::kPanZoom || mCurrentToolState == eState::kZoom )
        mCurrentMouseCursor = EMouseCursor::GrabHand;
    else if( mCurrentToolState == eState::kPick )
        mCurrentMouseCursor = EMouseCursor::EyeDropper;
    else if( mOdysseyPainterEditor->GetCurrentTool() )
        mCurrentMouseCursor = mOdysseyPainterEditor->GetCurrentTool()->GetMouseCursor();

    return mCurrentMouseCursor;
}

TOptional< TSharedRef< SWidget > >
FOdysseyPainterEditorViewportClient::MapCursor( FViewport* iViewport, const FCursorReply& iCursorReply )
{
    return FViewportClient::MapCursor( iViewport, iCursorReply );
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------ Viewport Inputs

EMouseCaptureMode
FOdysseyPainterEditorViewportClient::GetMouseCaptureMode() const
{
    //No Capture, the capture is managed by FOdysseySceneViewport
    //Because this capture activates HighPrecisionMouseMovements, which is applying acceleration to the mouse (on Mac at least)
    //And we don't want that
    return EMouseCaptureMode::NoCapture;
}

bool
FOdysseyPainterEditorViewportClient::InputKey( FViewport* iViewport, int32 iControllerId, FKey iKey, EInputEvent iEvent, float iAmountDepressed, bool iGamepad )
{
    TSharedPtr<SOdysseyViewport> viewportWidget = mOdysseyPainterEditorViewportPtr.Pin();
    UTexture* texture       = viewportWidget->GetTexture();
    if (!texture)
        return false;
    //Here you receive Mouse Buttons and Keyboard keys events
    //UE_LOG(LogTemp, Warning, TEXT("UE InputKey %s %s"), *iKey.ToString(), iEvent == EInputEvent::IE_Pressed ? TEXT("PRESSED") : iEvent == EInputEvent::IE_Released ? TEXT("RELEASED") : TEXT("OTHER"));

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

        bool ignoreDown = mOnMouseDoubleClick.IsBound() && mOnMouseDoubleClick.Execute(mCurrentPointInViewport, iKey);
        if (ignoreDown)
            return true;
        
        if (mKeysPressed.Contains(iKey))
            return true;
        
        mKeysPressed.Add( iKey );
    }

    //Cleanup PressedKeys
    for (int i = mKeysPressed.Num() - 1; i >= 0; i--)
    {
        if (!viewportWidget->GetViewport()->KeyState(mKeysPressed[i]))
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
                uint32 textureFullWidth = texture->Source.IsValid() ? texture->Source.GetSizeX() : texture->GetSurfaceWidth();
                uint32 textureFullHeight = texture->Source.IsValid() ? texture->Source.GetSizeY() : texture->GetSurfaceHeight();
                FVector2D viewportPoint(iViewport->GetMouseX(), iViewport->GetMouseY());
                FVector2D hudPoint = viewportWidget->ToLocal(viewportPoint) +  FVector2D(textureFullWidth / 2.f, textureFullHeight / 2.f);
                mCurrentHUDPoint = FOdysseyPoint(hudPoint.X, hudPoint.Y);
                if (mCurrentHUDElement->OnMouseDown(mCurrentHUDPoint, iKey))
                    return true;
            }
        }
        else if(iEvent == EInputEvent::IE_Released)
        {
            if (mCurrentHUDElement)
            {
                uint32 textureFullWidth = texture->Source.IsValid() ? texture->Source.GetSizeX() : texture->GetSurfaceWidth();
                uint32 textureFullHeight = texture->Source.IsValid() ? texture->Source.GetSizeY() : texture->GetSurfaceHeight();
                FVector2D viewportPoint(iViewport->GetMouseX(), iViewport->GetMouseY());
                FVector2D hudPoint = viewportWidget->ToLocal(viewportPoint) +  FVector2D(textureFullWidth / 2.f, textureFullHeight / 2.f);
                mCurrentHUDPoint = FOdysseyPoint(hudPoint.X, hudPoint.Y);
                mCurrentHUDPoint = FOdysseyPoint(hudPoint.X, hudPoint.Y);
                mCurrentHUDElement->OnMouseUp(mCurrentHUDPoint, iKey);
                mCurrentHUDElement = nullptr;
                return true;
            }
        }
    }

    //---

    FOdysseyPoint point_in_viewport( FOdysseyPoint::DefaultPoint() );
    point_in_viewport.x = iViewport->GetMouseX();
    point_in_viewport.y = iViewport->GetMouseY();

    if (!mIsMouseDown || mCurrentToolState != eState::kIdle )
        mCurrentToolState = InputChordToState();

    if (!mIsMouseDown && (iKey == EKeys::LeftMouseButton || iKey == EKeys::RightMouseButton))
        mMouseButton = iKey;

    if (iKey == EKeys::LeftMouseButton || iKey == EKeys::RightMouseButton)
    {
        if (iEvent == EInputEvent::IE_Pressed || iEvent == EInputEvent::IE_DoubleClick)
        {
            StartStylusInputRecord();
            if (!mIsRecordingStylus)
            {
                MouseDown(point_in_viewport);
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
                MouseUp(point_in_viewport);
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

    return true;
}

void
FOdysseyPainterEditorViewportClient::CapturedMouseMove( FViewport* iViewport, int32 iX, int32 iY )
{
    //This is called when the mouse is down and moving in the viewport
    //The viewport has already captured the mouse
    if (mIsRecordingStylus)
        return;

    //HUD
    if (mCurrentHUDElement)
    {
        TSharedPtr<SOdysseyViewport> viewportWidget = mOdysseyPainterEditorViewportPtr.Pin();
        UTexture* texture = viewportWidget->GetTexture();
        if (!texture)
            return;
        uint32 textureFullWidth = texture->Source.IsValid() ? texture->Source.GetSizeX() : texture->GetSurfaceWidth();
        uint32 textureFullHeight = texture->Source.IsValid() ? texture->Source.GetSizeY() : texture->GetSurfaceHeight();
        FVector2D viewportPoint(iViewport->GetMouseX(), iViewport->GetMouseY());
        FVector2D hudPoint = viewportWidget->ToLocal(viewportPoint) +  FVector2D(textureFullWidth / 2.f, textureFullHeight / 2.f);
        mCurrentHUDPoint = FOdysseyPoint(hudPoint.X, hudPoint.Y);
        mCurrentHUDElement->OnMouseDrag(mCurrentHUDPoint);
        return;
    }

    FOdysseyPoint point_in_viewport( FOdysseyPoint::DefaultPoint() );
    point_in_viewport.x = iX;
    point_in_viewport.y = iY;
    MouseDrag( point_in_viewport );
}

void
FOdysseyPainterEditorViewportClient::MouseEnter( FViewport* iViewport, int32 iX, int32 iY )
{
    if( mIsMouseDown )
        return;

    mCurrentToolState = eState::kIdle;
}

void
FOdysseyPainterEditorViewportClient::MouseLeave( FViewport* iViewport )
{
    if( mIsMouseDown )
        return;

    mCurrentToolState = eState::kIdle;
}


void
FOdysseyPainterEditorViewportClient::MouseMove(FViewport* iViewport, int32 iX, int32 iY)
{
    //This is called when the mouse hovers the viewport (really not the best name for that function)

    //If we don't have a surface, then we don't interact with anything
    UTexture* texture = mOdysseyPainterEditorViewportPtr.Pin()->GetTexture();
    if (!texture)
        return;

    //Ensures our huds hit proxies are clickable
    //Be cause sometimes we change the hud values but the proxy map needs to be invalidated
    //As we don't know everytime the hud values changes change, we need to call that here
    //RequestInvalidateHitProxy() is usually called after InputKey() if it returns false
    //But we return true on occasions where the hud needs to be invalidated too.
    RequestInvalidateHitProxy(iViewport);

    //HUD
    TSharedPtr<FOdysseyHUDElement> hudElement = GetHUDElement(iViewport, iViewport->GetMouseX(), iViewport->GetMouseY());
    if (mHoveredHUDElement && !hudElement)
    {
        mHoveredHUDElement->OnMouseLeave();
        mHoveredHUDElement = nullptr;
    }
    else if (!mHoveredHUDElement && hudElement)
    {
        mHoveredHUDElement = hudElement;
        mHoveredHUDElement->OnMouseEnter();
    }

    if (mHoveredHUDElement)
    {
        TSharedPtr<SOdysseyViewport> viewportWidget = mOdysseyPainterEditorViewportPtr.Pin();
        uint32 textureFullWidth = texture->Source.IsValid() ? texture->Source.GetSizeX() : texture->GetSurfaceWidth();
        uint32 textureFullHeight = texture->Source.IsValid() ? texture->Source.GetSizeY() : texture->GetSurfaceHeight();
        FVector2D viewportPoint(iViewport->GetMouseX(), iViewport->GetMouseY());
        FVector2D hudPoint = viewportWidget->ToLocal(viewportPoint) +  FVector2D(textureFullWidth / 2.f, textureFullHeight / 2.f);
        mCurrentHUDPoint = FOdysseyPoint(hudPoint.X, hudPoint.Y);
        mHoveredHUDElement->OnMouseHover(mCurrentHUDPoint);
    }

    TArray<FKey> pressedKeys = mKeysPressed;
    if (FOdysseyKeyState::GetLastKey() != FKey())
        pressedKeys.AddUnique(FOdysseyKeyState::GetLastKey());

    //Point In Viewport
    FOdysseyPoint pointInViewport = mCurrentPointInViewport;
    pointInViewport.x = iX;
    pointInViewport.y = iY;
    pointInViewport.keysDown = pressedKeys;
    pointInViewport.ComputeRelativeParameters(mCurrentPointInViewport);
    mCurrentPointInViewport = pointInViewport;

    //Point In Texture
    FOdysseyPoint pointInTexture = GetLocalMousePosition(mCurrentPointInViewport);
    pointInTexture.keysDown = pressedKeys;
    pointInTexture.ComputeRelativeParameters(mCurrentPointInTexture);
	bool hasMoved = !FMath::IsNearlyEqual(mCurrentPointInTexture.x - pointInTexture.x, 0.f) || !FMath::IsNearlyEqual(mCurrentPointInTexture.y - pointInTexture.y, 0.f);
    mCurrentPointInTexture = pointInTexture;

    if (mCurrentToolState == eState::kIdle)
    {
        if (!hasMoved)
            return;

        mOnMouseHover.ExecuteIfBound(mCurrentPointInTexture);
    }
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------ Input Functions----

void
FOdysseyPainterEditorViewportClient::MouseDown(const FOdysseyPoint& iPoint)
{
    if (mIsMouseDown)
        return;
        
    mIsMouseDown = true;

    //If we don't have a surface, then we don't interact with anything
    UTexture* texture = mOdysseyPainterEditorViewportPtr.Pin()->GetTexture();
    if (!texture)
        return;

    TArray<FKey> pressedKeys = mKeysPressed;
    if (FOdysseyKeyState::GetLastKey() != FKey())
        pressedKeys.AddUnique(FOdysseyKeyState::GetLastKey());

    //Point In Viewport
    FOdysseyPoint pointInViewport = iPoint;
    pointInViewport.keysDown = pressedKeys;
    pointInViewport.ComputeRelativeParameters(mCurrentPointInViewport);
    mCurrentPointInViewport = pointInViewport;

    //Point In Texture
    FOdysseyPoint pointInTexture = GetLocalMousePosition(mCurrentPointInViewport);
    pointInTexture.keysDown = pressedKeys;
    pointInTexture.ComputeRelativeParameters(mCurrentPointInTexture);
    mCurrentPointInTexture = pointInTexture;

    if( mCurrentToolState == eState::kIdle )
    {
        if (mOnMouseDown.IsBound())
            mOnMouseDown.Execute(mCurrentPointInTexture, mMouseButton);
    }
    else if( mCurrentToolState == eState::kRotate )
    {
        FIntPoint size = mOdysseyPainterEditorViewportPtr.Pin()->GetViewport()->GetSizeXY();
        FVector2D center = FVector2D( size.X / 2, size.Y / 2 );
        FVector2D position_in_viewport( iPoint.x, iPoint.y );
        FVector2D deltaCenter = position_in_viewport - center;
        mRotationReference = FMath::Atan2( -deltaCenter.Y, deltaCenter.X );
    }
    else if( mCurrentToolState == eState::kPanZoom )
    {
        if (mMouseButton == EKeys::LeftMouseButton)
        {
            mCurrentToolState = eState::kPan;
            mPanReference = FVector2D( iPoint.x, iPoint.y );
        }
        else if (mMouseButton == EKeys::RightMouseButton)
        {
            mCurrentToolState = eState::kZoom;

            uint32 width;
            uint32 height;
            mOdysseyPainterEditorViewportPtr.Pin()->ComputeTextureDisplayDimensions(width, height);

            double zoom = mOdysseyPainterEditorViewportPtr.Pin()->GetZoom();
            mZoomReference = ::FMath::Loge(zoom);
            mZoomViewportPointReference = FVector2D(iPoint.x, iPoint.y);
        }
    }
    else if( mCurrentToolState == eState::kPick && mMouseButton == EKeys::LeftMouseButton )
    {
        FOdysseyPoint strokePoint_in_texture = GetLocalMousePosition(iPoint);
        FVector2D position_in_texture(strokePoint_in_texture.x, strokePoint_in_texture.y );
        mOnPickColor.ExecuteIfBound(eOdysseyEventState::kAdjust, position_in_texture);
    }
}

void
FOdysseyPainterEditorViewportClient::MouseUp(const FOdysseyPoint& iPoint)
{
    if (!mIsMouseDown)
        return;

    mIsMouseDown = false;

    //If we don't have a surface, then we don't interact with anything
    UTexture* texture = mOdysseyPainterEditorViewportPtr.Pin()->GetTexture();
    if (!texture)
        return;

    TArray<FKey> pressedKeys = mKeysPressed;
    if (FOdysseyKeyState::GetLastKey() != FKey())
        pressedKeys.AddUnique(FOdysseyKeyState::GetLastKey());

    //Point In Viewport
    FOdysseyPoint pointInViewport = iPoint;
    pointInViewport.keysDown = pressedKeys;
    pointInViewport.ComputeRelativeParameters(mCurrentPointInViewport);
    mCurrentPointInViewport = pointInViewport;

    //Point In Texture
    FOdysseyPoint pointInTexture = GetLocalMousePosition(mCurrentPointInViewport);
    pointInTexture.keysDown = pressedKeys;
    pointInTexture.ComputeRelativeParameters(mCurrentPointInTexture);
    mCurrentPointInTexture = pointInTexture;

    if( mCurrentToolState == eState::kIdle)
    {    
        if (mOnMouseUp.IsBound())
            mOnMouseUp.Execute(mCurrentPointInTexture, mMouseButton);
    }
    else if( mCurrentToolState == eState::kRotate )
    {
    }
    else if( mCurrentToolState == eState::kPan && mMouseButton == EKeys::LeftMouseButton )
    {
    }
    else if (mCurrentToolState == eState::kZoom && mMouseButton == EKeys::LeftMouseButton)
    {
    }
    else if( mCurrentToolState == eState::kPick && mMouseButton == EKeys::LeftMouseButton )
    {
        FOdysseyPoint strokePoint_in_texture = GetLocalMousePosition(iPoint);
        FVector2D position_in_texture(strokePoint_in_texture.x, strokePoint_in_texture.y);
        mOnPickColor.ExecuteIfBound(eOdysseyEventState::kSet, position_in_texture);
    }
}

void
FOdysseyPainterEditorViewportClient::MouseDrag(const FOdysseyPoint& iPoint)
{
    if (!mIsMouseDown)
        return;

    //If we don't have a surface, then we don't interact with anything
    UTexture* texture = mOdysseyPainterEditorViewportPtr.Pin()->GetTexture();
    if (!texture)
        return;

    TArray<FKey> pressedKeys = mKeysPressed;
    if (FOdysseyKeyState::GetLastKey() != FKey())
        pressedKeys.AddUnique(FOdysseyKeyState::GetLastKey());

    //Point In Viewport
    FOdysseyPoint pointInViewport = iPoint;
    pointInViewport.keysDown = pressedKeys;
    pointInViewport.ComputeRelativeParameters(mCurrentPointInViewport);
    mCurrentPointInViewport = pointInViewport;

    //Point In Texture
    FOdysseyPoint pointInTexture = GetLocalMousePosition(mCurrentPointInViewport);
    pointInTexture.keysDown = pressedKeys;
    pointInTexture.ComputeRelativeParameters(mCurrentPointInTexture);
    bool hasMoved = !FMath::IsNearlyEqual(mCurrentPointInTexture.x - pointInTexture.x, 0.f) || !FMath::IsNearlyEqual(mCurrentPointInTexture.y - pointInTexture.y, 0.f);
    mCurrentPointInTexture = pointInTexture;

    if( mCurrentToolState == eState::kIdle )
    {
		if (!hasMoved)
			return;

        mOnMouseDrag.ExecuteIfBound(mCurrentPointInTexture);
    }
    else if( mCurrentToolState == eState::kPan)
    {
        FVector2D deltaReference( iPoint.x - mPanReference.X, iPoint.y - mPanReference.Y );

        mOdysseyPainterEditorViewportPtr.Pin()->AddPan( deltaReference );
        mPanReference = FVector2D( iPoint.x, iPoint.y );
    }
    else if( mCurrentToolState == eState::kRotate)
    {
        FIntPoint size = mOdysseyPainterEditorViewportPtr.Pin()->GetViewport()->GetSizeXY();

        FVector2D center = FVector2D( size.X / 2, size.Y / 2 );
        FVector2D position_in_viewport( iPoint.x, iPoint.y );
        FVector2D deltaCenter = position_in_viewport - center;
        float newRotation = FMath::Atan2( -deltaCenter.Y, deltaCenter.X );
        float deltaRotation = mRotationReference - newRotation;

        mOdysseyPainterEditorViewportPtr.Pin()->SetRotation( mOdysseyPainterEditorViewportPtr.Pin()->GetRotation() + deltaRotation );

        mRotationReference = newRotation;
    }
    else if (mCurrentToolState == eState::kZoom)
    {   
        TSharedPtr<SOdysseyViewport> viewportWidget = mOdysseyPainterEditorViewportPtr.Pin();

        FVector2D viewportMousePosition(iPoint.x, iPoint.y);
        float dist = viewportMousePosition.X - mZoomViewportPointReference.X;
        float smoothness = 200.f; //TODO: do a Setting to let the user change it at will

        if (dist > KINDA_SMALL_NUMBER || dist < KINDA_SMALL_NUMBER)
        {
            float newDist = mZoomReference + (dist / smoothness);
            float zoom = ::FMath::Exp(newDist);
            viewportWidget->SetZoom(zoom, mZoomViewportPointReference - mOdysseyPainterEditorViewportPtr.Pin()->GetViewportCenter());
        }
    }
    else if( mCurrentToolState == eState::kPick)
    {
        FOdysseyPoint strokePoint_in_texture = GetLocalMousePosition(iPoint);
        FVector2D position_in_texture(strokePoint_in_texture.x, strokePoint_in_texture.y);
    
        uint32 textureFullWidth = texture->Source.IsValid() ? texture->Source.GetSizeX() : texture->GetSurfaceWidth();
        uint32 textureFullHeight = texture->Source.IsValid() ? texture->Source.GetSizeY() : texture->GetSurfaceHeight();

        if( position_in_texture.X >= 0 && position_in_texture.X < textureFullWidth &&
            position_in_texture.Y >= 0 && position_in_texture.Y < textureFullHeight)
        {
			mOnPickColor.ExecuteIfBound(eOdysseyEventState::kAdjust, position_in_texture);
        }
    }
}

bool
FOdysseyPainterEditorViewportClient::KeyDown(FKey iKey)
{
    //KeyDown
    if (mOnKeyDown.IsBound() && mOnKeyDown.Execute(iKey))
        return true;

    if (iKey == EKeys::MouseScrollUp)
    {
        //ZoomIn
        FVector2D position_in_viewport(mCurrentPointInViewport.x, mCurrentPointInViewport.y);
        ZoomInInViewport(position_in_viewport);
    }
    else if (iKey == EKeys::MouseScrollDown)
    {
        //ZoomOut
        FVector2D position_in_viewport(mCurrentPointInViewport.x, mCurrentPointInViewport.y);
        ZoomOutInViewport(position_in_viewport);
    }

    return false;
}

bool
FOdysseyPainterEditorViewportClient::KeyUp(FKey iKey)
{
    return mOnKeyUp.IsBound() && mOnKeyUp.Execute(iKey);
}


//--------------------------------------------------------------------------------------
//------------------------------------------------------------------ Stylus Functions

void
FOdysseyPainterEditorViewportClient::StartStylusInputRecord()
{
    if (mIsRecordingStylus)
        return;

    auto end_time = std::chrono::steady_clock::now();
    auto delta = std::chrono::duration_cast<std::chrono::milliseconds>( end_time - mStylusLastEventTime).count();
    if(delta > 500 )
        return;

    //Down
    ReadStylusInput();
    mIsRecordingStylus = true;
}

void
FOdysseyPainterEditorViewportClient::StopStylusInputRecord()
{
    if (!mIsRecordingStylus)
        return;

    ReadStylusInput();

    InputSubsystem = GEditor->GetEditorSubsystem<UOdysseyStylusInputSubsystem>();
    InputSubsystem->Flush(); //Get late stylus events

    //UP
    mIsRecordingStylus = false;
}

FOdysseyPoint
FOdysseyPainterEditorViewportClient::StylusStateToPoint(const FStylusState& iState)
{
    TSharedPtr<SOdysseyViewport> odysseyViewportWidget = mOdysseyPainterEditorViewportPtr.Pin();
    if (!odysseyViewportWidget)
        return FOdysseyPoint();

    TSharedPtr< SViewport > viewportWidget = odysseyViewportWidget->GetViewportWidget();
    TSharedPtr<FOdysseySceneViewport> viewport = odysseyViewportWidget->GetViewport();
    
    float scale_dpi = viewport->GetCachedGeometry().GetAccumulatedLayoutTransform().GetScale();
    FVector2D position_in_viewport = viewportWidget->GetCachedGeometry().AbsoluteToLocal( iState.GetPosition() ) * scale_dpi;

    FOdysseyPoint point( position_in_viewport.X
                                        , position_in_viewport.Y
                                        , iState.GetZ()
                                        , iState.GetPressure()
                                        , iState.GetTimer()
                                        , iState.GetAltitude()
                                        , iState.GetAzimuth()
                                        , iState.GetTwist()
                                        , 0 //iState.GetPitch()
                                        , 0 // iState.GetRoll()
                                        , 0 ); // iState.GetYaw() );

    TArray<FKey> pressedKeys = mKeysPressed;
    pressedKeys.AddUnique(FOdysseyKeyState::GetLastKey());
    point.keysDown = pressedKeys;

    return point;
}

void
FOdysseyPainterEditorViewportClient::ReadStylusInput()
{
    if (!mIsRecordingStylus)
        return;

    for (int i = mLastStylusEventIndex; i < mStylusStates.Num(); i++ )
    {
        const FStylusState& state = mStylusStates[i];

        FOdysseyPoint point = StylusStateToPoint(state);
        
        //Force MouseDown when using the Right Mouse Button to allow hovered mouse clicks
        if (!mStylusIsDown && (state.IsStylusDown() || mMouseButton == EKeys::RightMouseButton ))
        {
            //MouseDown
            MouseDown(point);
            mStylusIsDown = true;
            mLastStylusEventIndex = i;
        }
        else if (mStylusIsDown && !state.IsStylusDown() && !mKeysPressed.Contains(mMouseButton))
        {
            //MouseUp
            MouseUp(point);
            mStylusIsDown = false;
            mLastStylusEventIndex = i;
        }
        else if (mStylusIsDown)
        {
            //MouseMove
            MouseDrag(point);
            mLastStylusEventIndex = i;
            //CapturedMouseMoveWithStrokePoint( point );
        }
    }
}

void
FOdysseyPainterEditorViewportClient::OnStylusStateChanged( const TWeakPtr<SWidget> iWidget, const TArray<FStylusState>& iStates, int32 iIndex )
{
    mStylusLastEventTime = std::chrono::steady_clock::now();

    TSharedPtr<SOdysseyViewport> odysseyViewportWidget = mOdysseyPainterEditorViewportPtr.Pin();
    if (!odysseyViewportWidget)
        return;

    //If we don't have a surface, then we don't interact with anything
    UTexture* texture = odysseyViewportWidget->GetTexture();
    if (!texture)
        return;

    TSharedPtr<SWidget> inWidget = iWidget.Pin();
    if( !inWidget)
        return;

    TSharedPtr< SViewport > viewport = odysseyViewportWidget->GetViewportWidget();
    if( inWidget != viewport )
        return;

    //---

    mStylusStates = iStates;
    mLastStylusEventIndex = 0;

    ReadStylusInput();
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------ Internal Input Functions

FOdysseyPainterEditorViewportClient::eState
FOdysseyPainterEditorViewportClient::InputChordToState()
{
    if (FOdysseyKeyState::GetLastKey() == FKey())
        return eState::kIdle;

    FModifierKeysState ModifierKeysState = FSlateApplication::Get().GetModifierKeys();
    const FInputChord activeChord(FOdysseyKeyState::GetLastKey(),
        EModifierKey::FromBools(
            ModifierKeysState.IsControlDown(),
            ModifierKeysState.IsAltDown(),
            ModifierKeysState.IsShiftDown(),
            ModifierKeysState.IsCommandDown()
        )
    );

    if (FOdysseyPainterEditorCommands::Get().PanZoomViewport->HasActiveChord(activeChord))
    {
        return mIsMouseDown ? mCurrentToolState : eState::kPanZoom;
    }
    else if (FOdysseyPainterEditorCommands::Get().RotateViewport->HasActiveChord(activeChord))
    {
        return eState::kRotate;
    }
    else if (FOdysseyPainterEditorCommands::Get().PickColorInViewport->HasActiveChord(activeChord))
    {
        return eState::kPick;
    }
    return eState::kIdle;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------ FGCObject API
void
FOdysseyPainterEditorViewportClient::AddReferencedObjects( FReferenceCollector& ioCollector )
{
    ioCollector.AddReferencedObject( mCheckerboardTexture );
}

FString FOdysseyPainterEditorViewportClient::GetReferencerName() const
{
    return TEXT("FOdysseyPainterEditorViewportClient");
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------- Private API

void
FOdysseyPainterEditorViewportClient::CreateCheckerboardTexture()
{
    DestroyCheckerboardTexture();

    const UOdysseyPainterEditorSettings& settings = *GetDefault< UOdysseyPainterEditorSettings >();
    mCheckerboardTexture = FImageUtils::CreateCheckerboardTexture( settings.CheckerColorOne, settings.CheckerColorTwo, settings.CheckerSize );
}

void
FOdysseyPainterEditorViewportClient::DestroyCheckerboardTexture()
{
    if( mCheckerboardTexture )
    {
        if( mCheckerboardTexture->GetResource() )
        {
            mCheckerboardTexture->ReleaseResource();
        }
        mCheckerboardTexture->MarkAsGarbage();
        mCheckerboardTexture = NULL;
    }
}

void
FOdysseyPainterEditorViewportClient::ZoomInInViewport( const FVector2D& iPositionInViewport )
{
    TSharedPtr<SOdysseyViewport> viewportWidget = mOdysseyPainterEditorViewportPtr.Pin();

    //If we don't have a surface, then we don't have a local mouse position
    UTexture* texture = mOdysseyPainterEditorViewportPtr.Pin()->GetTexture();
    if (!texture)
        return;

    int textureWidth = texture->GetSurfaceWidth();
    int textureHeight = texture->GetSurfaceHeight();

    FVector2D pos = iPositionInViewport - viewportWidget->GetViewportCenter();

    viewportWidget->ZoomExponential(viewportWidget->GetZoom(), 0.1, pos);
}

void
FOdysseyPainterEditorViewportClient::ZoomOutInViewport( const FVector2D& iPositionInViewport )
{
    TSharedPtr<SOdysseyViewport> viewportWidget = mOdysseyPainterEditorViewportPtr.Pin();

    //If we don't have a surface, then we don't have a local mouse position
    UTexture* texture = mOdysseyPainterEditorViewportPtr.Pin()->GetTexture();
    if (!texture)
        return;

    int textureWidth = texture->GetSurfaceWidth();
    int textureHeight = texture->GetSurfaceHeight();

    FVector2D pos = iPositionInViewport - viewportWidget->GetViewportCenter();

    viewportWidget->ZoomExponential(viewportWidget->GetZoom(), -0.1, pos);
}

double
FOdysseyPainterEditorViewportClient::GetZoom() const
{
    return mOdysseyPainterEditorViewportPtr.Pin()->GetZoom();
}

FVector2D
FOdysseyPainterEditorViewportClient::GetLocalMousePosition( const FVector2D& iMouseInViewport ) const
{
    return mOdysseyPainterEditorViewportPtr.Pin()->GetTransformToDisplayedTexture().Inverse().TransformPoint(iMouseInViewport);
}

FOdysseyPoint
FOdysseyPainterEditorViewportClient::GetLocalMousePosition( const FOdysseyPoint& iPointInViewport ) const
{
    FVector2D position_in_viewport(iPointInViewport.x, iPointInViewport.y);
    FVector2D position_in_texture = mOdysseyPainterEditorViewportPtr.Pin()->GetTransformToSourceTexture().Inverse().TransformPoint(position_in_viewport);

    FOdysseyPoint point_in_texture(iPointInViewport);
    point_in_texture.x = position_in_texture.X;
    point_in_texture.y = position_in_texture.Y;

    return point_in_texture;
}

void 
FOdysseyPainterEditorViewportClient::DrawUVsOntoViewport( const FViewport* iViewport, FCanvas* ioCanvas, int32 iUVChannel, const FStaticMeshVertexBuffer& iVertexBuffer, const FIndexArrayView& iIndices )
{
    UTexture* texture = mOdysseyPainterEditorViewportPtr.Pin()->GetTexture();
    if (!texture)
        return;

    if (((uint32)iUVChannel < iVertexBuffer.GetNumTexCoords()))
    {
        TSharedPtr<SOdysseyViewport> viewportWidget = mOdysseyPainterEditorViewportPtr.Pin();

        uint32 width, height;
        viewportWidget->ComputeTextureDisplayDimensions(width, height);

        FVector vp1(0.f, 0.f, 0.f);
        FVector vp2(iViewport->GetSizeXY().X, 0.f, 0.f);
        FVector vp3(iViewport->GetSizeXY().X, iViewport->GetSizeXY().Y, 0.f);
        FVector vp4(0.f, iViewport->GetSizeXY().Y, 0.f);        

        FVector2D textureSurfaceSize(texture->GetSurfaceWidth(), texture->GetSurfaceHeight());
        FTransform2D transform = mOdysseyPainterEditorViewportPtr.Pin()->GetTransformToDisplayedTexture();

        //draw triangles
        uint32 numIndices = iIndices.Num();
        for (uint32 i = 0; i < numIndices - 2; i += 3)
        {
            FVector2D UVs[3];
            bool isOutOfBounds[3];

            float fudgeFactor = 1.0f / 1024.0f;
            for (int32 Corner = 0; Corner < 3; Corner++)
            {
                UVs[Corner] = (FVector2D)(iVertexBuffer.GetVertexUV(iIndices[i + Corner], iUVChannel));

                isOutOfBounds[Corner] = (UVs[Corner].X < -fudgeFactor || UVs[Corner].X >(1.0f + fudgeFactor)) || (UVs[Corner].Y < -fudgeFactor || UVs[Corner].Y >(1.0f + fudgeFactor));
            }

            // Clamp the UV triangle to the [0,1] range (with some fudge).
            int32 numUVs = 3;
            fudgeFactor = 0.1f;
            FVector2D bias(0.0f, 0.0f);

            float minU = UVs[0].X;
            float minV = UVs[0].Y;
            for (int32 j = 1; j < numUVs; ++j)
            {
                minU = FMath::Min(minU, UVs[j].X);
                minV = FMath::Min(minU, UVs[j].Y);
            }

            if (minU < -fudgeFactor || minU >(1.0f + fudgeFactor))
            {
                bias.X = FMath::FloorToFloat(minU);
            }
            if (minV < -fudgeFactor || minV >(1.0f + fudgeFactor))
            {
                bias.Y = FMath::FloorToFloat(minV);
            }

            for (int32 j = 0; j < numUVs; j++)
            {
                UVs[j] += bias;
            }

            for (int32 edge = 0; edge < 3; edge++)
            {
                int32 corner1 = edge;
                int32 corner2 = (edge + 1) % 3;
                FLinearColor lc = mMeshSelector->GetMeshColor();
                ::ULIS::FColor c = ::ULIS::FColor::FromRGBAF( lc.R, lc.G, lc.B, lc.A );

                FLinearColor color = ( isOutOfBounds[corner1] || isOutOfBounds[corner2] ) ? FLinearColor( 0.6f, 0.0f, 0.0f ) : FLinearColor( c.RedF(), c.GreenF(), c.BlueF(), c.AlphaF() );

                FVector pIntersect;
                FVector2D p1 = transform.TransformPoint(UVs[corner1] * textureSurfaceSize);
                FVector2D p2 = transform.TransformPoint(UVs[corner2] * textureSurfaceSize);

                #define V(p) FVector(p, 0.f)
                bool intersect = FMath::SegmentIntersection2D(V(p1), V(p2), vp1, vp2, pIntersect) ||
                                 FMath::SegmentIntersection2D(V(p1), V(p2), vp2, vp3, pIntersect) ||
                                 FMath::SegmentIntersection2D(V(p1), V(p2), vp3, vp4, pIntersect) ||
                                 FMath::SegmentIntersection2D(V(p1), V(p2), vp4, vp1, pIntersect);
                #undef V

                //Clipping. We don't even begin to calculate the coordinates of points we know won't be visible in the viewport
                if ( !intersect &&
                     !(p1.X >= vp1.X && p1.X < vp3.X && p1.Y >= vp1.Y && p1.Y < vp3.Y) &&
                     !(p2.X >= vp1.X && p2.X < vp3.X && p2.Y >= vp1.Y && p2.Y < vp3.Y) )
                {
                    continue;
                }

                FVector origin = FVector(p1, 0.f);
                FVector endPos = FVector(p2, 0.f);

                FBatchedElements* batchedElements = ioCanvas->GetBatchedElements(FCanvas::ET_Line);
                FHitProxyId hitProxyId = ioCanvas->GetHitProxyId();
                batchedElements->AddTranslucentLine(origin, endPos, color, hitProxyId, 1.f);
            }
        }
    }
}

TSharedPtr<FOdysseyHUDElement>
FOdysseyPainterEditorViewportClient::GetHUDElement(FViewport* iViewport, int32 iX, int32 iY)
{
    HOdysseyHUDElementHitProxy* hitproxy = HitProxyCast<HOdysseyHUDElementHitProxy>(iViewport->GetHitProxy(iX, iY));
    if (!hitproxy)
        return nullptr;
    
    return hitproxy->HUDElement();
}

#undef LOCTEXT_NAMESPACE
