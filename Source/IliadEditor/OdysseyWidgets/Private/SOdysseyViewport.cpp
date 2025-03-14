// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "SOdysseyViewport.h"
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
#include "OdysseyStyle.h"
#include "Math/OdysseyMathUtils.h"

#include <ULIS>



#define MinZoom 0.01
#define MaxZoom 200.0
#define NeutralZoom 1.0
#define ZoomStep 0.025
#define RotationStep 15

#define ScrollbarThumbRatio 0.1f
#define ScrollbarSpaceRatio (1.f - ScrollbarThumbRatio)


#define LOCTEXT_NAMESPACE "Widgets"

/////////////////////////////////////////////////////
// SOdysseyViewport
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
void
SOdysseyViewport::Construct( const FArguments& InArgs )
{
    mTexture = InArgs._Texture;

    mFlipStateUV = FVector2D(0.f, 0.f);
    mPan = FVector2D( 0.f, 0.f );
    mRotation = 0;
    mZoom = 1;

    UpdateTransform();

    mAlignWithViewport = true;

    FMenuBuilder ViewportMenu(true, NULL);

    ViewportMenu.AddSubMenu(
        FText::FromString("Zoom options"),
        FText::FromString(""),
        FNewMenuDelegate::CreateLambda([this](FMenuBuilder& ZoomMenuBuilder)
        {
                FUIAction Zoom25Action(FExecuteAction::CreateSP(this, &SOdysseyViewport::HandleZoomMenuEntryClicked, 0.25));
                ZoomMenuBuilder.AddMenuEntry(LOCTEXT("viewport.zoom.25percent.name", "25%"), LOCTEXT("viewport.zoom.25percent.tooltip", "Show the texture at a quarter of its size."), FSlateIcon(), Zoom25Action);

                FUIAction Zoom50Action(FExecuteAction::CreateSP(this, &SOdysseyViewport::HandleZoomMenuEntryClicked, 0.5));
                ZoomMenuBuilder.AddMenuEntry(LOCTEXT("viewport.zoom.50percent.name", "50%"), LOCTEXT("viewport.zoom.50percent.tooltip", "Show the texture at half its size."), FSlateIcon(), Zoom50Action);

                FUIAction Zoom100Action(FExecuteAction::CreateSP(this, &SOdysseyViewport::HandleZoomMenuEntryClicked, 1.0));
                ZoomMenuBuilder.AddMenuEntry(LOCTEXT("viewport.zoom.100percent.name", "100%"), LOCTEXT("viewport.zoom.100percent.tooltip", "Show the texture in its original size."), FSlateIcon(), Zoom100Action);

                FUIAction Zoom200Action(FExecuteAction::CreateSP(this, &SOdysseyViewport::HandleZoomMenuEntryClicked, 2.0));
                ZoomMenuBuilder.AddMenuEntry(LOCTEXT("viewport.zoom.200percent.name", "200%"), LOCTEXT("viewport.zoom.200percent.tooltip", "Show the texture at twice its size."), FSlateIcon(), Zoom200Action);

                FUIAction Zoom400Action(FExecuteAction::CreateSP(this, &SOdysseyViewport::HandleZoomMenuEntryClicked, 4.0));
                ZoomMenuBuilder.AddMenuEntry(LOCTEXT("viewport.zoom.400percent.name", "400%"), LOCTEXT("viewport.zoom.400percent.tooltip", "Show the texture at four times its size."), FSlateIcon(), Zoom400Action);

                ZoomMenuBuilder.AddMenuSeparator();

                FUIAction ZoomFitAction(
                    FExecuteAction::CreateSP(this, &SOdysseyViewport::HandleZoomMenuFitClicked),
                    FCanExecuteAction(),
                    FIsActionChecked::CreateSP(this, &SOdysseyViewport::IsZoomMenuFitChecked)
                );
                ZoomMenuBuilder.AddMenuEntry(LOCTEXT("viewport.zoom.fit.name", "Scale To Fit"), LOCTEXT("viewport.zoom.fit.tooltip", "Scale the texture to fit the viewport."), FSlateIcon(), ZoomFitAction, NAME_None, EUserInterfaceActionType::ToggleButton);
        })
    );

    ViewportMenu.AddSubMenu(
        FText::FromString("Flip options"),
        FText::FromString(""),
        FNewMenuDelegate::CreateLambda([this](FMenuBuilder& FlipMenuBuilder)
            {
                FUIAction AlignCenterOption(
                    FExecuteAction::CreateSP(this, &SOdysseyViewport::HandleAlignWithViewportClicked),
                    FCanExecuteAction(),
                    FIsActionChecked::CreateSP(this, &SOdysseyViewport::IsAlignWithViewportChecked)
                );
                FlipMenuBuilder.AddMenuEntry(LOCTEXT("viewport.flip.align.name", "Align with viewport center on flip"), LOCTEXT("viewport.flip.align.tooltip", "Keep the center of the viewport at the same position when flipping it when on"), FSlateIcon(), AlignCenterOption, NAME_None, EUserInterfaceActionType::ToggleButton);
            })
    );


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
                            .OnUserScrolled(this, &SOdysseyViewport::HandleVerticalScrollBarScrolled)
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
                    .OnUserScrolled(this, &SOdysseyViewport::HandleHorizontalScrollBarScrolled)
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
                .Text( this, &SOdysseyViewport::GetTextureInfosValue)
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
                        .Text(LOCTEXT("viewport.rotation", "Rotation: "))
                    ]
                + SHorizontalBox::Slot()
                    .Padding(2.0f, 0.0f)
                    .VAlign(VAlign_Center)
                    [
                        SNew(SSpinBox< int >)
                            .Value(this, &SOdysseyViewport::GetGuiRotationValue)
                            .OnValueChanged(this, &SOdysseyViewport::HandleRotationChanged)
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
                            .OnPressed(this, &SOdysseyViewport::HandleRotationLeft)
                        [
                            SNew(SImage) .Image(FOdysseyStyle::GetBrush("PainterEditor.RotateLeft"))
                        ]
                    ]

                + SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(4.0f, 0.0f)
                    .VAlign(VAlign_Center)
                    [
                        SNew(SButton)
                            .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                            .OnPressed(this, &SOdysseyViewport::HandleViewportReset)
                        [
                            SNew(SImage) .Image(FOdysseyStyle::GetBrush("PainterEditor.RotateReset"))
                        ]
                    ]

                + SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(4.0f, 0.0f)
                    .VAlign(VAlign_Center)
                    [
                        SNew(SButton)
                            .ButtonStyle( FCoreStyle::Get(), "NoBorder" )
                            .OnPressed(this, &SOdysseyViewport::HandleRotationRight)
                        [
                            SNew(SImage) .Image(FOdysseyStyle::GetBrush("PainterEditor.RotateRight"))
                        ]
                    ]
                + SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(4.0f, 0.0f)
                    .VAlign(VAlign_Center)
                    [
                        SNew(SCheckBox)
                            .Style(&FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("CheckBox.BasicOnOff"))
                            .OnCheckStateChanged(this, &SOdysseyViewport::HandleFlipHorizontal)
                            .IsChecked_Lambda([this]() -> ECheckBoxState { return mFlipStateUV.X == 1 ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; })
                            [
                                SNew(SImage).Image(FOdysseyStyle::GetBrush("PainterEditor.FlipHorizontal16"))
                            ]
                    ]
                + SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(4.0f, 0.0f)
                    .VAlign(VAlign_Center)
                    [
                        SNew(SCheckBox)
                            .Style(&FOdysseyStyle::GetWidgetStyle<FCheckBoxStyle>("CheckBox.BasicOnOff"))
                            .OnCheckStateChanged(this, &SOdysseyViewport::HandleFlipVertical)
                            .IsChecked_Lambda([this]() -> ECheckBoxState { return mFlipStateUV.Y == 1 ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; })
                            [
                                SNew(SImage).Image(FOdysseyStyle::GetBrush("PainterEditor.FlipVertical16"))
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
                            .Text(LOCTEXT("viewport.zoom", "Zoom:"))
                    ]

                + SHorizontalBox::Slot()
                    .FillWidth(1.0f)
                    .Padding(4.0f, 0.0f)
                    .VAlign(VAlign_Center)
                    [
                        //TODO UE5: MaxFractionnal digits is set correctly in UE5. In UE4, we have to call SetMaxFractionnalDigits/SetMinFractionalDigits
                        SAssignNew( mZoomSpinBox, SSpinBox< float > )
                            .Value( this, &SOdysseyViewport::GetGuiZoomValue )
                            .OnValueChanged( this, &SOdysseyViewport::HandleZoomSliderChanged )
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
                                ViewportMenu.MakeWidget()
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
SOdysseyViewport::GetViewport() const
{
    return  mViewport;
}


TSharedPtr< SViewport >
SOdysseyViewport::GetViewportWidget( ) const
{
    return  mViewportWidget;
}


TSharedPtr< SScrollBar >
SOdysseyViewport::GetVerticalScrollBar( ) const
{
    return  mVerticalScrollBar;
}


TSharedPtr< SScrollBar >
SOdysseyViewport::GetHorizontalScrollBar( ) const
{
    return  mHorizontalScrollBar;
}


UTexture*
SOdysseyViewport::GetTexture() const
{
    return  mTexture.Get();
}


void
SOdysseyViewport::UpdateScrollBars()
{
    UTexture* texture = GetTexture();
    if (!texture)
        return;

    float width = texture->GetSurfaceWidth();
    float height = texture->GetSurfaceHeight();

    //Get the BoundingBox

    //All calculations are done in the Transform Coordinate system
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
    mVerticalScrollBar->SetState((1.0 - pos.Y) * ScrollbarSpaceRatio, ScrollbarThumbRatio);
}

void SOdysseyViewport::UpdateTransform()
{
    mTransform = FTransform2D( GetFlipMatrix() );
    mTransform = mTransform.Concatenate( FTransform2D(FQuat2D(mRotation)));
    mTransform = mTransform.Concatenate( FTransform2D( mZoom, mPan ) );
}

const FMatrix2x2 SOdysseyViewport::GetFlipMatrix() const
{
    int flipX = mFlipStateUV.X == 0 ? 1 : -1;
    int flipY = mFlipStateUV.Y == 0 ? 1 : -1;

    const FMatrix2x2 flipState = FMatrix2x2(flipX, 0.f,
                                            0.f, flipY);

    return flipState;
}


void SOdysseyViewport::SetViewportClient(TSharedPtr<class FViewportClient> InViewportClient)
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

    SetFitToViewport(true);
    UpdateScrollBars();
}


//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- SWidget overrides
void SOdysseyViewport::Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime )
{
    if (mIsFitToViewport) //If someone wants to, find somewhere else than the tick method to fit the viewport continuously
        FitToViewport();

    mViewport->Invalidate();
    mViewport->InvalidateDisplay();
}


//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Private Callbacks

float
SOdysseyViewport::GetGuiZoomValue() const
{
    return GetZoom() * 100;
}

FVector2D
SOdysseyViewport::GetTranslationFromSlidersOffsets( float InScrollOffsetFractionX, float InScrollOffsetFractionY )
{
    UTexture* texture = GetTexture();
    if (!texture)
        return FVector2D(0.f, 0.f);

    float width = texture->GetSurfaceWidth();
    float height = texture->GetSurfaceHeight();

    //Get the BoundingBox

    //All calculations are done in the Transform Coordinate system
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

    FVector2D pos = FVector2D( 1.f - (InScrollOffsetFractionX / (ScrollbarSpaceRatio)), 1.f - (InScrollOffsetFractionY / (ScrollbarSpaceRatio)));
    pos *= dist;
    pos += minPos;
    pos -= GetViewportCenter();

    return pos;
}



void
SOdysseyViewport::HandleHorizontalScrollBarScrolled(float InScrollOffsetFraction)
{
    FVector2D translation = GetTranslationFromSlidersOffsets(InScrollOffsetFraction, mVerticalScrollBar->DistanceFromTop());
    mPan = FVector2D(translation.X, mPan.Y);
    UpdateTransform();
    mHorizontalScrollBar->SetState(FMath::Clamp(InScrollOffsetFraction, 0.f, ScrollbarSpaceRatio), ScrollbarThumbRatio);
    SetFitToViewport(false);
}


void
SOdysseyViewport::HandleVerticalScrollBarScrolled( float InScrollOffsetFraction )
{
    FVector2D translation = GetTranslationFromSlidersOffsets(mHorizontalScrollBar->DistanceFromTop(), InScrollOffsetFraction);
    mPan = FVector2D(mPan.X, translation.Y);
    UpdateTransform();
    mVerticalScrollBar->SetState(FMath::Clamp(InScrollOffsetFraction, 0.f, ScrollbarSpaceRatio), ScrollbarThumbRatio);
    SetFitToViewport(false);
}


void
SOdysseyViewport::HandleZoomMenuEntryClicked( double ZoomValue )
{
    SetZoom( ZoomValue );
}


void
SOdysseyViewport::HandleZoomMenuFitClicked()
{
    ToggleFitToViewport();
}

void
SOdysseyViewport::HandleAlignWithViewportClicked()
{
    ToggleAlignWithViewport();
}

void
SOdysseyViewport::HandleRotationLeft()
{
    RotateLeft();
}

void
SOdysseyViewport::HandleRotationRight()
{
    RotateRight();
}

void SOdysseyViewport::HandleFlipHorizontal(ECheckBoxState iState)
{
    FlipHorizontal();
}

void SOdysseyViewport::HandleFlipVertical(ECheckBoxState iState)
{
    FlipVertical();
}

void
SOdysseyViewport::HandleViewportReset()
{
    mTransform = FTransform2D(1.0f, FVector2D(0.0f, 0.0f));
    mFlipStateUV = FVector2D(0.f, 0.f);
    mPan = FVector2D( 0.f, 0.f );
    mRotation = 0;
    mZoom = 1.f;
    SetFitToViewport(false);
    UpdateScrollBars();
}


bool
SOdysseyViewport::IsZoomMenuFitChecked() const
{
    return GetFitToViewport();
}

bool
SOdysseyViewport::IsAlignWithViewportChecked() const
{
    return mAlignWithViewport;
}

void
SOdysseyViewport::HandleRotationChanged( int newRotation )
{
    SetRotation( FMath::DegreesToRadians(newRotation) );
}

int
SOdysseyViewport::GetGuiRotationValue() const
{
    float angle = FMath::RadiansToDegrees(GetRotation());
    if (angle < 0)
        angle += 360.0;

    FMath::Fmod( angle, 360.0 );
    return FMath::RoundHalfFromZero(angle);
}

FText
SOdysseyViewport::GetTextureInfosValue( ) const
{
    UTexture* texture       = GetTexture();
    if (!texture)
        return LOCTEXT("viewport.no-texture-provided", "No Texture Provided");

    return FText::Format( LOCTEXT("viewport.texture-dimensions","{0}x{1} px"), FText::AsNumber(texture->GetSurfaceWidth() ), FText::AsNumber(texture->GetSurfaceHeight() ));
}


void
SOdysseyViewport::HandleZoomSliderChanged( float NewValue )
{
    SetZoom( NewValue / 100.f);
}


//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Navigation API
double
SOdysseyViewport::GetZoom() const
{
    return mZoom;
}


void
SOdysseyViewport::SetZoom( double ZoomValue, const FVector2D& iZoomPosition)
{
    Zoom(ZoomValue, iZoomPosition);
    SetFitToViewport(false);
    UpdateScrollBars();
}

void
SOdysseyViewport::Zoom(double ZoomValue, const FVector2D& iZoomPosition)
{
    ZoomValue = FMath::Clamp( ZoomValue, MinZoom, MaxZoom );

    FTransform2D zoomTransform = FTransform2D(ZoomValue / GetZoom(), FVector2D(0, 0));

    mZoom = ZoomValue;

    FVector2D translation = iZoomPosition;

    FTransform2D translationDiff = mTransform.Concatenate(FTransform2D(-translation));
    translationDiff = FTransform2D(Concatenate(translationDiff, zoomTransform.GetMatrix()));
    translationDiff = translationDiff.Concatenate(FTransform2D(translation));

    mPan = translationDiff.GetTranslation();

    UpdateTransform();
}

void
SOdysseyViewport::ZoomExponential(float iBaseZoom, float iSliderOffsetToAdd, const FVector2D& iZoomPosition)
{
    double sliderPos = FMath::Loge(iBaseZoom);
    sliderPos += iSliderOffsetToAdd;
    double newZoom = FMath::Clamp(double(FMath::Exp(sliderPos)), MinZoom, MaxZoom);
    SetZoom( newZoom, iZoomPosition);
}

bool
SOdysseyViewport::GetFitToViewport() const
{
    return mIsFitToViewport;
}


void
SOdysseyViewport::SetFitToViewport( bool bFitToViewport )
{
    if (!bFitToViewport)
    {
        mIsFitToViewport = bFitToViewport;
        return;
    }

    mIsFitToViewport = bFitToViewport;
}

void
SOdysseyViewport::FitToViewport()
{
    UTexture* texture = GetTexture();
    if (!texture)
        return;

    if (mViewport->GetSizeXY().X == 0 || mViewport->GetSizeXY().Y == 0)
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

    float viewportRatio = (float)mViewport->GetSizeXY().X / (float)mViewport->GetSizeXY().Y;
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
SOdysseyViewport::ToggleFitToViewport()
{
    SetFitToViewport(!mIsFitToViewport);
}

void
SOdysseyViewport::ToggleAlignWithViewport()
{
    mAlignWithViewport = !mAlignWithViewport;
}


double SOdysseyViewport::GetRotation() const
{
    return mRotation;
}

void SOdysseyViewport::SetRotation(double RotationValue, const FVector2D& iPivotPoint)
{

    mRotation = fmod( 2 * PI + fmod(RotationValue, 2 * PI), 2 * PI); //Positive modulo;
    UpdateTransform();
    UpdateScrollBars();
    //SetFitToViewport(false);
}

void SOdysseyViewport::Rotate(double RotationValue, const FVector2D& iPivotPoint)
{
    mRotation = fmod( 2 * PI + fmod(mRotation + RotationValue, 2 * PI), 2 * PI); //Positive modulo
    UpdateTransform();
}

FVector2D SOdysseyViewport::GetPan() const
{
    return mPan;
}

FVector2D SOdysseyViewport::GetFlip() const
{
    return mFlipStateUV;
}

FVector2D SOdysseyViewport::GetViewportCenter() const
{
    return FVector2D(mViewport->GetSizeXY().X / 2.0f, mViewport->GetSizeXY().Y / 2.0f);
}

void SOdysseyViewport::AddPan( FVector2D iPanValue )
{
    mPan = mPan + iPanValue;
    UpdateTransform();
    UpdateScrollBars();
    SetFitToViewport(false);
}

void
SOdysseyViewport::SOdysseyViewport::Pan(FVector2D iPanValue)
{
    mPan = iPanValue;
    UpdateTransform();
}

void SOdysseyViewport::ResetPan()
{
    mPan = FVector2D(0.0f, 0.0f);
    UpdateTransform();
    UpdateScrollBars();
    SetFitToViewport(false);
}


void SOdysseyViewport::RotateLeft()
{
    Rotate(- FMath::DegreesToRadians(RotationStep));
}

void SOdysseyViewport::RotateRight()
{
    Rotate(FMath::DegreesToRadians(RotationStep));
}

void SOdysseyViewport::FlipHorizontal()
{
    mFlipStateUV.X = int(mFlipStateUV.X + 1) % 2;

    if( mAlignWithViewport )
    {
        HandleHorizontalScrollBarScrolled(ScrollbarSpaceRatio - mHorizontalScrollBar->DistanceFromTop());
        SetRotation(-mRotation);
    }

    UpdateTransform();
}

void SOdysseyViewport::FlipVertical()
{
    mFlipStateUV.Y = int(mFlipStateUV.Y + 1) % 2;

    if (mAlignWithViewport)
    {
        HandleVerticalScrollBarScrolled(ScrollbarSpaceRatio - mVerticalScrollBar->DistanceFromTop());
    }

    UpdateTransform();
}

void SOdysseyViewport::ComputeTextureDisplayDimensions( uint32& Width, uint32& Height ) const
{
    Width = 0;
    Height = 0;

    UTexture* texture = GetTexture();
    if (!texture)
        return;

    Width = texture->GetSurfaceWidth() * GetZoom();
    Height = texture->GetSurfaceHeight() * GetZoom();
}

FTransform2D
SOdysseyViewport::GetTransformToDisplayedTexture()
{
    UTexture* texture = GetTexture();
    if (!texture)
        return FTransform2D();


    uint32 width = texture->GetSurfaceWidth();
    uint32 height = texture->GetSurfaceHeight();

    FVector2D translation = (FVector2D(width, height) / 2.0f);
    FTransform2D transform(-translation);

    transform = transform.Concatenate(mTransform);

    translation = GetViewportCenter();
    transform = transform.Concatenate(FTransform2D(translation));

    return transform;
}

FTransform2D
SOdysseyViewport::GetTransformToSourceTexture()
{
    UTexture* texture = GetTexture();
    if (!texture)
        return FTransform2D();

    //Convert the position from the displayed texture size to the the position in the texture source size
    uint32 textureFullWidth = texture->Source.IsValid() ? texture->Source.GetSizeX() : texture->GetSurfaceWidth();
    uint32 textureFullHeight = texture->Source.IsValid() ? texture->Source.GetSizeY() : texture->GetSurfaceHeight();

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

    uint32 width = texture->GetSurfaceWidth();
    uint32 height = texture->GetSurfaceHeight();

    FVector2D translation = (FVector2D(width, height) / 2.0f);

    FTransform2D transform(FScale2D(width / textureFullWidth, height / textureFullHeight));

    transform = transform.Concatenate(FTransform2D(-translation));
    transform = transform.Concatenate(mTransform);

    translation = GetViewportCenter();
    transform = transform.Concatenate(FTransform2D(translation));

    return transform;
}

FVector2D
SOdysseyViewport::ToLocal(const FVector2D& iPoint) const
{
    FVector2D center = GetViewportCenter();
    FVector2D pos = iPoint - center;

    FVector2D tpos = mTransform.Inverse().TransformPoint(pos);

    return tpos;
}

FVector2D
SOdysseyViewport::ToWorld(const FVector2D& iPoint) const
{
    FVector2D center = GetViewportCenter();
    FVector2D tpos = mTransform.TransformPoint(iPoint) + center;

    return tpos;
}

#undef LOCTEXT_NAMESPACE
