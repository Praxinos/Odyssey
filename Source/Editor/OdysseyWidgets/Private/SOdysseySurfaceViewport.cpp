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
#include "OdysseyMathUtils.h"

#include <ULIS>



#define MinZoom 0.01
#define MaxZoom 20.0
#define NeutralZoom 1.0
#define ZoomStep 0.025
#define RotationStep 15

#define ScrollbarThumbRatio 0.1f
#define ScrollbarSpaceRatio (1.f - ScrollbarThumbRatio)


#define LOCTEXT_NAMESPACE "OdysseySurfaceViewport"

/////////////////////////////////////////////////////
// SOdysseySurfaceViewport
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
void
SOdysseySurfaceViewport::Construct( const FArguments& InArgs )
{
    mSurface = InArgs._Surface;

    mTransform = FTransform2D(1.0f, FVector2D(0.0f, 0.0f));

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
                        SAssignNew(mVerticalScrollBar, SScrollBar)
                            .AlwaysShowScrollbar(true)
                            .Thickness(FVector2D(10.f, 10.f))
                            .OnUserScrolled(this, &SOdysseySurfaceViewport::HandleVerticalScrollBarScrolled)
                    ]
            ]

        + SVerticalBox::Slot()
            .AutoHeight()
            [
                // horizontal scrollbar
                SAssignNew(mHorizontalScrollBar, SScrollBar)
                    .Orientation( Orient_Horizontal )
                    .AlwaysShowScrollbar(true)
                    .Thickness(FVector2D(10.f, 10.f))
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
                .Text( this, &SOdysseySurfaceViewport::GetSurfaceInfosValue)
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
                            .Value(this, &SOdysseySurfaceViewport::GetGuiRotationValue)
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
                        SAssignNew( mZoomSpinBox, SSpinBox< float > )
                            .Value( this, &SOdysseySurfaceViewport::GetGuiZoomValue )
                            .OnValueChanged( this, &SOdysseySurfaceViewport::HandleZoomSliderChanged )
                            // .LinearDeltaSensitivity(20)  // If we're an unbounded spinbox, what value do we divide mouse movement by before multiplying by Delta. Requires Delta to be set. But we choosed not to use this option here.
                            .TypeInterface(MakeShared<TNumericUnitTypeInterface<float>>(EUnit::Percentage))
                            .ShiftMouseMovePixelPerDelta( 15 )
                            .Delta( 1 )
                            .SliderExponent( 0.8f ) // Can't work properly if the following options are in use :  .LinearDeltaSensitivity .MinValue .MaxValue
                            .SliderExponentNeutralValue( 100.0f )
                            .MaxFractionalDigits( 2 )
                            .MinFractionalDigits( 2 )
                            .MinDesiredWidth( 70.0f )

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
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API


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
    return  mVerticalScrollBar;
}


TSharedPtr< SScrollBar >
SOdysseySurfaceViewport::GetHorizontalScrollBar( ) const
{
    return  mHorizontalScrollBar;
}


IOdysseySurface*
SOdysseySurfaceViewport::GetSurface() const
{
    return  mSurface.Get();
}


void
SOdysseySurfaceViewport::UpdateScrollBars()
{
    IOdysseySurface* surface = GetSurface();
    if (!surface)
        return;

    UTexture* texture = surface->Texture();
    if (!texture)
        return;

    float width = texture->GetSurfaceWidth();
    float height = texture->GetSurfaceHeight();

    //Get the BoundingBox

    //All calculations are done in the Transform Coodinate system
    //So (0,0) is bottom left
    TArray<FVector2D> points;
    points.Add(mTransform.TransformPoint(FVector2D(width / 2.f, height / 2.f)));
    points.Add(mTransform.TransformPoint(FVector2D(-width / 2.f, height / 2.f)));
    points.Add(mTransform.TransformPoint(FVector2D(width / 2.f, -height / 2.f)));
    points.Add(mTransform.TransformPoint(FVector2D(-width / 2.f, -height / 2.f)));

    FBox2D bbox(points);
    
    FVector2D minPos( -bbox.GetSize().X / 2.f, -bbox.GetSize().Y / 2.f);
    FVector2D maxPos( mViewport->GetSizeXY().X + bbox.GetSize().X / 2.f, mViewport->GetSizeXY().Y + bbox.GetSize().Y / 2.f);
    
    FVector2D dist = maxPos - minPos;
    FVector2D center = bbox.GetCenter() + GetViewportCenter();

    FVector2D pos = (center - minPos) / dist;
    pos = pos.ClampAxes(0.f, 1.f) ;

    mHorizontalScrollBar->SetState((1.0 - pos.X) * ScrollbarSpaceRatio, ScrollbarThumbRatio);
    mVerticalScrollBar->SetState(pos.Y * ScrollbarSpaceRatio, ScrollbarThumbRatio);
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

    UpdateScrollBars();
}


//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- SWidget overrides
void SOdysseySurfaceViewport::Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime )
{
    if (mIsFitToViewport) //If someone wants to, find somewhere else than the tick method to fit the viewport continuously
        FitToViewport();

    mViewport->Invalidate();
    mViewport->InvalidateDisplay();
}


//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Private Callbacks

float
SOdysseySurfaceViewport::GetGuiZoomValue() const
{
    return GetZoom() * 100;
}

FVector2D
SOdysseySurfaceViewport::GetTranslationFromSlidersOffsets( float InScrollOffsetFractionX, float InScrollOffsetFractionY )
{
    IOdysseySurface* surface = GetSurface();
    if (!surface)
        return FVector2D(0.f, 0.f);

    UTexture* texture = surface->Texture();
    if (!texture)
        return FVector2D(0.f, 0.f);

    float width = texture->GetSurfaceWidth();
    float height = texture->GetSurfaceHeight();

    //Get the BoundingBox

    //All calculations are done in the Transform Coodinate system
    //So (0,0) is bottom left
    TArray<FVector2D> points;
    points.Add(mTransform.TransformPoint(FVector2D(width / 2.f, height / 2.f)));
    points.Add(mTransform.TransformPoint(FVector2D(-width / 2.f, height / 2.f)));
    points.Add(mTransform.TransformPoint(FVector2D(width / 2.f, -height / 2.f)));
    points.Add(mTransform.TransformPoint(FVector2D(-width / 2.f, -height / 2.f)));

    FBox2D bbox(points);

    FVector2D minPos(-bbox.GetSize().X / 2.f, -bbox.GetSize().Y / 2.f);
    FVector2D maxPos(mViewport->GetSizeXY().X + bbox.GetSize().X / 2.f, mViewport->GetSizeXY().Y + bbox.GetSize().Y / 2.f);

    FVector2D dist = maxPos - minPos;

    FVector2D pos = FVector2D( 1.f - (InScrollOffsetFractionX / (ScrollbarSpaceRatio)), InScrollOffsetFractionY / (ScrollbarSpaceRatio));
    pos *= dist;
    pos += minPos;
    pos -= GetViewportCenter();

    return pos; 
}



void
SOdysseySurfaceViewport::HandleHorizontalScrollBarScrolled(float InScrollOffsetFraction)
{
    FVector2D translation = GetTranslationFromSlidersOffsets(InScrollOffsetFraction, mVerticalScrollBar->DistanceFromTop());
    mTransform.SetTranslation(FVector2D(translation.X, mTransform.GetTranslation().Y));
    mHorizontalScrollBar->SetState(FMath::Clamp(InScrollOffsetFraction, 0.f, ScrollbarSpaceRatio), ScrollbarThumbRatio);
    SetFitToViewport(false);
}


void
SOdysseySurfaceViewport::HandleVerticalScrollBarScrolled( float InScrollOffsetFraction )
{
    FVector2D translation = GetTranslationFromSlidersOffsets(mHorizontalScrollBar->DistanceFromTop(), InScrollOffsetFraction);
    mTransform.SetTranslation(FVector2D(mTransform.GetTranslation().X, translation.Y));
    mVerticalScrollBar->SetState(FMath::Clamp(InScrollOffsetFraction, 0.f, ScrollbarSpaceRatio), ScrollbarThumbRatio);
    SetFitToViewport(false);
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
    Pan(FVector2D(0.f, 0.f));
    Rotate( 0 );
    Zoom(1.f);
    SetFitToViewport(false);
    UpdateScrollBars();
}


bool
SOdysseySurfaceViewport::IsZoomMenuFitChecked() const
{
    return GetFitToViewport();
}

void
SOdysseySurfaceViewport::HandleRotationChanged( int newRotation )
{
    SetRotation( -FMath::DegreesToRadians(newRotation) );
}

int
SOdysseySurfaceViewport::GetGuiRotationValue() const
{
    float angle = -FMath::RadiansToDegrees(GetRotation());
    if (angle < 0)
        angle += 360.0;
    return FMath::RoundHalfFromZero(angle);
}

FText
SOdysseySurfaceViewport::GetSurfaceInfosValue( ) const
{
    IOdysseySurface* surface = GetSurface();
    if (!surface)
        return NSLOCTEXT("No Texture Provided","No Texture Provided", "No Texture Provided");

    UTexture* texture       = surface->Texture();
    if (!texture)
        return NSLOCTEXT("No Texture Provided","No Texture Provided", "No Texture Provided");

    /* ::ULIS::FFormatMetrics format(Surface->Block()->Format());
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
    SetZoom( NewValue / 100.f);
}


//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Navigation API
double
SOdysseySurfaceViewport::GetZoom() const
{
    return mTransform.GetMatrix().GetScale().GetVector()[0];
}


void
SOdysseySurfaceViewport::SetZoom( double ZoomValue, const FVector2D& iZoomPosition)
{
    Zoom(ZoomValue, iZoomPosition);
    SetFitToViewport(false);
    UpdateScrollBars();
}

void
SOdysseySurfaceViewport::Zoom(double ZoomValue, const FVector2D& iZoomPosition)
{
    ZoomValue = FMath::Clamp( ZoomValue, MinZoom, MaxZoom );

    FVector2D inverse(1.f, -1.f);
    FVector2D translation = iZoomPosition * inverse;

    mTransform = mTransform.Concatenate(FTransform2D(-translation));
    mTransform = mTransform.Concatenate(FTransform2D(ZoomValue / GetZoom()));
    mTransform = mTransform.Concatenate(FTransform2D(translation));
}

void
SOdysseySurfaceViewport::ZoomExponential(float iBaseZoom, float iSliderOffsetToAdd, const FVector2D& iZoomPosition)
{
    double sliderPos = FMath::Loge(iBaseZoom);
    sliderPos += iSliderOffsetToAdd;
    double newZoom = FMath::Clamp(double(FMath::Exp(sliderPos)), MinZoom, MaxZoom);
    SetZoom( newZoom, iZoomPosition);
}

const FTransform2D& SOdysseySurfaceViewport::GetTransform() const
{
    return mTransform;
}

bool
SOdysseySurfaceViewport::GetFitToViewport() const
{
    return mIsFitToViewport;
}


void
SOdysseySurfaceViewport::SetFitToViewport( bool bFitToViewport )
{
    if (!bFitToViewport)
    {
        mIsFitToViewport = bFitToViewport;
        return;
    }

    mIsFitToViewport = bFitToViewport;
}

void
SOdysseySurfaceViewport::FitToViewport()
{
    IOdysseySurface* surface = GetSurface();
    if (!surface)
        return;

    UTexture* texture = surface->Texture();
    if (!texture)
        return;

    float width = texture->GetSurfaceWidth();
    float height = texture->GetSurfaceHeight();

    //Get the BoundingBox

    //All calculations are done in the Transform Coodinate system
    //So (0,0) is bottom left
    TArray<FVector2D> points;
    points.Add(mTransform.TransformPoint(FVector2D(width / 2.f, height / 2.f)));
    points.Add(mTransform.TransformPoint(FVector2D(-width / 2.f, height / 2.f)));
    points.Add(mTransform.TransformPoint(FVector2D(width / 2.f, -height / 2.f)));
    points.Add(mTransform.TransformPoint(FVector2D(-width / 2.f, -height / 2.f)));

    FBox2D bbox(points);

    float viewportRatio = mViewport->GetSizeXY().X / mViewport->GetSizeXY().Y;
    float textureRatio = bbox.GetSize().X / bbox.GetSize().Y;

    float zoom = GetZoom();
    if (textureRatio > viewportRatio)
    {
        //width
        zoom = GetZoom() * (mViewport->GetSizeXY().X / bbox.GetSize().X);
    }
    else
    {
        //height
        zoom = GetZoom() * (mViewport->GetSizeXY().Y / bbox.GetSize().Y);
    }

    Zoom(zoom);
    Pan(FVector2D(0.0f, 0.0f));
    UpdateScrollBars();
}


void
SOdysseySurfaceViewport::ToggleFitToViewport()
{
    SetFitToViewport(!mIsFitToViewport);
}


double SOdysseySurfaceViewport::GetRotation() const
{
    //PATCH: At first I wanted to use t.GetMatrix().GetRotationAngle(), but it uses Atan instead of Atan2 leading to wrong angles
    float A, B, C, D;
    mTransform.GetMatrix().GetMatrix(A, B, C, D);
    return FMath::Atan2(B, D);
    //return mTransform.GetMatrix().GetRotationAngle();
}

void SOdysseySurfaceViewport::SetRotation(double RotationValue, const FVector2D& iPivotPoint)
{
    Rotate(RotationValue, iPivotPoint);
    UpdateScrollBars();
    //SetFitToViewport(false);
}

void SOdysseySurfaceViewport::Rotate(double RotationValue, const FVector2D& iPivotPoint)
{
    mTransform = mTransform.Concatenate(FTransform2D(FVector2D(FVector2D(iPivotPoint.X, -iPivotPoint.Y))));
    mTransform = mTransform.Concatenate(FTransform2D(FQuat2D(RotationValue - GetRotation())));
    mTransform = mTransform.Concatenate(FTransform2D(FVector2D(-FVector2D(iPivotPoint.X, -iPivotPoint.Y))));
}

FVector2D SOdysseySurfaceViewport::GetPan() const
{
    //ordinate inversion because the computer coordinate system is inverted
    return FVector2D(mTransform.GetTranslation().X, -mTransform.GetTranslation().Y);
}

FVector2D SOdysseySurfaceViewport::GetViewportCenter() const
{
    return FVector2D(mViewport->GetSizeXY().X / 2.0f, mViewport->GetSizeXY().Y / 2.0f);
}

void SOdysseySurfaceViewport::AddPan( FVector2D iPanValue )
{
    //ordinate inversion because the computer coordinate system is inverted
    FVector2D panValue = mTransform.GetTranslation() + FVector2D(iPanValue.X, -iPanValue.Y);
    Pan(panValue);
    UpdateScrollBars();
    SetFitToViewport(false);
}

void
SOdysseySurfaceViewport::SOdysseySurfaceViewport::Pan(FVector2D iPanValue)
{
    mTransform.SetTranslation(iPanValue);
}

void SOdysseySurfaceViewport::ResetPan()
{
    mTransform.SetTranslation(FVector2D(0.0f, 0.0f));
    UpdateScrollBars();
    SetFitToViewport(false);
}


void SOdysseySurfaceViewport::RotateLeft()
{
    SetRotation(GetRotation() + FMath::DegreesToRadians(RotationStep));
}

void SOdysseySurfaceViewport::RotateRight()
{
    SetRotation(GetRotation() - FMath::DegreesToRadians(RotationStep));
}

void SOdysseySurfaceViewport::ComputeTextureDisplayDimensions( uint32& Width, uint32& Height ) const
{
    Width = 0;
    Height = 0;

    IOdysseySurface* surface = GetSurface();
    if (!surface)
        return;

    UTexture* texture = surface->Texture();
    if (!texture)
        return;
    
    Width = texture->GetSurfaceWidth() * GetZoom();
    Height = texture->GetSurfaceHeight() * GetZoom();
}

FVector2D
SOdysseySurfaceViewport::ToLocal(const FVector2D& iPoint) const
{
    FVector2D inverse(1.f, -1.f);
    FVector2D center = GetViewportCenter();
    FVector2D pos = (iPoint - center) * inverse;
    FVector2D tpos = mTransform.Inverse().TransformPoint(pos) * inverse;

    return tpos;
}

FVector2D
SOdysseySurfaceViewport::ToWorld(const FVector2D& iPoint) const
{
    FVector2D inverse(1.f, -1.f);
    FVector2D center = GetViewportCenter();
    FVector2D pos = iPoint * inverse;
    FVector2D tpos = (mTransform.TransformPoint(pos) * inverse) + center;

    return tpos;
}

#undef LOCTEXT_NAMESPACE
