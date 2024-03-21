// IDDN.FR.001.220036.002.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
#include "SequencerKeyCollection.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "CineCameraComponent.h"
#include "Math/UnitConversion.h"
#include "LevelEditorSequencerIntegration.h"
#include "Fonts/FontMeasure.h"
#include "Editor.h"
#include "AssetEditorViewportLayout.h"
#include "Engine/Selection.h"
#include "SEnumCombo.h"
#include "Widgets/Input/NumericUnitTypeInterface.inl"
#include "AnimatedRange.h"

#include "CinematicBoardTrack/MovieSceneCinematicBoardTrack.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"
#include "EposSequenceEditorCommands.h"
#include "EposSequenceEditorToolkit.h"
#include "EposSequenceHelpers.h"
#include "NoteTrack/MovieSceneNoteSection.h"
#include "PlaneActor.h"
#include "StoryNote.h"
#include "StoryboardViewport/FilmOverlays.h"
#include "StoryboardViewport/SNotes.h"
#include "StoryboardViewport/SNoteSettings.h"
#include "StoryboardViewport/SStoryboardTransportRange.h"
#include "StoryboardViewport/StoryboardViewportCommands.h"
#include "Styles/EposSequenceEditorStyle.h"
#include "Tools/EposSequenceTools.h"


#define LOCTEXT_NAMESPACE "SStoryboardLevelViewport"

#define MinZoom 0.01
#define MaxZoom 200.0
#define ScrollbarThumbRatio 0.1f
#define ScrollbarSpaceRatio (1.f - ScrollbarThumbRatio)

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

const FKey&
FStoryboardLevelViewportInputProcessor::GetKey()
{
    return mKey;
}

bool
FStoryboardLevelViewportInputProcessor::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
    if (InKeyEvent.GetKey().IsModifierKey())
        return false;

    mKey = InKeyEvent.GetKey();
    return false; //false means Unreal will continue as if we did nothing
}

/** Key up input */
bool
FStoryboardLevelViewportInputProcessor::HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
    if (InKeyEvent.GetKey() == mKey)
    {
        mKey = FKey();
    }
    return false; //false means Unreal will continue as if we did nothing
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
    virtual bool IsActorEditorContextVisible() const { return false; }
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


SStoryboardLevelViewport::~SStoryboardLevelViewport()
{
    FSlateApplication::Get().RegisterInputPreProcessor(mInputProcessor);
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SStoryboardLevelViewport::Construct(const FArguments& InArgs)
{
    ParentLayout = InArgs._ParentLayout;
    LayoutName = InArgs._LayoutName;
    RevertToLayoutName = InArgs._RevertToLayoutName;

    mInputProcessor = MakeShared<FStoryboardLevelViewportInputProcessor>();
    FSlateApplication::Get().RegisterInputPreProcessor(mInputProcessor);

    ViewportClient = MakeShareable( new FStoryboardViewportClient() );

    FAssetEditorViewportConstructionArgs ViewportConstructionArgs;
    ViewportConstructionArgs.ConfigKey = LayoutName;
    ViewportConstructionArgs.ParentLayout = ParentLayout.Pin();
    ViewportConstructionArgs.bRealtime = true;
    ViewportWidget = SNew( SStoryboardPreviewViewport, ViewportConstructionArgs )
        .LevelEditorViewportClient(ViewportClient)
        .ParentLevelEditor(InArgs._ParentLevelEditor);
    //ViewportWidget->SetRenderTransformPivot( FVector2D( .5f, .5f ) );
    ViewportClient->SetViewportWidget(ViewportWidget);

    // Automatically engage game-view to hide editor only sprites. This needs to be done
    // after the Viewport Client and Widget are constructed as they reset the view to defaults
    // as part of their initialization.
    ViewportClient->SetGameView(true);

    TypeInterfaceProxy = MakeShareable( new FTypeInterfaceProxy );

    FLevelEditorSequencerIntegration::Get().GetOnSequencersChanged().AddSP( this, &SStoryboardLevelViewport::OnSequencerChanged );

    FLinearColor Gray(.3f, .3f, .3f, 1.f);

    FilmOverlayOptions = SNew(SFilmOverlayOptions);
    //FilmOverlayOptions->GetFilmOverlayWidget()->SetRenderTransformPivot( FVector2D( .5f, .5f ) );

    //---

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
            .ForegroundColor(FAppStyle::Get().GetSlateColor("SelectionColor").GetColor(FWidgetStyle()))
            [
                SNew(SNonThrottledSpinBox<double>)
                .TypeInterface(TypeInterfaceProxy)
                .Style(FAppStyle::Get(), "Sequencer.HyperlinkSpinBox")
                .Font(FAppStyle::Get().GetFontStyle("Sequencer.FixedFont"))
                .OnValueCommitted(this, &SStoryboardLevelViewport::OnTimeCommitted)
                .OnValueChanged(this, &SStoryboardLevelViewport::SetTime)
                .MinValue(TOptional<double>())
                .MaxValue(TOptional<double>())
                .OnEndSliderMovement(this, &SStoryboardLevelViewport::SetTime)
                .Value(this, &SStoryboardLevelViewport::GetTime)
                .ToolTipText(LOCTEXT("TimeLocalToCurrentSequence", "The current time of the sequence relative to the focused sequence."))
                .Delta_Lambda([this]()
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

    //---

    //HACK: ue4
    TSharedPtr<SSpinBox<float>> planeDistanceSpinBox;
    TSharedPtr<SSpinBox<float>> cameraFocalLengthSpinBox;

    const UEnum* scalePlaneEnum = FindObject<UEnum>( nullptr, TEXT( "/Script/EposTracksEditor.EScalePlane" ) );

    mNoteSplitter = SNew( SSplitter )
        .Orientation( Orient_Vertical )
        .Style( FAppStyle::Get(), "SplitterDark" )
        .PhysicalSplitterHandleSize( 2.0f )
        .MinimumSlotHeight( 3 * 16.f ) // Roughly 3 lines (this is also used for the first (3D scene) part)
        + SSplitter::Slot()
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
                [
                    SNew(SBox)
                    .Clipping( EWidgetClipping::ClipToBoundsAlways )
                    .HAlign(HAlign_Center)
                    [
                        SAssignNew(mViewportTransformBox, SBox)
                        .HeightOverride(this, &SStoryboardLevelViewport::GetDesiredViewportHeight)
                        .WidthOverride(this, &SStoryboardLevelViewport::GetDesiredViewportWidth)
                        [
                            SNew(SOverlay)
                            //.Clipping( EWidgetClipping::ClipToBoundsAlways )

                            + SOverlay::Slot()
                            [
                                ViewportWidget.ToSharedRef()
                            ]

                            + SOverlay::Slot()
                            [
                                FilmOverlayOptions->GetFilmOverlayWidget()
                            ]

                            + SOverlay::Slot()
                            [
                                SAssignNew( mWidgetNotesAsOverlay, SNotesAsOverlay )
                                .Visibility_Lambda( [=]() { return ( GetMutableDefault<UEposSequenceEditorSettings>()->NoteSettings.DisplayNoteAsOverlay /*&& SStoryboardLevelViewport::GetVisibleWidgetIndex() == 0*/ ) ? EVisibility::HitTestInvisible : EVisibility::Collapsed; } )
                                .ListItemsSource( &mNotes )
                            ]
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
                                    .Text_Lambda([this]{ return UIData.ShotName; })
                                    .ToolTipText(LOCTEXT("CurrentSequence", "The name of the currently evaluated sequence."))
                                ]

                                + SHorizontalBox::Slot()
                                .HAlign(HAlign_Right)
                                .AutoWidth()
                                .Padding(FMargin(5.f, 0.f, 0.f, 0.f))
                                [
                                    SNew(STextBlock)
                                    .ColorAndOpacity(Gray)
                                    .Text_Lambda([this] { return UIData.CameraName; })
                                    .ToolTipText(LOCTEXT("CurrentCamera", "The name of the current camera."))
                                ]

                                + SHorizontalBox::Slot()
                                .HAlign(HAlign_Right)
                                .AutoWidth()
                                .Padding(FMargin(5.f, 0.f, 0.f, 0.f))
                                [
                                    SNew(STextBlock)
                                    .ColorAndOpacity(Gray)
                                    .Text_Lambda([this] { return FText::Join( FText::FromString( TEXT(", ") ), UIData.SelectedPlanes ); })
                                    .ToolTipText(LOCTEXT("SelectedPlanes", "The name of all selected planes."))
                                ]
                            ]

                            + SHorizontalBox::Slot()
                            .HAlign(HAlign_Center)
                            .AutoWidth()
                            [
                                SNew(STextBlock)
                                .ColorAndOpacity(Gray)
                                .Text_Lambda([this] { return UIData.Filmback; })
                                .ToolTipText(LOCTEXT("CurrentFilmback", "The name of the current shot's filmback (the imaging area of the frame/sensor)."))
                            ]

                            + SHorizontalBox::Slot()
                            .HAlign(HAlign_Right)
                            [
                                SNew(STextBlock)
                                .Font(FAppStyle::Get().GetFontStyle("Sequencer.FixedFont"))
                                .ColorAndOpacity(Gray)
                                .Text_Lambda([this] { return UIData.LocalPlaybackTime; })
                                .ToolTipText(LOCTEXT("LocalPlaybackTime", "The current playback time relative to the currently evaluated sequence."))
                            ]
                        ]

                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .HAlign(HAlign_Center)
                        .Padding(0, 5, 0, 2)
                        [
                            SNew( SWidgetSwitcher )
                            .WidgetIndex( this, &SStoryboardLevelViewport::GetScaleVisibleWidgetIndex )

                            + SWidgetSwitcher::Slot()
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
                                    .Text_Lambda([this] { return FText::Format( LOCTEXT( "PlaneDistanceLabel", "{0} Distance" ), mPlaneToMove.IsValid() ? FText::FromString( mPlaneToMove->GetActorLabel() ) : FText::GetEmpty() ); })
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
                                    .OnValueCommitted_Lambda( [this]( float iNewValue, ETextCommit::Type iType ) { SetMoveAndScalePlaneDistance( iNewValue ); } )
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
                                    SNew( SEnumComboBox, scalePlaneEnum )
                                    .CurrentValue( this, &SStoryboardLevelViewport::GetScalePlaneType )
                                    //.ButtonStyle( FAppStyle::Get(), "FlatButton.Light" )
                                    //.ContentPadding( FMargin( 2, 0 ) )
                                    //.Font( FAppStyle::Get().GetFontStyle( "Sequencer.AnimationOutliner.RegularFont" ) )
                                    .OnEnumSelectionChanged( this, &SStoryboardLevelViewport::OnScalePlaneTypeChanged )
                                    .ToolTipText( LOCTEXT( "PlaneScaleTooltip", "Scale the plane accordingly to its parent camera." ) )
                                ]
                            ]

                            + SWidgetSwitcher::Slot()
                            [
                                SNew( SHorizontalBox )
                                .Visibility( this, &SStoryboardLevelViewport::GetCameraFocalLengthVisibility )

                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                .VAlign(VAlign_Center)
                                .Padding( 10, 0 )
                                [
                                    SNew(STextBlock)
                                    .ColorAndOpacity(Gray)
                                    .Text_Lambda([this] { return FText::Format( LOCTEXT( "CameraFocalLengthLabel", "{0} Focal Length" ), mCameraToFocalLength.IsValid() ? FText::FromString( mCameraToFocalLength->GetActorLabel() ) : FText::GetEmpty() ); })
                                ]

                                + SHorizontalBox::Slot()
                                .AutoWidth()
                                [
                                    SAssignNew( cameraFocalLengthSpinBox, SSpinBox<float> )
                                    .ToolTipText( LOCTEXT( "CameraFocalLengthTooltip", "Modify the current focal length of the camera." ) )
                                    .PreventThrottling( true ) // To refresh the viewport during value change
                                    .LinearDeltaSensitivity( 15 )  // If we're an unbounded spinbox, what value do we divide mouse movement by before multiplying by Delta. Requires Delta to be set.
                                    .Delta( 1 )
                                    .SliderExponent( 0.8f ) // Can't work properly if the following options are in use :  .LinearDeltaSensitivity .MinValue .MaxValue
                                    .SliderExponentNeutralValue( 100 )
                                    .Value( this, &SStoryboardLevelViewport::GetCameraFocalLength )
                                    .OnValueChanged( this, &SStoryboardLevelViewport::SetCameraFocalLength )
                                    .OnValueCommitted_Lambda( [this]( float iNewValue, ETextCommit::Type iType ) { SetCameraFocalLength( iNewValue ); } )
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
                                    SNew( SEnumComboBox, scalePlaneEnum )
                                    .CurrentValue( this, &SStoryboardLevelViewport::GetScalePlaneType )
                                    //.ButtonStyle( FAppStyle::Get(), "FlatButton.Light" )
                                    //.ContentPadding( FMargin( 2, 0 ) )
                                    //.Font( FAppStyle::Get().GetFontStyle( "Sequencer.AnimationOutliner.RegularFont" ) )
                                    .OnEnumSelectionChanged( this, &SStoryboardLevelViewport::OnScalePlaneTypeChanged )
                                    .ToolTipText( LOCTEXT( "PlaneScaleTooltip", "Scale the plane accordingly to its parent camera." ) )
                                ]
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

        + SSplitter::Slot()
        .Value( 0.1 )
        [
            SAssignNew( mWidgetNotesInViewport, SNotesInViewport )
            .Visibility_Lambda( [this]() { return ( GetMutableDefault<UEposSequenceEditorSettings>()->NoteSettings.DisplayNoteInViewport && SStoryboardLevelViewport::GetVisibleWidgetIndex() == 0 ) ? EVisibility::Visible : EVisibility::Collapsed; } )
            .ListItemsSource( &mNotes )
        ];

    //TODO: HACK:
    // UE5: MaxFractionnal digits is set correctly in UE5.
    // UE4, we have to call SetMaxFractionnalDigits/SetMinFractionalDigits
    planeDistanceSpinBox->SetMinFractionalDigits( 4 );
    planeDistanceSpinBox->SetMaxFractionalDigits( 4 );
    cameraFocalLengthSpinBox->SetMinFractionalDigits( 4 );
    cameraFocalLengthSpinBox->SetMaxFractionalDigits( 4 );

    //---

    //HACK: ue4
    TSharedPtr<SSpinBox<float>> viewportRotationSpinBox;
    TSharedPtr<SSpinBox<float>> viewportZoomSpinBox;

    TSharedRef<SWidget> MainViewport = SNew(SBorder)
        .BorderImage(FAppStyle::Get().GetBrush("BlackBrush"))
        .ForegroundColor(Gray)
        .Padding(0)
        [
            SNew(SVerticalBox)

            // Toolbar
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
                    FilmOverlayOptions.ToSharedRef()
                ]

                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew( SNoteSettings )
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign( VAlign_Center )
                [
                    SNew(SVerticalBox)
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign( VAlign_Center )
                        [
                            SAssignNew( viewportZoomSpinBox, SSpinBox<float> )
                            .TypeInterface( MakeShareable( new TNumericUnitTypeInterface<float>( EUnit::Percentage ) ) )
                            .MinDesiredWidth( 55 )
                            .Justification( ETextJustify::Right )
                            .ToolTipText( LOCTEXT( "ViewportZoomTooltip", "Change the viewport zoom." ) )
                            .PreventThrottling( true ) // To refresh the viewport during value change
                            .Delta( 1 )
                            .SliderExponent( 0.8f ) // Can't work properly if the following options are in use :  .LinearDeltaSensitivity .MinValue .MaxValue
                            .SliderExponentNeutralValue( 100 )
                            .OnValueCommitted_Lambda( [this] ( float Value, ETextCommit::Type) { SetViewportZoom(Value / 100.f); } )
                            .OnValueChanged_Lambda( [this] ( float Value) { SetViewportZoom(Value / 100.f); } )
                            .Value_Lambda( [this] () { return GetViewportZoom() * 100.f; } )
                        ]
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign( VAlign_Center )
                        [
                            SNew( SComboButton )
                            .ToolTipText( LOCTEXT( "ViewportZoomTooltip", "Change the viewport zoom." ) )
                            .OnGetMenuContent( this, &SStoryboardLevelViewport::OnGetViewportZoomMenuContent )
                        ]
                    ]
                    + SVerticalBox::Slot()
                    .AutoHeight()
                    [
                        SNew(SHorizontalBox)
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign( VAlign_Center )
                        [
                            SAssignNew( viewportRotationSpinBox, SSpinBox<float> )
                            .TypeInterface( MakeShareable( new TNumericUnitTypeInterface<float>( EUnit::Degrees ) ) )
                            .MinDesiredWidth( 55 )
                            .Justification( ETextJustify::Right )
                            .ToolTipText( LOCTEXT( "ViewportRotationTooltip", "Change the viewport rotation." ) )
                            .PreventThrottling( true ) // To refresh the viewport during value change
                            .LinearDeltaSensitivity( 15 )  // If we're an unbounded spinbox, what value do we divide mouse movement by before multiplying by Delta. Requires Delta to be set.
                            .Delta( 1 )
                            .SliderExponent( 0.8f ) // Can't work properly if the following options are in use :  .LinearDeltaSensitivity .MinValue .MaxValue
                            .SliderExponentNeutralValue( 100 )
                            .OnValueCommitted_Lambda( [this] ( float Value, ETextCommit::Type) { SetViewportRotation(Value); } )
                            .OnValueChanged_Lambda( [this] ( float Value) { SetViewportRotation(Value); } )
                            .Value( this, &SStoryboardLevelViewport::GetViewportRotation )
                        ]

                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign( VAlign_Center )
                        [
                            SNew( SComboButton )
                            .ToolTipText( LOCTEXT( "ViewportRotationTooltip", "Change the viewport rotation." ) )
                            .OnGetMenuContent( this, &SStoryboardLevelViewport::OnGetViewportRotationMenuContent )
                        ]
                    ]
                ]

                + SHorizontalBox::Slot()
                .AutoWidth()
                .Padding(4.0f, 0.0f)
                .VAlign(VAlign_Center)
                [
                    SNew(SButton)
                    .ButtonStyle(FAppStyle::Get(), "HoverHintOnly")
                    .ToolTipText( LOCTEXT( "ViewportResetTransformTooltip", "Reset Viewport Pan, Zoom and Rotation." ) )
                    .OnClicked(this, &SStoryboardLevelViewport::OnViewportResetTransformButtonClicked)
                    [
                        SNew(SImage)
                        .Image( FEposSequenceEditorStyle::Get().GetBrush( "Viewport.ResetTransform" ) )
                    ]
                ]

                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew( SSpacer )
                    .Size( FVector2D( 0, 55 ) )
                ]
            ]

            // Viewport + options + notes
            + SVerticalBox::Slot()
            .Padding( 5.f, 0.f )
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                [
                    mNoteSplitter.ToSharedRef()
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SAssignNew(mVerticalScrollBar, SScrollBar)
                    .AlwaysShowScrollbar(true)
                    .Thickness(FVector2D(10.f, 10.f))
                    .OnUserScrolled(this, &SStoryboardLevelViewport::OnVerticalScrollBarScrolled)
                ]
            ]

            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SAssignNew(mHorizontalScrollBar, SScrollBar)
                    .Orientation( Orient_Horizontal )
                    .AlwaysShowScrollbar(true)
                    .Thickness(FVector2D(10.f, 10.f))
                    .OnUserScrolled(this, &SStoryboardLevelViewport::OnHorizontalScrollBarScrolled)
            ]

            // Timeline + playback
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
                        .Text(LOCTEXT("NoSequencerMessage", "No active Board/Shot Sequence Editor detected. Please edit a Board/Shot Sequence to enable full controls."))
                    ]
                ]
            ]
        ];

    //TODO: HACK:
    // UE5: MaxFractionnal digits is set correctly in UE5.
    // UE4, we have to call SetMaxFractionnalDigits/SetMinFractionalDigits
    viewportRotationSpinBox->SetMinFractionalDigits( 2 );
    viewportRotationSpinBox->SetMaxFractionalDigits( 2 );

    viewportZoomSpinBox->SetMinFractionalDigits( 2 );
    viewportZoomSpinBox->SetMaxFractionalDigits( 2 );

    //---

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

    mViewportTransformBox->SetRenderTransformPivot( FVector2D( .5f, .5f ) );

    UpdateViewportWidgetTransform();

    OnSequencerChanged();

    //---

    CommandList = MakeShareable( new FUICommandList );
    //CommandList = ViewportWidget->GetCommandList();

    CommandList->MapAction(
        FEposSequenceEditorCommands::Get().StoryboardViewportAdd10Rotate,
        FUIAction( FExecuteAction::CreateSP( this, &SStoryboardLevelViewport::AddViewportRotation, 10.f ) )
    );
    CommandList->MapAction(
        FEposSequenceEditorCommands::Get().StoryboardViewportSubstract10Rotate,
        FUIAction( FExecuteAction::CreateSP( this, &SStoryboardLevelViewport::AddViewportRotation, -10.f ) )
    );

    for( auto command : FEposSequenceEditorCommands::Get().StoryboardViewportSetRotationX )
    {
        CommandList->MapAction(
            command.Value,
            FUIAction(
                FExecuteAction::CreateSP( this, &SStoryboardLevelViewport::SetViewportRotation, float( command.Key ) ),
                FCanExecuteAction(),
                FIsActionChecked::CreateSP( this, &SStoryboardLevelViewport::IsViewportRotationChecked, float( command.Key ) )
            )
        );
    }

    CommandList->MapAction(
        FEposSequenceEditorCommands::Get().StoryboardViewportAdd10Zoom,
        FUIAction( FExecuteAction::CreateSP( this, &SStoryboardLevelViewport::AddViewportZoom, 0.1f ) )
    );
    CommandList->MapAction(
        FEposSequenceEditorCommands::Get().StoryboardViewportSubstract10Zoom,
        FUIAction( FExecuteAction::CreateSP( this, &SStoryboardLevelViewport::AddViewportZoom, -0.1f ) )
    );
    CommandList->MapAction(
        FEposSequenceEditorCommands::Get().StoryboardViewportFitToScreen,
        FUIAction( FExecuteAction::CreateSP( this, &SStoryboardLevelViewport::FitToScreen ) )
    );
    CommandList->MapAction(
        FEposSequenceEditorCommands::Get().StoryboardViewportResetPanZoomRotate,
        FUIAction( FExecuteAction::CreateSP( this, &SStoryboardLevelViewport::ResetViewportTransform ) )
    );

    for( auto command : FEposSequenceEditorCommands::Get().StoryboardViewportSetZoomX )
    {
        CommandList->MapAction(
            command.Value,
            FUIAction(
                FExecuteAction::CreateSP( this, &SStoryboardLevelViewport::SetViewportZoom, float( command.Key ), FVector2D(0.0f, 0.0f) ),
                FCanExecuteAction(),
                FIsActionChecked::CreateSP( this, &SStoryboardLevelViewport::IsViewportZoomChecked, float( command.Key ) )
            )
        );
    }

    // Ensure the commands are registered
    FStoryboardViewportCommands::Register();
    //FLevelSequenceEditorCommands::Register(); //TODO: !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    FilmOverlayOptions->BindCommands( CommandList.ToSharedRef() );
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

TSharedPtr<SLevelViewport> SStoryboardLevelViewport::GetLevelViewport() const
{
    return ViewportWidget;
}

int32 SStoryboardLevelViewport::GetVisibleWidgetIndex() const
{
    return mCurrentSquencer.IsValid() ? 0 : 1;
}

EVisibility SStoryboardLevelViewport::GetControlsVisibility() const
{
    return mCurrentSquencer.IsValid() ? EVisibility::Visible : EVisibility::Collapsed;
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

        const FSlateFontInfo PlayTimeFont = FAppStyle::Get().GetFontStyle("Sequencer.FixedFont");

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
    AllowableSpace.Y -= ViewportControls->GetVisibility().IsVisible() ? ViewportControls->GetDesiredSize().Y : 0.f;

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

FSlateRenderTransform
SStoryboardLevelViewport::GetViewportTransform() const
{
    float radian = FUnitConversion::Convert( mViewportRotation, EUnit::Degrees, EUnit::Radians );
    FSlateRenderTransform transform = FSlateRenderTransform( FQuat2D( radian ) );
    transform = transform.Concatenate(FSlateRenderTransform( mViewportZoom ));
    transform = transform.Concatenate(FSlateRenderTransform( mViewportPan ));
    return transform;
}

void
SStoryboardLevelViewport::UpdateViewportWidgetTransform()
{
    FSlateRenderTransform transform = GetViewportTransform();
    mViewportTransformBox->SetRenderTransform( transform );
    //FilmOverlayOptions->GetFilmOverlayWidget()->SetRenderTransform( transform );
}

void
SStoryboardLevelViewport::ResetViewportTransform()
{
    SetViewportPan(FVector2D(0.f, 0.f));
    SetViewportZoom(1.f);
    SetViewportRotation(0.f);
}

FReply
SStoryboardLevelViewport::OnViewportResetTransformButtonClicked()
{
    ResetViewportTransform();
    return FReply::Handled();
}

float
SStoryboardLevelViewport::GetViewportRotation() const
{
    return mViewportRotation;
}

void
SStoryboardLevelViewport::SetViewportRotation( float iRotation )
{
    //Compute new pan value
    float oldRotationRadians = FUnitConversion::Convert( mViewportRotation, EUnit::Degrees, EUnit::Radians );
    float newRotationDegrees = FMath::Fmod(iRotation + 180, 360) - 180;
    float newRotationRadians = FUnitConversion::Convert( newRotationDegrees, EUnit::Degrees, EUnit::Radians );

    FSlateRenderTransform transform = GetViewportTransform();
    transform = transform.Concatenate(FSlateRenderTransform(FQuat2D(newRotationRadians - oldRotationRadians)));

    mViewportRotation = newRotationDegrees;
    mViewportPan = transform.GetTranslation();

    UpdateViewportWidgetTransform();
}

void
SStoryboardLevelViewport::AddViewportRotation( float iDeltaRotation )
{
    SetViewportRotation( mViewportRotation + iDeltaRotation );
}

bool
SStoryboardLevelViewport::IsViewportRotationChecked( float iRotation )
{
    return FMath::IsNearlyEqual( mViewportRotation, iRotation );
}

TSharedRef<SWidget>
SStoryboardLevelViewport::OnGetViewportRotationMenuContent() const
{
    // create zoom menu
    FMenuBuilder ViewportRotationMenuBuilder( true, CommandList );
    {
        TNumericUnitTypeInterface<int> degrees( EUnit::Degrees );

        ViewportRotationMenuBuilder.BeginSection( NAME_None, LOCTEXT( "storyboard-viewport-add-rotation-section", "Add Viewport Rotation" ) );
        ViewportRotationMenuBuilder.AddMenuEntry( FEposSequenceEditorCommands::Get().StoryboardViewportAdd10Rotate, NAME_None, FText::FromString( TEXT( "+" ) + degrees.ToString( 10 ) ) );
        ViewportRotationMenuBuilder.AddMenuEntry( FEposSequenceEditorCommands::Get().StoryboardViewportSubstract10Rotate, NAME_None, FText::FromString( degrees.ToString( -10 ) ) );
        ViewportRotationMenuBuilder.EndSection();

        ViewportRotationMenuBuilder.BeginSection( NAME_None, LOCTEXT( "storyboard-viewport-set-rotation-section", "Set Viewport Rotation" ) );
        for( auto command : FEposSequenceEditorCommands::Get().StoryboardViewportSetRotationX )
        {
            ViewportRotationMenuBuilder.AddMenuEntry( command.Value, NAME_None, FText::FromString( degrees.ToString( command.Key ) ) );
        }
        ViewportRotationMenuBuilder.EndSection();
    }

    return ViewportRotationMenuBuilder.MakeWidget();
}

//---

float
SStoryboardLevelViewport::GetViewportZoom() const
{
    return mViewportZoom;
}

void
SStoryboardLevelViewport::SetViewportZoom( float iZoom, FVector2D iZoomPosition )
{
    float oldZoom = mViewportZoom;
    float newZoom = FMath::Clamp( iZoom, MinZoom, MaxZoom );
    FTransform2D zoomTransform = FTransform2D(newZoom / oldZoom, FVector2D(0,0));

    FSlateRenderTransform transform = GetViewportTransform();
    transform = transform.Concatenate(FTransform2D(-iZoomPosition));
    transform = FTransform2D( Concatenate(transform, zoomTransform.GetMatrix()));
    transform = transform.Concatenate(FTransform2D(iZoomPosition));

    mViewportZoom = newZoom;
    mViewportPan = transform.GetTranslation();

    UpdateViewportWidgetTransform();
}

void
SStoryboardLevelViewport::AddViewportZoom( float iDeltaZoom )
{
    SetViewportZoom(mViewportZoom + iDeltaZoom);
}

void
SStoryboardLevelViewport::FitToScreen()
{
    float width = ViewportWidget->GetCachedGeometry().GetLocalSize().X;
    float height = ViewportWidget->GetCachedGeometry().GetLocalSize().Y;

    float selfWidth = ViewportWidget->GetCachedGeometry().GetLocalSize().X;
    float selfHeight = ViewportWidget->GetCachedGeometry().GetLocalSize().Y;

    //Get the BoundingBox

    //All calculations are done in the Transform Coodinate system
    //So (0,0) is bottom left
    TArray<FVector2D> points;
    FSlateRenderTransform transform = GetViewportTransform();
    points.Add(transform.TransformPoint(FVector2D(width / 2.f, height / 2.f)));
    points.Add(transform.TransformPoint(FVector2D(-width / 2.f, height / 2.f)));
    points.Add(transform.TransformPoint(FVector2D(width / 2.f, -height / 2.f)));
    points.Add(transform.TransformPoint(FVector2D(-width / 2.f, -height / 2.f)));

    FBox2D bbox(points);

    float selfRatio = (float)selfWidth / selfHeight;
    float ratio = bbox.GetSize().X / bbox.GetSize().Y;

    float zoom = GetViewportZoom();
    if (ratio > selfRatio)
    {
        //width
        zoom *= selfWidth / bbox.GetSize().X;
    }
    else
    {
        //height
        zoom *= selfHeight / bbox.GetSize().Y;
    }

    SetViewportZoom(zoom);
    SetViewportPan(FVector2D(0.0f, 0.0f));
}

bool
SStoryboardLevelViewport::IsViewportZoomChecked( float iZoom )
{
    return FMath::IsNearlyEqual( mViewportZoom, iZoom );
}

TSharedRef<SWidget>
SStoryboardLevelViewport::OnGetViewportZoomMenuContent() const
{
    // create zoom menu
    FMenuBuilder ViewportZoomMenuBuilder( true, CommandList );
    {
        TNumericUnitTypeInterface<int> percentage( EUnit::Percentage );

        ViewportZoomMenuBuilder.BeginSection( NAME_None, LOCTEXT( "storyboard-viewport-add-zoom-section", "Add Viewport Zoom" ) );
        ViewportZoomMenuBuilder.AddMenuEntry( FEposSequenceEditorCommands::Get().StoryboardViewportAdd10Zoom, NAME_None, FText::FromString( TEXT( "+" ) + percentage.ToString( 10 ) ) );
        ViewportZoomMenuBuilder.AddMenuEntry( FEposSequenceEditorCommands::Get().StoryboardViewportSubstract10Zoom, NAME_None, FText::FromString( percentage.ToString( -10 ) ) );
        ViewportZoomMenuBuilder.EndSection();

        ViewportZoomMenuBuilder.BeginSection( NAME_None, LOCTEXT( "storyboard-viewport-set-zoom-section", "Set Viewport Zoom" ) );
        for( auto command : FEposSequenceEditorCommands::Get().StoryboardViewportSetZoomX )
        {
            ViewportZoomMenuBuilder.AddMenuEntry( command.Value, NAME_None, FText::FromString( percentage.ToString( FMath::RoundToInt32(command.Key * 100.f) ) ) );
        }
        ViewportZoomMenuBuilder.EndSection();

        ViewportZoomMenuBuilder.AddSeparator();
        ViewportZoomMenuBuilder.AddMenuEntry( FEposSequenceEditorCommands::Get().StoryboardViewportFitToScreen, NAME_None, LOCTEXT( "storyboard-viewport-fit-to-screen", "Fit To Screen" ) );
    }

    return ViewportZoomMenuBuilder.MakeWidget();
}

//---

FVector2D
SStoryboardLevelViewport::GetViewportPan() const
{
    return mViewportPan;
}

void
SStoryboardLevelViewport::SetViewportPan( FVector2D iPan )
{
    mViewportPan = iPan;
    UpdateViewportWidgetTransform();
}

//---

void
SStoryboardLevelViewport::UpdateScrollBars()
{
    float width = ViewportWidget->GetCachedGeometry().GetLocalSize().X;
    float height = ViewportWidget->GetCachedGeometry().GetLocalSize().Y;

    float selfWidth = ViewportWidget->GetCachedGeometry().GetLocalSize().X;
    float selfHeight = ViewportWidget->GetCachedGeometry().GetLocalSize().Y;

    //Get the BoundingBox

    //All calculations are done in the Transform Coodinate system
    //So (0,0) is bottom left
    TArray<FVector2D> points;
    FSlateRenderTransform transform = GetViewportTransform();
    points.Add(transform.TransformPoint(FVector2D(width / 2.f, height / 2.f)));
    points.Add(transform.TransformPoint(FVector2D(-width / 2.f, height / 2.f)));
    points.Add(transform.TransformPoint(FVector2D(width / 2.f, -height / 2.f)));
    points.Add(transform.TransformPoint(FVector2D(-width / 2.f, -height / 2.f)));

    FBox2D bbox(points);

    FVector2D minPos( -bbox.GetSize().X / 2.f, -bbox.GetSize().Y / 2.f);
    FVector2D maxPos( selfWidth + bbox.GetSize().X / 2.f, selfHeight + bbox.GetSize().Y / 2.f);

    FVector2D dist = maxPos - minPos;
    FVector2D center = bbox.GetCenter() + FVector2D(selfWidth / 2.f, selfHeight / 2.f);

    FVector2D pos = (center - minPos) / dist;
    pos = pos.ClampAxes(0.f, 1.f) ;

    mHorizontalScrollBar->SetState((1.0 - pos.X) * ScrollbarSpaceRatio, ScrollbarThumbRatio);
    mVerticalScrollBar->SetState((1.0 - pos.Y) * ScrollbarSpaceRatio, ScrollbarThumbRatio);
}



FVector2D
SStoryboardLevelViewport::GetTranslationFromSlidersOffsets( float InScrollOffsetFractionX, float InScrollOffsetFractionY )
{
    float width = ViewportWidget->GetCachedGeometry().GetLocalSize().X;
    float height = ViewportWidget->GetCachedGeometry().GetLocalSize().Y;

    float selfWidth = ViewportWidget->GetCachedGeometry().GetLocalSize().X;
    float selfHeight = ViewportWidget->GetCachedGeometry().GetLocalSize().Y;

    //Get the BoundingBox

    //All calculations are done in the Transform Coodinate system
    //So (0,0) is bottom left
    TArray<FVector2D> points;
    FSlateRenderTransform transform = GetViewportTransform();
    points.Add(transform.TransformPoint(FVector2D(width / 2.f, height / 2.f)));
    points.Add(transform.TransformPoint(FVector2D(-width / 2.f, height / 2.f)));
    points.Add(transform.TransformPoint(FVector2D(width / 2.f, -height / 2.f)));
    points.Add(transform.TransformPoint(FVector2D(-width / 2.f, -height / 2.f)));

    FBox2D bbox(points);

    FVector2D minPos(-bbox.GetSize().X / 2.f, -bbox.GetSize().Y / 2.f);
    FVector2D maxPos(selfWidth + bbox.GetSize().X / 2.f, selfHeight + bbox.GetSize().Y / 2.f);

    FVector2D dist = maxPos - minPos;

    FVector2D pos = FVector2D( 1.f - (InScrollOffsetFractionX / (ScrollbarSpaceRatio)), 1.f - (InScrollOffsetFractionY / (ScrollbarSpaceRatio)));
    pos *= dist;
    pos += minPos;
    pos -= FVector2D(selfWidth / 2.f, selfHeight / 2.f);

    return pos;
}

void
SStoryboardLevelViewport::OnHorizontalScrollBarScrolled(float InScrollOffsetFraction)
{
    FVector2D translation = GetTranslationFromSlidersOffsets(InScrollOffsetFraction, mVerticalScrollBar->DistanceFromTop());
    FSlateRenderTransform transform = GetViewportTransform();
    SetViewportPan(FVector2D(translation.X, transform.GetTranslation().Y));
    //mHorizontalScrollBar->SetState(FMath::Clamp(InScrollOffsetFraction, 0.f, ScrollbarSpaceRatio), ScrollbarThumbRatio);
}


void
SStoryboardLevelViewport::OnVerticalScrollBarScrolled( float InScrollOffsetFraction )
{
    FVector2D translation = GetTranslationFromSlidersOffsets(mHorizontalScrollBar->DistanceFromTop(), InScrollOffsetFraction);
    FSlateRenderTransform transform = GetViewportTransform();
    SetViewportPan(FVector2D(transform.GetTranslation().X, translation.Y));
    //mVerticalScrollBar->SetState(FMath::Clamp(InScrollOffsetFraction, 0.f, ScrollbarSpaceRatio), ScrollbarThumbRatio);
}

//---

int32
SStoryboardLevelViewport::GetScaleVisibleWidgetIndex() const
{
    if( GetCameraFocalLengthVisibility().IsVisible() )
        return 1;

    return 0;
}

EVisibility
SStoryboardLevelViewport::GetMoveAndScalePlaneVisibility() const
{
    if( !mPlaneToMove.IsValid() )
        return EVisibility::Hidden;

    ACineCameraActor* camera = Cast<ACineCameraActor>( mPlaneToMove->GetAttachParentActor() );

    return ShotSequenceTools::CanMoveAndScalePlane( mPlaneToMove.Get(), camera ) ? EVisibility::Visible : EVisibility::Hidden;
}

float
SStoryboardLevelViewport::GetMoveAndScalePlaneDistance() const
{
    if( !mPlaneToMove.IsValid() )
        return 0.f;

    ACineCameraActor* camera = Cast<ACineCameraActor>( mPlaneToMove->GetAttachParentActor() );

    if( !ShotSequenceTools::CanMoveAndScalePlane( mPlaneToMove.Get(), camera ) )
        return 0.f;

    float distance = camera->GetDistanceTo( mPlaneToMove.Get() );

    return distance;
}
void
SStoryboardLevelViewport::SetMoveAndScalePlaneDistance( float iDistance )
{
    if( !mPlaneToMove.IsValid() )
        return;

    ACineCameraActor* camera = Cast<ACineCameraActor>( mPlaneToMove->GetAttachParentActor() );

    ShotSequenceTools::MoveAndScalePlane( mPlaneToMove.Get(), camera, iDistance, mScalePlaneType );
}

int32
SStoryboardLevelViewport::GetScalePlaneType() const
{
    return int32( mScalePlaneType );
}
void
SStoryboardLevelViewport::OnScalePlaneTypeChanged( int32 iScalePlaneType, ESelectInfo::Type iSelectType )
{
    mScalePlaneType = EScalePlane( iScalePlaneType );
}

EVisibility
SStoryboardLevelViewport::GetCameraFocalLengthVisibility() const
{
    if( !mCameraToFocalLength.IsValid() )
        return EVisibility::Hidden;

    return EVisibility::Visible;
}

float
SStoryboardLevelViewport::GetCameraFocalLength() const
{
    if( !mCameraToFocalLength.IsValid() )
        return 0.f;

    float focal_length = mCameraToFocalLength->GetCineCameraComponent()->CurrentFocalLength;

    return focal_length;
}
void
SStoryboardLevelViewport::SetCameraFocalLength( float iFocalLength )
{
    if( !mCameraToFocalLength.IsValid() )
        return;

    TArray<AActor*> children;
    mCameraToFocalLength->GetAttachedActors( children );

    TArray<TWeakObjectPtr<APlaneActor>> planes;
    for( auto child : children )
    {
        APlaneActor* plane = Cast<APlaneActor>( child );
        if( plane )
            planes.Add( plane );
    }

    ShotSequenceTools::SetCameraFocalLengthAndScalePlane( planes, mCameraToFocalLength.Get(), iFocalLength, mScalePlaneType );
}

//---

FReply
SStoryboardLevelViewport::OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& iMouseEvent)
{
    FModifierKeysState ModifierKeysState = FSlateApplication::Get().GetModifierKeys();
    const FInputChord activeChord(mInputProcessor->GetKey(),
        EModifierKey::FromBools(
            ModifierKeysState.IsControlDown(),
            ModifierKeysState.IsAltDown(),
            ModifierKeysState.IsShiftDown(),
            ModifierKeysState.IsCommandDown()
        )
    );

    if (!activeChord.IsValidChord())
        return FReply::Unhandled();


    if (FEposSequenceEditorCommands::Get().StoryboardViewportHoldToPanZoom->HasActiveChord(activeChord))
    {
        if (iMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
        {
            mIsPaning = true;
            mPanMouseInitialPosition = MyGeometry.AbsoluteToLocal(iMouseEvent.GetScreenSpacePosition());
            mPanInitialPan = mViewportPan;
            return FReply::Handled().CaptureMouse(AsShared()).PreventThrottling();
        }
        else if (iMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
        {
            mIsZooming = true;
            mZoomInitialZoom = FMath::Loge(mViewportZoom);
            mZoomMouseInitialPosition = MyGeometry.AbsoluteToLocal(iMouseEvent.GetScreenSpacePosition());
            return FReply::Handled().CaptureMouse(AsShared()).PreventThrottling();
        }
    }

    if (FEposSequenceEditorCommands::Get().StoryboardViewportHoldToRotate->HasActiveChord(activeChord))
    {
        mIsRotating = true;

        mRotateMouseInitialPosition = MyGeometry.AbsoluteToLocal(iMouseEvent.GetScreenSpacePosition());
        FVector2D size = MyGeometry.GetLocalSize(); //mOdysseyPainterEditorViewportPtr.Pin()->GetViewport()->GetSizeXY();
        FVector2D center = FVector2D( size.X / 2.f, size.Y / 2.f );
        mRotateCenter = center;

        FVector2D deltaCenter = mRotateMouseInitialPosition - mRotateCenter;
        mRotateInitialMouseAngle = FMath::Atan2( -deltaCenter.Y, deltaCenter.X );
        mRotateInitialRotation = mViewportRotation;

        return FReply::Handled().CaptureMouse(AsShared()).PreventThrottling();
    }
    return FReply::Unhandled();
}

FReply
SStoryboardLevelViewport::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& iMouseEvent)
{
    if( mIsPaning )
    {
        FVector2D mousePosition = MyGeometry.AbsoluteToLocal(iMouseEvent.GetScreenSpacePosition());
        FVector2D delta = mousePosition - mPanMouseInitialPosition;
        SetViewportPan(mPanInitialPan + delta);

        return FReply::Handled();
    }
    else if (mIsZooming)
    {
        FVector2D mousePosition = MyGeometry.AbsoluteToLocal(iMouseEvent.GetScreenSpacePosition());
        float delta = mousePosition.X - mZoomMouseInitialPosition.X;
        float smoothness = 200.f; //TODO: do a Setting to let the user change it at will
        FVector2D size = MyGeometry.GetLocalSize();
        FVector2D center = FVector2D( size.X / 2.f, size.Y / 2.f );

        if (delta > KINDA_SMALL_NUMBER || delta < KINDA_SMALL_NUMBER)
        {
            float zoom = FMath::Exp(mZoomInitialZoom + (delta / smoothness));
            SetViewportZoom(zoom, mZoomMouseInitialPosition - center);
        }

        return FReply::Handled();
    }
    else if( mIsRotating )
    {
        FVector2D mousePosition = MyGeometry.AbsoluteToLocal(iMouseEvent.GetScreenSpacePosition());
        FVector2D deltaCenter = mousePosition - mRotateCenter;
        float newAngle = FMath::Atan2( -deltaCenter.Y, deltaCenter.X );
        float deltaAngle = mRotateInitialMouseAngle - newAngle;
        float degrees = FUnitConversion::Convert( deltaAngle, EUnit::Radians, EUnit::Degrees );
        SetViewportRotation(mRotateInitialRotation + degrees);

        return FReply::Handled();
    }
    return FReply::Unhandled();
}

FReply
SStoryboardLevelViewport::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& iMouseEvent)
{
    if (mIsPaning)
    {
        mIsPaning = false;
        return FReply::Handled().ReleaseMouseCapture();
    }
    else if (mIsZooming)
    {
        mIsZooming = false;
        return FReply::Handled().ReleaseMouseCapture();
    }
    else if (mIsRotating)
    {
        mIsRotating = false;
        return FReply::Handled().ReleaseMouseCapture();
    }

    return FReply::Unhandled();
}

FReply SStoryboardLevelViewport::OnKeyDown( const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent )
{
    // Explicitly disallow the following keys which are by default mapped to navigate the sequencer timeline
    // because we don't want viewport and timeline navigation at the same time. Viewport takes precedence.
    if( InKeyEvent.GetKey() == EKeys::Up ||
        InKeyEvent.GetKey() == EKeys::Down ||
        InKeyEvent.GetKey() == EKeys::Left ||
        InKeyEvent.GetKey() == EKeys::Right ||
        InKeyEvent.GetKey() == EKeys::PageUp ||
        InKeyEvent.GetKey() == EKeys::PageDown )
    {
        return FReply::Unhandled();
    }

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

void SStoryboardLevelViewport::Setup()
{
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
            const bool bShowWorkingRange = false, bShowViewRange = true, bShowPlaybackRange = true;
            TimeRangeContainer->SetContent(Sequencer->MakeTimeRange(DecoratedTransportControls.ToSharedRef(), bShowWorkingRange, bShowViewRange, bShowPlaybackRange));
        }
    }
}

void SStoryboardLevelViewport::CleanUp()
{
    if( TransportControlsContainer )
        TransportControlsContainer->SetContent(SNullWidget::NullWidget);
    if( TimeRangeContainer )
        TimeRangeContainer->SetContent(SNullWidget::NullWidget);

}

void SStoryboardLevelViewport::OnSequencerChanged()
{
    CleanUp();
    mCurrentSquencer = nullptr;

    //---

    TArray<TWeakPtr<ISequencer>> sequencers = FLevelEditorSequencerIntegration::Get().GetSequencers();
    if( !sequencers.Num() )
        return;

    mCurrentSquencer = sequencers.Last();

    Setup();
}

ISequencer* SStoryboardLevelViewport::GetSequencer() const
{
    return mCurrentSquencer.Pin().Get();
}

void SStoryboardLevelViewport::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

    UpdateScrollBars();

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
    UMovieSceneCinematicBoardTrack* CinematicBoardTrack = Cast<UMovieSceneCinematicBoardTrack>(Sequence->GetMovieScene()->FindTrack(UMovieSceneCinematicBoardTrack::StaticClass()));

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
    TArray<ACineCameraActor*> selected_cameras;
    SelectedActors->GetSelectedObjects( selected_planes );
    SelectedActors->GetSelectedObjects( selected_cameras );

    for( auto selected_plane : selected_planes )
        UIData.SelectedPlanes.Add( FText::FromString( selected_plane->GetName() ) );

    //-

    mPlaneToMove = nullptr;
    if( selected_planes.Num() == 1 )
    {
        mPlaneToMove = selected_planes[0];
    }

    mCameraToFocalLength = nullptr;
    if( selected_cameras.Num() == 1 )
    {
        mCameraToFocalLength = selected_cameras[0];
    }

    //-

    mNotes.Empty();
    TArray<TWeakObjectPtr<UMovieSceneNoteSection>> note_sections = EposSequenceHelpers::GetNotesRecursive( *Sequencer, Sequence, Sequencer->GetFocusedTemplateID(), OuterTime.FrameNumber );
    for( auto note_section : note_sections )
        mNotes.Add( note_section->GetNote() );

    if( mWidgetNotesInViewport.IsValid() )
        mWidgetNotesInViewport->RefreshList();
    if( mWidgetNotesAsOverlay.IsValid() )
        mWidgetNotesAsOverlay->RefreshList();
}

#undef LOCTEXT_NAMESPACE
