// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorViewportClient.h"

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
#include "FOdysseySceneViewport.h"
#include "ImageUtils.h"
#include "RawMesh.h"
#include "RendererInterface.h"
#include "StaticMeshResources.h"
#include "Texture2DPreview.h"
#include "TextureResource.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "UnrealEdGlobals.h"
#include "VolumeTexturePreview.h"
#include "Widgets/Layout/SScrollBar.h"
#include "Widgets/SViewport.h"

#include "IOdysseyStylusInputModule.h"
#include "OdysseyPaintEngine.h"
#include "OdysseyHUDElement.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSettings.h"
#include "Mesh/FOdysseyMeshSelector.h"
#include "OdysseySurface.h"
#include "SOdysseyCursorWidget.h"
#include "SOdysseyViewport.h"
#include "OdysseyPainterEditorCommands.h"
#include "OdysseyKeyState.h"
#include "MouseDeltaTracker.h"
#include "OdysseyStylusInputSettings.h"
#include "OdysseyViewportCommands.h"
#include "StylusInputTabletContext.h"
#include "Tools/OdysseyPainterEditorTool.h"

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
    UOdysseyPainterEditorSettings* settings = GetMutableDefault<UOdysseyPainterEditorSettings>();
    settings->GetOnCheckerSizeChanged().RemoveAll( this );
    settings->GetOnCheckerColorChanged().RemoveAll( this );

    DestroyCheckerboardTexture();
}

FOdysseyPainterEditorViewportClient::FOdysseyPainterEditorViewportClient( FOdysseyPainterEditor*                    iOdysseyPainterEditor,
                                                                          TWeakPtr< SOdysseyViewport >              iOdysseyPainterEditorViewport,
                                                                          FOdysseyMeshSelector*                     iMeshSelector)
    : mOdysseyPainterEditor(iOdysseyPainterEditor)
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

    ENQUEUE_RENDER_COMMAND(InitOdysseyPainterEditorViewportClientTextures)(
        [&](FRHICommandListImmediate& RHICmdList)
        {
            FSamplerStateInitializerRHI SamplerStateInitializerNN(SF_Point , AM_Clamp, AM_Clamp, AM_Clamp);
            mNearestNeighbourTexture.SamplerStateRHI = RHICreateSamplerState(SamplerStateInitializerNN);
            FSamplerStateInitializerRHI SamplerStateInitializerB(SF_Trilinear , AM_Clamp, AM_Clamp, AM_Clamp);
            mBilinearTexture.SamplerStateRHI = RHICreateSamplerState(SamplerStateInitializerB);
        }
    );

    FRenderCommandFence fence;
    fence.BeginFence();
    fence.Wait();

    UOdysseyPainterEditorSettings* settings = GetMutableDefault<UOdysseyPainterEditorSettings>();
    settings->GetOnCheckerColorChanged().AddRaw( this, &FOdysseyPainterEditorViewportClient::CreateCheckerboardTexture );
    settings->GetOnCheckerSizeChanged().AddRaw( this, &FOdysseyPainterEditorViewportClient::CreateCheckerboardTexture );

    CreateCheckerboardTexture();
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------ Draw Viewport
void
FOdysseyPainterEditorViewportClient::Draw( FViewport* iViewport, FCanvas* ioCanvas )
{
    RegisterWindow(mOdysseyPainterEditorViewportPtr.Pin().ToSharedRef());

    const UOdysseyPainterEditorSettings& settings = *GetDefault<UOdysseyPainterEditorSettings>();
    ioCanvas->Clear(settings.GetBackgroundColor());

    UTexture* texture       = mOdysseyPainterEditorViewportPtr.Pin()->GetTexture();
    if (!texture)
        return;

    if (!texture->GetResource())
        return;

    // Fully stream in the texture before drawing it.
    texture->BlockOnAnyAsyncBuild();
    texture->SetForceMipLevelsToBeResident( 30.0f );
    texture->WaitForStreaming();

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

    // Figure out the size we need
    const float mipLevel = 0;

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
                    RenderModule.RequestVirtualTextureTiles(AllocatedVT, ScreenSpaceSize, ViewportPositon, ViewportSize, UV0, UV1, 0);
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

    FOdysseyHUDElement::FDrawHUDParams params;
    params.mCanvas = ioCanvas;
    params.mTextureWidth = texture->GetSurfaceWidth();
    params.mTextureHeight = texture->GetSurfaceHeight();

    params.mTextureToHUD = FOdysseyHUDElement::FDrawHUDParams::FTextureToHUD::CreateLambda(
        [this, w = params.mTextureWidth, h = params.mTextureHeight](const FVector2D& iPosition)
        {
            TSharedPtr<SOdysseyViewport> viewportWidget = mOdysseyPainterEditorViewportPtr.Pin();
            if (!viewportWidget)
                return FVector2D(0, 0);
            return viewportWidget->ToWorld(iPosition - FVector2D(w / 2.f, h / 2.f));
        }
    );
    params.mHUDToTexture = FOdysseyHUDElement::FDrawHUDParams::FTextureToHUD::CreateLambda(
        [this, w = params.mTextureWidth, h = params.mTextureHeight](const FVector2D& iPosition)
        {
            TSharedPtr<SOdysseyViewport> viewportWidget = mOdysseyPainterEditorViewportPtr.Pin();
            if (!viewportWidget)
                return FVector2D(0, 0);
            return viewportWidget->ToLocal(iPosition) + FVector2D(w / 2.f, h / 2.f);
        }
    );

    mOdysseyPainterEditor->HUDSystem()->Draw(params);
}

EMouseCursor::Type
FOdysseyPainterEditorViewportClient::GetCursor( FViewport* iViewport, int32 iX, int32 iY )
{
    if( mCurrentToolState == eState::kPan || mCurrentToolState == eState::kPanZoom || mCurrentToolState == eState::kZoom )
        mCurrentMouseCursor = EMouseCursor::GrabHand;
    else if( mOdysseyPainterEditor->GetCurrentTool() )
    {
        return mOdysseyPainterEditor->GetCurrentTool()->GetMouseCursor().GetMouseCursorNative();
    }

    return mCurrentMouseCursor;
}

TOptional< TSharedRef< SWidget > >
FOdysseyPainterEditorViewportClient::MapCursor( FViewport* iViewport, const FCursorReply& iCursorReply )
{
    // - The widget MUST BE retain by the object (should be in a class variable instead of static)
    // - When using SWidget, there is a really small (but perceptible) lag -_-
    //
    // So don't use SWidget as cursor.
    // This is just for information.
    //
    //static TSharedRef< SWidget > widget =
    //    SNew( SBox )
    //    .WidthOverride( 10 )
    //    .HeightOverride( 10 )
    //    [
    //        SNew( SBorder )
    //            .BorderImage( FAppStyle::GetBrush( "WhiteBrush" ) )
    //            .BorderBackgroundColor( FSlateColor( FLinearColor( 255, 0, 0, 255 ) ) )
    //    ];
    //return widget;

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
FOdysseyPainterEditorViewportClient::InputKey( const FInputKeyEventArgs& iEventArgs )
{
    TSharedPtr<SOdysseyViewport> viewportWidget = mOdysseyPainterEditorViewportPtr.Pin();
    UTexture* texture       = viewportWidget->GetTexture();
    if (!texture)
        return false;
    //Here you receive Mouse Buttons and Keyboard keys events
    //UE_LOG(LogTemp, Warning, TEXT("UE InputKey %s %s"), *iKey.ToString(), iEvent == EInputEvent::IE_Pressed ? TEXT("PRESSED") : iEvent == EInputEvent::IE_Released ? TEXT("RELEASED") : TEXT("OTHER"));

    // Don't manage the repeat event in the viewport client (return false)
    // Otherwise it takes priority over the shortcuts which may use the repeat flag
    // And then, the shortcut doesn't activate its repeat mode
    if( iEventArgs.Event == EInputEvent::IE_Repeat )
        return false;

    if( iEventArgs.Event == EInputEvent::IE_Pressed )
    {
        //key already pressed, don't send a KeyDown or MouseDown twice
        //Can happen on windows with some touch options
        if (mKeysPressed.Contains( iEventArgs.Key ))
            return true;

        mKeysPressed.Add( iEventArgs.Key );
    }
    else if( iEventArgs.Event == EInputEvent::IE_Released )
    {
        //key already released, don't send a KeyUp or MouseUp twice
        //Can happen on windows with some touch options
        if (!mKeysPressed.Contains( iEventArgs.Key ))
            return true;

        mKeysPressed.Remove( iEventArgs.Key );
    }
    else if( iEventArgs.Event == EInputEvent::IE_DoubleClick )
    {
        //HUD
        TSharedPtr<FOdysseyHUDElement> hudElement = GetHUDElement( iEventArgs.Viewport, iEventArgs.Viewport->GetMouseX(), iEventArgs.Viewport->GetMouseY());
        if (hudElement && hudElement->OnMouseDoubleClick(mCurrentHUDPoint, iEventArgs.Key ))
            return true;

        bool ignoreDown = mOnMouseDoubleClick.IsBound() && mOnMouseDoubleClick.Execute(mCurrentPointInViewport, iEventArgs.Key );
        if (ignoreDown)
            return true;

        if (mKeysPressed.Contains( iEventArgs.Key ))
            return true;

        mKeysPressed.Add( iEventArgs.Key );
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
    if ( iEventArgs.Key == EKeys::LeftMouseButton || iEventArgs.Key == EKeys::RightMouseButton)
    {
        if ( iEventArgs.Event == EInputEvent::IE_Pressed || iEventArgs.Event == EInputEvent::IE_DoubleClick)
        {
            mCurrentHUDElement = GetHUDElement( iEventArgs.Viewport, iEventArgs.Viewport->GetMouseX(), iEventArgs.Viewport->GetMouseY());
            if (mCurrentHUDElement)
            {
                uint32 textureFullWidth = texture->Source.IsValid() ? texture->Source.GetSizeX() : texture->GetSurfaceWidth();
                uint32 textureFullHeight = texture->Source.IsValid() ? texture->Source.GetSizeY() : texture->GetSurfaceHeight();
                FVector2D viewportPoint( iEventArgs.Viewport->GetMouseX(), iEventArgs.Viewport->GetMouseY());
                mHUDMouseDownReference = viewportPoint;
                FVector2D hudPoint = viewportWidget->ToLocal(viewportPoint) +  FVector2D(textureFullWidth / 2.f, textureFullHeight / 2.f);

                mCurrentHUDPoint = FOdysseyPoint(hudPoint.X, hudPoint.Y);
                mCurrentHUDPoint.keysDown = mKeysPressed;

                if (mCurrentHUDElement->OnMouseDown(mCurrentHUDPoint, iEventArgs.Key))
                    return true;
            }
        }
        else if( iEventArgs.Event == EInputEvent::IE_Released)
        {
            if (mCurrentHUDElement)
            {
                uint32 textureFullWidth = texture->Source.IsValid() ? texture->Source.GetSizeX() : texture->GetSurfaceWidth();
                uint32 textureFullHeight = texture->Source.IsValid() ? texture->Source.GetSizeY() : texture->GetSurfaceHeight();
                FVector2D viewportPoint( iEventArgs.Viewport->GetMouseX(), iEventArgs.Viewport->GetMouseY());

                float deltaX = viewportPoint.X - mHUDMouseDownReference.X;
                float deltaY = viewportPoint.Y - mHUDMouseDownReference.Y;
                float deltaSquared = deltaX * deltaX + deltaY * deltaY;
                bool bNoMouseMovement = deltaSquared < MOUSE_CLICK_DRAG_DELTA;

                FVector2D hudPoint = viewportWidget->ToLocal(viewportPoint) +  FVector2D(textureFullWidth / 2.f, textureFullHeight / 2.f);
                mCurrentHUDPoint = FOdysseyPoint(hudPoint.X, hudPoint.Y);

                if (bNoMouseMovement)
                    mCurrentHUDElement->OnMouseClick(mCurrentHUDPoint, iEventArgs.Key );

                bool handled = mCurrentHUDElement->OnMouseUp(mCurrentHUDPoint, iEventArgs.Key );
                mCurrentHUDElement = nullptr;

                if( handled )
                    return true;
            }
        }
    }

    //---

    FOdysseyPoint point_in_viewport( FOdysseyPoint::DefaultPoint() );
    point_in_viewport.x = iEventArgs.Viewport->GetMouseX();
    point_in_viewport.y = iEventArgs.Viewport->GetMouseY();

    if (!mIsMouseDown || mCurrentToolState != eState::kIdle )
        mCurrentToolState = InputChordToState();

    if (!mIsMouseDown && ( iEventArgs.Key == EKeys::LeftMouseButton || iEventArgs.Key == EKeys::RightMouseButton))
        mMouseButton = iEventArgs.Key;

    if ( iEventArgs.Key == EKeys::LeftMouseButton || iEventArgs.Key == EKeys::RightMouseButton)
    {
        if ( iEventArgs.Event == EInputEvent::IE_Pressed || iEventArgs.Event == EInputEvent::IE_DoubleClick)
        {
            StartStylusInputRecord();
            if (!mIsRecordingStylus)
            {
                MouseDown(point_in_viewport);
            }
            else
            {
                ReadStylusInput();
            }
        }
        else if( iEventArgs.Event == EInputEvent::IE_Released)
        {
            if (!mIsRecordingStylus)
            {
                MouseUp(point_in_viewport);
            }
            else
            {
                ReadStylusInput( eStylusEventFence::kStylusUp );
            }
        }
    }
    else
    {
        if ( iEventArgs.Event == EInputEvent::IE_Pressed)
        {
            return KeyDown( iEventArgs.Key );
        }
        else if( iEventArgs.Event == EInputEvent::IE_Released)
        {
            return KeyUp( iEventArgs.Key );
        }
    }

    return true;
}

void
FOdysseyPainterEditorViewportClient::CapturedMouseMove( FViewport* iViewport, int32 iX, int32 iY )
{
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
        FVector2D hudPointCoords = viewportWidget->ToLocal(viewportPoint) +  FVector2D(textureFullWidth / 2.f, textureFullHeight / 2.f);
        FOdysseyPoint hudPoint = FOdysseyPoint(hudPointCoords.X, hudPointCoords.Y);

        hudPoint.keysDown = mKeysPressed;
        hudPoint.ComputeRelativeParameters(mCurrentHUDPoint);

        mCurrentHUDPoint = hudPoint;

        mCurrentHUDElement->OnMouseDrag(mCurrentHUDPoint);
        return;
    }

    if (mIsRecordingStylus)
        return;

    FOdysseyPoint point_in_viewport(FOdysseyPoint::DefaultPoint());
    point_in_viewport.x = iX;
    point_in_viewport.y = iY;
    MouseDrag(point_in_viewport);
}

void
FOdysseyPainterEditorViewportClient::MouseEnter( FViewport* iViewport, int32 iX, int32 iY )
{
    if( mIsMouseDown )
        return;

    mIsFocused = true;
    ClearQueue();
    mCurrentToolState = eState::kIdle;
}

void
FOdysseyPainterEditorViewportClient::MouseLeave( FViewport* iViewport )
{
    if( mIsMouseDown )
        return;

    mIsFocused = false;
    mCurrentToolState = eState::kIdle;
}


void
FOdysseyPainterEditorViewportClient::MouseMove(FViewport* iViewport, int32 iX, int32 iY)
{
    mIsFocused = true;

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
        TSharedPtr<SOdysseyViewport> viewportWidget = mOdysseyPainterEditorViewportPtr.Pin();
        uint32 textureFullWidth = texture->Source.IsValid() ? texture->Source.GetSizeX() : texture->GetSurfaceWidth();
        uint32 textureFullHeight = texture->Source.IsValid() ? texture->Source.GetSizeY() : texture->GetSurfaceHeight();
        FVector2D viewportPoint(iViewport->GetMouseX(), iViewport->GetMouseY());
        FVector2D hudPointCoords = viewportWidget->ToLocal(viewportPoint) +  FVector2D(textureFullWidth / 2.f, textureFullHeight / 2.f);
        FOdysseyPoint hudPoint = FOdysseyPoint(hudPointCoords.X, hudPointCoords.Y);

        hudPoint.ComputeRelativeParameters(mCurrentHUDPoint);

        mCurrentHUDPoint = hudPoint;

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
        mMouseDownReference = mCurrentPointInViewport;
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

    float deltaX = iPoint.x - mMouseDownReference.x;
    float deltaY = iPoint.y - mMouseDownReference.y;
    float deltaSquared = deltaX * deltaX + deltaY * deltaY;
    bool bNoMouseMovement = deltaSquared < MOUSE_CLICK_DRAG_DELTA;

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
        if (bNoMouseMovement && mOnMouseClick.IsBound())
            mOnMouseClick.Execute(mCurrentPointInTexture, mMouseButton);

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

    if (!mKeysPressed.Contains(mMouseButton))
        mMouseButton = FKey();
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
    auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - mStylusLastEventTime).count();
    if (delta > 500)
        return;

    mIsRecordingStylus = true;
}

void
FOdysseyPainterEditorViewportClient::StopStylusInputRecord()
{
    if (!mIsRecordingStylus)
        return;

    ClearQueue();

    mIsRecordingStylus = false;
}

FOdysseyPoint FOdysseyPainterEditorViewportClient::StylusPacketToPoint(const UE::StylusInput::FStylusInputPacket& iPacket)
{
    TSharedPtr<SOdysseyViewport> odysseyViewportWidget = mOdysseyPainterEditorViewportPtr.Pin();
    if (!odysseyViewportWidget)
        return FOdysseyPoint();


    TSharedPtr< SViewport > viewportWidget = odysseyViewportWidget->GetViewportWidget();
    TSharedPtr<FOdysseySceneViewport> viewport = odysseyViewportWidget->GetViewport();


    if( !mStylusInputWindow.IsValid() )
        return FOdysseyPoint();

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

    float scale_dpi = viewport->GetCachedGeometry().GetAccumulatedLayoutTransform().GetScale();
    FVector2D position_in_viewport = viewportWidget->GetCachedGeometry().AbsoluteToLocal( packetPos ) * scale_dpi;
    position_in_viewport += Window->GetRectInScreen().GetTopLeft();

    const UE::StylusInput::IStylusInputTabletContext* tabletContext = GetTabletContext(mStylusInputInstance, iPacket.TabletContextID);
    FOdysseyPoint point;

#if PLATFORM_WINDOWS
    if( tabletContext )
    {
        UE::StylusInput::ETabletSupportedProperties capabilities = tabletContext->GetSupportedProperties();
        point.x =        EnumHasAnyFlags(capabilities, UE::StylusInput::ETabletSupportedProperties::X) ? position_in_viewport.X : 0.f;
        point.y =        EnumHasAnyFlags(capabilities, UE::StylusInput::ETabletSupportedProperties::Y) ? position_in_viewport.Y : 0.f;
        point.z =        EnumHasAnyFlags(capabilities, UE::StylusInput::ETabletSupportedProperties::Z) ? iPacket.Z : 0.f;
        point.pressure = EnumHasAnyFlags(capabilities, UE::StylusInput::ETabletSupportedProperties::NormalPressure) ? iPacket.NormalPressure : 1.f;
        point.time =     EnumHasAnyFlags(capabilities, UE::StylusInput::ETabletSupportedProperties::TimerTick) ? iPacket.TimerTick : 1.f;
        point.altitude = EnumHasAnyFlags(capabilities, UE::StylusInput::ETabletSupportedProperties::AltitudeOrientation) ? iPacket.AltitudeOrientation : 1.f;
        point.azimuth =  EnumHasAnyFlags(capabilities, UE::StylusInput::ETabletSupportedProperties::AzimuthOrientation) ? iPacket.AzimuthOrientation : 1.f;
        point.twist =    EnumHasAnyFlags(capabilities, UE::StylusInput::ETabletSupportedProperties::TwistOrientation) ? iPacket.TwistOrientation : 1.f;
        point.pitch =    EnumHasAnyFlags(capabilities, UE::StylusInput::ETabletSupportedProperties::PitchRotation) ? iPacket.PitchRotation : 1.f;
        point.roll =     EnumHasAnyFlags(capabilities, UE::StylusInput::ETabletSupportedProperties::RollRotation) ? iPacket.RollRotation : 1.f;
        point.yaw =      EnumHasAnyFlags(capabilities, UE::StylusInput::ETabletSupportedProperties::YawRotation) ? iPacket.YawRotation : 1.f;
    }
#elif PLATFORM_MAC
    point.x = position_in_viewport.X;
    point.y = position_in_viewport.Y;
    point.z = iPacket.Z;
    point.pressure = iPacket.NormalPressure;
    point.time = iPacket.TimerTick;
    point.altitude = iPacket.AltitudeOrientation;
    point.azimuth = iPacket.AzimuthOrientation;
    point.twist = iPacket.TwistOrientation;
    point.pitch = iPacket.PitchRotation;
    point.roll = iPacket.RollRotation;
    point.yaw = iPacket.YawRotation;
#endif

    TArray<FKey> pressedKeys = mKeysPressed;
    pressedKeys.AddUnique(FOdysseyKeyState::GetLastKey());
    point.keysDown = pressedKeys;

    return point;
}

void
FOdysseyPainterEditorViewportClient::ReadStylusInput(eStylusEventFence iUntilEventType)
{
    if (!mIsFocused || mPacketQueue.Num() == 0)
    {
        ClearQueue();
        return;
    }

    UE::StylusInput::FStylusInputPacket packet;
    while (mPacketQueue.Dequeue(packet))
    {
        FOdysseyPoint point = StylusPacketToPoint(packet);
        if (packet.Type == UE::StylusInput::EPacketType::StylusDown)
        {
            //MouseDown
            mStylusIsDown = true;
            MouseDown(point);
            mEventsConsumedSinceLastUp++;

            if( iUntilEventType == eStylusEventFence::kStylusDown )
                return;
        }
        else if (packet.Type == UE::StylusInput::EPacketType::StylusUp)
        {
            //MouseUp
            MouseUp(point);
            mStylusIsDown = false;
            mEventsConsumedSinceLastUp = 0;
            StopStylusInputRecord();

            if ( iUntilEventType == eStylusEventFence::kStylusUp )
                return;
        }
        else if (mStylusIsDown)
        {
            //MouseMove
            MouseDrag(point);
            mEventsConsumedSinceLastUp++;
        }
    }
}

void FOdysseyPainterEditorViewportClient::OnPacket(const UE::StylusInput::FStylusInputPacket& iPacket, UE::StylusInput::IStylusInputInstance* iInstance)
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
// FIX: MOVE WINTAB COORDINATES WHEN MAIN SCREEN IS NOT ON THE (TOP) LEFT OF USER PHYSICAL DESKTOP - AWAITING FOR EPIC PULL REQUEST VALIDATION

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
    {
        ClearQueue();
    }
    mPacketQueue.Enqueue(iPacket);
#endif
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

    if (FOdysseyViewportCommands::Get().PanZoomViewport->HasActiveChord(activeChord))
    {
        return mIsMouseDown ? mCurrentToolState : eState::kPanZoom;
    }
    else if (FOdysseyViewportCommands::Get().RotateViewport->HasActiveChord(activeChord))
    {
        return eState::kRotate;
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

void FOdysseyPainterEditorViewportClient::Tick(float DeltaTime)
{
    if( mIsRecordingStylus )
        ReadStylusInput(eStylusEventFence::kStylusUp);
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------- Private API

void
FOdysseyPainterEditorViewportClient::CreateCheckerboardTexture()
{
    DestroyCheckerboardTexture();

    const UOdysseyPainterEditorSettings& settings = *GetDefault< UOdysseyPainterEditorSettings >();
    mCheckerboardTexture = FImageUtils::CreateCheckerboardTexture( settings.GetCheckerColorOne(), settings.GetCheckerColorTwo(), settings.GetCheckerSize() );
}

void
FOdysseyPainterEditorViewportClient::DestroyCheckerboardTexture()
{
    // Must use GetValid(), to not go here if mCheckerboardTexture is not null BUT it is already garbage collected
    if( GetValid( mCheckerboardTexture ) )
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

    viewportWidget->ZoomExponential(viewportWidget->GetZoom(), 0.1f, pos);
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

    viewportWidget->ZoomExponential(viewportWidget->GetZoom(), -0.1f, pos);
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
