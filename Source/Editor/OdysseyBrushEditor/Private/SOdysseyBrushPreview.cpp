// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#include "SOdysseyBrushPreview.h"
#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "Editor/UnrealEdEngine.h"
#include "Engine/Texture.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "ImageUtils.h"
#include "Slate/SceneViewport.h"
#include "Texture2DPreview.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "UnrealEdGlobals.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SToolTip.h"
#include "Widgets/SViewport.h"
#include "Widgets/Text/STextBlock.h"

#include <memory>

#define LOCTEXT_NAMESPACE "OdysseyBrushPreview"

//////////////////////////////////////////////////////////////////////////
// FOdysseyBrushPreviewViewportClient

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction


FOdysseyBrushPreviewViewportClient::FOdysseyBrushPreviewViewportClient( TWeakPtr< FOdysseyBrushEditor > InEditor,
                                                                        TWeakPtr< SOdysseyBrushPreview > InOdysseyBrushPreview )
    : EditorPtr( InEditor )
    , OdysseyBrushPreviewPtr( InOdysseyBrushPreview )
{
    check( EditorPtr.IsValid() && OdysseyBrushPreviewPtr.IsValid() );
}


FOdysseyBrushPreviewViewportClient::~FOdysseyBrushPreviewViewportClient()
{
}


//--------------------------------------------------------------------------------------
//------------------------------------------------------------ FViewportClient interface


void
FOdysseyBrushPreviewViewportClient::Draw( FViewport* Viewport, FCanvas* Canvas )
{
    Canvas->Clear( FColor( 220, 220, 220 ) );

    static  UTexture2D* DefaultTexture = nullptr;
    if( DefaultTexture == nullptr )
        DefaultTexture = LoadObject< UTexture2D >( nullptr, TEXT( "/Iliad/BrushThumbnails/OdysseyBrushDefaultThumbnail_512.OdysseyBrushDefaultThumbnail_512" ), nullptr, LOAD_None, nullptr );

    FIntPoint size = Viewport->GetSizeXY();

    Canvas->DrawTile(
        (float)0,
        (float)0,
        (float)size.X,
        (float)size.Y,
        0.0f,
        0.0f,
        1.0f,
        1.0f,
        FLinearColor::White,
        DefaultTexture->Resource,
        true);
}


//--------------------------------------------------------------------------------------
//------------------------------------------------------------------ FGCObject interface


void
FOdysseyBrushPreviewViewportClient::AddReferencedObjects( FReferenceCollector& Collector )
{
    //Collector.AddReferencedObject(NONE);
}


//--------------------------------------------------------------------------------------
//------------------------------------------------------------------- Internal callbacks
// Nothing ATM

//////////////////////////////////////////////////////////////////////////
// SOdysseyBrushPreview

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------ Widget construction


void
SOdysseyBrushPreview::Construct( const FArguments& InArgs, const TSharedRef<FOdysseyBrushEditor>& InEditor )
{
    this->ChildSlot
    [
        SNew(SVerticalBox)
        +SVerticalBox::Slot()
        .AutoHeight()
        .HAlign(HAlign_Fill)
        .VAlign(VAlign_Fill)
        [
            SNew(SHorizontalBox)
            +SHorizontalBox::Slot()
            .FillWidth( 1.0 )
            .HAlign(HAlign_Fill)
            .VAlign(VAlign_Fill)
            [
                SNew(SBox)
                .HeightOverride( 59 )
                [
                    SAssignNew(ViewportWidget, SViewport)
                            .EnableGammaCorrection(false)
                            .IsEnabled(FSlateApplication::Get().GetNormalExecutionAttribute())
                            .ShowEffectWhenDisabled(false)
                            .EnableBlending(true)
                            .ToolTip(SNew(SToolTip).Text(this, &SOdysseyBrushPreview::GetPreviewToolTip ) )
                ]
            ]
        ]
    ];

    ViewportClient = MakeShareable( new  FOdysseyBrushPreviewViewportClient( InEditor, SharedThis( this ) ) );
    Viewport = MakeShareable( new  FSceneViewport( ViewportClient.Get(), ViewportWidget ) );
    // The viewport widget needs an interface so it knows what should render
    ViewportWidget->SetViewportInterface( Viewport.ToSharedRef() );
}


//--------------------------------------------------------------------------------------
//------------------------------------------------------------- Widget viewports getters


TSharedPtr< FSceneViewport >
SOdysseyBrushPreview::GetViewport( )  const
{
    return  Viewport;
}


TSharedPtr< SViewport >
SOdysseyBrushPreview::GetViewportWidget()  const
{
    return  ViewportWidget;
}


//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- SWidget overrides


void
SOdysseyBrushPreview::Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime )
{
    Viewport->Invalidate();
    Viewport->InvalidateDisplay();
}


//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Drawing utility


void
SOdysseyBrushPreview::Refresh()
{
    Viewport->Invalidate();
    Viewport->InvalidateDisplay();
}


//--------------------------------------------------------------------------------------
//------------------------------------------------------------------- Internal callbacks


FText
SOdysseyBrushPreview::GetPreviewToolTip()  const
{
    return  FText::FromString( "Preview" );
}



#undef LOCTEXT_NAMESPACE
