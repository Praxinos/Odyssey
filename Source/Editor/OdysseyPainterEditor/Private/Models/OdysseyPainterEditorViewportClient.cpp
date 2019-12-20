// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

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
#include "Slate/SceneViewport.h"
#include "Texture2DPreview.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "UnrealEdGlobals.h"
#include "VolumeTexturePreview.h"
#include "Widgets/Layout/SScrollBar.h"
#include "Widgets/SViewport.h"

#include "IOdysseyStylusInputModule.h"
#include "OdysseyLayerStack.h"
#include "OdysseyPaintEngine.h"
#include "OdysseyPainterEditorSettings.h"
#include "OdysseyPainterEditorToolkit.h"
#include "OdysseySurface.h"
#include "SOdysseyCursorWidget.h"
#include "SOdysseySurfaceViewport.h"

#include <memory>
#include <ULIS_CORE>

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorViewportClientt"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorViewportClient
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorViewportClient::FOdysseyPainterEditorViewportClient( TWeakPtr< IOdysseyPainterEditorToolkit >  iOdysseyPainterEditor,
                                                                          TWeakPtr< SOdysseySurfaceViewport >       iOdysseyPainterEditorViewport,
                                                                          FOdysseyMeshSelector*                     iMeshSelector)
    : InputSubsystem( nullptr )
    , mLastKey( EKeys::Invalid )
    , mLastEvent( EInputEvent::IE_MAX )
    , mMouseCaptureMode( FViewportClient::CaptureMouseOnClick() )
    , mOdysseyPainterEditorPtr( iOdysseyPainterEditor )
    , mOdysseyPainterEditorViewportPtr( iOdysseyPainterEditorViewport )
    , mMeshSelector( iMeshSelector )
    , mCheckerboardTexture( NULL )
    , mCurrentMouseCursor( EMouseCursor::Default )
    , mPivotPointRatio( FVector2D( 0.5, 0.5 ) )
    , mCurrentToolState( eState::kIdle )
{
    check( mOdysseyPainterEditorPtr.IsValid() &&
           mOdysseyPainterEditorViewportPtr.IsValid() );

    InputSubsystem = GEditor->GetEditorSubsystem<UOdysseyStylusInputSubsystem>();

    //PATCH: as I don't know how to initialize usubsystem inside settings ctor (as usubsystem are called after)
    UOdysseyPainterEditorSettings* settings = GetMutableDefault< UOdysseyPainterEditorSettings >();
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
    //UE_LOG( LogStylusInput, Log, TEXT("OnStylusInputChanged") );

    mMouseCaptureMode = FViewportClient::CaptureMouseOnClick();
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------ FViewportClient API
void
FOdysseyPainterEditorViewportClient::Draw( FViewport* iViewport, FCanvas* ioCanvas )
{
    if( !mOdysseyPainterEditorPtr.IsValid() )
        return;

    // Draw on tick or catch up
    /*
    auto paintengine = mOdysseyPainterEditorPtr.Pin()->PaintEngine();
    FVector2D oldpoint = FVector2D( RefEventStrokePoint.x, RefEventStrokePoint.y );
    FVector2D position_in_viewport( Viewport->GetMouseX(), Viewport->GetMouseY() );
    FVector2D position_in_texture = GetLocalMousePosition( position_in_viewport );
    if( mCurrentToolState == eState::kDrawing
    && ( paintengine->GetStokePaintOnTick() || ( paintengine->GetSmoothingCatchUp() && oldpoint != position_in_texture ) ) )
    {
        FOdysseyStrokePoint point = RefEventStrokePoint;
        point.x = position_in_texture.X;
        point.y = position_in_texture.Y;
        paintengine->PushStroke( point );
    }
    */

    // Send Tick to PaintEngine
    mOdysseyPainterEditorPtr.Pin()->PaintEngine()->Tick();

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
    const UOdysseyPainterEditorSettings& settings = *GetDefault<UOdysseyPainterEditorSettings>();
    ioCanvas->Clear( settings.BackgroundColor );
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
    const float mipLevel = 1;

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
    mPivotPointRatio = FVector2D( position_in_texture.X / mOdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Width(), position_in_texture.Y / mOdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Height() );

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

        /* TODO: Unreal BoxItem doesn't support rotation, so we can't draw it properly. We'll have to come up with our own HUD
        // Draw a white border around the texture to show its extents
        if( settings.TextureBorderEnabled )
        {
            FCanvasBoxItem boxItem( FVector2D( xPos, yPos ), FVector2D( width , height ) );
            boxItem.SetColor( Settings.TextureBorderColor );
            Canvas->DrawItem( boxItem );
        }
        */
    }

    // Draw Cursor Preview
    auto paintEngine = mOdysseyPainterEditorPtr.Pin()->PaintEngine();
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

    mLastKey = iKey;
    mLastEvent = iEvent;

    if( mMouseCaptureMode == EMouseCaptureMode::NoCapture
        && ( iKey == EKeys::LeftMouseButton
             || iKey == EKeys::RightMouseButton ) )
        return true;

    FOdysseyStrokePoint point_in_viewport( FOdysseyStrokePoint::DefaultPoint() );
    point_in_viewport.x = iViewport->GetMouseX();
    point_in_viewport.y = iViewport->GetMouseY();
    return InputKeyWithStrokePoint( point_in_viewport, iControllerId, iKey, iEvent, iAmountDepressed, iGamepad );
}

void
FOdysseyPainterEditorViewportClient::CapturedMouseMove( FViewport* iViewport, int32 iX, int32 iY )
{
    FOdysseyStrokePoint point_in_viewport( FOdysseyStrokePoint::DefaultPoint() );
    point_in_viewport.x = iX;
    point_in_viewport.y = iY;
    CapturedMouseMoveWithStrokePoint( point_in_viewport );
}

void
FOdysseyPainterEditorViewportClient::OnStylusStateChanged( const TWeakPtr<SWidget> iWidget, const FStylusState& iState, int32 iIndex )
{
    mMouseCaptureMode = EMouseCaptureMode::NoCapture;

    //---

    if( !iWidget.IsValid() )
        return;

    TSharedPtr< SViewport > viewport = mOdysseyPainterEditorViewportPtr.Pin()->GetViewportWidget();
    const SWidget* widget = viewport.Get();

    if( iWidget.Pin().Get() != widget )
        return;

    //---

    //UE_LOG( LogStylusInput, Log, TEXT("OnStylusStateChanged index:%d x:%f y:%f pressure:%f down:%d tilt:%f %f azimuth:%f altitude:%f"), iIndex, 
    //        iState.GetPosition().X, iState.GetPosition().Y, 
    //        iState.GetPressure(), iState.IsStylusDown(), 
    //        iState.GetTilt().X, iState.GetTilt().Y, 
    //        iState.GetAzimuth(), iState.GetAltitude() );

    float scale_dpi = mOdysseyPainterEditorViewportPtr.Pin()->GetViewport()->GetCachedGeometry().GetAccumulatedLayoutTransform().GetScale();
    FVector2D position_in_viewport = widget->GetCachedGeometry().AbsoluteToLocal( iState.GetPosition() ) * scale_dpi;
	
    //UE_LOG( LogStylusInput, Log, TEXT( "OnStylusStateChanged AbsoluteToLocal: screen:%f %f -> local (ref widget): %f %f" ), State.GetPosition().X, State.GetPosition().Y, local.X, local.Y );

    FOdysseyStrokePoint stroke_point( position_in_viewport.X
                                      , position_in_viewport.Y
                                      , iState.GetZ()
                                      , iState.GetPressure()
                                      , iState.GetAltitude()
                                      , iState.GetAzimuth()
                                      , iState.GetTwist()
                                      , 0 //iState.GetPitch()
                                      , 0 // iState.GetRoll()
                                      , 0 ); // iState.GetYaw() );

  //---

    static TQueue< FOdysseyStrokePoint > queue;

    if( iState.IsStylusDown() )
        queue.Enqueue( stroke_point );
    else
        queue.Empty();

    static bool is_dragging = false;
    if( mLastKey == EKeys::LeftMouseButton && mLastEvent == EInputEvent::IE_Pressed )
    {
        is_dragging = true;

        FOdysseyStrokePoint first;
        queue.Dequeue( first );
        InputKeyWithStrokePoint( first, 0, mLastKey, mLastEvent );
    }
    else if( mLastKey == EKeys::LeftMouseButton && mLastEvent == EInputEvent::IE_Released )
    {
        FOdysseyStrokePoint last;
        queue.Dequeue( last );
        InputKeyWithStrokePoint( last, 0, mLastKey, mLastEvent );

        is_dragging = false;
    }
    else if( is_dragging ) // MUSTY BE the last, or at least after "is_dragging = false;"
    {
        FOdysseyStrokePoint point;
        while( queue.Dequeue( point ) ) // Process all the down'd points which occur before having the ue pressed event
            CapturedMouseMoveWithStrokePoint( point );
    }

    mLastKey = EKeys::Invalid;
    mLastEvent = EInputEvent::IE_MAX;
}

bool
FOdysseyPainterEditorViewportClient::InputKeyWithStrokePoint( const FOdysseyStrokePoint& iPointInViewport, int32 iControllerId, FKey iKey, EInputEvent iEvent, float iAmountDepressed, bool iGamepad )
{
    if( mCurrentToolState == eState::kIdle )
    {
        if( iKey == EKeys::LeftMouseButton && iEvent == EInputEvent::IE_Pressed )
        {
            mCurrentToolState = eState::kDrawing;
            //mOdysseyPainterEditorPtr.Pin()->BeginTransaction( LOCTEXT("Stroke in ILIAD", "Stroke in ILIAD") );
            //mOdysseyPainterEditorPtr.Pin()->MarkTransactionAsDirty();

            FOdysseyStrokePoint point_in_texture = GetLocalMousePosition( iPointInViewport );
            mOdysseyPainterEditorPtr.Pin()->PaintEngine()->PushStroke( point_in_texture );

            return true;
        }
        else if( iKey == EKeys::Escape && iEvent == EInputEvent::IE_Pressed )
        {
            mOdysseyPainterEditorPtr.Pin()->PaintEngine()->AbortStroke();
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

            mOdysseyPainterEditorPtr.Pin()->PaintEngine()->EndStroke();
            //mOdysseyPainterEditorPtr.Pin()->EndTransaction();

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

            FVector2D position_in_viewport( iPointInViewport.x, iPointInViewport.y );
            FVector2D position_in_texture = GetLocalMousePosition( position_in_viewport );
            mOdysseyPainterEditorPtr.Pin()->SetColor( mOdysseyPainterEditorPtr.Pin()->LayerStack()->GetResultBlock()->GetIBlock()->PixelColor( position_in_texture.X, position_in_texture.Y ) );

            return true;
        }
    }
    else if( mCurrentToolState == eState::kPicking )
    {
        if( iKey == EKeys::LeftMouseButton && iEvent == EInputEvent::IE_Released )
        {
            mCurrentToolState = eState::kPick;
            return true;
        }
        else if( iKey == EKeys::LeftAlt && iEvent == EInputEvent::IE_Released )
        {
            mCurrentToolState = eState::kIdle;
            return true;
        }
    }

    return false;
}

void
FOdysseyPainterEditorViewportClient::CapturedMouseMoveWithStrokePoint( const FOdysseyStrokePoint& iPointInViewport )
{
    if( mCurrentToolState == eState::kDrawing )
    {
        auto paintengine = mOdysseyPainterEditorPtr.Pin()->PaintEngine();
        /*
        if( paintengine->GetStokePaintOnTick() )
            return;
        */

        FOdysseyStrokePoint point_in_texture = GetLocalMousePosition( iPointInViewport );
        paintengine->PushStroke( point_in_texture );
    }
    else if( mCurrentToolState == eState::kPanning )
    {
        FVector2D deltaReference( iPointInViewport.x - mPanReference.X, iPointInViewport.y - mPanReference.Y );
        FVector2D delta_in_viewport = FVector2D();

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
        FVector2D position_in_viewport( iPointInViewport.x, iPointInViewport.y );
        FVector2D position_in_texture = GetLocalMousePosition( position_in_viewport );
        mOdysseyPainterEditorPtr.Pin()->SetColor( mOdysseyPainterEditorPtr.Pin()->LayerStack()->GetResultBlock()->GetIBlock()->PixelColor( position_in_texture.X, position_in_texture.Y ) );
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
FOdysseyPainterEditorViewportClient::CaptureMouseOnClick()
{
    return mMouseCaptureMode;
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
    double zoom = 1.0;
    bool fitToViewport = mOdysseyPainterEditorViewportPtr.Pin()->GetFitToViewport();

    if( fitToViewport )
    {
        //The member zoom is overriden by the fit to viewport. The drawing function uses another way to calculate the effective zoom, and we do the same here
        uint32 width, height;
        mOdysseyPainterEditorViewportPtr.Pin()->CalculateTextureDisplayDimensions( width, height );
        zoom = static_cast<double>( width ) / static_cast<double>( mOdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Width() );
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
    double zoom = GetZoom();
    FVector2D pan = mOdysseyPainterEditorViewportPtr.Pin()->GetPan();
    FVector2D textureViewportPosition = GetViewportScrollBarPositions();
    FVector2D ratio = FVector2D( mOdysseyPainterEditorViewportPtr.Pin()->GetViewportHorizontalScrollBarRatio(), mOdysseyPainterEditorViewportPtr.Pin()->GetViewportVerticalScrollBarRatio() );
    FVector2D viewportSize = FVector2D( mOdysseyPainterEditorViewportPtr.Pin()->GetViewport()->GetSizeXY().X, mOdysseyPainterEditorViewportPtr.Pin()->GetViewport()->GetSizeXY().Y );
    int32 yOffset = ( ratio.Y > 1.0f ) ? ( ( viewportSize.Y - ( viewportSize.Y / ratio.Y ) ) * 0.5f ) : 0;
    int32 xOffset = ( ratio.X > 1.0f ) ? ( ( viewportSize.X - ( viewportSize.X / ratio.X ) ) * 0.5f ) : 0;

    int textureWidth = mOdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Width();
    int textureHeight = mOdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Height();

    FVector2D texturePanPivot = FVector2D( mPivotPointRatio.X * textureWidth, mPivotPointRatio.Y * textureHeight ) - 0.5 * FVector2D( textureWidth, textureHeight );

    FVector2D position = FVector2D( ( iMouseInViewport.X + textureViewportPosition.X - xOffset - pan.X ) / zoom, ( iMouseInViewport.Y + textureViewportPosition.Y - yOffset - pan.Y ) / zoom );

    if( iWithRotation )
    {
        if( mOdysseyPainterEditorViewportPtr.Pin()->GetRotationInDegrees() != 0 )
        {
            float rotation = FMath::DegreesToRadians( mOdysseyPainterEditorViewportPtr.Pin()->GetRotationInDegrees() );
            FVector2D center = FVector2D( mOdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Width(), mOdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Height() ) / 2;
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
    return point_in_texture;
}

void 
FOdysseyPainterEditorViewportClient::DrawUVsOntoViewport( const FViewport* iViewport, FCanvas* ioCanvas, int32 iUVChannel, const FStaticMeshVertexBuffer& iVertexBuffer, const FIndexArrayView& iIndices )
{
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

        /* If we want to draw a bounding box to the UV
        FCanvasTileItem BoxBackgroundTileItem(uvBoxOrigin, GWhiteTexture, FVector2D(Width, Height), FLinearColor(0, 0, 0, 0.0f));
        BoxBackgroundTileItem.PivotPoint = FVector2D( 0.5, 0.5 );
        BoxBackgroundTileItem.Rotation.Add(0, mOdysseyPainterEditorViewportPtr.Pin()->GetRotationInDegrees(), 0 );
        BoxBackgroundTileItem.BlendMode = SE_BLEND_AlphaComposite;
        InCanvas->DrawItem(BoxBackgroundTileItem);
        */

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
                ::ULIS::CColor c = ::ULIS::CColor::FromRGBF( lc.R, lc.G, lc.B, lc.A );

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
