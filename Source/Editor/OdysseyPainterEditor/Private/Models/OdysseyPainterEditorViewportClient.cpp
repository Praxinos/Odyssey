// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#include "Models/OdysseyPainterEditorViewportClient.h"
#include "Widgets/Layout/SScrollBar.h"
#include "CanvasItem.h"
#include "Editor/UnrealEdEngine.h"
#include "Engine/Texture2D.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "Engine/TextureCube.h"
#include "Engine/VolumeTexture.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/TextureRenderTargetCube.h"
#include "UnrealEdGlobals.h"
#include "CubemapUnwrapUtils.h"
#include "Slate/SceneViewport.h"
#include "Texture2DPreview.h"
#include "VolumeTexturePreview.h"
#include "OdysseyPainterEditorSettings.h"
#include "SOdysseySurfaceViewport.h"
#include "OdysseyPainterEditorToolkit.h"
#include "CanvasTypes.h"
#include "ImageUtils.h"
#include "OdysseyPaintEngine.h"
#include "OdysseyLayerStack.h"
#include "OdysseySurface.h"
#include "SOdysseyCursorWidget.h"
#include "RawMesh.h"

#include <memory>
#include <ULIS_CORE>

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorViewportClientt"


/////////////////////////////////////////////////////
// FOdysseyPainterEditorViewportClient
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorViewportClient::FOdysseyPainterEditorViewportClient( TWeakPtr< IOdysseyPainterEditorToolkit >  InOdysseyPainterEditor,
                                                                          TWeakPtr< SOdysseySurfaceViewport >       InOdysseyPainterEditorViewport,
                                                                          FOdysseyMeshSelector*                     InMeshSelector)
    : mMouseCaptureMode( FViewportClient::CaptureMouseOnClick() )
    , OdysseyPainterEditorPtr( InOdysseyPainterEditor )
    , OdysseyPainterEditorViewportPtr( InOdysseyPainterEditorViewport )
    , MeshSelector( InMeshSelector )
    , CheckerboardTexture( NULL )
    , CurrentMouseCursor( EMouseCursor::Default )
    , PivotPointRatio ( FVector2D( 0.5, 0.5 ) )
    , CurrentToolState( eState::kIdle )
{
    check( OdysseyPainterEditorPtr.IsValid() &&
           OdysseyPainterEditorViewportPtr.IsValid() );

    ModifyCheckerboardTextureColors();
}


FOdysseyPainterEditorViewportClient::~FOdysseyPainterEditorViewportClient( )
{
    DestroyCheckerboardTexture();
}


//--------------------------------------------------------------------------------------
//------------------------------------------------------------------ FViewportClient API
void
FOdysseyPainterEditorViewportClient::Draw( FViewport* Viewport, FCanvas* Canvas )
{
    if( !OdysseyPainterEditorPtr.IsValid() )
        return;

    // Draw on tick or catch up
    /*
    auto paintengine = OdysseyPainterEditorPtr.Pin()->PaintEngine();
    FVector2D oldpoint = FVector2D( RefEventStrokePoint.x, RefEventStrokePoint.y );
    FVector2D position_in_viewport( Viewport->GetMouseX(), Viewport->GetMouseY() );
    FVector2D position_in_texture = GetLocalMousePosition( position_in_viewport );
    if( CurrentToolState == eState::kDrawing
    &&  ( paintengine->GetStokePaintOnTick() || ( paintengine->GetSmoothingCatchUp() && oldpoint != position_in_texture ) ) )
    {
        FOdysseyStrokePoint point = RefEventStrokePoint;
        point.x = position_in_texture.X;
        point.y = position_in_texture.Y;
        paintengine->PushStroke( point );
    }
    */

    // Send Tick to PaintEngine
    OdysseyPainterEditorPtr.Pin()->PaintEngine()->Tick();

    double Rotation         = OdysseyPainterEditorViewportPtr.Pin()->GetRotationInDegrees();
    FVector2D Pan           = OdysseyPainterEditorViewportPtr.Pin()->GetPan();

    UTexture* Texture       = OdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Texture();
    FVector2D Ratio         = FVector2D( OdysseyPainterEditorViewportPtr.Pin()->GetViewportHorizontalScrollBarRatio(),
                                         OdysseyPainterEditorViewportPtr.Pin()->GetViewportVerticalScrollBarRatio() );
    FVector2D ViewportSize  = FVector2D( OdysseyPainterEditorViewportPtr.Pin()->GetViewport()->GetSizeXY().X,
                                         OdysseyPainterEditorViewportPtr.Pin()->GetViewport()->GetSizeXY().Y );
    FVector2D ScrollBarPos  = GetViewportScrollBarPositions();
    int32 YOffset           = ( Ratio.Y > 1.0f ) ? ( ( ViewportSize.Y - ( ViewportSize.Y / Ratio.Y ) ) * 0.5f ) + Pan.Y: Pan.Y;
    int32 YPos              = YOffset - ScrollBarPos.Y;
    int32 XOffset           = ( Ratio.X > 1.0f ) ? ( ( ViewportSize.X - ( ViewportSize.X / Ratio.X ) ) * 0.5f ) + Pan.X: Pan.X;
    int32 XPos              = XOffset - ScrollBarPos.X;

    UpdateScrollBars();
    const UOdysseyPainterEditorSettings& Settings = *GetDefault<UOdysseyPainterEditorSettings>();
    Canvas->Clear( Settings.BackgroundColor );
    UTexture2D* Texture2D = Cast< UTexture2D >( Texture );

    // Fully stream in the texture before drawing it.
    if (Texture2D)
    {
        Texture2D->SetForceMipLevelsToBeResident( 30.0f );
        Texture2D->WaitForStreaming();
    }

    // Figure out the size we need
    uint32 Width, Height;
    OdysseyPainterEditorViewportPtr.Pin()->CalculateTextureDisplayDimensions( Width, Height );
    const float MipLevel = 1;

    TRefCountPtr<FBatchedElementParameters> BatchedElementParameters;

    if (GMaxRHIFeatureLevel >= ERHIFeatureLevel::SM4)
    {
        //ODYSSEY: PATCH
        bool bIsNormalMap = Texture2D->IsNormalMap();
        bool bIsSingleChannel = Texture2D->CompressionSettings == TC_Grayscale || Texture2D->CompressionSettings == TC_Alpha;
        bool bIsVirtual = Texture2D->IsCurrentlyVirtualTextured();
        float LayerIndex = 0.f;
        BatchedElementParameters = new FBatchedElementTexture2DPreviewParameters(MipLevel, LayerIndex, bIsNormalMap, bIsSingleChannel, bIsVirtual);
    }

    FVector2D viewport_center( Viewport->GetSizeXY().X / 2, Viewport->GetSizeXY().Y / 2 );
    FVector2D position_in_texture = GetLocalMousePosition( viewport_center, false );
    PivotPointRatio = FVector2D(position_in_texture.X / OdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Width(), position_in_texture.Y / OdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Height() );

    // Draw background Checker
    {
        FCanvasTileItem TileItem( FVector2D(XPos, YPos), CheckerboardTexture->Resource, FVector2D(Width, Height), FVector2D(0.f, 0.f), FVector2D( Width / CheckerboardTexture->GetSizeX(), Height / CheckerboardTexture->GetSizeY()), FLinearColor::White);
        TileItem.BlendMode = SE_BLEND_Opaque;
        TileItem.PivotPoint.Set(PivotPointRatio.X, PivotPointRatio.Y);
        TileItem.Rotation.Add(0, Rotation, 0);
        Canvas->DrawItem(TileItem);
    }

    // Draw Drawing Surface
    if ( Texture->Resource != nullptr )
    {
        FCanvasTileItem TileItem( FVector2D( XPos, YPos ), Texture->Resource, FVector2D( Width, Height ), FLinearColor::White );
        TileItem.BatchedElementParameters = BatchedElementParameters;
        uint32 Result = (uint32)SE_BLEND_RGBA_MASK_START;
        Result += (1 << 0);
        Result += (1 << 1);
        Result += (1 << 2);
        Result += (1 << 3);
        TileItem.BlendMode = (ESimpleElementBlendMode)Result;
        TileItem.PivotPoint.Set(PivotPointRatio.X, PivotPointRatio.Y);
        TileItem.Rotation.Add(0, Rotation, 0 );
        Canvas->DrawItem( TileItem );

        /* TODO: Unreal BoxItem doesn't support rotation, so we can't draw it properly. We'll have to come up with our own HUD
        // Draw a white border around the texture to show its extents
        if (Settings.TextureBorderEnabled)
        {
            FCanvasBoxItem BoxItem( FVector2D(XPos, YPos), FVector2D(Width , Height ) );
            BoxItem.SetColor( Settings.TextureBorderColor );
            Canvas->DrawItem( BoxItem );
        }
        */
    }

    if( MeshSelector->GetCurrentMesh() )
    {
        int currentLOD = MeshSelector->GetCurrentLOD();
        int currentUV = MeshSelector->GetCurrentUVChannel();
        if( currentLOD >= 0 && currentUV >= 0 )
        {
            FRawMesh RawMesh;
            //ODYSSEY: PATCH
            //MeshSelector->GetCurrentMesh()->SourceModels[currentLOD].LoadRawMesh(RawMesh);
            MeshSelector->GetCurrentMesh()->GetSourceModel(currentLOD).LoadRawMesh(RawMesh);

            FIndexArrayView IndexBuffer = MeshSelector->GetCurrentMesh()->RenderData.Get()->LODResources[currentLOD].IndexBuffer.GetArrayView();
            DrawUVsOntoViewport(Viewport, Canvas, currentUV, MeshSelector->GetCurrentMesh()->RenderData.Get()->LODResources[0].VertexBuffers.StaticMeshVertexBuffer, IndexBuffer );
        }
    }

}


bool
FOdysseyPainterEditorViewportClient::InputKey( FViewport* Viewport, int32 ControllerId, FKey Key, EInputEvent Event, float AmountDepressed, bool Gamepad )
{
    FOdysseyStrokePoint point_in_viewport( FOdysseyStrokePoint::DefaultPoint() );
    point_in_viewport.x = Viewport->GetMouseX();
    point_in_viewport.y = Viewport->GetMouseY();
    return InputKeyWithStrokePoint( point_in_viewport, ControllerId, Key, Event, AmountDepressed, Gamepad );
}


void
FOdysseyPainterEditorViewportClient::CapturedMouseMove( FViewport* Viewport, int32 X, int32 Y )
{
    FOdysseyStrokePoint point_in_viewport( FOdysseyStrokePoint::DefaultPoint() );
    point_in_viewport.x = X;
    point_in_viewport.y = Y;
    CapturedMouseMoveWithStrokePoint( point_in_viewport );
}


bool
FOdysseyPainterEditorViewportClient::InputKeyWithStrokePoint( const FOdysseyStrokePoint& iPointInViewport, int32 ControllerId, FKey Key, EInputEvent Event, float AmountDepressed, bool bGamepad )
{
    if( CurrentToolState == eState::kIdle )
    {
        if( Key == EKeys::LeftMouseButton && Event == EInputEvent::IE_Pressed )
        {
            CurrentToolState = eState::kDrawing;
            //OdysseyPainterEditorPtr.Pin()->BeginTransaction( LOCTEXT("Stroke in ILIAD", "Stroke in ILIAD") );
            //OdysseyPainterEditorPtr.Pin()->MarkTransactionAsDirty();

            FOdysseyStrokePoint point_in_texture = GetLocalMousePosition( iPointInViewport );
            OdysseyPainterEditorPtr.Pin()->PaintEngine()->PushStroke( point_in_texture );

            return true;
        }
        else if( Key == EKeys::Escape && Event == EInputEvent::IE_Pressed )
        {
            OdysseyPainterEditorPtr.Pin()->PaintEngine()->AbortStroke();
            return true;
        }
        else if( Key == EKeys::P && Event == EInputEvent::IE_Pressed )
        {
            CurrentToolState = eState::kPan;
            return true;
        }
        else if( Key == EKeys::R && Event == EInputEvent::IE_Pressed )
        {
            CurrentToolState = eState::kRotate;
            return true;
        }
        else if( Key == EKeys::LeftAlt && Event == EInputEvent::IE_Pressed )
        {
            CurrentToolState = eState::kPick;
            return true;
        }
        else if( Key == EKeys::MouseScrollUp )
        {
            FVector2D position_in_viewport( iPointInViewport.x, iPointInViewport.y );
            ZoomInInViewport( position_in_viewport );
            return true;
        }
        else if( Key == EKeys::MouseScrollDown )
        {
            FVector2D position_in_viewport( iPointInViewport.x, iPointInViewport.y );
            ZoomOutInViewport( position_in_viewport );
            return true;
        }
    }

    else if( CurrentToolState == eState::kDrawing )
    {
        if( Key == EKeys::LeftMouseButton && Event == EInputEvent::IE_Released )
        {
            CurrentToolState = eState::kIdle;

            OdysseyPainterEditorPtr.Pin()->PaintEngine()->EndStroke();
            //OdysseyPainterEditorPtr.Pin()->EndTransaction();

            // Test:
            OdysseyPainterEditorPtr.Pin()->PaintEngine()->UpdateBrushCursorPreview();
            return true;
        }
    }

    else if( CurrentToolState == eState::kRotate )
    {
        if( Key == EKeys::R && Event == EInputEvent::IE_Released )
        {
            CurrentToolState = eState::kIdle;
            return true;
        }
        else if( Key == EKeys::LeftMouseButton && Event == EInputEvent::IE_Pressed )
        {
            CurrentToolState = eState::kRotating;

            FIntPoint size = OdysseyPainterEditorViewportPtr.Pin()->GetViewport()->GetSizeXY();
            FVector2D center = FVector2D( size.X / 2, size.Y / 2);
            FVector2D position_in_viewport( iPointInViewport.x, iPointInViewport.y );
            FVector2D deltaCenter = position_in_viewport - center;
            RotationReference = FMath::Atan2( -deltaCenter.Y, deltaCenter.X );

            return true;
        }
    }
    else if( CurrentToolState == eState::kRotating )
    {
        if( Key == EKeys::LeftMouseButton && Event == EInputEvent::IE_Released )
        {
            CurrentToolState = eState::kRotate;
            return true;
        }
        else if( Key == EKeys::R && Event == EInputEvent::IE_Released )
        {
            CurrentToolState = eState::kIdle;
            return true;
        }
    }

    else if( CurrentToolState == eState::kPan )
    {
        if( Key == EKeys::P && Event == EInputEvent::IE_Released )
        {
            CurrentToolState = eState::kIdle;
            return true;
        }
        else if( Key == EKeys::LeftMouseButton && Event == EInputEvent::IE_Pressed )
        {
            CurrentToolState = eState::kPanning;

            PanReference = FVector2D( iPointInViewport.x, iPointInViewport.y );
            return true;
        }
    }
    else if( CurrentToolState == eState::kPanning )
    {
        if( Key == EKeys::LeftMouseButton && Event == EInputEvent::IE_Released )
        {
            CurrentToolState = eState::kPan;
            return true;
        }
        else if( Key == EKeys::P && Event == EInputEvent::IE_Released )
        {
            CurrentToolState = eState::kIdle;
            return true;
        }
    }

    else if( CurrentToolState == eState::kPick )
    {
        if( Key == EKeys::LeftAlt && Event == EInputEvent::IE_Released )
        {
            CurrentToolState = eState::kIdle;
            return true;
        }
        else if( Key == EKeys::LeftMouseButton && Event == EInputEvent::IE_Pressed )
        {
            CurrentToolState = eState::kPicking;

            FVector2D position_in_viewport( iPointInViewport.x, iPointInViewport.y );
            FVector2D position_in_texture = GetLocalMousePosition( position_in_viewport );
            OdysseyPainterEditorPtr.Pin()->SetColor( OdysseyPainterEditorPtr.Pin()->LayerStack()->GetResultBlock()->GetIBlock()->PixelColor( position_in_texture.X, position_in_texture.Y ) );

            return true;
        }
    }
    else if( CurrentToolState == eState::kPicking )
    {
        if( Key == EKeys::LeftMouseButton && Event == EInputEvent::IE_Released )
        {
            CurrentToolState = eState::kPick;
            return true;
        }
        else if( Key == EKeys::LeftAlt && Event == EInputEvent::IE_Released )
        {
            CurrentToolState = eState::kIdle;
            return true;
        }
    }

    return  false;
}


void
FOdysseyPainterEditorViewportClient::CapturedMouseMoveWithStrokePoint( const FOdysseyStrokePoint& iPointInViewport )
{
    if( CurrentToolState == eState::kDrawing )
    {
        auto paintengine = OdysseyPainterEditorPtr.Pin()->PaintEngine();
        /*
        if( paintengine->GetStokePaintOnTick() )
            return;
        */

        FOdysseyStrokePoint point_in_texture = GetLocalMousePosition( iPointInViewport );
        paintengine->PushStroke( point_in_texture );
    }
    else if( CurrentToolState == eState::kPanning )
    {
        FVector2D deltaReference( iPointInViewport.x - PanReference.X, iPointInViewport.y - PanReference.Y );
        FVector2D delta_in_viewport = FVector2D();
        
        float rotation = FMath::DegreesToRadians( OdysseyPainterEditorViewportPtr.Pin()->GetRotationInDegrees() );

        delta_in_viewport.X = deltaReference.X * FMath::Cos(rotation) + deltaReference.Y * FMath::Sin(rotation);
        delta_in_viewport.Y = -deltaReference.X * FMath::Sin(rotation) + deltaReference.Y * FMath::Cos(rotation);
        
        OdysseyPainterEditorViewportPtr.Pin()->AddPan( delta_in_viewport );
        PanReference = FVector2D( iPointInViewport.x, iPointInViewport.y);
    }
    else if( CurrentToolState == eState::kRotating )
    {
        FIntPoint size = OdysseyPainterEditorViewportPtr.Pin()->GetViewport()->GetSizeXY();

        FVector2D center = FVector2D( size.X / 2, size.Y / 2);
        FVector2D position_in_viewport( iPointInViewport.x, iPointInViewport.y );
        FVector2D deltaCenter = position_in_viewport - center;
        float newRotation = FMath::Atan2( -deltaCenter.Y, deltaCenter.X );
        float deltaRotation = RotationReference - newRotation;

        OdysseyPainterEditorViewportPtr.Pin()->SetRotationInDegrees( OdysseyPainterEditorViewportPtr.Pin()->GetRotationInDegrees() + FMath::RadiansToDegrees( deltaRotation ) );

        RotationReference = newRotation;
    }
    else if( CurrentToolState == eState::kPicking )
    {
        FVector2D position_in_viewport( iPointInViewport.x, iPointInViewport.y );
        FVector2D position_in_texture = GetLocalMousePosition( position_in_viewport );
        OdysseyPainterEditorPtr.Pin()->SetColor( OdysseyPainterEditorPtr.Pin()->LayerStack()->GetResultBlock()->GetIBlock()->PixelColor( position_in_texture.X, position_in_texture.Y ) );
    }
}

void
FOdysseyPainterEditorViewportClient::MouseEnter( FViewport* Viewport,int32 x, int32 y )
{
    CurrentToolState = eState::kIdle;
}

void
FOdysseyPainterEditorViewportClient::MouseLeave( FViewport* Viewport )
{
    CurrentToolState = eState::kIdle;
}


EMouseCursor::Type
FOdysseyPainterEditorViewportClient::GetCursor( FViewport* Viewport,int32 X,int32 Y )
{
    if( CurrentToolState == eState::kPan || CurrentToolState == eState::kPanning )
        CurrentMouseCursor = EMouseCursor::GrabHand;
    else if( CurrentToolState == eState::kPick || CurrentToolState == eState::kPicking )
        CurrentMouseCursor = EMouseCursor::EyeDropper;
    else
        CurrentMouseCursor = EMouseCursor::Crosshairs;

    return  CurrentMouseCursor;
}


TOptional< TSharedRef< SWidget > >
FOdysseyPainterEditorViewportClient::MapCursor( FViewport* Viewport, const FCursorReply& CursorReply )
{
    return  FViewportClient::MapCursor( Viewport, CursorReply );
}


EMouseCaptureMode
FOdysseyPainterEditorViewportClient::CaptureMouseOnClick()
{
    return mMouseCaptureMode;
}


//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------ FGCObject API
void
FOdysseyPainterEditorViewportClient::AddReferencedObjects( FReferenceCollector& Collector )
{
    Collector.AddReferencedObject(CheckerboardTexture);
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API
void
FOdysseyPainterEditorViewportClient::ModifyCheckerboardTextureColors()
{
    DestroyCheckerboardTexture();

    const UOdysseyPainterEditorSettings& Settings = *GetDefault< UOdysseyPainterEditorSettings >();
    CheckerboardTexture = FImageUtils::CreateCheckerboardTexture( Settings.CheckerColorOne, Settings.CheckerColorTwo, Settings.CheckerSize );
}


FText
FOdysseyPainterEditorViewportClient::GetDisplayedResolution() const
{
    uint32 Height = 1;
    uint32 Width = 1;
    OdysseyPainterEditorViewportPtr.Pin()->CalculateTextureDisplayDimensions(Width, Height);
    return  FText::Format( NSLOCTEXT( "OdysseyPainterEditor",
                                      "DisplayedResolution",
                                      "Displayed: {0}x{1}" ),
                           FText::AsNumber( FMath::Max( ( uint32 )1, Width ) ),
                           FText::AsNumber( FMath::Max( ( uint32 )1, Height ) ) );
}



//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------- Private API
void
FOdysseyPainterEditorViewportClient::UpdateScrollBars()
{
    TSharedPtr<SOdysseySurfaceViewport> Viewport = OdysseyPainterEditorViewportPtr.Pin();

    if (!Viewport.IsValid() || !Viewport->GetVerticalScrollBar().IsValid() || !Viewport->GetHorizontalScrollBar().IsValid())
    {
        return;
    }

    float VRatio = OdysseyPainterEditorViewportPtr.Pin()->GetViewportVerticalScrollBarRatio();
    float HRatio = OdysseyPainterEditorViewportPtr.Pin()->GetViewportHorizontalScrollBarRatio();
    float VDistFromBottom = Viewport->GetVerticalScrollBar()->DistanceFromBottom();
    float HDistFromBottom = Viewport->GetHorizontalScrollBar()->DistanceFromBottom();

    if (VRatio < 1.0f)
    {
        if (VDistFromBottom < 1.0f)
        {
            Viewport->GetVerticalScrollBar()->SetState(FMath::Clamp(1.0f - VRatio - VDistFromBottom, 0.0f, 1.0f), VRatio);
        }
        else
        {
            Viewport->GetVerticalScrollBar()->SetState(0.0f, VRatio);
        }
    }

    if (HRatio < 1.0f)
    {
        if (HDistFromBottom < 1.0f)
        {
            Viewport->GetHorizontalScrollBar()->SetState(FMath::Clamp(1.0f - HRatio - HDistFromBottom, 0.0f, 1.0f), HRatio);
        }
        else
        {
            Viewport->GetHorizontalScrollBar()->SetState(0.0f, HRatio);
        }
    }
}


FVector2D
FOdysseyPainterEditorViewportClient::GetViewportScrollBarPositions() const
{
    FVector2D Positions = FVector2D::ZeroVector;
    if (OdysseyPainterEditorViewportPtr.Pin()->GetVerticalScrollBar().IsValid() && OdysseyPainterEditorViewportPtr.Pin()->GetHorizontalScrollBar().IsValid())
    {
        uint32 Width, Height;
        UTexture* Texture = OdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Texture();
        float VRatio = OdysseyPainterEditorViewportPtr.Pin()->GetViewportVerticalScrollBarRatio();
        float HRatio = OdysseyPainterEditorViewportPtr.Pin()->GetViewportHorizontalScrollBarRatio();
        float VDistFromBottom = OdysseyPainterEditorViewportPtr.Pin()->GetVerticalScrollBar()->DistanceFromBottom();
        float HDistFromBottom = OdysseyPainterEditorViewportPtr.Pin()->GetHorizontalScrollBar()->DistanceFromBottom();

        OdysseyPainterEditorViewportPtr.Pin()->CalculateTextureDisplayDimensions(Width, Height);

        if ((OdysseyPainterEditorViewportPtr.Pin()->GetVerticalScrollBar()->GetVisibility() == EVisibility::Visible) && VDistFromBottom < 1.0f)
        {
            Positions.Y = FMath::Clamp(1.0f - VRatio - VDistFromBottom, 0.0f, 1.0f) * Height;
        }
        else
        {
            Positions.Y = 0.0f;
        }

        if ((OdysseyPainterEditorViewportPtr.Pin()->GetHorizontalScrollBar()->GetVisibility() == EVisibility::Visible) && HDistFromBottom < 1.0f)
        {
            Positions.X = FMath::Clamp(1.0f - HRatio - HDistFromBottom, 0.0f, 1.0f) * Width;
        }
        else
        {
            Positions.X = 0.0f;
        }
    }

    return Positions;
}

void
FOdysseyPainterEditorViewportClient::DestroyCheckerboardTexture()
{
    if (CheckerboardTexture)
    {
        if (CheckerboardTexture->Resource)
        {
            CheckerboardTexture->ReleaseResource();
        }
        CheckerboardTexture->MarkPendingKill();
        CheckerboardTexture = NULL;
    }
}


void
FOdysseyPainterEditorViewportClient::ZoomInInViewport( const FVector2D& iPositionInViewport )
{
    FVector2D Pan = OdysseyPainterEditorViewportPtr.Pin()->GetPan();

    // Diff between before and after the zoom
    float oldHScrollSize = OdysseyPainterEditorViewportPtr.Pin()->GetViewportHorizontalScrollBarRatio();
    float oldVScrollSize = OdysseyPainterEditorViewportPtr.Pin()->GetViewportVerticalScrollBarRatio();

    OdysseyPainterEditorViewportPtr.Pin()->ZoomIn();

    float newHScrollSize = OdysseyPainterEditorViewportPtr.Pin()->GetViewportHorizontalScrollBarRatio();
    float newVScrollSize = OdysseyPainterEditorViewportPtr.Pin()->GetViewportVerticalScrollBarRatio();


    float HScrollSizeDiff = newHScrollSize - oldHScrollSize;
    float VScrollSizeDiff = newVScrollSize - oldVScrollSize;
    //-------

    //Useful variables to determine the new position of the scrollbars
    float VDistFromBottom = OdysseyPainterEditorViewportPtr.Pin()->GetVerticalScrollBar()->DistanceFromBottom();
    float HDistFromBottom = OdysseyPainterEditorViewportPtr.Pin()->GetHorizontalScrollBar()->DistanceFromBottom();

    float xCursorOnViewport = iPositionInViewport.X;
    float yCursorOnViewport = iPositionInViewport.Y;

    FIntPoint size = OdysseyPainterEditorViewportPtr.Pin()->GetViewport()->GetSizeXY();

    float ratioX = (xCursorOnViewport - Pan.X) / FMath::Max(float(size.X), 1.f);
    float ratioY = (yCursorOnViewport - Pan.Y) / FMath::Max(float(size.Y), 1.f);
    //------

    //Set the scrollbars
    OdysseyPainterEditorViewportPtr.Pin()->GetHorizontalScrollBar()->SetState(FMath::Clamp(1 - (HDistFromBottom + newHScrollSize - HScrollSizeDiff * (1 - ratioX)), 0.0f, 1.0f - newHScrollSize), newHScrollSize);
    OdysseyPainterEditorViewportPtr.Pin()->GetVerticalScrollBar()->SetState(FMath::Clamp(1 - (VDistFromBottom + newVScrollSize - VScrollSizeDiff * (1 - ratioY)), 0.0f, 1.0f - newVScrollSize), newVScrollSize);
}

void
FOdysseyPainterEditorViewportClient::ZoomOutInViewport( const FVector2D& iPositionInViewport )
{
    FVector2D Pan = OdysseyPainterEditorViewportPtr.Pin()->GetPan();

    // Diff between before and after the zoom
    float oldHScrollSize = OdysseyPainterEditorViewportPtr.Pin()->GetViewportHorizontalScrollBarRatio();
    float oldVScrollSize = OdysseyPainterEditorViewportPtr.Pin()->GetViewportVerticalScrollBarRatio();

    OdysseyPainterEditorViewportPtr.Pin()->ZoomOut();

    float newHScrollSize = OdysseyPainterEditorViewportPtr.Pin()->GetViewportHorizontalScrollBarRatio();
    float newVScrollSize = OdysseyPainterEditorViewportPtr.Pin()->GetViewportVerticalScrollBarRatio();


    float HScrollSizeDiff = newHScrollSize - oldHScrollSize;
    float VScrollSizeDiff = newVScrollSize - oldVScrollSize;
    //-------

    //Useful variables to determine the new position of the scrollbars
    float VDistFromBottom = OdysseyPainterEditorViewportPtr.Pin()->GetVerticalScrollBar()->DistanceFromBottom();
    float HDistFromBottom = OdysseyPainterEditorViewportPtr.Pin()->GetHorizontalScrollBar()->DistanceFromBottom();

    float xCursorOnViewport = iPositionInViewport.X;
    float yCursorOnViewport = iPositionInViewport.Y;

    FIntPoint size = OdysseyPainterEditorViewportPtr.Pin()->GetViewport()->GetSizeXY();

    float ratioX = (xCursorOnViewport - Pan.X) / FMath::Max(float(size.X), 1.f);
    float ratioY = (yCursorOnViewport - Pan.Y) / FMath::Max(float(size.Y), 1.f);
    //------

    //Set the scrollbars
    OdysseyPainterEditorViewportPtr.Pin()->GetHorizontalScrollBar()->SetState(FMath::Clamp(1 - (HDistFromBottom + newHScrollSize - HScrollSizeDiff * (1 - ratioX)), 0.0f, 1.0f - newHScrollSize), newHScrollSize);
    OdysseyPainterEditorViewportPtr.Pin()->GetVerticalScrollBar()->SetState(FMath::Clamp(1 - (VDistFromBottom + newVScrollSize - VScrollSizeDiff * (1 - ratioY)), 0.0f, 1.0f - newVScrollSize), newVScrollSize);
}


double
FOdysseyPainterEditorViewportClient::GetZoom() const
{
    double zoom = 1.0;
    bool fitToViewport = OdysseyPainterEditorViewportPtr.Pin()->GetFitToViewport();

    if( fitToViewport )
    {
        //The member zoom is overriden by the fit to viewport. The drawing function uses another way to calculate the effective zoom, and we do the same here
        uint32 Width, Height;
        OdysseyPainterEditorViewportPtr.Pin()->CalculateTextureDisplayDimensions(Width, Height);
        zoom = static_cast<double> (Width) / static_cast<double> (OdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Width());
    }
    else
    {
        //The member zoom is used to draw the viewport, we can use it
        zoom = OdysseyPainterEditorViewportPtr.Pin()->GetZoom();
    }

    return  zoom;
}


FVector2D
FOdysseyPainterEditorViewportClient::GetLocalMousePosition( const FVector2D& iMouseInViewport, const bool iWithRotation )  const
{
    double zoom = GetZoom();
    FVector2D Pan = OdysseyPainterEditorViewportPtr.Pin()->GetPan();
    FVector2D TextureViewportPosition = GetViewportScrollBarPositions();
    FVector2D Ratio = FVector2D( OdysseyPainterEditorViewportPtr.Pin()->GetViewportHorizontalScrollBarRatio(), OdysseyPainterEditorViewportPtr.Pin()->GetViewportVerticalScrollBarRatio() );
    FVector2D ViewportSize = FVector2D(OdysseyPainterEditorViewportPtr.Pin()->GetViewport()->GetSizeXY().X, OdysseyPainterEditorViewportPtr.Pin()->GetViewport()->GetSizeXY().Y);
    int32 YOffset = (Ratio.Y > 1.0f) ? ((ViewportSize.Y - (ViewportSize.Y / Ratio.Y)) * 0.5f) : 0;
    int32 XOffset = (Ratio.X > 1.0f) ? ((ViewportSize.X - (ViewportSize.X / Ratio.X)) * 0.5f) : 0;
    
    int textureWidth = OdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Width();
    int textureHeight = OdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Height();
    
    FVector2D texturePanPivot = FVector2D(PivotPointRatio.X * textureWidth, PivotPointRatio.Y * textureHeight) - 0.5 * FVector2D( textureWidth, textureHeight );
    
    FVector2D position = FVector2D (( iMouseInViewport.X + TextureViewportPosition.X - XOffset - Pan.X ) / zoom, ( iMouseInViewport.Y + TextureViewportPosition.Y - YOffset - Pan.Y ) / zoom);

    if( iWithRotation )
    {            
        if( OdysseyPainterEditorViewportPtr.Pin()->GetRotationInDegrees() != 0)
        {
            float rotation = FMath::DegreesToRadians( OdysseyPainterEditorViewportPtr.Pin()->GetRotationInDegrees() );
            FVector2D center = FVector2D( OdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Width(), OdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Height() ) / 2;
            position -= center;

            FVector2D pivotPan = texturePanPivot;
            pivotPan.X = texturePanPivot.X * FMath::Cos(rotation) + texturePanPivot.Y * FMath::Sin(rotation) - texturePanPivot.X;
            pivotPan.Y = -texturePanPivot.X * FMath::Sin(rotation) + texturePanPivot.Y * FMath::Cos(rotation) - texturePanPivot.Y;

            float x = position.X * FMath::Cos(rotation) + position.Y * FMath::Sin(rotation) + center.X;
            float y = -position.X * FMath::Sin(rotation) + position.Y * FMath::Cos(rotation) + center.Y;

            position.X = x - pivotPan.X;
            position.Y = y - pivotPan.Y;
        }
    }

    return  position;
}


FOdysseyStrokePoint   
FOdysseyPainterEditorViewportClient::GetLocalMousePosition( const FOdysseyStrokePoint& iPointInViewport )  const
{
    FVector2D position_in_viewport( iPointInViewport.x, iPointInViewport.y );
    FVector2D position_in_texture = GetLocalMousePosition( position_in_viewport );

    FOdysseyStrokePoint point_in_texture( iPointInViewport );
    point_in_texture.x = position_in_texture.X;
    point_in_texture.y = position_in_texture.Y;
    return point_in_texture;
}


void FOdysseyPainterEditorViewportClient::DrawUVsOntoViewport(FViewport* InViewport, FCanvas* InCanvas, int32 UVChannel, FStaticMeshVertexBuffer& VertexBuffer, FIndexArrayView& Indices )
{
    FVector2D Pan = OdysseyPainterEditorViewportPtr.Pin()->GetPan();

    uint32 Width, Height;
    OdysseyPainterEditorViewportPtr.Pin()->CalculateTextureDisplayDimensions( Width, Height );
    float rotation = -FMath::DegreesToRadians( OdysseyPainterEditorViewportPtr.Pin()->GetRotationInDegrees() );

    double zoom = GetZoom();
    FVector2D TextureViewportPosition = GetViewportScrollBarPositions();
    FVector2D Ratio = FVector2D( OdysseyPainterEditorViewportPtr.Pin()->GetViewportHorizontalScrollBarRatio(), OdysseyPainterEditorViewportPtr.Pin()->GetViewportVerticalScrollBarRatio() );
    FVector2D ViewportSize = FVector2D(OdysseyPainterEditorViewportPtr.Pin()->GetViewport()->GetSizeXY().X, OdysseyPainterEditorViewportPtr.Pin()->GetViewport()->GetSizeXY().Y);
    int32 YOffset = (Ratio.Y > 1.0f) ? ((ViewportSize.Y - (ViewportSize.Y / Ratio.Y)) * 0.5f) : 0;
    int32 XOffset = (Ratio.X > 1.0f) ? ((ViewportSize.X - (ViewportSize.X / Ratio.X)) * 0.5f) : 0;

    FVector2D ScrollBarPos  = GetViewportScrollBarPositions();
    int32 YPos              = FMath::Min( int(YOffset - ScrollBarPos.Y), 0 );
    int32 XPos              = FMath::Min( int(XOffset - ScrollBarPos.X), 0 );

    if( ( ( uint32 )UVChannel < VertexBuffer.GetNumTexCoords() ) )
    {
        //calculate scaling
        const int32 MinY = YOffset;
        const int32 MinX = XOffset;
        const FVector2D UVBoxOrigin(MinX + XPos + Pan.X, MinY + YPos + Pan.Y);

        /* If we want to draw a bounding box to the UV
        FCanvasTileItem BoxBackgroundTileItem(UVBoxOrigin, GWhiteTexture, FVector2D(Width, Height), FLinearColor(0, 0, 0, 0.0f));
        BoxBackgroundTileItem.PivotPoint = FVector2D( 0.5, 0.5 );
        BoxBackgroundTileItem.Rotation.Add(0, OdysseyPainterEditorViewportPtr.Pin()->GetRotationInDegrees(), 0 );
        BoxBackgroundTileItem.BlendMode = SE_BLEND_AlphaComposite;
        InCanvas->DrawItem(BoxBackgroundTileItem);
        */

        //draw triangles
        uint32 NumIndices = Indices.Num();
        FCanvasLineItem LineItem;
        for (uint32 i = 0; i < NumIndices - 2; i += 3)
        {
            FVector2D UVs[3];
            bool bOutOfBounds[3];

            float FudgeFactor = 1.0f/1024.0f;
            for (int32 Corner = 0; Corner < 3; Corner++)
            {
                UVs[Corner] = (VertexBuffer.GetVertexUV(Indices[i + Corner], UVChannel));

                bOutOfBounds[Corner] = (UVs[Corner].X < -FudgeFactor || UVs[Corner].X > (1.0f+FudgeFactor)) || (UVs[Corner].Y < -FudgeFactor || UVs[Corner].Y > (1.0f+FudgeFactor));
            }

            // Clamp the UV triangle to the [0,1] range (with some fudge).
            int32 NumUVs = 3;
            FudgeFactor = 0.1f;
            FVector2D Bias(0.0f,0.0f);

            float MinU = UVs[0].X;
            float MinV = UVs[0].Y;
            for (int32 j = 1; j < NumUVs; ++j)
            {
                MinU = FMath::Min(MinU,UVs[j].X);
                MinV = FMath::Min(MinU,UVs[j].Y);
            }

            if (MinU < -FudgeFactor || MinU > (1.0f+FudgeFactor))
            {
                Bias.X = FMath::FloorToFloat(MinU);
            }
            if (MinV < -FudgeFactor || MinV > (1.0f+FudgeFactor))
            {
                Bias.Y = FMath::FloorToFloat(MinV);
            }

            for (int32 j = 0; j < NumUVs; j++)
            {
                UVs[j] += Bias;
            }

            int maxX = InViewport->GetSizeXY().X;
            int maxY = InViewport->GetSizeXY().Y;
            float distSquared = (maxX * maxX / 2 + maxY * maxY / 2 );

            for (int32 Edge = 0; Edge < 3; Edge++)
            {
                int32 Corner1 = Edge;
                int32 Corner2 = (Edge + 1) % 3;
                FLinearColor lc = MeshSelector->GetMeshColor();
                ::ULIS::CColor c = ::ULIS::CColor::FromRGBF( lc.R, lc.G, lc.B, lc.A );

                FLinearColor Color = (bOutOfBounds[Corner1] || bOutOfBounds[Corner2]) ? FLinearColor(0.6f, 0.0f, 0.0f) : FLinearColor( c.RedF(), c.GreenF(), c.BlueF(), c.AlphaF() );
                LineItem.SetColor(Color);

                if( OdysseyPainterEditorViewportPtr.Pin()->GetRotationInDegrees() != 0 )
                {
                    FVector2D center = FVector2D( UVBoxOrigin.X + ( ( OdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Width() * zoom ) / 2 ), UVBoxOrigin.Y + ( ( OdysseyPainterEditorViewportPtr.Pin()->GetSurface()->Height() * zoom ) / 2 ) );

                    FVector2D positionP1( UVs[Corner1] * FVector2D(Width, Height) + UVBoxOrigin );
                    FVector2D positionP2( UVs[Corner2] * FVector2D(Width, Height) + UVBoxOrigin );
                    positionP1 -= center;
                    positionP2 -= center;

                    //Clipping. We don't even begin to calculate the coordinates of points we know won't be visible in the viewport
                    if( ( ( positionP1.X * positionP1.X + positionP1.Y * positionP1.Y ) > distSquared && ( positionP2.X * positionP2.X + positionP2.Y * positionP2.Y ) > distSquared ) )
                    {
                        continue;
                    }

                    float x1 = positionP1.X * FMath::Cos(rotation) + positionP1.Y * FMath::Sin(rotation) + center.X;
                    float y1 = -positionP1.X * FMath::Sin(rotation) + positionP1.Y * FMath::Cos(rotation) + center.Y;

                    float x2 = positionP2.X * FMath::Cos(rotation) + positionP2.Y * FMath::Sin(rotation) + center.X;
                    float y2 = -positionP2.X * FMath::Sin(rotation) + positionP2.Y * FMath::Cos(rotation) + center.Y;

                    FVector Origin = FVector();
                    FVector EndPos = FVector();

                    Origin.X = x1;
                    Origin.Y = y1;
                    Origin.Z = 0;
                    EndPos.X = x2;
                    EndPos.Y = y2;
                    EndPos.Z = 0;

                    FBatchedElements* BatchedElements = InCanvas->GetBatchedElements( FCanvas::ET_Line );
                    FHitProxyId HitProxyId = InCanvas->GetHitProxyId();
                    BatchedElements->AddTranslucentLine( Origin, EndPos, Color, HitProxyId, 1.f );
                }
                else
                {
                    FVector2D Origin2D = FVector2D( UVs[Corner1] * FVector2D(Width, Height) + UVBoxOrigin );
                    FVector2D EndPos2D = FVector2D( UVs[Corner2] * FVector2D(Width, Height) + UVBoxOrigin );

                    FVector Origin = FVector();
                    FVector EndPos = FVector();

                    Origin.X = Origin2D.X;
                    Origin.Y = Origin2D.Y;
                    Origin.Z = 0;
                    EndPos.X = EndPos2D.X;
                    EndPos.Y = EndPos2D.Y;
                    EndPos.Z = 0;

                    //Clipping. We don't even begin to calculate the coordinates of points we know won't be visible in the viewport
                    if( ( Origin.X < 0 && EndPos.X < 0 ) || ( Origin.X > maxX && EndPos.X > maxX ) || ( Origin.Y < 0 && EndPos.Y < 0 ) || ( Origin.Y > maxY && EndPos.Y > maxY ) )
                    {
                        continue;
                    }

                    FBatchedElements* BatchedElements = InCanvas->GetBatchedElements( FCanvas::ET_Line );
                    FHitProxyId HitProxyId = InCanvas->GetHitProxyId();
                    BatchedElements->AddTranslucentLine( Origin, EndPos, Color, HitProxyId, 1.f );
                }
            }
        }
    }
}

#undef LOCTEXT_NAMESPACE

