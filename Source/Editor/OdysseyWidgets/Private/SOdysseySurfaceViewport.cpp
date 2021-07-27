// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "SOdysseySurfaceViewport.h"
#include "ObjectEditorUtils.h"
#include "Engine/Texture.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SScrollBar.h"
#include "Widgets/SToolTip.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/SViewport.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Input/NumericTypeInterface.h"
#include "Widgets/Input/NumericUnitTypeInterface.inl"
#include "FOdysseySceneViewport.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "OdysseySurface.h"
#include "OdysseyBlock.h"
#include "OdysseyStyleSet.h"

#include <ULIS3>



#define MinZoom 0.01
#define MaxZoom 20.0
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
    mSurface = InArgs._Surface;
    mOnParameterChanged = InArgs._OnParameterChanged;

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

        /*FUIAction AutoFilterAction(
        FExecuteAction::CreateSP(this,&SOdysseySurfaceViewport::HandleAutoFilterClicked),
        FCanExecuteAction(),
        FIsActionChecked::CreateSP(this,&SOdysseySurfaceViewport::IsAutoFilterChecked)
        );
        ZoomMenuBuilder.AddMenuEntry(LOCTEXT("AutoFilterAction","Auto filter"),LOCTEXT("AutoFilterActionHint","Select the best filter (Nearest Neighbour or Bilinear) for the texture depending on the zoom level"),FSlateIcon(),AutoFilterAction,NAME_None,EUserInterfaceActionType::ToggleButton);*/
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
                                        SAssignNew(mViewportWidget, SViewport)
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
                        SAssignNew(mTextureViewportVerticalScrollBar, SScrollBar)
                            .AlwaysShowScrollbar(true)
                            // .Visibility(EVisibility::Visible)
                            // .Visibility(this, &SOdysseySurfaceViewport::HandleVerticalScrollBarVisibility)
                            .OnUserScrolled(this, &SOdysseySurfaceViewport::HandleVerticalScrollBarScrolled)
                    ]
            ]

        + SVerticalBox::Slot()
            .AutoHeight()
            [
                // horizontal scrollbar
                SAssignNew(mTextureViewportHorizontalScrollBar, SScrollBar)
                    .Orientation( Orient_Horizontal )
                    .AlwaysShowScrollbar(true)
                    // .Visibility(EVisibility::Visible)
                    // .Visibility(this, &SOdysseySurfaceViewport::HandleHorizontalScrollBarVisibility)
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
                        SNew(SSpinBox< int >)
                            .Value(this, &SOdysseySurfaceViewport::HandleRotationValue)
                            .OnValueChanged(this, &SOdysseySurfaceViewport::HandleRotationChanged)
                            .LinearDeltaSensitivity(10)  // If we're an unbounded spinbox, what value do we divide mouse movement by before multiplying by Delta. Requires Delta to be set.
                            .Delta(1)
                            .TypeInterface(MakeShared<TNumericUnitTypeInterface< int >>(EUnit::Degrees))
                            .MinDesiredWidth(48.0f)
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
            .HAlign(HAlign_Right)
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
                        //TODO UE5: MaxFractionnal digits is set correctly in UE5. In UE4, we have to call SetMaxFractionnalDigits/SetMinFractionalDigits
                        SAssignNew( mZoomSpinBox, SSpinBox< float >)
                            .Value(this, &SOdysseySurfaceViewport::GetGuiZoomValue)
                            .OnValueChanged(this, &SOdysseySurfaceViewport::HandleZoomSliderChanged)
                            .LinearDeltaSensitivity(20)  // If we're an unbounded spinbox, what value do we divide mouse movement by before multiplying by Delta. Requires Delta to be set.
                            .Delta(1.f)
                            .TypeInterface(MakeShared<TNumericUnitTypeInterface<float>>(EUnit::Percentage))
                            .MaxFractionalDigits(2)
                            .MinFractionalDigits(2)
                            .MinDesiredWidth(70.0f)
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

    mZoomSpinBox->SetMaxFractionalDigits(2);
    mZoomSpinBox->SetMinFractionalDigits(2);
    SetZoom( 1.0 );
    SetRotationInDegrees( 0 );
    SetPan( FVector2D( 0.f, 0.f ) );
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API
/* void
SOdysseySurfaceViewport::SetSurface( IOdysseySurface* iValue )
{
    Surface = iValue;
} */


TSharedPtr< FOdysseySceneViewport >
SOdysseySurfaceViewport::GetViewport() const
{
    return  mViewport;
}


TSharedPtr< SViewport >
SOdysseySurfaceViewport::GetViewportWidget( ) const
{
    return  mViewportWidget;
}


TSharedPtr< SScrollBar >
SOdysseySurfaceViewport::GetVerticalScrollBar( ) const
{
    return  mTextureViewportVerticalScrollBar;
}


TSharedPtr< SScrollBar >
SOdysseySurfaceViewport::GetHorizontalScrollBar( ) const
{
    return  mTextureViewportHorizontalScrollBar;
}


IOdysseySurface*
SOdysseySurfaceViewport::GetSurface() const
{
    return  mSurface.Get();
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

    if( mViewportClient.IsValid() )
    {
        mViewportClient.Reset();
        mViewport.Reset();
        mViewportWidget.Reset();
    }

    mViewportClient  = InViewportClient;
    mViewport        = MakeShareable(new FOdysseySceneViewport(mViewportClient.Get(), mViewportWidget));
    mViewportWidget->SetViewportInterface(mViewport.ToSharedRef());
}


//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- SWidget overrides
void SOdysseySurfaceViewport::Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime )
{
    mViewport->Invalidate();
    mViewport->InvalidateDisplay();
}


//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Private Callbacks

float
SOdysseySurfaceViewport::GetGuiZoomValue() const
{
    return mZoom * 100;
}

void
SOdysseySurfaceViewport::HandleHorizontalScrollBarScrolled( float InScrollOffsetFraction )
{
    float Ratio = GetViewportHorizontalScrollBarRatio();
    float MaxOffset = (Ratio < 1.0f) ? 1.0f - Ratio : 0.0f;
    InScrollOffsetFraction = FMath::Clamp(InScrollOffsetFraction, 0.0f, MaxOffset);
    mTextureViewportHorizontalScrollBar->SetState(InScrollOffsetFraction, Ratio);
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

    mTextureViewportVerticalScrollBar->SetState(InScrollOffsetFraction, Ratio);
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
SOdysseySurfaceViewport::HandleAutoFilterClicked()
{
    ToggleAutoFilter();
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

bool
SOdysseySurfaceViewport::IsAutoFilterChecked() const
{
    return mIsAutoFilter;
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
    IOdysseySurface* surface = GetSurface();
    if (!surface)
        return NSLOCTEXT("No Texture Provided","No Texture Provided", "No Texture Provided");

    UTexture* texture       = surface->Texture();
    if (!texture)
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
    return FText::Format( NSLOCTEXT("Texture Infos","Texture Infos","{0}x{1} px"), FText::AsNumber(surface->Width() ), FText::AsNumber(surface->Height() ));
}


void
SOdysseySurfaceViewport::HandleZoomSliderChanged( float NewValue )
{
    SetZoom( NewValue / 100.f );
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
    return mZoom;
}


void
SOdysseySurfaceViewport::SetZoom( double ZoomValue )
{
    mZoom = FMath::Clamp( ZoomValue, MinZoom, MaxZoom );

    SetFitToViewport( false );

    IOdysseySurface* surface = GetSurface();
    if (surface)
    {
        UTexture* texture       = surface->Texture();
        if (texture)
        {
            if( mZoom >= 1.0 && texture->Filter != TextureFilter::TF_Nearest )
            {
                FObjectEditorUtils::SetPropertyValue(texture, "Filter", TextureFilter::TF_Nearest);
            }
            else if( mZoom < 1.0 && texture->Filter != TextureFilter::TF_Bilinear )
            {
                FObjectEditorUtils::SetPropertyValue(texture, "Filter", TextureFilter::TF_Bilinear);
            }
        }
    }

    mOnParameterChanged.ExecuteIfBound();
}


void
SOdysseySurfaceViewport::ZoomIn()
{
    SetZoom( mZoom + ZoomStep );
}


void
SOdysseySurfaceViewport::ZoomOut()
{
    SetZoom( mZoom - ZoomStep );
}


bool
SOdysseySurfaceViewport::GetFitToViewport() const
{
    return mIsFitToViewport;
}


void
SOdysseySurfaceViewport::SetFitToViewport( bool bFitToViewport )
{
    mIsFitToViewport = bFitToViewport;
}


void
SOdysseySurfaceViewport::ToggleFitToViewport()
{
    bool bFitToViewport = GetFitToViewport();
    SetFitToViewport(!bFitToViewport);
}

void
SOdysseySurfaceViewport::ToggleAutoFilter()
{
    mIsAutoFilter = !mIsAutoFilter;
}


double SOdysseySurfaceViewport::GetRotationInDegrees() const
{
    return mRotation;
}

void SOdysseySurfaceViewport::SetRotationInDegrees(double RotationValue)
{
    mRotation = RotationValue;

    mOnParameterChanged.ExecuteIfBound();
}

FVector2D SOdysseySurfaceViewport::GetPan() const
{
    return mPan;
}

void SOdysseySurfaceViewport::SetPan( FVector2D PanValue )
{
    mPan = PanValue;

    mOnParameterChanged.ExecuteIfBound();
}

void SOdysseySurfaceViewport::AddPan( FVector2D PanValue )
{
    mPan+=PanValue;

    mOnParameterChanged.ExecuteIfBound();
}


void SOdysseySurfaceViewport::RotateLeft()
{
    mRotation -= RotationStep;

    mOnParameterChanged.ExecuteIfBound();
}

void SOdysseySurfaceViewport::RotateRight()
{
    mRotation += RotationStep;

    mOnParameterChanged.ExecuteIfBound();
}

void SOdysseySurfaceViewport::CalculateTextureDisplayDimensions( uint32& Width, uint32& Height ) const
{
    IOdysseySurface* surface = GetSurface();
    if (!surface)
    {
        Width = 0;
        Height = 0;
        return;
    }

    UTexture* texture = surface->Texture();
    if (!texture)
    {
        Width = 0;
        Height = 0;
        return;
    }

    uint32 ImportedWidth = texture->GetSurfaceWidth(); //Get the displayed size of the texture instead of its Source size
    uint32 ImportedHeight = texture->GetSurfaceHeight();

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
        Width = texture->GetSurfaceWidth() * GetZoom();
        Height = texture->GetSurfaceHeight() * GetZoom();
    }
}


#undef LOCTEXT_NAMESPACE
