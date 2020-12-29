// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "SOdysseySurfaceViewport.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Widgets/Layout/SScrollBar.h"
#include "Widgets/SToolTip.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/SViewport.h"
#include "Widgets/Input/SSlider.h"
#include "Engine/Texture.h"
#include "FOdysseySceneViewport.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "OdysseySurface.h"
#include "OdysseyBlock.h"
#include "OdysseyStyleSet.h"

#include <ULIS3>



#define MaxZoom 16.0
#define MinZoom 0.01
#define ZoomStep 0.025
#define RotationStep 15

#define LOCTEXT_NAMESPACE "OdysseySurfaceViewport"

/////////////////////////////////////////////////////
// SOdysseySurfaceViewport
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
void
SOdysseySurfaceViewport::Construct( const FArguments& InArgs )
{
    // create zoom menu
    FMenuBuilder ZoomMenuBuilder(true, NULL);
    {
        FUIAction Zoom25Action(FExecuteAction::CreateSP(this, &SOdysseySurfaceViewport::HandleZoomMenuEntryClicked, 0.25));
        ZoomMenuBuilder.AddMenuEntry(LOCTEXT("Zoom25Action", "25%"), LOCTEXT("Zoom25ActionHint", "Show the texture at a quarter of its size."), FSlateIcon(), Zoom25Action);

        FUIAction Zoom50Action(FExecuteAction::CreateSP(this, &SOdysseySurfaceViewport::HandleZoomMenuEntryClicked, 0.5));
        ZoomMenuBuilder.AddMenuEntry(LOCTEXT("Zoom50Action", "50%"), LOCTEXT("Zoom50ActionHint", "Show the texture at half its size."), FSlateIcon(), Zoom50Action);

        FUIAction Zoom100Action(FExecuteAction::CreateSP(this, &SOdysseySurfaceViewport::HandleZoomMenuEntryClicked, 1.0));
        ZoomMenuBuilder.AddMenuEntry(LOCTEXT("Zoom100Action", "100%"), LOCTEXT("Zoom100ActionHint", "Show the texture in its original size."), FSlateIcon(), Zoom100Action);

        FUIAction Zoom200Action(FExecuteAction::CreateSP(this, &SOdysseySurfaceViewport::HandleZoomMenuEntryClicked, 2.0));
        ZoomMenuBuilder.AddMenuEntry(LOCTEXT("Zoom200Action", "200%"), LOCTEXT("Zoom200ActionHint", "Show the texture at twice its size."), FSlateIcon(), Zoom200Action);

        FUIAction Zoom400Action(FExecuteAction::CreateSP(this, &SOdysseySurfaceViewport::HandleZoomMenuEntryClicked, 4.0));
        ZoomMenuBuilder.AddMenuEntry(LOCTEXT("Zoom400Action", "400%"), LOCTEXT("Zoom400ActionHint", "Show the texture at four times its size."), FSlateIcon(), Zoom400Action);

        ZoomMenuBuilder.AddMenuSeparator();

        FUIAction ZoomFitAction(
            FExecuteAction::CreateSP(this, &SOdysseySurfaceViewport::HandleZoomMenuFitClicked),
            FCanExecuteAction(),
            FIsActionChecked::CreateSP(this, &SOdysseySurfaceViewport::IsZoomMenuFitChecked)
            );
        ZoomMenuBuilder.AddMenuEntry(LOCTEXT("ZoomFitAction", "Scale To Fit"), LOCTEXT("ZoomFillActionHint", "Scale the texture to fit the viewport."), FSlateIcon(), ZoomFitAction, NAME_None, EUserInterfaceActionType::ToggleButton);
    }


    TSharedPtr<SHorizontalBox> HorizontalBox;

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
                        SNew(SVerticalBox)

                        + SVerticalBox::Slot()
                            .FillHeight(1)
                            [
                                SNew(SOverlay)

                                // viewport canvas
                                + SOverlay::Slot()
                                    [
                                        SAssignNew(ViewportWidget, SViewport)
                                            .EnableGammaCorrection(false)
                                            .IsEnabled(FSlateApplication::Get().GetNormalExecutionAttribute())
                                            .ShowEffectWhenDisabled(false)
                                            .EnableBlending(true)
                                    ]
                            ]
                    ]

                + SHorizontalBox::Slot()
                    .AutoWidth()
                    [
                        // vertical scroll bar
                        SAssignNew(TextureViewportVerticalScrollBar, SScrollBar)
                            .Visibility(this, &SOdysseySurfaceViewport::HandleVerticalScrollBarVisibility)
                            .OnUserScrolled(this, &SOdysseySurfaceViewport::HandleVerticalScrollBarScrolled)
                    ]
            ]

        + SVerticalBox::Slot()
            .AutoHeight()
            [
                // horizontal scrollbar
                SAssignNew(TextureViewportHorizontalScrollBar, SScrollBar)
                    .Orientation( Orient_Horizontal )
                    .Visibility(this, &SOdysseySurfaceViewport::HandleHorizontalScrollBarVisibility)
                    .OnUserScrolled(this, &SOdysseySurfaceViewport::HandleHorizontalScrollBarScrolled)
            ]

        + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0.0f, 2.0f, 0.0f, 0.0f)
            [
                SAssignNew(HorizontalBox, SHorizontalBox)
            ]
    ];


    // zoom slider and rotation
    HorizontalBox->AddSlot()
        .FillWidth(1.0f)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Center)
            [
                SNew(STextBlock)
                .Text( this, &SOdysseySurfaceViewport::HandleSurfaceInfosTextValue )
            ]
            + SHorizontalBox::Slot()
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Center)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()

                    .AutoWidth()
                    .Padding(4.0f, 0.0f)
                    .VAlign(VAlign_Center)
                    [
                        SNew(STextBlock)
                        .Text(LOCTEXT("RotationLabel", "Rotation: "))
                    ]

                + SHorizontalBox::Slot()
                    .Padding(2.0f, 0.0f)
                    .VAlign(VAlign_Center)
                    [
                        SNew( SBox )
                        .WidthOverride( 50 )
                        [
                            SNew( SSpinBox< int > )
                            .MinValue( -359 )
                            .MaxValue( 359 )
                            .OnValueChanged( this, &SOdysseySurfaceViewport::HandleRotationChanged )
                            .Value(this, &SOdysseySurfaceViewport::HandleRotationValue)
                        ]
                    ]

                + SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(4.0f, 0.0f)
                    .VAlign(VAlign_Center)
                    [
                        SNew(SButton)
                            .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                            .OnPressed(this, &SOdysseySurfaceViewport::HandleRotationLeft)
                        [
                            SNew(SImage) .Image(FOdysseyStyle::GetBrush("PainterEditor.RotateLeft16"))
                        ]
                    ]

                + SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(4.0f, 0.0f)
                    .VAlign(VAlign_Center)
                    [
                        SNew(SButton)
                            .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                            .OnPressed(this, &SOdysseySurfaceViewport::HandleViewportReset)
                        [
                            SNew(SImage) .Image(FOdysseyStyle::GetBrush("PainterEditor.RotateReset16"))
                        ]
                    ]

                + SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(4.0f, 0.0f)
                    .VAlign(VAlign_Center)
                    [
                        SNew(SButton)
                            .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                            .OnPressed(this, &SOdysseySurfaceViewport::HandleRotationRight)
                        [
                            SNew(SImage) .Image(FOdysseyStyle::GetBrush("PainterEditor.RotateRight16"))
                        ]
                    ]
            ]
            + SHorizontalBox::Slot()
            .VAlign(VAlign_Center)
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                    .AutoWidth()
                    .VAlign(VAlign_Center)
                    [
                        SNew(STextBlock)
                            .Text(LOCTEXT("ZoomLabel", "Zoom:"))
                    ]

                + SHorizontalBox::Slot()
                    .FillWidth(1.0f)
                    .Padding(4.0f, 0.0f)
                    .VAlign(VAlign_Center)
                    [
                        SNew(SSlider)
                            .OnValueChanged(this, &SOdysseySurfaceViewport::HandleZoomSliderChanged)
                            .Value(this, &SOdysseySurfaceViewport::HandleZoomSliderValue)
                    ]

                + SHorizontalBox::Slot()
                    .AutoWidth()
                    .VAlign(VAlign_Center)
                    [
                        SNew(STextBlock)
                            .Text(this, &SOdysseySurfaceViewport::HandleZoomPercentageText)
                    ]

                + SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(2.0f, 0.0f, 0.0f, 0.0f)
                    .VAlign(VAlign_Center)
                    [
                        SNew(SComboButton)
                            .ContentPadding(FMargin(0.0))
                            .MenuContent()
                            [
                                ZoomMenuBuilder.MakeWidget()
                            ]
                    ]
             ]
        ];

    SetZoom( 1.0 );
    SetRotationInDegrees( 0.f );
    SetPan( FVector2D( 0.f, 0.f ) );
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API
void
SOdysseySurfaceViewport::SetSurface( IOdysseySurface* iValue )
{
    Surface = iValue;
}


TSharedPtr< FOdysseySceneViewport >
SOdysseySurfaceViewport::GetViewport() const
{
    return  Viewport;
}


TSharedPtr< SViewport >
SOdysseySurfaceViewport::GetViewportWidget( ) const
{
    return  ViewportWidget;
}


TSharedPtr< SScrollBar >
SOdysseySurfaceViewport::GetVerticalScrollBar( ) const
{
    return  TextureViewportVerticalScrollBar;
}


TSharedPtr< SScrollBar >
SOdysseySurfaceViewport::GetHorizontalScrollBar( ) const
{
    return  TextureViewportHorizontalScrollBar;
}


IOdysseySurface*
SOdysseySurfaceViewport::GetSurface() const
{
    return  Surface;
}


float SOdysseySurfaceViewport::GetViewportVerticalScrollBarRatio() const
{
    uint32 Height = 1;
    uint32 Width = 1;
    float WidgetHeight = 1.0f;
    if (GetVerticalScrollBar().IsValid())
    {
        CalculateTextureDisplayDimensions(Width, Height);

        WidgetHeight = GetViewport()->GetSizeXY().Y;
    }

    return WidgetHeight / Height;
}


float SOdysseySurfaceViewport::GetViewportHorizontalScrollBarRatio() const
{
    uint32 Width = 1;
    uint32 Height = 1;
    float WidgetWidth = 1.0f;
    if (GetHorizontalScrollBar().IsValid())
    {
        CalculateTextureDisplayDimensions(Width, Height);

        WidgetWidth = GetViewport()->GetSizeXY().X;
    }

    return WidgetWidth / Width;
}


void SOdysseySurfaceViewport::SetViewportClient(TSharedPtr<class FViewportClient> InViewportClient)
{
    if (!InViewportClient.IsValid())
        return;

    if( ViewportClient.IsValid() )
    {
        ViewportClient.Reset();
        Viewport.Reset();
        ViewportWidget.Reset();
    }

    ViewportClient  = InViewportClient;
    Viewport        = MakeShareable(new FOdysseySceneViewport(ViewportClient.Get(), ViewportWidget));
    ViewportWidget->SetViewportInterface(Viewport.ToSharedRef());
}


//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- SWidget overrides
void SOdysseySurfaceViewport::Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime )
{
    Viewport->Invalidate();
    Viewport->InvalidateDisplay();
}


//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Private Callbacks
void
SOdysseySurfaceViewport::HandleHorizontalScrollBarScrolled( float InScrollOffsetFraction )
{
    float Ratio = GetViewportHorizontalScrollBarRatio();
    float MaxOffset = (Ratio < 1.0f) ? 1.0f - Ratio : 0.0f;
    InScrollOffsetFraction = FMath::Clamp(InScrollOffsetFraction, 0.0f, MaxOffset);
    TextureViewportHorizontalScrollBar->SetState(InScrollOffsetFraction, Ratio);
}


EVisibility
SOdysseySurfaceViewport::HandleHorizontalScrollBarVisibility() const
{
    if( GetViewportHorizontalScrollBarRatio() < 1.0f )
        return  EVisibility::Visible;

    return  EVisibility::Collapsed;
}


void
SOdysseySurfaceViewport::HandleVerticalScrollBarScrolled( float InScrollOffsetFraction )
{
    float Ratio = GetViewportVerticalScrollBarRatio();
    float MaxOffset = (Ratio < 1.0f) ? 1.0f - Ratio : 0.0f;
    InScrollOffsetFraction = FMath::Clamp(InScrollOffsetFraction, 0.0f, MaxOffset);

    TextureViewportVerticalScrollBar->SetState(InScrollOffsetFraction, Ratio);
}


EVisibility
SOdysseySurfaceViewport::HandleVerticalScrollBarVisibility()  const
{
    if (GetViewportVerticalScrollBarRatio() < 1.0f)
        return  EVisibility::Visible;

    return  EVisibility::Collapsed;
}


void
SOdysseySurfaceViewport::HandleZoomMenuEntryClicked( double ZoomValue )
{
    SetZoom( ZoomValue );
}


void
SOdysseySurfaceViewport::HandleZoomMenuFitClicked()
{
    ToggleFitToViewport();
}

void
SOdysseySurfaceViewport::HandleRotationLeft()
{
    RotateLeft();
}

void
SOdysseySurfaceViewport::HandleRotationRight()
{
    RotateRight();
}

void
SOdysseySurfaceViewport::HandleViewportReset()
{
    SetRotationInDegrees( 0 );
    SetPan(FVector2D(0, 0));
}


bool
SOdysseySurfaceViewport::IsZoomMenuFitChecked() const
{
    return GetFitToViewport();
}


FText
SOdysseySurfaceViewport::HandleZoomPercentageText( ) const
{
    const bool bFitToViewport = GetFitToViewport();
    if(bFitToViewport)
    {
        return LOCTEXT("ZoomFitText", "Fit");
    }

    return FText::AsPercent(GetZoom());
}


void
SOdysseySurfaceViewport::HandleRotationChanged( int newRotation )
{
    SetRotationInDegrees( newRotation );
}

int
SOdysseySurfaceViewport::HandleRotationValue( ) const
{
    return ( fmod(GetRotationInDegrees(), 360.0) );
}

FText
SOdysseySurfaceViewport::HandleSurfaceInfosTextValue( ) const
{
    if (!Surface || !Surface->Texture() /* || !Surface->Block() */)
        return NSLOCTEXT("No Texture Provided","No Texture Provided", "No Texture Provided");

    /* ::ul3::FFormatMetrics format(Surface->Block()->Format());
    FText formatName;
    switch(format.CM)
    {
        CM_GREY: formatName = FText::FromString("Grey"); break;
        CM_RGB: formatName = FText::FromString("RGB"); break;
        CM_HSV:	formatName = FText::FromString("HSV"); break;
        CM_HSL:	formatName = FText::FromString("HSL"); break;
        CM_CMY:	formatName = FText::FromString("CMY"); break;
        CM_CMYK: formatName = FText::FromString("CMYK"); break;
        CM_YUV: formatName = FText::FromString("YUV"); break;
        CM_Lab:	formatName = FText::FromString("Lab"); break;
        CM_XYZ:	formatName = FText::FromString("XYZ"); break;
        CM_Yxy: formatName = FText::FromString("Yxy"); break;

        default: break;
    }

    if (Surface->Block()->HasAlpha())
    {
        formatName = formatName.Join("", FText::FromString("A"));
    } */

    //return FText::Format( NSLOCTEXT("Texture Infos","Texture Infos","{0}x{1} px | {2} {3} bits"), FText::AsNumber( Surface->Width() ), FText::AsNumber( Surface->Height() ), formatName, FText::AsNumber(format.BPC) );
    return FText::Format( NSLOCTEXT("Texture Infos","Texture Infos","{0}x{1} px"), FText::AsNumber( Surface->Width() ), FText::AsNumber( Surface->Height() ));
}


void
SOdysseySurfaceViewport::HandleZoomSliderChanged( float NewValue )
{
    SetZoom( NewValue * MaxZoom );
}


float
SOdysseySurfaceViewport::HandleZoomSliderValue( ) const
{
    return  ( GetZoom() / MaxZoom );
}


//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Navigation API
double
SOdysseySurfaceViewport::GetZoom() const
{
    return Zoom;
}


void
SOdysseySurfaceViewport::SetZoom( double ZoomValue )
{
    Zoom = FMath::Clamp( ZoomValue, MinZoom, MaxZoom );
    SetFitToViewport( false );

    mOnParameterChanged.Broadcast();
}


void
SOdysseySurfaceViewport::ZoomIn()
{
    SetZoom( Zoom + ZoomStep );
}


void
SOdysseySurfaceViewport::ZoomOut()
{
    SetZoom( Zoom - ZoomStep );
}


bool
SOdysseySurfaceViewport::GetFitToViewport() const
{
    return IsFitToViewport;
}


void
SOdysseySurfaceViewport::SetFitToViewport( bool bFitToViewport )
{
    IsFitToViewport = bFitToViewport;
}


void
SOdysseySurfaceViewport::ToggleFitToViewport()
{
    bool bFitToViewport = GetFitToViewport();
    SetFitToViewport(!bFitToViewport);
}


double SOdysseySurfaceViewport::GetRotationInDegrees() const
{
    return Rotation;
}

void SOdysseySurfaceViewport::SetRotationInDegrees(double RotationValue)
{
    Rotation = RotationValue;

    mOnParameterChanged.Broadcast();
}

FVector2D SOdysseySurfaceViewport::GetPan() const
{
    return Pan;
}

void SOdysseySurfaceViewport::SetPan( FVector2D PanValue )
{
    Pan = PanValue;

    mOnParameterChanged.Broadcast();
}

void SOdysseySurfaceViewport::AddPan( FVector2D PanValue )
{
    Pan+=PanValue;

    mOnParameterChanged.Broadcast();
}


void SOdysseySurfaceViewport::RotateLeft()
{
    Rotation -= RotationStep;

    mOnParameterChanged.Broadcast();
}

void SOdysseySurfaceViewport::RotateRight()
{
    Rotation += RotationStep;

    mOnParameterChanged.Broadcast();
}

void SOdysseySurfaceViewport::CalculateTextureDisplayDimensions( uint32& Width, uint32& Height ) const
{
    if (!Surface || !Surface->Texture())
    {
        Width = 0;
        Height = 0;
        return;
    }

    uint32 ImportedWidth = Surface->Texture()->GetSizeX(); //Get the displayed size of the texture instead of its Source size
    uint32 ImportedHeight = Surface->Texture()->GetSizeY();

    Width = ImportedWidth;
    Height = ImportedHeight;


    // catch if the Width and Height are still zero for some reason
    if ((Width == 0) || (Height == 0))
    {
        Width = 0;
        Height = 0;

        return;
    }

    // See if we need to uniformly scale it to fit in viewport
    // Cap the size to effective dimensions
    uint32 ViewportW = GetViewport()->GetSizeXY().X;
    uint32 ViewportH = GetViewport()->GetSizeXY().Y;
    uint32 MaxWidth;
    uint32 MaxHeight;

    const bool bFitToViewport = GetFitToViewport();
    if (bFitToViewport)
    {
        // Subtract off the viewport space devoted to padding (2 * PreviewPadding)
        // so that the texture is padded on all sides
        MaxWidth = ViewportW;
        MaxHeight = ViewportH;


        // First, scale up based on the size of the viewport
        if (MaxWidth > MaxHeight)
        {
            Height = Height * MaxWidth / Width;
            Width = MaxWidth;
        }
        else
        {
            Width = Width * MaxHeight / Height;
            Height = MaxHeight;
        }

        // then, scale again if our width and height is impacted by the scaling
        if (Width > MaxWidth)
        {
            Height = Height * MaxWidth / Width;
            Width = MaxWidth;
        }
        if (Height > MaxHeight)
        {
            Width = Width * MaxHeight / Height;
            Height = MaxHeight;
        }
    }
    else
    {
        Width = Surface->Texture()->GetSizeX() * GetZoom();
        Height = Surface->Texture()->GetSizeY() * GetZoom();
    }
}


#undef LOCTEXT_NAMESPACE
