// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Models/OdysseyPainterEditorViewportClient.h"

#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "CubemapUnwrapUtils.h"
#include "Editor/UnrealEdEngine.h"
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
#include "OdysseyPainterEditorSettings.h"
#include "OdysseyStylusInputSettings.h"
// #include "OdysseyPainterEditorToolkit.h"
#include "OdysseyPainterEditorData.h"
#include "Mesh/FOdysseyMeshSelector.h"
#include "OdysseySurface.h"
#include "SOdysseyCursorWidget.h"
#include "SOdysseySurfaceViewport.h"

#include <memory>
#include <chrono>
#include <ULIS3>

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorViewportClientt"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorViewportClient
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorViewportClient::FOdysseyPainterEditorViewportClient( TWeakPtr< FOdysseyPainterEditorData >     iOdysseyPainterEditorData,
                                                                          TWeakPtr< SOdysseySurfaceViewport >       iOdysseyPainterEditorViewport,
                                                                          FOdysseyMeshSelector*                     iMeshSelector)
    : InputSubsystem( nullptr )
    , mLastKey( EKeys::Invalid )
    , mLastEvent( EInputEvent::IE_MAX )
    , mOdysseyPainterEditorDataPtr( iOdysseyPainterEditorData )
    , mOdysseyPainterEditorViewportPtr( iOdysseyPainterEditorViewport )
    , mMeshSelector( iMeshSelector )
    , mCheckerboardTexture( NULL )
    , mCurrentMouseCursor( EMouseCursor::Default )
    , mPivotPointRatio( FVector2D( 0.5, 0.5 ) )
    , mCurrentToolState( eState::kIdle )
    , mIsCapturedByStylus(false)
{
    check( // mOdysseyPainterEditorDataPtr.IsValid() &&
           mOdysseyPainterEditorViewportPtr.IsValid() );

    InputSubsystem = GEditor->GetEditorSubsystem<UOdysseyStylusInputSubsystem>();

    //PATCH: as I don't know how to initialize usubsystem inside settings ctor (as usubsystem are called after)
	UOdysseyStylusInputSettings* settings = GetMutableDefault< UOdysseyStylusInputSettings >();
    settings->RefreshStylusInputDriver();
    //PATCH

    InputSubsystem->AddMessageHandler( *this );
    InputSubsystem->OnStylusInputChanged().BindRaw( this, &FOdysseyPainterEditorViewportClient::OnStylusInputChanged );

    ModifyCheckerboardTextureColors();
}

FOdysseyPainterEditorViewportClient::~FOdysseyPainterEditorViewportClient( )
{
    InputSubsystem->OnStylusInputChanged().Unbind();
    InputSubsystem->RemoveMessageHandler( *this );

    DestroyCheckerboardTexture();
}

void
FOdysseyPainterEditorViewportClient::OnStylusInputChanged( TSharedPtr<IStylusInputInterfaceInternal> iStylusInput )
{
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------ FViewportClient API
void
FOdysseyPainterEditorViewportClient::Draw( FViewport* iViewport, FCanvas* ioCanvas )
{
    /* if( !mOdysseyPainterEditorDataPtr.IsValid() )
        return; */

    // Draw on tick or catch up
    // auto paintengine = mOdysseyPainterEditorDataPtr.Pin()->PaintEngine();
    // FVector2D oldpoint = FVector2D( RefEventStrokePoint.x, RefEventStrokePoint.y );
    // FVector2D position_in_viewport( Viewport->GetMouseX(), Viewport->GetMouseY() );
    // FVector2D position_in_texture = GetLocalMousePosition( position_in_viewport );
    // if( mCurrentToolState == eState::kDrawing
    // && ( paintengine->GetStokePaintOnTick() || ( paintengine->GetSmoothingCatchUp() && oldpoint != position_in_texture ) ) )
    // {
    //     FOdysseyStrokePoint point = RefEventStrokePoint;
    //     point.x = position_in_texture.X;
    //     point.y = position_in_texture.Y;
    //     paintengine->PushStroke( point );
    // }

    // Send Tick to PaintEngine
	mOdysseyPainterEditorDataPtr.Pin()->PaintEngine()->Tick();

	const UOdysseyPainterEditorSettings& settings = *GetDefault<UOdysseyPainterEditorSettings>();
	ioCanvas->Clear(settings.BackgroundColor);

    if (!mOdysseyPainterEditorViewportPtr.Pin()->GetSurface() || !mOdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Texture())
    {
        return;
    }

    double rotation         = mOdysseyPainterEditorViewportPtr.Pin()->GetRotationInDegrees();
    FVector2D pan           = mOdysseyPainterEditorViewportPtr.Pin()->GetPan();

    UTexture* texture       = mOdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Texture();
    FVector2D ratio         = FVector2D( mOdysseyPainterEditorViewportPtr.Pin()->GetViewportHorizontalScrollBarRatio(),
                                         mOdysseyPainterEditorViewportPtr.Pin()->GetViewportVerticalScrollBarRatio() );
    FVector2D viewportSize  = FVector2D( mOdysseyPainterEditorViewportPtr.Pin()->GetViewport()->GetSizeXY().X,
                                         mOdysseyPainterEditorViewportPtr.Pin()->GetViewport()->GetSizeXY().Y );
    FVector2D scrollBarPos  = GetViewportScrollBarPositions();
    int32 yOffset           = ( ratio.Y > 1.0f ) ? ( ( viewportSize.Y - ( viewportSize.Y / ratio.Y ) ) * 0.5f ) + pan.Y: pan.Y;
    int32 yPos              = yOffset - scrollBarPos.Y;
    int32 xOffset           = ( ratio.X > 1.0f ) ? ( ( viewportSize.X - ( viewportSize.X / ratio.X ) ) * 0.5f ) + pan.X: pan.X;
    int32 xPos              = xOffset - scrollBarPos.X;

    UpdateScrollBars();
    UTexture2D* texture2D = Cast< UTexture2D >( texture );

    // Fully stream in the texture before drawing it.
    if( texture2D )
    {
        texture2D->SetForceMipLevelsToBeResident( 30.0f );
        texture2D->WaitForStreaming();
    }

    // Figure out the size we need
    uint32 width, height;
    mOdysseyPainterEditorViewportPtr.Pin()->CalculateTextureDisplayDimensions( width, height );
    const float mipLevel = 0; //should be -1, but as we are editing only the first mipmap, then the other mipmaps are not updated and so we cannot use -1 to have automatic mipmap selection.

    TRefCountPtr<FBatchedElementParameters> batchedElementParameters;

    if( GMaxRHIFeatureLevel >= ERHIFeatureLevel::SM5 )
    {
        //ODYSSEY: PATCH
        bool isNormalMap = texture2D->IsNormalMap();
        bool isSingleChannel = texture2D->CompressionSettings == TC_Grayscale || texture2D->CompressionSettings == TC_Alpha;
        bool isVirtual = texture2D->IsCurrentlyVirtualTextured();
        bool isVTSPS = texture2D->IsVirtualTexturedWithSinglePhysicalSpace();
        bool isTextureArray = false;
        float layerIndex = 0.f;
        batchedElementParameters = new FBatchedElementTexture2DPreviewParameters( mipLevel, layerIndex, isNormalMap, isSingleChannel, isVTSPS, isVirtual, isTextureArray );
    }

    FVector2D viewport_center( iViewport->GetSizeXY().X / 2, iViewport->GetSizeXY().Y / 2 );
    FVector2D position_in_texture = GetLocalMousePosition( viewport_center, false );
    mPivotPointRatio = FVector2D( position_in_texture.X / mOdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Texture()->GetSizeX(), position_in_texture.Y / mOdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Texture()->GetSizeY());

    // Draw background Checker
    {
        FCanvasTileItem tileItem( FVector2D( xPos, yPos ), mCheckerboardTexture->Resource, FVector2D( width, height ), FVector2D( 0.f, 0.f ), FVector2D( width / mCheckerboardTexture->GetSizeX(), height / mCheckerboardTexture->GetSizeY() ), FLinearColor::White );
        tileItem.BlendMode = SE_BLEND_Opaque;
        tileItem.PivotPoint.Set( mPivotPointRatio.X, mPivotPointRatio.Y );
        tileItem.Rotation.Add( 0, rotation, 0 );
        ioCanvas->DrawItem( tileItem );
    }

    // Draw Drawing Surface
    if( texture->Resource != nullptr )
    {
        FCanvasTileItem tileItem( FVector2D( xPos, yPos ), texture->Resource, FVector2D( width, height ), FLinearColor::White );
        tileItem.BatchedElementParameters = batchedElementParameters;
        uint32 result = (uint32)SE_BLEND_RGBA_MASK_START;
        result += ( 1 << 0 );
        result += ( 1 << 1 );
        result += ( 1 << 2 );
        result += ( 1 << 3 );
        tileItem.BlendMode = (ESimpleElementBlendMode)result;
        tileItem.PivotPoint.Set( mPivotPointRatio.X, mPivotPointRatio.Y );
        tileItem.Rotation.Add( 0, rotation, 0 );
        ioCanvas->DrawItem( tileItem );

        // TODO: Unreal BoxItem doesn't support rotation, so we can't draw it properly. We'll have to come up with our own HUD
        // Draw a white border around the texture to show its extents
        // if( settings.TextureBorderEnabled )
        // {
        //     FCanvasBoxItem boxItem( FVector2D( xPos, yPos ), FVector2D( width , height ) );
        //     boxItem.SetColor( Settings.TextureBorderColor );
        //     Canvas->DrawItem( boxItem );
        // }
    }

    // Draw Cursor Preview
    if( mOdysseyPainterEditorDataPtr.Pin()->DrawBrushPreview() )
    {
        auto paintEngine = mOdysseyPainterEditorDataPtr.Pin()->PaintEngine();
        paintEngine->UpdateBrushCursorPreview();
        if( paintEngine->mBrushCursorPreviewSurface )
        {
            paintEngine->mBrushCursorPreviewSurface->Texture()->SetForceMipLevelsToBeResident( 30.0f );
            paintEngine->mBrushCursorPreviewSurface->Texture()->WaitForStreaming();
            FVector2D pos_in_viewport( iViewport->GetMouseX(), iViewport->GetMouseY() );
            FVector2D shift = paintEngine->mBrushCursorPreviewShift;
            FVector2D pos = pos_in_viewport + shift * GetZoom();
            FVector2D size = FVector2D( paintEngine->mBrushCursorPreviewSurface->Block()->Width(), paintEngine->mBrushCursorPreviewSurface->Block()->Height() ) * GetZoom();
            FCanvasTileItem cursorItem( pos, paintEngine->mBrushCursorPreviewSurface->Texture()->Resource, size, FLinearColor::White );
            uint32 result = (uint32)SE_BLEND_RGBA_MASK_START;
            result += ( 1 << 0 );
            result += ( 1 << 1 );
            result += ( 1 << 2 );
            result += ( 1 << 3 );
            cursorItem.BlendMode = (ESimpleElementBlendMode)result;
            ioCanvas->DrawItem( cursorItem );
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
            //mMeshSelector->GetCurrentMesh()->SourceModels[currentLOD].LoadRawMesh(RawMesh);
            mMeshSelector->GetCurrentMesh()->GetSourceModel( currentLOD ).LoadRawMesh( rawMesh );

            FIndexArrayView indexBuffer = mMeshSelector->GetCurrentMesh()->RenderData.Get()->LODResources[currentLOD].IndexBuffer.GetArrayView();
            DrawUVsOntoViewport( iViewport, ioCanvas, currentUV, mMeshSelector->GetCurrentMesh()->RenderData.Get()->LODResources[0].VertexBuffers.StaticMeshVertexBuffer, indexBuffer );
        }
    }
}

bool
FOdysseyPainterEditorViewportClient::InputKey( FViewport* iViewport, int32 iControllerId, FKey iKey, EInputEvent iEvent, float iAmountDepressed, bool iGamepad )
{
    //UE_LOG( LogStylusInput, Log, TEXT( "InputKey Key:%s Event:%d" ), *iKey.GetFName().ToString(), iEvent );

    if( iEvent == EInputEvent::IE_Pressed )
    {
        mKeysPressed.Add( iKey );
    }
    else if( iEvent == EInputEvent::IE_Released )
    {
        mKeysPressed.Remove( iKey );
    }

    mLastKey = iKey;
    mLastEvent = iEvent;

    auto end_time = std::chrono::steady_clock::now();
    auto delta = std::chrono::duration_cast<std::chrono::milliseconds>( end_time - mStylusLastEventTime).count();

    if( (mIsCapturedByStylus || delta < 500) && ( iKey == EKeys::LeftMouseButton || iKey == EKeys::RightMouseButton ) )
        return true;

    FOdysseyStrokePoint point_in_viewport( FOdysseyStrokePoint::DefaultPoint() );
    point_in_viewport.x = iViewport->GetMouseX();
    point_in_viewport.y = iViewport->GetMouseY();
    return InputKeyWithStrokePoint( point_in_viewport, iControllerId, iKey, iEvent, iAmountDepressed, iGamepad );
}

void
FOdysseyPainterEditorViewportClient::CapturedMouseMove( FViewport* iViewport, int32 iX, int32 iY )
{
    auto end_time = std::chrono::steady_clock::now();
    auto delta = std::chrono::duration_cast<std::chrono::milliseconds>( end_time - mStylusLastEventTime).count();

    if (mIsCapturedByStylus || delta < 500)
        return;

    FOdysseyStrokePoint point_in_viewport( FOdysseyStrokePoint::DefaultPoint() );
    point_in_viewport.x = iX;
    point_in_viewport.y = iY;
    CapturedMouseMoveWithStrokePoint( point_in_viewport );
}

void
FOdysseyPainterEditorViewportClient::OnStylusStateChanged( const TWeakPtr<SWidget> iWidget, const FStylusState& iState, int32 iIndex )
{
    //If we don't have a surface, then we don't interact with anything
    if (!mOdysseyPainterEditorViewportPtr.Pin()->GetSurface() || !mOdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Texture())
        return;

    //---

    if( !iWidget.IsValid() )
        return;

    TSharedPtr< SViewport > viewport = mOdysseyPainterEditorViewportPtr.Pin()->GetViewportWidget();
    const SWidget* widget = viewport.Get();

    if( iWidget.Pin().Get() != widget )
        return;

    //---

    /* UE_LOG( LogStylusInput, Log, TEXT("OnStylusStateChanged index:%d x:%f y:%f pressure:%f down:%d tilt:%f %f azimuth:%f altitude:%f"), iIndex, 
            iState.GetPosition().X, iState.GetPosition().Y, 
            iState.GetPressure(), iState.IsStylusDown(), 
            iState.GetTilt().X, iState.GetTilt().Y, 
            iState.GetAzimuth(), iState.GetAltitude() ); */

    float scale_dpi = mOdysseyPainterEditorViewportPtr.Pin()->GetViewport()->GetCachedGeometry().GetAccumulatedLayoutTransform().GetScale();
    FVector2D position_in_viewport = widget->GetCachedGeometry().AbsoluteToLocal( iState.GetPosition() ) * scale_dpi;
	
    //UE_LOG( LogStylusInput, Log, TEXT( "OnStylusStateChanged AbsoluteToLocal: screen:%f %f -> local (ref widget): %f %f" ), iState.GetPosition().X, iState.GetPosition().Y, position_in_viewport.X, position_in_viewport.Y );

    FOdysseyStrokePoint stroke_point( position_in_viewport.X
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

    stroke_point.keysDown = mKeysPressed;
  //---

    static TQueue< FOdysseyStrokePoint > queue;
    static bool stylusWasDown = false;
    static bool is_dragging = false;

    bool isDownEvent = !stylusWasDown && iState.IsStylusDown();
    bool isUpEvent = stylusWasDown && !iState.IsStylusDown();
    bool isMoveEvent = stylusWasDown == iState.IsStylusDown();

    stylusWasDown = iState.IsStylusDown();

    //enqueue stroke points if stylus is down or was down in last event
    //queue will be emptied when a EInputEvent::IE_Released will be received
    /* if (iState.IsStylusDown() || stylusWasDown )
    {
        queue.Enqueue( stroke_point );
    } */

    

    /* if( !iState.IsStylusDown() )
        queue.Enqueue( stroke_point ); */

    
    if( isDownEvent )
    {
        //UE_LOG( LogStylusInput, Log, TEXT( "OnStylusStateChanged Pressed" ) );
        // is_dragging = true;

        /* FOdysseyStrokePoint point;
        queue.Dequeue( point ); */
        InputKeyWithStrokePoint( stroke_point, 0, EKeys::LeftMouseButton, EInputEvent::IE_Pressed );
        mIsCapturedByStylus = true;

        /* while( queue.Dequeue( point ) ) // Process all the down'd points which occur before having the ue pressed event
        {
            //If stylus is up, then we keep an event for the release
            if (!iState.IsStylusDown() && queue.IsEmpty())
            {
                queue.Enqueue( point );
                break;
            }
            CapturedMouseMoveWithStrokePoint( point );
        } */
    }
    else if( isUpEvent )
    {
        //UE_LOG( LogStylusInput, Log, TEXT( "OnStylusStateChanged Released" ) );

        InputKeyWithStrokePoint( stroke_point, 0, EKeys::LeftMouseButton, EInputEvent::IE_Released );
        mIsCapturedByStylus = false;

        /* FOdysseyStrokePoint point;
        while( queue.Dequeue( point ) )
        {
            //If stylus is up, then we keep an event for the release
            if (queue.IsEmpty())
            {
                InputKeyWithStrokePoint( point, 0, mLastKey, mLastEvent );
                break;
            }
            CapturedMouseMoveWithStrokePoint( point );
        }
	
		is_dragging = false;*/
    }
    else if( iState.IsStylusDown() ) // MUSTY BE the last, or at least after "is_dragging = false;"
    {
        //UE_LOG( LogStylusInput, Log, TEXT( "OnStylusStateChanged Dragging" ) );

        CapturedMouseMoveWithStrokePoint( stroke_point );
        
        /* FOdysseyStrokePoint point;
        while( queue.Dequeue( point ) )
        {
            //If stylus is up, then we keep an event for the release
            if (!iState.IsStylusDown() && queue.IsEmpty())
            {
                queue.Enqueue( point );
                break;
            }
            CapturedMouseMoveWithStrokePoint( point );
        } */

    }
    else
    {
		// Register the point in texture when hovering the canvas
		mCurrentPointInTexture = GetLocalMousePosition(stroke_point);
        //UE_LOG( LogStylusInput, Log, TEXT( "OnStylusStateChanged Nothing" ) );
    }

    mStylusLastEventTime = std::chrono::steady_clock::now();
    mLastKey = EKeys::Invalid;
    mLastEvent = EInputEvent::IE_MAX;
}

bool
FOdysseyPainterEditorViewportClient::InputKeyWithStrokePoint( const FOdysseyStrokePoint& iPointInViewport, int32 iControllerId, FKey iKey, EInputEvent iEvent, float iAmountDepressed, bool iGamepad )
{
    //If we don't have a surface, then we don't interact with anything
    if (!mOdysseyPainterEditorViewportPtr.Pin()->GetSurface() || !mOdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Texture())
    {
        return false;
    }

    FOdysseyStrokePoint lastPointInTexture = mCurrentPointInTexture;
    mCurrentPointInTexture = GetLocalMousePosition( iPointInViewport );
    mCurrentPointInTexture.keysDown = mKeysPressed;

    if( mCurrentToolState == eState::kIdle )
    {
        if( iKey == EKeys::LeftMouseButton && iEvent == EInputEvent::IE_Pressed )
        {
            mCurrentToolState = eState::kDrawing;
            //mOdysseyPainterEditorDataPtr.Pin()->BeginTransaction( LOCTEXT("Stroke in ILIAD", "Stroke in ILIAD") );
            //mOdysseyPainterEditorDataPtr.Pin()->MarkTransactionAsDirty();
			mOdysseyPainterEditorDataPtr.Pin()->PaintEngine()->BeginStroke( mCurrentPointInTexture, lastPointInTexture );
            return true;
        }
        else if( iKey == EKeys::Escape && iEvent == EInputEvent::IE_Pressed )
        {
			mOdysseyPainterEditorDataPtr.Pin()->PaintEngine()->AbortStroke();
            return true;
        }
        else if( iKey == EKeys::P && iEvent == EInputEvent::IE_Pressed )
        {
            mCurrentToolState = eState::kPan;
            return true;
        }
        else if( iKey == EKeys::R && iEvent == EInputEvent::IE_Pressed )
        {
            mCurrentToolState = eState::kRotate;
            return true;
        }
        else if( iKey == EKeys::LeftAlt && iEvent == EInputEvent::IE_Pressed )
        {
            mCurrentToolState = eState::kPick;
            return true;
        }
        else if( iKey == EKeys::MouseScrollUp )
        {
            FVector2D position_in_viewport( iPointInViewport.x, iPointInViewport.y );
            ZoomInInViewport( position_in_viewport );
            return true;
        }
        else if( iKey == EKeys::MouseScrollDown )
        {
            FVector2D position_in_viewport( iPointInViewport.x, iPointInViewport.y );
            ZoomOutInViewport( position_in_viewport );
            return true;
        }
    }

    else if( mCurrentToolState == eState::kDrawing )
    {
        if( iKey == EKeys::LeftMouseButton && iEvent == EInputEvent::IE_Released )
        {
            mCurrentToolState = eState::kIdle;

			mOdysseyPainterEditorDataPtr.Pin()->PaintEngine()->EndStroke();
            //mOdysseyPainterEditorDataPtr.Pin()->EndTransaction();

            return true;
        }
    }

    else if( mCurrentToolState == eState::kRotate )
    {
        if( iKey == EKeys::R && iEvent == EInputEvent::IE_Released )
        {
            mCurrentToolState = eState::kIdle;
            return true;
        }
        else if( iKey == EKeys::LeftMouseButton && iEvent == EInputEvent::IE_Pressed )
        {
            mCurrentToolState = eState::kRotating;

            FIntPoint size = mOdysseyPainterEditorViewportPtr.Pin()->GetViewport()->GetSizeXY();
            FVector2D center = FVector2D( size.X / 2, size.Y / 2 );
            FVector2D position_in_viewport( iPointInViewport.x, iPointInViewport.y );
            FVector2D deltaCenter = position_in_viewport - center;
            mRotationReference = FMath::Atan2( -deltaCenter.Y, deltaCenter.X );

            return true;
        }
    }
    else if( mCurrentToolState == eState::kRotating )
    {
        if( iKey == EKeys::LeftMouseButton && iEvent == EInputEvent::IE_Released )
        {
            mCurrentToolState = eState::kRotate;
            return true;
        }
        else if( iKey == EKeys::R && iEvent == EInputEvent::IE_Released )
        {
            mCurrentToolState = eState::kIdle;
            return true;
        }
    }

    else if( mCurrentToolState == eState::kPan )
    {
        if( iKey == EKeys::P && iEvent == EInputEvent::IE_Released )
        {
            mCurrentToolState = eState::kIdle;
            return true;
        }
        else if( iKey == EKeys::LeftMouseButton && iEvent == EInputEvent::IE_Pressed )
        {
            mCurrentToolState = eState::kPanning;

            mPanReference = FVector2D( iPointInViewport.x, iPointInViewport.y );
            return true;
        }
    }
    else if( mCurrentToolState == eState::kPanning )
    {
        if( iKey == EKeys::LeftMouseButton && iEvent == EInputEvent::IE_Released )
        {
            mCurrentToolState = eState::kPan;
            return true;
        }
        else if( iKey == EKeys::P && iEvent == EInputEvent::IE_Released )
        {
            mCurrentToolState = eState::kIdle;
            return true;
        }
    }

    else if( mCurrentToolState == eState::kPick )
    {
        if( iKey == EKeys::LeftAlt && iEvent == EInputEvent::IE_Released )
        {
            mCurrentToolState = eState::kIdle;
            return true;
        }
        else if( iKey == EKeys::LeftMouseButton && iEvent == EInputEvent::IE_Pressed )
        {
            mCurrentToolState = eState::kPicking;

            FOdysseyStrokePoint strokePoint_in_texture = GetLocalMousePosition(iPointInViewport);
            FVector2D position_in_texture(strokePoint_in_texture.x, strokePoint_in_texture.y );
			mOnPickColor.Broadcast(eOdysseyEventState::kAdjust, position_in_texture);

            return true;
        }
    }
    else if( mCurrentToolState == eState::kPicking )
    {
        if( iKey == EKeys::LeftMouseButton && iEvent == EInputEvent::IE_Released )
        {
            mCurrentToolState = eState::kPick;

            FOdysseyStrokePoint strokePoint_in_texture = GetLocalMousePosition(iPointInViewport);
            FVector2D position_in_texture(strokePoint_in_texture.x, strokePoint_in_texture.y);
			mOnPickColor.Broadcast(eOdysseyEventState::kSet, position_in_texture);
            return true;
        }
        else if( iKey == EKeys::LeftAlt && iEvent == EInputEvent::IE_Released )
        {
            mCurrentToolState = eState::kIdle;

            FOdysseyStrokePoint strokePoint_in_texture = GetLocalMousePosition(iPointInViewport);
            FVector2D position_in_texture(strokePoint_in_texture.x, strokePoint_in_texture.y);
			mOnPickColor.Broadcast(eOdysseyEventState::kSet, position_in_texture);
            return true;
        }
    }

    return false;
}

void
FOdysseyPainterEditorViewportClient::CapturedMouseMoveWithStrokePoint( const FOdysseyStrokePoint& iPointInViewport )
{
    //If we don't have a surface, then we don't interact with anything
    if (!mOdysseyPainterEditorViewportPtr.Pin()->GetSurface() || !mOdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Texture())
    {
        return;
    }

    
    FOdysseyStrokePoint lastPointInTexture = mCurrentPointInTexture;
    mCurrentPointInTexture = GetLocalMousePosition( iPointInViewport );
    mCurrentPointInTexture.keysDown = mKeysPressed;

    if( mCurrentToolState == eState::kDrawing )
    {
        auto paintengine = mOdysseyPainterEditorDataPtr.Pin()->PaintEngine();
        
        // if( paintengine->GetStokePaintOnTick() )
        //     return;

		if (long(mCurrentPointInTexture.x) == long(lastPointInTexture.x) && long(mCurrentPointInTexture.y) == long(lastPointInTexture.y))
			return;

        paintengine->PushStroke( mCurrentPointInTexture );
    }
    else if( mCurrentToolState == eState::kPanning )
    {
        FVector2D deltaReference( iPointInViewport.x - mPanReference.X, iPointInViewport.y - mPanReference.Y );
        FVector2D delta_in_viewport = FVector2D(0,0);

        float rotation = FMath::DegreesToRadians( mOdysseyPainterEditorViewportPtr.Pin()->GetRotationInDegrees() );

        delta_in_viewport.X = deltaReference.X * FMath::Cos( rotation ) + deltaReference.Y * FMath::Sin( rotation );
        delta_in_viewport.Y = -deltaReference.X * FMath::Sin( rotation ) + deltaReference.Y * FMath::Cos( rotation );

        mOdysseyPainterEditorViewportPtr.Pin()->AddPan( delta_in_viewport );
        mPanReference = FVector2D( iPointInViewport.x, iPointInViewport.y );
    }
    else if( mCurrentToolState == eState::kRotating )
    {
        FIntPoint size = mOdysseyPainterEditorViewportPtr.Pin()->GetViewport()->GetSizeXY();

        FVector2D center = FVector2D( size.X / 2, size.Y / 2 );
        FVector2D position_in_viewport( iPointInViewport.x, iPointInViewport.y );
        FVector2D deltaCenter = position_in_viewport - center;
        float newRotation = FMath::Atan2( -deltaCenter.Y, deltaCenter.X );
        float deltaRotation = mRotationReference - newRotation;

        mOdysseyPainterEditorViewportPtr.Pin()->SetRotationInDegrees( mOdysseyPainterEditorViewportPtr.Pin()->GetRotationInDegrees() + FMath::RadiansToDegrees( deltaRotation ) );

        mRotationReference = newRotation;
    }
    else if( mCurrentToolState == eState::kPicking )
    {
        FOdysseyStrokePoint strokePoint_in_texture = GetLocalMousePosition(iPointInViewport);
        FVector2D position_in_texture(strokePoint_in_texture.x, strokePoint_in_texture.y);

        if( position_in_texture.X >= 0 && position_in_texture.X < mOdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Texture()->Source.GetSizeX() &&
            position_in_texture.Y >= 0 && position_in_texture.Y < mOdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Texture()->Source.GetSizeY())
        {
			mOnPickColor.Broadcast(eOdysseyEventState::kAdjust, position_in_texture);
        }
    }
}

void
FOdysseyPainterEditorViewportClient::MouseEnter( FViewport* iViewport, int32 iX, int32 iY )
{
    if( mCurrentToolState == eState::kDrawing
        || mCurrentToolState == eState::kPanning
        || mCurrentToolState == eState::kRotating
        || mCurrentToolState == eState::kPicking )
        return;

    mCurrentToolState = eState::kIdle;
}

void
FOdysseyPainterEditorViewportClient::MouseLeave( FViewport* iViewport )
{
    if( mCurrentToolState == eState::kDrawing
        || mCurrentToolState == eState::kPanning
        || mCurrentToolState == eState::kRotating
        || mCurrentToolState == eState::kPicking )
        return;

    mCurrentToolState = eState::kIdle;
}


void
FOdysseyPainterEditorViewportClient::MouseMove(FViewport* iViewport, int32 iX, int32 iY)
{
    //If we don't have a surface, then we don't interact with anything
    if (!mOdysseyPainterEditorViewportPtr.Pin()->GetSurface() || !mOdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Texture())
    {
        return;
    }


    FOdysseyStrokePoint lastPointInTexture = mCurrentPointInTexture;
    mCurrentPointInTexture.x = iX;
    mCurrentPointInTexture.y = iY;
    mCurrentPointInTexture = GetLocalMousePosition(mCurrentPointInTexture);
    mCurrentPointInTexture.keysDown = mKeysPressed;

    if (mCurrentToolState == eState::kIdle)
    {
        auto paintengine = mOdysseyPainterEditorDataPtr.Pin()->PaintEngine();

        // if( paintengine->GetStokePaintOnTick() )
        //     return;

        if (long(mCurrentPointInTexture.x) == long(lastPointInTexture.x) && long(mCurrentPointInTexture.y) == long(lastPointInTexture.y))
            return;

        paintengine->SetCurrentStrokePoint(mCurrentPointInTexture);
    }
}

EMouseCursor::Type
FOdysseyPainterEditorViewportClient::GetCursor( FViewport* iViewport, int32 iX, int32 iY )
{
    if( mCurrentToolState == eState::kPan || mCurrentToolState == eState::kPanning )
        mCurrentMouseCursor = EMouseCursor::GrabHand;
    else if( mCurrentToolState == eState::kPick || mCurrentToolState == eState::kPicking )
        mCurrentMouseCursor = EMouseCursor::EyeDropper;
    else
        mCurrentMouseCursor = EMouseCursor::Crosshairs;

    return mCurrentMouseCursor;
}

TOptional< TSharedRef< SWidget > >
FOdysseyPainterEditorViewportClient::MapCursor( FViewport* iViewport, const FCursorReply& iCursorReply )
{
    return FViewportClient::MapCursor( iViewport, iCursorReply );
}

EMouseCaptureMode
FOdysseyPainterEditorViewportClient::GetMouseCaptureMode() const
{
    //No Capture, the capture is managed by FOdysseySceneViewport
    //Because this capture activates HighPrecisionMouseMovements, which is applying acceleration to the mouse (on Mac at least)
    //And we don't want that
    return EMouseCaptureMode::NoCapture;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------ FGCObject API
void
FOdysseyPainterEditorViewportClient::AddReferencedObjects( FReferenceCollector& ioCollector )
{
    ioCollector.AddReferencedObject( mCheckerboardTexture );
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API
void
FOdysseyPainterEditorViewportClient::ModifyCheckerboardTextureColors()
{
    DestroyCheckerboardTexture();

    const UOdysseyPainterEditorSettings& settings = *GetDefault< UOdysseyPainterEditorSettings >();
    mCheckerboardTexture = FImageUtils::CreateCheckerboardTexture( settings.CheckerColorOne, settings.CheckerColorTwo, settings.CheckerSize );
}

FText
FOdysseyPainterEditorViewportClient::GetDisplayedResolution() const
{
    uint32 height = 1;
    uint32 width = 1;
    mOdysseyPainterEditorViewportPtr.Pin()->CalculateTextureDisplayDimensions( width, height );
    return FText::Format( NSLOCTEXT( "OdysseyPainterEditor",
                                      "DisplayedResolution",
                                      "Displayed: {0}x{1}" ),
                           FText::AsNumber( FMath::Max( uint32( 1 ), width ) ),
                           FText::AsNumber( FMath::Max( uint32( 1 ), height ) ) );
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------- Private API
void
FOdysseyPainterEditorViewportClient::UpdateScrollBars()
{
    TSharedPtr<SOdysseySurfaceViewport> viewport = mOdysseyPainterEditorViewportPtr.Pin();

    if( !viewport.IsValid() || !viewport->GetVerticalScrollBar().IsValid() || !viewport->GetHorizontalScrollBar().IsValid() )
        return;

    float vRatio = mOdysseyPainterEditorViewportPtr.Pin()->GetViewportVerticalScrollBarRatio();
    float hRatio = mOdysseyPainterEditorViewportPtr.Pin()->GetViewportHorizontalScrollBarRatio();
    float vDistFromBottom = viewport->GetVerticalScrollBar()->DistanceFromBottom();
    float hDistFromBottom = viewport->GetHorizontalScrollBar()->DistanceFromBottom();

    if( vRatio < 1.0f )
    {
        if( vDistFromBottom < 1.0f )
        {
            viewport->GetVerticalScrollBar()->SetState( FMath::Clamp( 1.0f - vRatio - vDistFromBottom, 0.0f, 1.0f ), vRatio );
        }
        else
        {
            viewport->GetVerticalScrollBar()->SetState( 0.0f, vRatio );
        }
    }

    if( hRatio < 1.0f )
    {
        if( hDistFromBottom < 1.0f )
        {
            viewport->GetHorizontalScrollBar()->SetState( FMath::Clamp( 1.0f - hRatio - hDistFromBottom, 0.0f, 1.0f ), hRatio );
        }
        else
        {
            viewport->GetHorizontalScrollBar()->SetState( 0.0f, hRatio );
        }
    }
}

FVector2D
FOdysseyPainterEditorViewportClient::GetViewportScrollBarPositions() const
{
    FVector2D positions = FVector2D::ZeroVector;
    if( mOdysseyPainterEditorViewportPtr.Pin()->GetVerticalScrollBar().IsValid() && mOdysseyPainterEditorViewportPtr.Pin()->GetHorizontalScrollBar().IsValid() )
    {
        uint32 width, height;
        float vRatio = mOdysseyPainterEditorViewportPtr.Pin()->GetViewportVerticalScrollBarRatio();
        float hRatio = mOdysseyPainterEditorViewportPtr.Pin()->GetViewportHorizontalScrollBarRatio();
        float vDistFromBottom = mOdysseyPainterEditorViewportPtr.Pin()->GetVerticalScrollBar()->DistanceFromBottom();
        float hDistFromBottom = mOdysseyPainterEditorViewportPtr.Pin()->GetHorizontalScrollBar()->DistanceFromBottom();

        mOdysseyPainterEditorViewportPtr.Pin()->CalculateTextureDisplayDimensions( width, height );

        if( ( mOdysseyPainterEditorViewportPtr.Pin()->GetVerticalScrollBar()->GetVisibility() == EVisibility::Visible ) && vDistFromBottom < 1.0f )
        {
            positions.Y = FMath::Clamp( 1.0f - vRatio - vDistFromBottom, 0.0f, 1.0f ) * height;
        }
        else
        {
            positions.Y = 0.0f;
        }

        if( ( mOdysseyPainterEditorViewportPtr.Pin()->GetHorizontalScrollBar()->GetVisibility() == EVisibility::Visible ) && hDistFromBottom < 1.0f )
        {
            positions.X = FMath::Clamp( 1.0f - hRatio - hDistFromBottom, 0.0f, 1.0f ) * width;
        }
        else
        {
            positions.X = 0.0f;
        }
    }

    return positions;
}

void
FOdysseyPainterEditorViewportClient::DestroyCheckerboardTexture()
{
    if( mCheckerboardTexture )
    {
        if( mCheckerboardTexture->Resource )
        {
            mCheckerboardTexture->ReleaseResource();
        }
        mCheckerboardTexture->MarkPendingKill();
        mCheckerboardTexture = NULL;
    }
}

void
FOdysseyPainterEditorViewportClient::ZoomInInViewport( const FVector2D& iPositionInViewport )
{
    //If we don't have a surface, then we don't interact with anything
    if (!mOdysseyPainterEditorViewportPtr.Pin()->GetSurface() || !mOdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Texture())
    {
        return;
    }

    FVector2D pan = mOdysseyPainterEditorViewportPtr.Pin()->GetPan();

    // Diff between before and after the zoom
    float oldHScrollSize = mOdysseyPainterEditorViewportPtr.Pin()->GetViewportHorizontalScrollBarRatio();
    float oldVScrollSize = mOdysseyPainterEditorViewportPtr.Pin()->GetViewportVerticalScrollBarRatio();

    mOdysseyPainterEditorViewportPtr.Pin()->ZoomIn();

    float newHScrollSize = mOdysseyPainterEditorViewportPtr.Pin()->GetViewportHorizontalScrollBarRatio();
    float newVScrollSize = mOdysseyPainterEditorViewportPtr.Pin()->GetViewportVerticalScrollBarRatio();


    float hScrollSizeDiff = newHScrollSize - oldHScrollSize;
    float vScrollSizeDiff = newVScrollSize - oldVScrollSize;
    //-------

    //Useful variables to determine the new position of the scrollbars
    float vDistFromBottom = mOdysseyPainterEditorViewportPtr.Pin()->GetVerticalScrollBar()->DistanceFromBottom();
    float hDistFromBottom = mOdysseyPainterEditorViewportPtr.Pin()->GetHorizontalScrollBar()->DistanceFromBottom();

    float xCursorOnViewport = iPositionInViewport.X;
    float yCursorOnViewport = iPositionInViewport.Y;

    FIntPoint size = mOdysseyPainterEditorViewportPtr.Pin()->GetViewport()->GetSizeXY();

    float ratioX = ( xCursorOnViewport - pan.X ) / FMath::Max( float( size.X ), 1.f );
    float ratioY = ( yCursorOnViewport - pan.Y ) / FMath::Max( float( size.Y ), 1.f );
    //------

    //Set the scrollbars
    mOdysseyPainterEditorViewportPtr.Pin()->GetHorizontalScrollBar()->SetState( FMath::Clamp( 1 - ( hDistFromBottom + newHScrollSize - hScrollSizeDiff * ( 1 - ratioX ) ), 0.0f, 1.0f - newHScrollSize ), newHScrollSize );
    mOdysseyPainterEditorViewportPtr.Pin()->GetVerticalScrollBar()->SetState( FMath::Clamp( 1 - ( vDistFromBottom + newVScrollSize - vScrollSizeDiff * ( 1 - ratioY ) ), 0.0f, 1.0f - newVScrollSize ), newVScrollSize );
}

void
FOdysseyPainterEditorViewportClient::ZoomOutInViewport( const FVector2D& iPositionInViewport )
{
    //If we don't have a surface, then we don't interact with anything
    if (!mOdysseyPainterEditorViewportPtr.Pin()->GetSurface() || !mOdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Texture())
    {
        return;
    }

    FVector2D pan = mOdysseyPainterEditorViewportPtr.Pin()->GetPan();

    // Diff between before and after the zoom
    float oldHScrollSize = mOdysseyPainterEditorViewportPtr.Pin()->GetViewportHorizontalScrollBarRatio();
    float oldVScrollSize = mOdysseyPainterEditorViewportPtr.Pin()->GetViewportVerticalScrollBarRatio();

    mOdysseyPainterEditorViewportPtr.Pin()->ZoomOut();

    float newHScrollSize = mOdysseyPainterEditorViewportPtr.Pin()->GetViewportHorizontalScrollBarRatio();
    float newVScrollSize = mOdysseyPainterEditorViewportPtr.Pin()->GetViewportVerticalScrollBarRatio();


    float hScrollSizeDiff = newHScrollSize - oldHScrollSize;
    float vScrollSizeDiff = newVScrollSize - oldVScrollSize;
    //-------

    //Useful variables to determine the new position of the scrollbars
    float vDistFromBottom = mOdysseyPainterEditorViewportPtr.Pin()->GetVerticalScrollBar()->DistanceFromBottom();
    float hDistFromBottom = mOdysseyPainterEditorViewportPtr.Pin()->GetHorizontalScrollBar()->DistanceFromBottom();

    float xCursorOnViewport = iPositionInViewport.X;
    float yCursorOnViewport = iPositionInViewport.Y;

    FIntPoint size = mOdysseyPainterEditorViewportPtr.Pin()->GetViewport()->GetSizeXY();

    float ratioX = ( xCursorOnViewport - pan.X ) / FMath::Max( float( size.X ), 1.f );
    float ratioY = ( yCursorOnViewport - pan.Y ) / FMath::Max( float( size.Y ), 1.f );
    //------

    //Set the scrollbars
    mOdysseyPainterEditorViewportPtr.Pin()->GetHorizontalScrollBar()->SetState( FMath::Clamp( 1 - ( hDistFromBottom + newHScrollSize - hScrollSizeDiff * ( 1 - ratioX ) ), 0.0f, 1.0f - newHScrollSize ), newHScrollSize );
    mOdysseyPainterEditorViewportPtr.Pin()->GetVerticalScrollBar()->SetState( FMath::Clamp( 1 - ( vDistFromBottom + newVScrollSize - vScrollSizeDiff * ( 1 - ratioY ) ), 0.0f, 1.0f - newVScrollSize ), newVScrollSize );
}

double
FOdysseyPainterEditorViewportClient::GetZoom() const
{
    //If we don't have a surface, then we return a default 100% zoom
    if (!mOdysseyPainterEditorViewportPtr.Pin()->GetSurface() || !mOdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Texture())
    {
        return 1.0;
    }

    double zoom = 1.0;
    bool fitToViewport = mOdysseyPainterEditorViewportPtr.Pin()->GetFitToViewport();

    if( fitToViewport )
    {
        //The member zoom is overriden by the fit to viewport. The drawing function uses another way to calculate the effective zoom, and we do the same here
        uint32 width, height;
        mOdysseyPainterEditorViewportPtr.Pin()->CalculateTextureDisplayDimensions( width, height );
        zoom = static_cast<double>( width ) / static_cast<double>( mOdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Texture()->GetSizeX() );
    }
    else
    {
        //The member zoom is used to draw the viewport, we can use it
        zoom = mOdysseyPainterEditorViewportPtr.Pin()->GetZoom();
    }

    return zoom;
}

FVector2D
FOdysseyPainterEditorViewportClient::GetLocalMousePosition( const FVector2D& iMouseInViewport, const bool iWithRotation ) const
{
    //If we don't have a surface, then we don't have a local mouse position
    if (!mOdysseyPainterEditorViewportPtr.Pin()->GetSurface() || !mOdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Texture())
    {
        return FVector2D(0,0);
    }

    double zoom = GetZoom();
    FVector2D pan = mOdysseyPainterEditorViewportPtr.Pin()->GetPan();
    FVector2D textureViewportPosition = GetViewportScrollBarPositions();
    FVector2D ratio = FVector2D( mOdysseyPainterEditorViewportPtr.Pin()->GetViewportHorizontalScrollBarRatio(), mOdysseyPainterEditorViewportPtr.Pin()->GetViewportVerticalScrollBarRatio() );
    FVector2D viewportSize = FVector2D( mOdysseyPainterEditorViewportPtr.Pin()->GetViewport()->GetSizeXY().X, mOdysseyPainterEditorViewportPtr.Pin()->GetViewport()->GetSizeXY().Y );
    int32 yOffset = ( ratio.Y > 1.0f ) ? ( ( viewportSize.Y - ( viewportSize.Y / ratio.Y ) ) * 0.5f ) : 0;
    int32 xOffset = ( ratio.X > 1.0f ) ? ( ( viewportSize.X - ( viewportSize.X / ratio.X ) ) * 0.5f ) : 0;

    int textureWidth = mOdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Texture()->GetSizeX();
    int textureHeight = mOdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Texture()->GetSizeY();

    FVector2D texturePanPivot = FVector2D( mPivotPointRatio.X * textureWidth, mPivotPointRatio.Y * textureHeight ) - 0.5 * FVector2D( textureWidth, textureHeight );

    FVector2D position = FVector2D( ( iMouseInViewport.X + textureViewportPosition.X - xOffset - pan.X ) / zoom, ( iMouseInViewport.Y + textureViewportPosition.Y - yOffset - pan.Y ) / zoom );

    if( iWithRotation )
    {
        if( mOdysseyPainterEditorViewportPtr.Pin()->GetRotationInDegrees() != 0 )
        {
            float rotation = FMath::DegreesToRadians( mOdysseyPainterEditorViewportPtr.Pin()->GetRotationInDegrees() );
            FVector2D center = FVector2D(textureWidth, textureHeight) / 2;
            position -= center;

            FVector2D pivotPan = texturePanPivot;
            pivotPan.X = texturePanPivot.X * FMath::Cos( rotation ) + texturePanPivot.Y * FMath::Sin( rotation ) - texturePanPivot.X;
            pivotPan.Y = -texturePanPivot.X * FMath::Sin( rotation ) + texturePanPivot.Y * FMath::Cos( rotation ) - texturePanPivot.Y;

            float x = position.X * FMath::Cos( rotation ) + position.Y * FMath::Sin( rotation ) + center.X;
            float y = -position.X * FMath::Sin( rotation ) + position.Y * FMath::Cos( rotation ) + center.Y;

            position.X = x - pivotPan.X;
            position.Y = y - pivotPan.Y;
        }
    }

    return position;
}

FOdysseyStrokePoint
FOdysseyPainterEditorViewportClient::GetLocalMousePosition( const FOdysseyStrokePoint& iPointInViewport ) const
{
    FVector2D position_in_viewport( iPointInViewport.x, iPointInViewport.y );
    FVector2D position_in_texture = GetLocalMousePosition( position_in_viewport );

    FOdysseyStrokePoint point_in_texture( iPointInViewport );
    point_in_texture.x = position_in_texture.X;
    point_in_texture.y = position_in_texture.Y;

    //Convert the position from the displayed texture size to the the position in the texture source size
    UTexture2D* texture = mOdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Texture();

    uint32 textureFullWidth = texture->Source.GetSizeX();
    uint32 textureFullHeight = texture->Source.GetSizeY();
    switch (texture->PowerOfTwoMode)
    {
    case ETexturePowerOfTwoSetting::None:
        break;

    case ETexturePowerOfTwoSetting::PadToPowerOfTwo:
        textureFullWidth = FMath::RoundUpToPowerOfTwo(textureFullWidth);
        textureFullHeight = FMath::RoundUpToPowerOfTwo(textureFullHeight);
        break;

    case ETexturePowerOfTwoSetting::PadToSquarePowerOfTwo:
        textureFullWidth = textureFullHeight = FMath::Max(FMath::RoundUpToPowerOfTwo(textureFullWidth), FMath::RoundUpToPowerOfTwo(textureFullHeight));
        break;

    default:
        checkf(false, TEXT("Unknown entry in ETexturePowerOfTwoSetting::Type"));
        break;
    }

    point_in_texture.x = point_in_texture.x * textureFullWidth / texture->GetSizeX();
    point_in_texture.y = point_in_texture.y * textureFullHeight / texture->GetSizeY();

    return point_in_texture;
}

void 
FOdysseyPainterEditorViewportClient::DrawUVsOntoViewport( const FViewport* iViewport, FCanvas* ioCanvas, int32 iUVChannel, const FStaticMeshVertexBuffer& iVertexBuffer, const FIndexArrayView& iIndices )
{
    //If we don't have a surface, we can't draw UVs
    if (!mOdysseyPainterEditorViewportPtr.Pin()->GetSurface() || !mOdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Texture())
    {
        return;
    }

    FVector2D pan = mOdysseyPainterEditorViewportPtr.Pin()->GetPan();

    uint32 width, height;
    mOdysseyPainterEditorViewportPtr.Pin()->CalculateTextureDisplayDimensions( width, height );
    float rotation = -FMath::DegreesToRadians( mOdysseyPainterEditorViewportPtr.Pin()->GetRotationInDegrees() );

    double zoom = GetZoom();
    FVector2D ratio = FVector2D( mOdysseyPainterEditorViewportPtr.Pin()->GetViewportHorizontalScrollBarRatio(), mOdysseyPainterEditorViewportPtr.Pin()->GetViewportVerticalScrollBarRatio() );
    FVector2D viewportSize = FVector2D( mOdysseyPainterEditorViewportPtr.Pin()->GetViewport()->GetSizeXY().X, mOdysseyPainterEditorViewportPtr.Pin()->GetViewport()->GetSizeXY().Y );
    int32 YOffset = ( ratio.Y > 1.0f ) ? ( ( viewportSize.Y - ( viewportSize.Y / ratio.Y ) ) * 0.5f ) : 0;
    int32 XOffset = ( ratio.X > 1.0f ) ? ( ( viewportSize.X - ( viewportSize.X / ratio.X ) ) * 0.5f ) : 0;

    FVector2D scrollBarPos = GetViewportScrollBarPositions();
    int32 yPos = FMath::Min( int( YOffset - scrollBarPos.Y ), 0 );
    int32 xPos = FMath::Min( int( XOffset - scrollBarPos.X ), 0 );

    if( ( (uint32)iUVChannel < iVertexBuffer.GetNumTexCoords() ) )
    {
        //calculate scaling
        const int32 minY = YOffset;
        const int32 minX = XOffset;
        
        FVector2D pivotPan;
        
        pivotPan.X = pan.X * FMath::Cos( rotation ) + pan.Y * FMath::Sin( rotation );
        pivotPan.Y = -pan.X * FMath::Sin( rotation ) + pan.Y * FMath::Cos( rotation );
        
        const FVector2D uvBoxOrigin( minX + xPos + pivotPan.X, minY + yPos + pivotPan.Y );

        // If we want to draw a bounding box to the UV
        // FCanvasTileItem BoxBackgroundTileItem(uvBoxOrigin, GWhiteTexture, FVector2D(Width, Height), FLinearColor(0, 0, 0, 0.0f));
        // BoxBackgroundTileItem.PivotPoint = FVector2D( 0.5, 0.5 );
        // BoxBackgroundTileItem.Rotation.Add(0, mOdysseyPainterEditorViewportPtr.Pin()->GetRotationInDegrees(), 0 );
        // BoxBackgroundTileItem.BlendMode = SE_BLEND_AlphaComposite;
        // InCanvas->DrawItem(BoxBackgroundTileItem);

        //draw triangles
        uint32 numIndices = iIndices.Num();
        FCanvasLineItem lineItem; // still useful ?!
        for( uint32 i = 0; i < numIndices - 2; i += 3 )
        {
            FVector2D UVs[3];
            bool isOutOfBounds[3];

            float fudgeFactor = 1.0f / 1024.0f;
            for( int32 Corner = 0; Corner < 3; Corner++ )
            {
                UVs[Corner] = ( iVertexBuffer.GetVertexUV( iIndices[i + Corner], iUVChannel ) );

                isOutOfBounds[Corner] = ( UVs[Corner].X < -fudgeFactor || UVs[Corner].X >( 1.0f + fudgeFactor ) ) || ( UVs[Corner].Y < -fudgeFactor || UVs[Corner].Y >( 1.0f + fudgeFactor ) );
            }

            // Clamp the UV triangle to the [0,1] range (with some fudge).
            int32 numUVs = 3;
            fudgeFactor = 0.1f;
            FVector2D bias( 0.0f, 0.0f );

            float minU = UVs[0].X;
            float minV = UVs[0].Y;
            for( int32 j = 1; j < numUVs; ++j )
            {
                minU = FMath::Min( minU, UVs[j].X );
                minV = FMath::Min( minU, UVs[j].Y );
            }

            if( minU < -fudgeFactor || minU >( 1.0f + fudgeFactor ) )
            {
                bias.X = FMath::FloorToFloat( minU );
            }
            if( minV < -fudgeFactor || minV >( 1.0f + fudgeFactor ) )
            {
                bias.Y = FMath::FloorToFloat( minV );
            }

            for( int32 j = 0; j < numUVs; j++ )
            {
                UVs[j] += bias;
            }

            int maxX = iViewport->GetSizeXY().X;
            int maxY = iViewport->GetSizeXY().Y;
            float distSquared = ( maxX * maxX / 2 + maxY * maxY / 2 );

            for( int32 edge = 0; edge < 3; edge++ )
            {
                int32 corner1 = edge;
                int32 corner2 = ( edge + 1 ) % 3;
                FLinearColor lc = mMeshSelector->GetMeshColor();
                ::ul3::FPixelValue c = ::ul3::FPixelValue::FromRGBAF( lc.R, lc.G, lc.B, lc.A );

                FLinearColor color = ( isOutOfBounds[corner1] || isOutOfBounds[corner2] ) ? FLinearColor( 0.6f, 0.0f, 0.0f ) : FLinearColor( c.RedF(), c.GreenF(), c.BlueF(), c.AlphaF() );
                lineItem.SetColor( color );

                if( mOdysseyPainterEditorViewportPtr.Pin()->GetRotationInDegrees() != 0 )
                {
                    FVector2D center = FVector2D( uvBoxOrigin.X + ( ( mOdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Width() * zoom ) / 2 ), uvBoxOrigin.Y + ( ( mOdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Height() * zoom ) / 2 ) );

                    FVector2D positionP1( UVs[corner1] * FVector2D( width, height ) + uvBoxOrigin );
                    FVector2D positionP2( UVs[corner2] * FVector2D( width, height ) + uvBoxOrigin );
                    positionP1 -= center;
                    positionP2 -= center;

                    //Clipping. We don't even begin to calculate the coordinates of points we know won't be visible in the viewport
                    if( ( ( positionP1.X * positionP1.X + positionP1.Y * positionP1.Y ) > distSquared && ( positionP2.X * positionP2.X + positionP2.Y * positionP2.Y ) > distSquared ) )
                    {
                        continue;
                    }

                    float x1 = positionP1.X * FMath::Cos( rotation ) + positionP1.Y * FMath::Sin( rotation ) + center.X;
                    float y1 = -positionP1.X * FMath::Sin( rotation ) + positionP1.Y * FMath::Cos( rotation ) + center.Y;

                    float x2 = positionP2.X * FMath::Cos( rotation ) + positionP2.Y * FMath::Sin( rotation ) + center.X;
                    float y2 = -positionP2.X * FMath::Sin( rotation ) + positionP2.Y * FMath::Cos( rotation ) + center.Y;

                    FVector origin = FVector();
                    FVector endPos = FVector();

                    origin.X = x1;
                    origin.Y = y1;
                    origin.Z = 0;
                    endPos.X = x2;
                    endPos.Y = y2;
                    endPos.Z = 0;

                    FBatchedElements* batchedElements = ioCanvas->GetBatchedElements( FCanvas::ET_Line );
                    FHitProxyId hitProxyId = ioCanvas->GetHitProxyId();
                    batchedElements->AddTranslucentLine( origin, endPos, color, hitProxyId, 1.f );
                }
                else
                {
                    FVector2D origin2D = FVector2D( UVs[corner1] * FVector2D( width, height ) + uvBoxOrigin );
                    FVector2D endPos2D = FVector2D( UVs[corner2] * FVector2D( width, height ) + uvBoxOrigin );

                    FVector origin = FVector();
                    FVector endPos = FVector();

                    origin.X = origin2D.X;
                    origin.Y = origin2D.Y;
                    origin.Z = 0;
                    endPos.X = endPos2D.X;
                    endPos.Y = endPos2D.Y;
                    endPos.Z = 0;

                    //Clipping. We don't even begin to calculate the coordinates of points we know won't be visible in the viewport
                    if( ( origin.X < 0 && endPos.X < 0 ) || ( origin.X > maxX && endPos.X > maxX ) || ( origin.Y < 0 && endPos.Y < 0 ) || ( origin.Y > maxY && endPos.Y > maxY ) )
                    {
                        continue;
                    }

                    FBatchedElements* batchedElements = ioCanvas->GetBatchedElements( FCanvas::ET_Line );
                    FHitProxyId hitProxyId = ioCanvas->GetHitProxyId();
                    batchedElements->AddTranslucentLine( origin, endPos, color, hitProxyId, 1.f );
                }
            }
        }
    }
}

#undef LOCTEXT_NAMESPACE
