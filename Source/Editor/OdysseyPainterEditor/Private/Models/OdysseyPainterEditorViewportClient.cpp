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
#include "PainterEditor/OdysseyPainterEditorHUDTab.h"
#include "OdysseyKeyState.h"

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
    , mLastKey( EKeys::Invalid )
    , mLastEvent( EInputEvent::IE_MAX )
    , mOdysseyPainterEditor(iOdysseyPainterEditor)
    , mOdysseyPainterEditorViewportPtr( iOdysseyPainterEditorViewport )
    , mMeshSelector( iMeshSelector )
    , mCheckerboardTexture( NULL )
    , mCurrentMouseCursor( EMouseCursor::Default )
    , mPivotPointRatio( FVector2D( 0.5, 0.5 ) )
    , mCurrentToolState( eState::kIdle )
    , mIsCapturedByStylus(false)
    , mNearestNeighbourTexture()
    , mBilinearTexture()
    , mIsCurrentModeActive(false)
    , mIsReadyToCreateTool(false)
{
    check( mOdysseyPainterEditorViewportPtr.IsValid() );

    InputSubsystem = GEditor->GetEditorSubsystem<UOdysseyStylusInputSubsystem>();

    //PATCH: as I don't know how to initialize usubsystem inside settings ctor (as usubsystem are called after)
	//UOdysseyStylusInputSettings* settings = GetMutableDefault< UOdysseyStylusInputSettings >();
    //settings->RefreshStylusInputDriver();
    //PATCH

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
        FCanvasTileItem tileItem(pan, mCheckerboardTexture->GetResource(), FVector2D( width, height ), FVector2D( 0.f, 0.f ), FVector2D( width / mCheckerboardTexture->GetSizeX(), height / mCheckerboardTexture->GetSizeY() ), FLinearColor::White );
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
            textureToDisplay = GetZoom() <= 1.0 ? &mBilinearTexture : &mNearestNeighbourTexture;
        }
        // END PATCH:

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

    //Tool system HUD drawing
    IOdysseySurfaceEditable* HUDSurface = mOdysseyPainterEditor->HUDSystem()->GetHUDSurface();
    UTexture* HUDTexture = nullptr;
    if (HUDSurface)
    {
        HUDTexture = HUDSurface->Texture();
    }

    // Draw HUD Surface
    if( HUDTexture && HUDTexture->Resource )
    {
        FTransform2D transformMinusCenter = FTransform2D( mOdysseyPainterEditorViewportPtr.Pin()->GetViewportCenter() * -1);
        FTransform2D transformAddHalfTexture = FTransform2D( FVector2D(texture->GetSurfaceWidth(), texture->GetSurfaceHeight()) / 2.f );
        FTransform2D transformViewport = mOdysseyPainterEditorViewportPtr.Pin()->GetTransform();
        FTransform2D transform = transformMinusCenter.Concatenate( transformViewport.Inverse() );
        transform.SetTranslation( transform.GetTranslation() + FVector2D(texture->GetSurfaceWidth(), texture->GetSurfaceHeight()) / 2.f );
        transform = transform.Inverse(); 
         
        if( mOdysseyPainterEditor->GetSelectedTool() && mOdysseyPainterEditor->GetSelectedTool()->GetHUD() )
            mOdysseyPainterEditor->GetSelectedTool()->GetHUD()->Draw( HUDSurface->Block().Get(), transform );
        
        FCanvasTileItem tileItem(FVector2D(0, 0), HUDTexture->Resource, FVector2D(iViewport->GetSizeXY().X, iViewport->GetSizeXY().Y), FLinearColor::White);
        tileItem.BatchedElementParameters = batchedElementParameters;
        uint32 result = (uint32)SE_BLEND_RGBA_MASK_START;
        result += ( 1 << 0 );
        result += ( 1 << 1 );
        result += ( 1 << 2 );
        result += ( 1 << 3 );
        tileItem.BlendMode = (ESimpleElementBlendMode)result;
        tileItem.PivotPoint = pivotPoint;
        ioCanvas->DrawItem( tileItem );
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
}

EMouseCursor::Type
FOdysseyPainterEditorViewportClient::GetCursor( FViewport* iViewport, int32 iX, int32 iY )
{
    if( mCurrentToolState == eState::kPan )
        mCurrentMouseCursor = EMouseCursor::GrabHand;
    else if( mCurrentToolState == eState::kPick )
        mCurrentMouseCursor = EMouseCursor::EyeDropper;
    else if( mOdysseyPainterEditor->GetSelectedTool() )
        mCurrentMouseCursor = mOdysseyPainterEditor->GetSelectedTool()->GetMouseCursor();

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
    //Here you receive Mouse Buttons and Keyboard keys events

    //Cleanup PressedKeys
    TSharedPtr<SOdysseyViewport> viewportWidget = mOdysseyPainterEditorViewportPtr.Pin();
    for (int i = mKeysPressed.Num() - 1; i >= 0; i--)
    {
        if (!viewportWidget->GetViewport()->KeyState(mKeysPressed[i]))
        {
            mKeysPressed.RemoveAt(i);
        }
    }

    if( iEvent == EInputEvent::IE_Pressed )
    {
        if (!mKeysPressed.Contains(iKey))
            mKeysPressed.Add( iKey );
    }
    else if( iEvent == EInputEvent::IE_Released )
    {
        mKeysPressed.Remove(iKey);
    }

    mLastKey = iKey;
    mLastEvent = iEvent;

    //---

    auto end_time = std::chrono::steady_clock::now();
    auto delta = std::chrono::duration_cast<std::chrono::milliseconds>( end_time - mStylusLastEventTime).count();

    if( (mIsCapturedByStylus || delta < 500) && ( iKey == EKeys::LeftMouseButton || iKey == EKeys::RightMouseButton ) )
        return true;

    FOdysseyPoint point_in_viewport( FOdysseyPoint::DefaultPoint() );
    point_in_viewport.x = iViewport->GetMouseX();
    point_in_viewport.y = iViewport->GetMouseY();
    return InputKeyWithStrokePoint( point_in_viewport, iControllerId, iKey, iEvent, iAmountDepressed, iGamepad );
}

void
FOdysseyPainterEditorViewportClient::CapturedMouseMove( FViewport* iViewport, int32 iX, int32 iY )
{
    //This is called when the mouse is down and moving in the viewport
    //The viewport has already captured the mouse

    auto end_time = std::chrono::steady_clock::now();
    auto delta = std::chrono::duration_cast<std::chrono::milliseconds>( end_time - mStylusLastEventTime).count();

    if (mIsCapturedByStylus || delta < 500)
        return;

    FOdysseyPoint point_in_viewport( FOdysseyPoint::DefaultPoint() );
    point_in_viewport.x = iX;
    point_in_viewport.y = iY;
    CapturedMouseMoveWithStrokePoint( point_in_viewport );
}

void
FOdysseyPainterEditorViewportClient::MouseEnter( FViewport* iViewport, int32 iX, int32 iY )
{
    if( mIsCurrentModeActive )
        return;

    mCurrentToolState = eState::kIdle;
}

void
FOdysseyPainterEditorViewportClient::MouseLeave( FViewport* iViewport )
{
    if( mIsCurrentModeActive )
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
    bool hasMoved = long(mCurrentPointInTexture.x) != long(pointInTexture.x) || long(mCurrentPointInTexture.y) != long(pointInTexture.y);
    mCurrentPointInTexture = pointInTexture;

    if (mCurrentToolState == eState::kIdle)
    {
        if (!hasMoved)
            return;

        mOnMouseHover.ExecuteIfBound(mCurrentPointInTexture);
    }
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------ Stylus Functions

void
FOdysseyPainterEditorViewportClient::OnStylusStateChanged( const TWeakPtr<SWidget> iWidget, const FStylusState& iState, int32 iIndex )
{
    //If we don't have a surface, then we don't interact with anything
    UTexture* texture = mOdysseyPainterEditorViewportPtr.Pin()->GetTexture();
    if (!texture)
        return;

    //---

    if( !iWidget.IsValid() )
        return;

    TSharedPtr< SViewport > viewport = mOdysseyPainterEditorViewportPtr.Pin()->GetViewportWidget();
    const SWidget* widget = viewport.Get();

    if( iWidget.Pin().Get() != widget )
        return;

    //---

    float scale_dpi = mOdysseyPainterEditorViewportPtr.Pin()->GetViewport()->GetCachedGeometry().GetAccumulatedLayoutTransform().GetScale();
    FVector2D position_in_viewport = widget->GetCachedGeometry().AbsoluteToLocal( iState.GetPosition() ) * scale_dpi;

    FOdysseyPoint stroke_point( position_in_viewport.X
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
    stroke_point.keysDown = pressedKeys;
  //---

    static TQueue< FOdysseyPoint > queue;
    static bool stylusWasDown = false;
    static bool is_dragging = false;

    bool isDownEvent = !stylusWasDown && iState.IsStylusDown();
    bool isUpEvent = stylusWasDown && !iState.IsStylusDown();
    bool isMoveEvent = stylusWasDown == iState.IsStylusDown();

    stylusWasDown = iState.IsStylusDown();
    
    if( isDownEvent )
    {
        InputKeyWithStrokePoint( stroke_point, 0, GetPointingDevicePressedKey(), EInputEvent::IE_Pressed );
        mIsCapturedByStylus = true;
    }
    else if( isUpEvent )
    {
        InputKeyWithStrokePoint( stroke_point, 0, GetPointingDevicePressedKey(), EInputEvent::IE_Released );
        mIsCapturedByStylus = false;
    }
    else if( iState.IsStylusDown() ) // MUSTY BE the last, or at least after "is_dragging = false;"
    {
        CapturedMouseMoveWithStrokePoint( stroke_point );
    }
    else
    {
		// Register the point in texture when hovering the canvas
        //Point In Viewport
        mCurrentPointInViewport = stroke_point;
        //Point In Texture
		mCurrentPointInTexture = GetLocalMousePosition(mCurrentPointInViewport);
    }

    mStylusLastEventTime = std::chrono::steady_clock::now();
    mLastKey = EKeys::Invalid;
    mLastEvent = EInputEvent::IE_MAX;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------ Internal Input Functions

FKey
FOdysseyPainterEditorViewportClient::GetPointingDevicePressedKey()
{
    if( mLastKey == EKeys::RightMouseButton )
    {
        return EKeys::RightMouseButton;
    }

    return EKeys::LeftMouseButton;
}

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

    if (FOdysseyPainterEditorCommands::Get().PanViewport->HasActiveChord(activeChord))
    {
        return eState::kPan;
    }
    else if (FOdysseyPainterEditorCommands::Get().RotateViewport->HasActiveChord(activeChord))
    {
        return eState::kRotate;
    }
    else if (FOdysseyPainterEditorCommands::Get().ZoomViewport->HasActiveChord(activeChord))
    {
        return eState::kZoom;
    }
    else if (FOdysseyPainterEditorCommands::Get().PickColorInViewport->HasActiveChord(activeChord))
    {
        return eState::kPick;
    }
    return eState::kIdle;
}

// - OnInputEvent

bool
FOdysseyPainterEditorViewportClient::InputKeyWithStrokePoint(const FOdysseyPoint& iPointInViewport, int32 iControllerId, FKey iKey, EInputEvent iEvent, float iAmountDepressed, bool iGamepad)
{

    //If we don't have a surface, then we don't interact with anything
    UTexture* texture = mOdysseyPainterEditorViewportPtr.Pin()->GetTexture();
    if (!texture)
        return false;
    
    TArray<FKey> pressedKeys = mKeysPressed;
    if (FOdysseyKeyState::GetLastKey() != FKey())
        pressedKeys.AddUnique(FOdysseyKeyState::GetLastKey());

    //Point In Viewport
    FOdysseyPoint pointInViewport = iPointInViewport;
    pointInViewport.keysDown = pressedKeys;
    pointInViewport.ComputeRelativeParameters(mCurrentPointInViewport);
    mCurrentPointInViewport = pointInViewport;

    //Point In Texture
    FOdysseyPoint pointInTexture = GetLocalMousePosition(iPointInViewport);
    pointInTexture.keysDown = pressedKeys;
    pointInTexture.ComputeRelativeParameters(mCurrentPointInTexture);
    mCurrentPointInTexture = pointInTexture;

    if (mCurrentToolState == eState::kIdle)
    {
        if (mIsCurrentModeActive)
            return OnInputEventRaw(iPointInViewport, iKey, iEvent);
        
        mCurrentToolState = InputChordToState();

        //If no state becomes active
        if (mCurrentToolState == eState::kIdle)
            return OnInputEventRaw(iPointInViewport, iKey, iEvent);
        
        return OnInputEventWithState(iPointInViewport, iKey, iEvent);
    }

    //If mCurrentToolState has an active state (pan, rotate, etc...)
    mCurrentToolState = InputChordToState();
    if (mCurrentToolState != eState::kIdle)
        return OnInputEventWithState(iPointInViewport, iKey, iEvent);

    return OnInputEventRaw(iPointInViewport, iKey, iEvent);
}

bool
FOdysseyPainterEditorViewportClient::OnInputEventRaw(const FOdysseyPoint& iPointInViewport, FKey iKey, EInputEvent iEvent)
{
    if ((iKey == EKeys::LeftMouseButton)||(iKey == EKeys::RightMouseButton))
    {
        if (iEvent == EInputEvent::IE_Pressed)
        {
            //LeftMouse Down
            mIsCurrentModeActive = true;
            return mOnMouseDown.IsBound() && mOnMouseDown.Execute(mCurrentPointInTexture, iKey);
        }
        else if (iEvent == EInputEvent::IE_Released)
        {
            mIsCurrentModeActive = false;
            return mOnMouseUp.IsBound() && mOnMouseUp.Execute(mCurrentPointInTexture, iKey);
        }
    }
    else if (iEvent == EInputEvent::IE_Pressed)
    {
        //KeyDown
        if (mOnKeyDown.IsBound() && mOnKeyDown.Execute(iKey))
            return true;

        if (iKey == EKeys::MouseScrollUp)
        {
            //ZoomIn
            FVector2D position_in_viewport(iPointInViewport.x, iPointInViewport.y);
            ZoomInInViewport(position_in_viewport);
            return true;
        }
        else if (iKey == EKeys::MouseScrollDown)
        {
            //ZoomOut
            FVector2D position_in_viewport(iPointInViewport.x, iPointInViewport.y);
            ZoomOutInViewport(position_in_viewport);
            return true;
        }
    }
    else if (iEvent == EInputEvent::IE_Released)
    {
        //KeyUp
        return mOnKeyUp.IsBound() && mOnKeyUp.Execute(iKey);
    }
    return false;
}

bool
FOdysseyPainterEditorViewportClient::OnInputEventWithState(const FOdysseyPoint& iPointInViewport, FKey iKey, EInputEvent iEvent)
{
    if( mCurrentToolState == eState::kRotate )
    {
        if( iKey == EKeys::LeftMouseButton && iEvent == EInputEvent::IE_Pressed )
        {
            mIsCurrentModeActive = true;

            FIntPoint size = mOdysseyPainterEditorViewportPtr.Pin()->GetViewport()->GetSizeXY();
            FVector2D center = FVector2D( size.X / 2, size.Y / 2 );
            FVector2D position_in_viewport( iPointInViewport.x, iPointInViewport.y );
            FVector2D deltaCenter = position_in_viewport - center;
            mRotationReference = FMath::Atan2( -deltaCenter.Y, deltaCenter.X );
            return true;
        }
        else if (iKey == EKeys::LeftMouseButton && iEvent == EInputEvent::IE_Released)
        {
            mIsCurrentModeActive = false;
            return true;
        }
    }
    else if( mCurrentToolState == eState::kPan )
    {
        if( iKey == EKeys::LeftMouseButton && iEvent == EInputEvent::IE_Pressed )
        {
            mIsCurrentModeActive = true;

            mPanReference = FVector2D( iPointInViewport.x, iPointInViewport.y );
            return true;
        }
        else if (iKey == EKeys::LeftMouseButton && iEvent == EInputEvent::IE_Released)
        {
            mIsCurrentModeActive = false;
            return true;
        }
    }
    else if (mCurrentToolState == eState::kZoom)
    {
        if (iKey == EKeys::LeftMouseButton && iEvent == EInputEvent::IE_Pressed)
        {
            mIsCurrentModeActive = true;

            uint32 width;
            uint32 height;
            mOdysseyPainterEditorViewportPtr.Pin()->ComputeTextureDisplayDimensions(width, height);

            double zoom = mOdysseyPainterEditorViewportPtr.Pin()->GetZoom();
            mZoomReference = ::FMath::Loge(zoom);
            mZoomSizeReference = width;
            mZoomViewportPointReference = FVector2D(iPointInViewport.x, iPointInViewport.y);
            return true;
        }
        else if (iKey == EKeys::LeftMouseButton && iEvent == EInputEvent::IE_Released)
        {
            mIsCurrentModeActive = false;
            return true;
        }
    }
    else if( mCurrentToolState == eState::kPick )
    {
        if( iKey == EKeys::LeftMouseButton && iEvent == EInputEvent::IE_Pressed )
        {
            mIsCurrentModeActive = true;

            FOdysseyPoint strokePoint_in_texture = GetLocalMousePosition(iPointInViewport);
            FVector2D position_in_texture(strokePoint_in_texture.x, strokePoint_in_texture.y );
			mOnPickColor.ExecuteIfBound(eOdysseyEventState::kAdjust, position_in_texture);
            return true;
        }
        else if (iKey == EKeys::LeftMouseButton && iEvent == EInputEvent::IE_Released)
        {
            FOdysseyPoint strokePoint_in_texture = GetLocalMousePosition(iPointInViewport);
            FVector2D position_in_texture(strokePoint_in_texture.x, strokePoint_in_texture.y);
            mOnPickColor.ExecuteIfBound(eOdysseyEventState::kSet, position_in_texture);
            
            mIsCurrentModeActive = false;
            return true;
        }
    }
    return false;
}

void
FOdysseyPainterEditorViewportClient::CapturedMouseMoveWithStrokePoint( const FOdysseyPoint& iPointInViewport )
{

    //If we don't have a surface, then we don't interact with anything
    UTexture* texture = mOdysseyPainterEditorViewportPtr.Pin()->GetTexture();
    if (!texture)
        return;

    TArray<FKey> pressedKeys = mKeysPressed;
    if (FOdysseyKeyState::GetLastKey() != FKey())
        pressedKeys.AddUnique(FOdysseyKeyState::GetLastKey());

    //Point In Viewport
    FOdysseyPoint pointInViewport = iPointInViewport;
    pointInViewport.keysDown = pressedKeys;
    pointInViewport.ComputeRelativeParameters(mCurrentPointInViewport);
    mCurrentPointInViewport = pointInViewport;

    //Point In Texture
    FOdysseyPoint pointInTexture = GetLocalMousePosition(mCurrentPointInViewport);
    pointInTexture.keysDown = pressedKeys;
    pointInTexture.ComputeRelativeParameters(mCurrentPointInTexture);
    bool hasMoved = long(mCurrentPointInTexture.x) != long(pointInTexture.x) || long(mCurrentPointInTexture.y) != long(pointInTexture.y);
    mCurrentPointInTexture = pointInTexture;

    if( mCurrentToolState == eState::kIdle && mIsCurrentModeActive )
    {
		if (!hasMoved)
			return;

        mOnMouseDrag.ExecuteIfBound(mCurrentPointInTexture);
        //mOdysseyPainterEditor->StrokeEngine()->To( mCurrentPointInTexture );
    }
    else if( mCurrentToolState == eState::kPan && mIsCurrentModeActive)
    {
        FVector2D deltaReference( iPointInViewport.x - mPanReference.X, iPointInViewport.y - mPanReference.Y );

        mOdysseyPainterEditorViewportPtr.Pin()->AddPan( deltaReference );
        mPanReference = FVector2D( iPointInViewport.x, iPointInViewport.y );
    }
    else if( mCurrentToolState == eState::kRotate && mIsCurrentModeActive)
    {
        FIntPoint size = mOdysseyPainterEditorViewportPtr.Pin()->GetViewport()->GetSizeXY();

        FVector2D center = FVector2D( size.X / 2, size.Y / 2 );
        FVector2D position_in_viewport( iPointInViewport.x, iPointInViewport.y );
        FVector2D deltaCenter = position_in_viewport - center;
        float newRotation = FMath::Atan2( -deltaCenter.Y, deltaCenter.X );
        float deltaRotation = mRotationReference - newRotation;

        mOdysseyPainterEditorViewportPtr.Pin()->SetRotation( mOdysseyPainterEditorViewportPtr.Pin()->GetRotation() + deltaRotation );

        mRotationReference = newRotation;
    }
    else if (mCurrentToolState == eState::kZoom && mIsCurrentModeActive)
    {   
        TSharedPtr<SOdysseyViewport> viewportWidget = mOdysseyPainterEditorViewportPtr.Pin();

        FVector2D viewportMousePosition(iPointInViewport.x, iPointInViewport.y);
        float dist = viewportMousePosition.X - mZoomViewportPointReference.X;
        float smoothness = 200.f; //TODO: do a Setting to let the user change it at will

        if (dist > KINDA_SMALL_NUMBER || dist < KINDA_SMALL_NUMBER)
        {
            float newDist = mZoomReference + (dist / smoothness);
            float zoom = ::FMath::Exp(newDist);
            viewportWidget->SetZoom(zoom, mZoomViewportPointReference - mOdysseyPainterEditorViewportPtr.Pin()->GetViewportCenter());
        }
    }
    else if( mCurrentToolState == eState::kPick && mIsCurrentModeActive)
    {
        FOdysseyPoint strokePoint_in_texture = GetLocalMousePosition(iPointInViewport);
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

#undef LOCTEXT_NAMESPACE
