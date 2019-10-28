// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#include "SOdysseyBrushTestArea.h"
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
#include "Widgets/SToolTip.h"
#include "Widgets/SViewport.h"
#include "Widgets/Text/STextBlock.h"

#include <memory>

#define LOCTEXT_NAMESPACE "OdysseyBrushTestArea"

//////////////////////////////////////////////////////////////////////////
// FOdysseyBrushTestAreaViewportClient

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction


FOdysseyBrushTestAreaViewportClient::FOdysseyBrushTestAreaViewportClient( TWeakPtr< FOdysseyBrushEditor > InEditor,
                                                                        TWeakPtr< SOdysseyBrushTestArea > InOdysseyBrushTestArea )
    : EditorPtr( InEditor )
    , OdysseyBrushTestAreaPtr( InOdysseyBrushTestArea )
{
    check( EditorPtr.IsValid() && OdysseyBrushTestAreaPtr.IsValid() );
}


FOdysseyBrushTestAreaViewportClient::~FOdysseyBrushTestAreaViewportClient()
{
}


//--------------------------------------------------------------------------------------
//------------------------------------------------------------ FViewportClient interface


void
FOdysseyBrushTestAreaViewportClient::Draw( FViewport* Viewport, FCanvas* Canvas )
{
    Canvas->Clear( FColor( 220, 220, 220 ) );
}


//--------------------------------------------------------------------------------------
//------------------------------------------------------------------ FGCObject interface


void
FOdysseyBrushTestAreaViewportClient::AddReferencedObjects( FReferenceCollector& Collector )
{
    //Collector.AddReferencedObject(NONE);
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------- Public interface


void
FOdysseyBrushTestAreaViewportClient::Resize( FVector2D iSize )
{
}


//--------------------------------------------------------------------------------------
//------------------------------------------------------------------- Internal callbacks
// Nothing ATM

//////////////////////////////////////////////////////////////////////////
// SOdysseyBrushTestArea

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------ Widget construction


void
SOdysseyBrushTestArea::Construct( const FArguments& InArgs, const TSharedRef<FOdysseyBrushEditor>& InEditor )
{
    this->ChildSlot
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
            .FillHeight(1.0f)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                    .FillWidth(1.0f)
                    [
                        SAssignNew(ViewportWidget, SViewport)
                            .EnableGammaCorrection(false)
                            .IsEnabled(FSlateApplication::Get().GetNormalExecutionAttribute())
                            .ShowEffectWhenDisabled(false)
                            .EnableBlending(true)
                    ]
            ]

    ];

    ViewportClient = MakeShareable( new  FOdysseyBrushTestAreaViewportClient( InEditor, SharedThis( this ) ) );
    Viewport = MakeShareable( new  FSceneViewport( ViewportClient.Get(), ViewportWidget ) );
    // The viewport widget needs an interface so it knows what should render
    ViewportWidget->SetViewportInterface( Viewport.ToSharedRef() );

    // This is pointless because used only in games !
    Viewport->SetOnSceneViewportResizeDel( FOnSceneViewportResize::CreateRaw( this, &SOdysseyBrushTestArea::OnViewportResized ) );
}


//--------------------------------------------------------------------------------------
//------------------------------------------------------------- Widget viewports getters


TSharedPtr< FSceneViewport >
SOdysseyBrushTestArea::GetViewport( )  const
{
    return  Viewport;
}


TSharedPtr< SViewport >
SOdysseyBrushTestArea::GetViewportWidget()  const
{
    return  ViewportWidget;
}


//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- SWidget overrides


void
SOdysseyBrushTestArea::Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime )
{
    Viewport->Invalidate();
    Viewport->InvalidateDisplay();
}


//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Drawing utility


void
SOdysseyBrushTestArea::Refresh()
{
    Viewport->Invalidate();
    Viewport->InvalidateDisplay();
}


//--------------------------------------------------------------------------------------
//------------------------------------------------------------------- Internal callbacks


void
SOdysseyBrushTestArea::OnViewportResized( FVector2D NewSize )
{
    ViewportClient->Resize( NewSize );
}



#undef LOCTEXT_NAMESPACE
