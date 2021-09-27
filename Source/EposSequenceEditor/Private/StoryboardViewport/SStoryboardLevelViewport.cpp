// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "StoryboardViewport/SStoryboardLevelViewport.h"

#include "Widgets/SBoxPanel.h"
#include "CineCameraActor.h"
#include "Framework/Commands/UICommandList.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBorder.h"
#include "SlateOptMacros.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/NumericTypeInterface.h"
#include "Widgets/Input/SSpinBox.h"
#include "EditorStyleSet.h"
#include "SLevelViewport.h"
#include "MovieScene.h"
#include "ISequencer.h"
#include "MovieSceneSequence.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"
#include "SequencerKeyCollection.h"
#include "StoryboardViewport/SStoryboardTransportRange.h"
#include "StoryboardViewport/FilmOverlays.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "CineCameraComponent.h"
#include "Math/UnitConversion.h"
#include "LevelEditorSequencerIntegration.h"
#include "Fonts/FontMeasure.h"
#include "Editor.h"
#include "Engine/Selection.h"

//#include "EposSequenceEditorCommands.h"
#include "EposSequenceEditorToolkit.h"
#include "PlaneActor.h"
#include "Tools/EposSequenceTools.h"


#define LOCTEXT_NAMESPACE "SStoryboardLevelViewport"

template<typename T>
struct SNonThrottledSpinBox : SSpinBox<T>
{
    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override
    {
        FReply Reply = SSpinBox<T>::OnMouseButtonDown(MyGeometry, MouseEvent);
        if (Reply.IsEventHandled())
        {
            Reply.PreventThrottling();
        }
        return Reply;
    }
};

struct FTypeInterfaceProxy : INumericTypeInterface<double>
{
    TSharedPtr<INumericTypeInterface<double>> Impl;

    /** Gets the minimum and maximum fractional digits. */
    virtual int32 GetMinFractionalDigits() const override
    {
        return 0;
    }
    virtual int32 GetMaxFractionalDigits() const override
    {
        return 0;
    }

    /** Sets the minimum and maximum fractional digits - A minimum greater than 0 will always have that many trailing zeros */
    virtual void SetMinFractionalDigits(const TAttribute<TOptional<int32>>& NewValue) override {}

    virtual void SetMaxFractionalDigits(const TAttribute<TOptional<int32>>& NewValue) override {}

    /** Convert the type to/from a string */
    virtual FString ToString(const double& Value) const override
    {
        if (Impl.IsValid())
        {
            return Impl->ToString(Value);
        }
        return FString();
    }

    virtual TOptional<double> FromString(const FString& InString, const double& InExistingValue) override
    {
        if (Impl.IsValid())
        {
            return Impl->FromString(InString, InExistingValue);
        }
        return TOptional<double>();
    }

    /** Check whether the typed character is valid */
    virtual bool IsCharacterValid(TCHAR InChar) const
    {
        if (Impl.IsValid())
        {
            return Impl->IsCharacterValid(InChar);
        }
        return false;
    }
};

FStoryboardViewportClient::FStoryboardViewportClient()
    : FLevelEditorViewportClient(nullptr)
{
    bDrawAxes = false;
    bIsRealtime = true;
    SetAllowCinematicControl(true);
    bDisableInput = false;
}

class SPreArrangedBox : public SCompoundWidget
{
public:
    DECLARE_DELEGATE_OneParam(FOnArrange, const FGeometry&);

    SLATE_BEGIN_ARGS(SPreArrangedBox){}
        SLATE_EVENT(FOnArrange, OnArrange)
        SLATE_DEFAULT_SLOT(FArguments, Content)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs)
    {
        OnArrange = InArgs._OnArrange;
        ChildSlot
        [
            InArgs._Content.Widget
        ];
    }

    virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override
    {
        OnArrange.ExecuteIfBound(AllottedGeometry);
        SCompoundWidget::OnArrangeChildren(AllottedGeometry, ArrangedChildren);
    }

private:

    FOnArrange OnArrange;
};

class SStoryboardPreviewViewport : public SLevelViewport
{
public:
    virtual const FSlateBrush* OnGetViewportBorderBrush() const override { return nullptr; }
    virtual EVisibility GetCurrentLevelTextVisibility() const override { return EVisibility::Collapsed; }
    virtual EVisibility GetSelectedActorsCurrentLevelTextVisibility() const override { return EVisibility::Collapsed; }
    virtual EVisibility GetViewportControlsVisibility() const override { return EVisibility::Collapsed; }

    virtual TSharedPtr<SWidget> MakeViewportToolbar() { return nullptr; }

    TSharedPtr<SWidget> MakeExternalViewportToolbar() { return SLevelViewport::MakeViewportToolbar(); }

    FSlateColor GetBorderColorAndOpacity() const
    {
        return OnGetViewportBorderColorAndOpacity();
    }

    const FSlateBrush* GetBorderBrush() const
    {
        return SLevelViewport::OnGetViewportBorderBrush();
    }

    EVisibility GetBorderVisibility() const
    {
        const EVisibility ViewportContentVisibility = SLevelViewport::OnGetViewportContentVisibility();
        return ViewportContentVisibility == EVisibility::Visible ? EVisibility::HitTestInvisible : ViewportContentVisibility;
    }

private:
    bool bShowToolbar;
};


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SStoryboardLevelViewport::Construct(const FArguments& InArgs)
{
    ParentLayout = InArgs._ParentLayout;
    LayoutName = InArgs._LayoutName;
    RevertToLayoutName = InArgs._RevertToLayoutName;

    ViewportClient = MakeShareable( new FStoryboardViewportClient() );

    ViewportWidget = SNew(SStoryboardPreviewViewport)
        .LevelEditorViewportClient(ViewportClient)
        .ParentLevelEditor(InArgs._ParentLevelEditor)
        .ParentLayout(ParentLayout.Pin())
        .ConfigKey(LayoutName)
        .Realtime(true);

    ViewportClient->SetViewportWidget(ViewportWidget);

    // Automatically engage game-view to hide editor only sprites. This needs to be done
    // after the Viewport Client and Widget are constructed as they reset the view to defaults
    // as part of their initialization.
    ViewportClient->SetGameView(true);

    TypeInterfaceProxy = MakeShareable( new FTypeInterfaceProxy );

    FEposSequenceEditorToolkit::OnOpened().AddSP(this, &SStoryboardLevelViewport::OnEditorOpened);

    FLinearColor Gray(.3f, .3f, .3f, 1.f);

    TSharedRef<SFilmOverlayOptions> FilmOverlayOptions = SNew(SFilmOverlayOptions);

    DecoratedTransportControls = SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        [
            SNew(SSpacer)
        ]

        + SHorizontalBox::Slot()
        .AutoWidth()
        .Padding(5.f, 0.f)
        .VAlign(VAlign_Center)
        .HAlign(HAlign_Center)
        [
            SNew(SBorder)
            .Padding(0)
            .BorderImage(nullptr)
            .ForegroundColor(FEditorStyle::GetSlateColor("SelectionColor").GetColor(FWidgetStyle()))
            [
                SNew(SNonThrottledSpinBox<double>)
                .TypeInterface(TypeInterfaceProxy)
                .Style(FEditorStyle::Get(), "Sequencer.HyperlinkSpinBox")
                .Font(FEditorStyle::GetFontStyle("Sequencer.FixedFont"))
                .OnValueCommitted(this, &SStoryboardLevelViewport::OnTimeCommitted)
                .OnValueChanged(this, &SStoryboardLevelViewport::SetTime)
                .MinValue(TOptional<double>())
                .MaxValue(TOptional<double>())
                .OnEndSliderMovement(this, &SStoryboardLevelViewport::SetTime)
                .Value(this, &SStoryboardLevelViewport::GetTime)
                .ToolTipText(LOCTEXT("TimeLocalToCurrentSequence", "The current time of the sequence relative to the focused sequence."))
                .Delta_Lambda([=]()
                {
                    return UIData.OuterResolution.AsDecimal() * UIData.OuterPlayRate.AsInterval();
                })
                .LinearDeltaSensitivity(25)
                .MinDesiredWidth(this, &SStoryboardLevelViewport::GetPlayTimeMinDesiredWidth)
            ]
        ]

        + SHorizontalBox::Slot()
        .AutoWidth()
        .VAlign(VAlign_Center)
        .HAlign(HAlign_Center)
        [
            SAssignNew(TransportControlsContainer, SBox)
        ]

        + SHorizontalBox::Slot()
        [
            SNew(SSpacer)
        ];

    //HACK: ue4
    TSharedPtr<SSpinBox<float>> planeDistanceSpinBox;

    TSharedRef<SWidget> MainViewport = SNew(SBorder)
        .BorderImage(FEditorStyle::GetBrush("BlackBrush"))
        .ForegroundColor(Gray)
        .Padding(0)
        [
            SNew(SVerticalBox)

            + SVerticalBox::Slot()
            .Padding(5.f)
            .AutoHeight()
            [
                SNew(SHorizontalBox)
                .Visibility_Lambda([] { return GLevelEditorModeTools().IsViewportUIHidden() ? EVisibility::Hidden : EVisibility::Visible; })

                + SHorizontalBox::Slot()
                [
                    ViewportWidget->MakeExternalViewportToolbar().ToSharedRef()
                ]

                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    FilmOverlayOptions
                ]
            ]

            + SVerticalBox::Slot()
            [
                SNew(SPreArrangedBox)
                .OnArrange(this, &SStoryboardLevelViewport::CacheDesiredViewportSize)
                [
                    SNew(SVerticalBox)

                    + SVerticalBox::Slot()
                    [
                        SNew(SSpacer)
                    ]

                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .HAlign(HAlign_Center)
                    [
                        SNew(SBox)
                        .HeightOverride(this, &SStoryboardLevelViewport::GetDesiredViewportHeight)
                        .WidthOverride(this, &SStoryboardLevelViewport::GetDesiredViewportWidth)
                        [
                            SNew(SOverlay)

                            + SOverlay::Slot()
                            [
                                ViewportWidget.ToSharedRef()
                            ]

                            + SOverlay::Slot()
                            [
                                FilmOverlayOptions->GetFilmOverlayWidget()
                            ]
                        ]
                    ]

                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .HAlign(HAlign_Center)
                    .Padding(10.f, 0.f)
                    [
                        SAssignNew(ViewportControls, SBox)
                        .Visibility(this, &SStoryboardLevelViewport::GetControlsVisibility)
                        .WidthOverride(this, &SStoryboardLevelViewport::GetDesiredViewportWidth)
                        .Padding(FMargin(0.f, 10.f, 0.f, 0.f))
                        [
                            SNew(SVerticalBox)

                            + SVerticalBox::Slot()
                            [
                                SNew(SHorizontalBox)

                                + SHorizontalBox::Slot()
                                .HAlign(HAlign_Left)
                                [
                                    SNew(SHorizontalBox)

                                    + SHorizontalBox::Slot()
                                    .HAlign(HAlign_Left)
                                    .AutoWidth()
                                    [
                                        SNew(STextBlock)
                                        .ColorAndOpacity(Gray)
                                        .Text_Lambda([=]{ return UIData.ShotName; })
                                        .ToolTipText(LOCTEXT("CurrentSequence", "The name of the currently evaluated sequence."))
                                    ]

                                    + SHorizontalBox::Slot()
                                    .HAlign(HAlign_Right)
                                    .AutoWidth()
                                    .Padding(FMargin(5.f, 0.f, 0.f, 0.f))
                                    [
                                        SNew(STextBlock)
                                        .ColorAndOpacity(Gray)
                                        .Text_Lambda([=] { return UIData.CameraName; })
                                        .ToolTipText(LOCTEXT("CurrentCamera", "The name of the current camera."))
                                    ]

                                    + SHorizontalBox::Slot()
                                    .HAlign(HAlign_Right)
                                    .AutoWidth()
                                    .Padding(FMargin(5.f, 0.f, 0.f, 0.f))
                                    [
                                        SNew(STextBlock)
                                        .ColorAndOpacity(Gray)
                                        .Text_Lambda([=] { return FText::Join( FText::FromString( TEXT(", ") ), UIData.SelectedPlanes ); })
                                        .ToolTipText(LOCTEXT("SelectedPlanes", "The name of all selected planes."))
                                    ]
                                ]

                                + SHorizontalBox::Slot()
                                .HAlign(HAlign_Center)
                                .AutoWidth()
                                [
                                    SNew(STextBlock)
                                    .ColorAndOpacity(Gray)
                                    .Text_Lambda([=] { return UIData.Filmback; })
                                    .ToolTipText(LOCTEXT("CurrentFilmback", "The name of the current shot's filmback (the imaging area of the frame/sensor)."))
                                ]

                                + SHorizontalBox::Slot()
                                .HAlign(HAlign_Right)
                                [
                                    SNew(STextBlock)
                                    .Font(FEditorStyle::GetFontStyle("Sequencer.FixedFont"))
                                    .ColorAndOpacity(Gray)
                                    .Text_Lambda([=] { return UIData.LocalPlaybackTime; })
                                    .ToolTipText(LOCTEXT("LocalPlaybackTime", "The current playback time relative to the currently evaluated sequence."))
                                ]
                            ]

                            + SVerticalBox::Slot()
                            .AutoHeight()
                            .HAlign(HAlign_Center)
                            //.Padding(0, 5, 0, 0)
                            [
                                SNew( SHorizontalBox )
                                .Visibility( this, &SStoryboardLevelViewport::GetMoveAndScalePlaneVisibility )

                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                .VAlign(VAlign_Center)
                                .Padding( 10, 0 )
                                [
                                    SNew(STextBlock)
                                    .ColorAndOpacity(Gray)
                                    .Text_Lambda([=] { return LOCTEXT( "PlaneDistanceLabel", "Plane Distance" ); })
                                ]

                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                [
                                    SAssignNew( planeDistanceSpinBox, SSpinBox<float> )
                                    .ToolTipText( LOCTEXT( "PlaneDistanceTooltip", "Modify the distance between the selected plane and its parent camera." ) )
                                    .PreventThrottling( true ) // To refresh the viewport during value change
                                    .LinearDeltaSensitivity( 15 )  // If we're an unbounded spinbox, what value do we divide mouse movement by before multiplying by Delta. Requires Delta to be set.
                                    .Delta( 1 )
                                    .SliderExponent( 0.8f ) // Can't work properly if the following options are in use :  .LinearDeltaSensitivity .MinValue .MaxValue
                                    .SliderExponentNeutralValue( 100 )
                                    .Value( this, &SStoryboardLevelViewport::GetMoveAndScalePlaneDistance )
                                    .OnValueChanged( this, &SStoryboardLevelViewport::SetMoveAndScalePlaneDistance )
                                    .OnValueCommitted_Lambda( [=]( float iNewValue, ETextCommit::Type iType ) { SetMoveAndScalePlaneDistance( iNewValue ); } )
                                ]

                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                .VAlign(VAlign_Center)
                                .Padding( 10, 0 )
                                [
                                    SNew(STextBlock)
                                    .ColorAndOpacity(Gray)
                                    .Text( LOCTEXT( "PlaneScaleLabel", "Scale" ) )
                                ]

                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                [
                                    SNew( SCheckBox )
                                    .ToolTipText( LOCTEXT( "PlaneScaleTooltip", "Move and scale the plane accordingly to its parent camera. Otherwise the plane is only moved." ) )
                                    .IsChecked( this, &SStoryboardLevelViewport::GetScalePlaneState )
                                    .OnCheckStateChanged( this, &SStoryboardLevelViewport::OnScalePlaneStateChanged )
                                ]
                            ]
                        ]
                    ]

                    + SVerticalBox::Slot()
                    [
                        SNew(SSpacer)
                    ]
                ]
            ]

            + SVerticalBox::Slot()
            .Padding(5.f)
            .AutoHeight()
            [
                SNew(SWidgetSwitcher)
                .WidgetIndex(this, &SStoryboardLevelViewport::GetVisibleWidgetIndex)

                + SWidgetSwitcher::Slot()
                [
                    SNew(SVerticalBox)

                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(5.f)
                    [
                        SAssignNew(TransportRange, SStoryboardTransportRange)
                    ]

                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .Padding(5.f, 0.f)
                    [
                        SAssignNew(TimeRangeContainer, SBox)
                    ]
                ]

                + SWidgetSwitcher::Slot()
                .VAlign(VAlign_Center)
                .HAlign(HAlign_Center)
                [
                    SNew(SBox)
                    .Padding(FMargin(5.f, 10.f))
                    [
                        SNew(STextBlock)
                        .ColorAndOpacity(Gray)
                        .Text(LOCTEXT("NoSequencerMessage", "No active Board/Shot Sequencer detected. Please edit a Board/Shot Sequence to enable full controls."))
                    ]
                ]
            ]
        ];

    //TODO: HACK:
    // UE5: MaxFractionnal digits is set correctly in UE5.
    // UE4, we have to call SetMaxFractionnalDigits/SetMinFractionalDigits
    planeDistanceSpinBox->SetMinFractionalDigits( 4 );
    planeDistanceSpinBox->SetMaxFractionalDigits( 4 );

    ChildSlot
    [
        SNew(SOverlay)

        + SOverlay::Slot()
        [
            MainViewport
        ]

        +SOverlay::Slot()
        [
            SNew(SBorder)
            .BorderImage(ViewportWidget.Get(), &SStoryboardPreviewViewport::GetBorderBrush)
            .BorderBackgroundColor(ViewportWidget.Get(), &SStoryboardPreviewViewport::GetBorderColorAndOpacity)
            .Visibility(ViewportWidget.Get(), &SStoryboardPreviewViewport::GetBorderVisibility)
            .Padding(0.0f)
            .ShowEffectWhenDisabled( false )
        ]
    ];

    FEposSequenceEditorToolkit::IterateOpenToolkits([&](FEposSequenceEditorToolkit& Toolkit){
        Setup(Toolkit);
        return false;
    });

    CommandList = MakeShareable( new FUICommandList );
    // Ensure the commands are registered
    //FLevelSequenceEditorCommands::Register(); //TODO: !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

TSharedPtr<SLevelViewport> SStoryboardLevelViewport::GetLevelViewport() const
{
    return ViewportWidget;
}

int32 SStoryboardLevelViewport::GetVisibleWidgetIndex() const
{
    return CurrentToolkit.IsValid() ? 0 : 1;
}

EVisibility SStoryboardLevelViewport::GetControlsVisibility() const
{
    return CurrentToolkit.IsValid() ? EVisibility::Visible : EVisibility::Collapsed;
}

TOptional<double> SStoryboardLevelViewport::GetMinTime() const
{
    ISequencer* Sequencer = GetSequencer();
    if (Sequencer)
    {
        FFrameRate   PlayRate      = Sequencer->GetLocalTime().Rate;
        UMovieScene* MovieScene    = Sequencer->GetFocusedMovieSceneSequence()->GetMovieScene();
        double       TimeInSeconds = MovieScene->GetEditorData().WorkStart;

        return (TimeInSeconds*PlayRate).GetFrame().Value;
    }
    return TOptional<double>();
}

TOptional<double> SStoryboardLevelViewport::GetMaxTime() const
{
    ISequencer* Sequencer = GetSequencer();
    if (Sequencer)
    {
        FFrameRate   PlayRate      = Sequencer->GetLocalTime().Rate;
        UMovieScene* MovieScene    = Sequencer->GetFocusedMovieSceneSequence()->GetMovieScene();
        double       TimeInSeconds = MovieScene->GetEditorData().WorkEnd;

        return (TimeInSeconds*PlayRate).GetFrame().Value;
    }
    return TOptional<double>();
}

void SStoryboardLevelViewport::OnTimeCommitted(double Value, ETextCommit::Type)
{
    SetTime(Value);
}

void SStoryboardLevelViewport::SetTime(double Value)
{
    // Clamp the value as the UI can't due to needing an unbounded spinbox for value-change-rate purposes.
    Value = FMath::Clamp(Value, GetMinTime().GetValue(), GetMaxTime().GetValue());

    ISequencer* Sequencer = GetSequencer();
    if (Sequencer)
    {
        FFrameRate SequencerPlayRate = Sequencer->GetLocalTime().Rate;
        Sequencer->SetLocalTime(FFrameTime::FromDecimal(Value));
    }
}

double SStoryboardLevelViewport::GetTime() const
{
    ISequencer* Sequencer = GetSequencer();
    if (Sequencer)
    {
        return Sequencer->GetLocalTime().Time.GetFrame().Value;
    }
    return 0;
}

float SStoryboardLevelViewport::GetPlayTimeMinDesiredWidth() const
{
    ISequencer* Sequencer = GetSequencer();
    if (Sequencer)
    {
        TRange<double> ViewRange = Sequencer->GetViewRange();

        FString LowerBoundStr = Sequencer->GetNumericTypeInterface()->ToString(ViewRange.GetLowerBoundValue());
        FString UpperBoundStr = Sequencer->GetNumericTypeInterface()->ToString(ViewRange.GetUpperBoundValue());

        const FSlateFontInfo PlayTimeFont = FEditorStyle::GetFontStyle("Sequencer.FixedFont");

        const TSharedRef< FSlateFontMeasure > FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();

        FVector2D LowerTextSize = FontMeasureService->Measure(LowerBoundStr, PlayTimeFont);
        FVector2D UpperTextSize = FontMeasureService->Measure(UpperBoundStr, PlayTimeFont);

        return FMath::Max(LowerTextSize.X, UpperTextSize.X);
    }

    return 0.f;
}

void SStoryboardLevelViewport::CacheDesiredViewportSize(const FGeometry& AllottedGeometry)
{
    FVector2D AllowableSpace = AllottedGeometry.GetLocalSize();
    AllowableSpace.Y -= ViewportControls->GetDesiredSize().Y;

    if (ViewportClient->IsAspectRatioConstrained())
    {
        const float MinSize = FMath::TruncToFloat(FMath::Min(AllowableSpace.X / ViewportClient->AspectRatio, AllowableSpace.Y));
        DesiredViewportSize = FVector2D(FMath::TruncToFloat(ViewportClient->AspectRatio * MinSize), MinSize);
    }
    else
    {
        DesiredViewportSize = AllowableSpace;
    }
}

FOptionalSize SStoryboardLevelViewport::GetDesiredViewportWidth() const
{
    return DesiredViewportSize.X;
}

FOptionalSize SStoryboardLevelViewport::GetDesiredViewportHeight() const
{
    return DesiredViewportSize.Y;
}

//---

EVisibility
SStoryboardLevelViewport::GetMoveAndScalePlaneVisibility() const
{
    ACineCameraActor* camera = mPlaneToMove ? Cast<ACineCameraActor>( mPlaneToMove->GetAttachParentActor() ) : nullptr;

    return ShotSequenceTools::CanMoveAndScalePlane( mPlaneToMove, camera ) ? EVisibility::Visible : EVisibility::Hidden;
}

float
SStoryboardLevelViewport::GetMoveAndScalePlaneDistance() const
{
    ACineCameraActor* camera = mPlaneToMove ? Cast<ACineCameraActor>( mPlaneToMove->GetAttachParentActor() ) : nullptr;

    if( !ShotSequenceTools::CanMoveAndScalePlane( mPlaneToMove, camera ) )
        return 0.f;

    float distance = camera->GetDistanceTo( mPlaneToMove );

    return distance;
}
void
SStoryboardLevelViewport::SetMoveAndScalePlaneDistance( float iDistance )
{
    ACineCameraActor* camera = mPlaneToMove ? Cast<ACineCameraActor>( mPlaneToMove->GetAttachParentActor() ) : nullptr;

    ShotSequenceTools::MoveAndScalePlane( mPlaneToMove, camera, iDistance, mScalePlane );
}

ECheckBoxState
SStoryboardLevelViewport::GetScalePlaneState() const
{
    return mScalePlane ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}
void
SStoryboardLevelViewport::OnScalePlaneStateChanged( ECheckBoxState iState )
{
    mScalePlane = ( iState == ECheckBoxState::Checked );
}

//---

FReply SStoryboardLevelViewport::OnKeyDown( const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent )
{
    if (CommandList->ProcessCommandBindings(InKeyEvent))
    {
        return FReply::Handled();
    }

    ISequencer* Sequencer = GetSequencer();
    if (Sequencer && Sequencer->GetCommandBindings()->ProcessCommandBindings(InKeyEvent))
    {
        return FReply::Handled();
    }

    return FReply::Unhandled();
}

void SStoryboardLevelViewport::Setup(FEposSequenceEditorToolkit& NewToolkit)
{
    CurrentToolkit = StaticCastSharedRef<FEposSequenceEditorToolkit>(NewToolkit.AsShared());

    NewToolkit.OnClosed().AddSP(this, &SStoryboardLevelViewport::OnEditorClosed);

    ISequencer* Sequencer = GetSequencer();
    if (Sequencer)
    {
        TypeInterfaceProxy->Impl = Sequencer->GetNumericTypeInterface();

        if (TransportRange.IsValid())
        {
            TransportRange->SetSequencer(Sequencer->AsShared());
        }

        if (TransportControlsContainer.IsValid())
        {
            TransportControlsContainer->SetContent(Sequencer->MakeTransportControls(true));
        }

        if (TimeRangeContainer.IsValid())
        {
            const bool bShowWorkingRange = true, bShowViewRange = false, bShowPlaybackRange = true;
            TimeRangeContainer->SetContent(Sequencer->MakeTimeRange(DecoratedTransportControls.ToSharedRef(), bShowWorkingRange, bShowViewRange, bShowPlaybackRange));
        }
    }
}

void SStoryboardLevelViewport::CleanUp()
{
    TransportControlsContainer->SetContent(SNullWidget::NullWidget);
    TimeRangeContainer->SetContent(SNullWidget::NullWidget);

}

void SStoryboardLevelViewport::OnEditorOpened(FEposSequenceEditorToolkit& Toolkit)
{
    if (!CurrentToolkit.IsValid())
    {
        Setup(Toolkit);
    }
}

void SStoryboardLevelViewport::OnEditorClosed()
{
    CleanUp();

    FEposSequenceEditorToolkit* NewToolkit = nullptr;
    FEposSequenceEditorToolkit::IterateOpenToolkits([&](FEposSequenceEditorToolkit& Toolkit){
        NewToolkit = &Toolkit;
        return false;
    });

    if (NewToolkit)
    {
        Setup(*NewToolkit);
    }
}

ISequencer* SStoryboardLevelViewport::GetSequencer() const
{
    TSharedPtr<FEposSequenceEditorToolkit> Toolkit = CurrentToolkit.Pin();
    if (Toolkit.IsValid())
    {
        return Toolkit->GetSequencer().Get();
    }

    return nullptr;
}

void SStoryboardLevelViewport::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

    ISequencer* Sequencer = GetSequencer();
    if (!Sequencer)
    {
        return;
    }

    UMovieSceneSequence* Sequence = Sequencer->GetFocusedMovieSceneSequence();
    if (!Sequence)
    {
        return;
    }

    // Find the cinematic shot track
    UMovieSceneCinematicBoardTrack* CinematicBoardTrack = Cast<UMovieSceneCinematicBoardTrack>(Sequence->GetMovieScene()->FindMasterTrack(UMovieSceneCinematicBoardTrack::StaticClass()));

    const FFrameRate OuterResolution = Sequencer->GetFocusedTickResolution();
    const FFrameRate OuterPlayRate   = Sequencer->GetFocusedDisplayRate();

    const FFrameTime OuterTime       = Sequencer->GetLocalTime().ConvertTo(OuterResolution);
    UIData.OuterResolution = OuterResolution;
    UIData.OuterPlayRate = OuterPlayRate;

    UMovieSceneCinematicBoardSection* CinematicBoardSection = nullptr;
    if (CinematicBoardTrack)
    {
        for (UMovieSceneSection* Section : CinematicBoardTrack->GetAllSections())
        {
            if (Section->GetRange().Contains(OuterTime.FrameNumber))
            {
                CinematicBoardSection = CastChecked<UMovieSceneCinematicBoardSection>(Section);
            }
        }
    }

    FText TimeFormat = LOCTEXT("TimeFormat", "{0}");

    TSharedPtr<INumericTypeInterface<double>> TimeDisplayFormatInterface = Sequencer->GetNumericTypeInterface();

    UMovieSceneSequence* SubSequence = CinematicBoardSection ? CinematicBoardSection->GetSequence() : nullptr;
    if (SubSequence)
    {
        FFrameRate                   InnerResolution       = SubSequence->GetMovieScene()->GetTickResolution();
        FMovieSceneSequenceTransform OuterToInnerTransform = CinematicBoardSection ? CinematicBoardSection->OuterToInnerTransform() : FMovieSceneSequenceTransform();
        const FFrameTime             InnerShotPosition     = OuterTime * OuterToInnerTransform;

        UIData.LocalPlaybackTime = FText::Format(
            TimeFormat,
            FText::FromString(TimeDisplayFormatInterface->ToString(InnerShotPosition.GetFrame().Value))
        );

        if (CinematicBoardSection)
        {
            UIData.ShotName = FText::FromString(CinematicBoardSection->GetBoardDisplayName());
        }
    }
    else
    {
        const FFrameTime DisplayTime = Sequencer->GetLocalTime().Time;

        UIData.LocalPlaybackTime = FText::Format(
            TimeFormat,
            FText::FromString(TimeDisplayFormatInterface->ToString(DisplayTime.GetFrame().Value))
            );

        UIData.ShotName = Sequence->GetDisplayName();
    }

    const FMovieSceneEditorData& EditorData = Sequence->GetMovieScene()->GetEditorData();

    FQualifiedFrameTime MasterStartTime(EditorData.WorkStart * OuterPlayRate, OuterPlayRate);
    UIData.MasterStartText = FText::Format(
        TimeFormat,
        FText::FromString(TimeDisplayFormatInterface->ToString(MasterStartTime.Time.GetFrame().Value))
    );

    FQualifiedFrameTime MasterEndTime(EditorData.WorkEnd * OuterPlayRate, OuterPlayRate);
    UIData.MasterEndText = FText::Format(
        TimeFormat,
        FText::FromString(TimeDisplayFormatInterface->ToString(MasterEndTime.Time.GetFrame().Value))
    );

    UIData.CameraName = FText::GetEmpty();

    UCameraComponent* CameraComponent = ViewportClient->GetCameraComponentForView();
    if (CameraComponent)
    {
        AActor* OuterActor = Cast<AActor>(CameraComponent->GetOuter());
        if (OuterActor != nullptr)
        {
            UIData.CameraName = FText::FromString(OuterActor->GetActorLabel());
        }

        UIData.Filmback = CameraComponent->GetFilmbackText();
    }
    else
    {
        UIData.Filmback = FText();
    }

    //-

    UIData.SelectedPlanes.Empty();

    USelection* SelectedActors = GEditor->GetSelectedActors();
    TArray<APlaneActor*> selected_planes;
    SelectedActors->GetSelectedObjects( selected_planes );

    for( auto selected_plane : selected_planes )
        UIData.SelectedPlanes.Add( FText::FromString( selected_plane->GetName() ) );

    //-

    mPlaneToMove = nullptr;
    if( selected_planes.Num() == 1 )
    {
        APlaneActor* plane = selected_planes[0];
        mPlaneToMove = plane;
    }
}

#undef LOCTEXT_NAMESPACE
