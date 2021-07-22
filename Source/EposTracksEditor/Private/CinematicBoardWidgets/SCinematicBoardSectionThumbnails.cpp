// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "CinematicBoardWidgets/SCinematicBoardSectionThumbnails.h"

#include "Brushes/SlateColorBrush.h"
#include "MovieSceneToolsUserSettings.h"
#include "SequencerSectionPainter.h"
#include "Widgets/Input/SSpinBox.h"

#include "CinematicBoardTrack/CinematicBoardSection.h"
#include "EposTracksToolbarHelpers.h"
#include "Settings/EposTracksEditorSettings.h"
#include "Styles/EposTracksEditorStyle.h"
#include "Tools/EposSequenceTools.h"

#define LOCTEXT_NAMESPACE "SCinematicBoardSectionThumbnails"

//---

void
SCinematicBoardSectionThumbnails::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;
    mOptionalWidgetsVisibility = InArgs._OptionalWidgetsVisibility;

    //---

    FToolBarBuilder LeftToolbarBuilder( nullptr, FMultiBoxCustomization::None );
    LeftToolbarBuilder.SetLabelVisibility( EVisibility::Collapsed );
    LeftToolbarBuilder.AddComboButton(
        FUIAction(
            FExecuteAction(),
            FCanExecuteAction(),
            FGetActionCheckState(),
            FIsActionButtonVisible::CreateLambda( [this](){ return mOptionalWidgetsVisibility.Get() == EVisibility::Visible; } )
        ),
        FOnGetContent::CreateSP( this, &SCinematicBoardSectionThumbnails::HandleAddBoardBeforeComboButtonGetMenuContent ),
        FText::GetEmpty(),
        LOCTEXT( "AddBoardBefore", "Add a new board or shot before" ),
        FSlateIcon( FEditorStyle::GetStyleSetName(), "Plus" ) );

    LeftToolbarBuilder.SetStyle( &*FEposTracksEditorStyle::Get(), "EposSection.ToolBar" );

    //-

    FToolBarBuilder MiddleToolbarBuilder( nullptr, FMultiBoxCustomization::None );
    MiddleToolbarBuilder.SetLabelVisibility( EVisibility::Collapsed );
    MiddleToolbarBuilder.AddToolBarButton(
        FUIAction(
            FExecuteAction::CreateLambda( [this](){ BoardSequenceTools::CreateCamera( mBoardSection.Pin()->GetSequencer().Get(), mBoardSection.Pin()->GetSectionObject()->GetInclusiveStartFrame() ); } ),
            FCanExecuteAction(),
            FGetActionCheckState(),
            FIsActionButtonVisible::CreateLambda( [this](){ return BoardSequenceTools::CanCreateCamera( mBoardSection.Pin()->GetSequencer().Get(), mBoardSection.Pin()->GetSectionObject()->GetInclusiveStartFrame() ); } )
        ),
        NAME_None,
        FText::GetEmpty(),
        LOCTEXT( "CameraToolTip", "Create a new Camera" ),
        FSlateIcon( FEposTracksEditorStyle::Get()->GetStyleSetName(), "EposTracksEditor.CreateCamera" ) );
    MiddleToolbarBuilder.AddComboButton(
        FUIAction(
            FExecuteAction(),
            FCanExecuteAction(),
            FGetActionCheckState(),
            FIsActionButtonVisible::CreateLambda( [this](){ return BoardSequenceTools::CanCreateCamera( mBoardSection.Pin()->GetSequencer().Get(), mBoardSection.Pin()->GetSectionObject()->GetInclusiveStartFrame() ); } )
        ),
        FOnGetContent::CreateRaw( this, &SCinematicBoardSectionThumbnails::MakeCameraMenu ),
        LOCTEXT( "CameraOptions", "Options" ),
        LOCTEXT( "CameraOptionsToolTip", "Camera Options" ),
        TAttribute<FSlateIcon>(),
        true );

    MiddleToolbarBuilder.SetStyle( &*FEposTracksEditorStyle::Get(), "EposSection.ToolBar" );

    //-

    FToolBarBuilder RightToolbarBuilder( nullptr, FMultiBoxCustomization::None );
    RightToolbarBuilder.SetLabelVisibility( EVisibility::Collapsed );
    RightToolbarBuilder.AddComboButton(
        FUIAction(
            FExecuteAction(),
            FCanExecuteAction(),
            FGetActionCheckState(),
            FIsActionButtonVisible::CreateLambda( [this](){ return mOptionalWidgetsVisibility.Get() == EVisibility::Visible; } )
        ),
        FOnGetContent::CreateSP( this, &SCinematicBoardSectionThumbnails::HandleAddBoardAfterComboButtonGetMenuContent ),
        FText::GetEmpty(),
        LOCTEXT( "AddBoardAfter", "Add a new board or shot after" ),
        FSlateIcon( FEditorStyle::GetStyleSetName(), "Plus" ) );

    RightToolbarBuilder.SetStyle( &*FEposTracksEditorStyle::Get(), "EposSection.ToolBar" );

    //---

    ChildSlot
    [
        SNew( SOverlay )
        + SOverlay::Slot()
        [
            SNew( SBox )
        ]
        + SOverlay::Slot()
        .HAlign( HAlign_Fill )
        .VAlign( VAlign_Center )
        [
            SNew( SHorizontalBox )
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign( VAlign_Center )
            [
                LeftToolbarBuilder.MakeWidget()
            ]
            + SHorizontalBox::Slot()
            [
                SNew( SSpacer )
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign( VAlign_Center )
            [
                SNew( SVerticalBox )
                + SVerticalBox::Slot()
                .AutoHeight()
                .HAlign( HAlign_Center )
                [
                    MiddleToolbarBuilder.MakeWidget()
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "CreateCameraInfo", "Don't forget to setup your camera settings" ) )
                    .ColorAndOpacity( FLinearColor( .5f, .5f, .0f ) )
                    .ShadowColorAndOpacity( FLinearColor::Black )
                    .ShadowOffset( FVector2D( 1.f, 1.f ) )
                    .Visibility_Lambda( [this]()
                                        {
                                            return BoardSequenceTools::CanCreateCamera( mBoardSection.Pin()->GetSequencer().Get(), mBoardSection.Pin()->GetSectionObject()->GetInclusiveStartFrame() )
                                                && mOptionalWidgetsVisibility.Get() == EVisibility::Visible
                                                ? EVisibility::Visible : EVisibility::Collapsed;
                                        } )
                ]
            ]
            + SHorizontalBox::Slot()
            [
                SNew( SSpacer )
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign( VAlign_Center )
            [
                RightToolbarBuilder.MakeWidget()
            ]
        ]
    ];
}

TSharedRef<SWidget>
SCinematicBoardSectionThumbnails::MakeCameraMenu()
{
    FMenuBuilder MenuBuilder( true, mBoardSection.Pin()->GetSequencer()->GetCommandBindings() );

    EposTracksToolbarHelpers::MakeCameraSettingsEntries( MenuBuilder );

    return MenuBuilder.MakeWidget();
}

TSharedRef<SWidget>
SCinematicBoardSectionThumbnails::HandleAddBoardBeforeComboButtonGetMenuContent()
{
    FMenuBuilder menuBuilder( true, nullptr );

    menuBuilder.AddMenuEntry( LOCTEXT( "section.create-shot-before-label", "New Shot" ),
                              LOCTEXT( "section.create-shot-before-tooltip", "Create a new section containing a new shot before this section with the same duration as the current section" ),
                              FSlateIcon( FEposTracksEditorStyle::Get()->GetStyleSetName(), "EposTracksEditor.NewSectionWithShotBeforeSection" ),
                              FUIAction( FExecuteAction::CreateLambda( [this]() { CinematicBoardTrackTools::InsertShot( mBoardSection.Pin()->GetSequencer().Get(), mBoardSection.Pin()->GetSectionObject()->GetInclusiveStartFrame() ); } ) ) );

    menuBuilder.AddMenuEntry( LOCTEXT( "section.create-board-before-label", "New Board" ),
                              LOCTEXT( "section.create-board-before-tooltip", "Create a new section containing a new board before this section with the same duration as the current section" ),
                              FSlateIcon( FEposTracksEditorStyle::Get()->GetStyleSetName(), "EposTracksEditor.NewSectionWithBoardBeforeSection" ),
                              FUIAction( FExecuteAction::CreateLambda( [this]() { CinematicBoardTrackTools::InsertBoard( mBoardSection.Pin()->GetSequencer().Get(), mBoardSection.Pin()->GetSectionObject()->GetInclusiveStartFrame() ); } ) ) );

    menuBuilder.AddSeparator();

    menuBuilder.AddMenuEntry( FUIAction( FExecuteAction::CreateLambda( [this]()
                                                                       {
                                                                           ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
                                                                           FFrameRate tick_resolution = sequencer->GetFocusedTickResolution();
                                                                           FFrameTime time = GetDefault<UEposTracksEditorSettings>()->DefaultDuration * tick_resolution;
                                                                           CinematicBoardTrackTools::InsertShot( sequencer, mBoardSection.Pin()->GetSectionObject()->GetInclusiveStartFrame() - 1, time.RoundToFrame().Value );
                                                                       } ) ),
                              CreatePopupNewSectionWithDurationWidget( LOCTEXT( "section.create-shot-before-with-duration-label", "New Shot of" ) ),
                              NAME_None,
                              LOCTEXT( "section.create-shot-before-with-duration-tooltip", "Create a new section containing a new shot before this section with the specified duration" ) );

    menuBuilder.AddMenuEntry( FUIAction( FExecuteAction::CreateLambda( [this]()
                                                                       {
                                                                           ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
                                                                           FFrameRate tick_resolution = sequencer->GetFocusedTickResolution();
                                                                           FFrameTime time = GetDefault<UEposTracksEditorSettings>()->DefaultDuration * tick_resolution;
                                                                           CinematicBoardTrackTools::InsertBoard( sequencer, mBoardSection.Pin()->GetSectionObject()->GetInclusiveStartFrame() - 1, time.RoundToFrame().Value );
                                                                       } ) ),
                              CreatePopupNewSectionWithDurationWidget( LOCTEXT( "section.create-board-before-with-duration-label", "New Board of" ) ),
                              NAME_None,
                              LOCTEXT( "section.create-board-before-with-duration-tooltip", "Create a new section containing a new board before this section with the specified duration" ) );

    return menuBuilder.MakeWidget();
}

TSharedRef<SWidget>
SCinematicBoardSectionThumbnails::HandleAddBoardAfterComboButtonGetMenuContent()
{
    FMenuBuilder menuBuilder( true, nullptr );

    menuBuilder.AddMenuEntry( LOCTEXT( "section.create-shot-after-label", "New Shot" ),
                              LOCTEXT( "section.create-shot-after-tooltip", "Create a new section containing a new shot after this section with the same duration as the current section" ),
                              FSlateIcon( FEposTracksEditorStyle::Get()->GetStyleSetName(), "EposTracksEditor.NewSectionWithShotAfterSection" ),
                              FUIAction( FExecuteAction::CreateLambda( [this]() { CinematicBoardTrackTools::InsertShot( mBoardSection.Pin()->GetSequencer().Get(), mBoardSection.Pin()->GetSectionObject()->GetExclusiveEndFrame() - 1 ); } ) ) );

    menuBuilder.AddMenuEntry( LOCTEXT( "section.create-board-after-label", "New Board" ),
                              LOCTEXT( "section.create-board-after-tooltip", "Create a new section containing a new board after this section with the same duration as the current section" ),
                              FSlateIcon( FEposTracksEditorStyle::Get()->GetStyleSetName(), "EposTracksEditor.NewSectionWithBoardAfterSection" ),
                              FUIAction( FExecuteAction::CreateLambda( [this]() { CinematicBoardTrackTools::InsertBoard( mBoardSection.Pin()->GetSequencer().Get(), mBoardSection.Pin()->GetSectionObject()->GetExclusiveEndFrame() - 1 ); } ) ) );

    menuBuilder.AddSeparator();

    menuBuilder.AddMenuEntry( FUIAction( FExecuteAction::CreateLambda( [this]()
                                                                       {
                                                                           ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
                                                                           FFrameRate tick_resolution = sequencer->GetFocusedTickResolution();
                                                                           FFrameTime time = GetDefault<UEposTracksEditorSettings>()->DefaultDuration * tick_resolution;
                                                                           CinematicBoardTrackTools::InsertShot( sequencer, mBoardSection.Pin()->GetSectionObject()->GetExclusiveEndFrame() - 1, time.RoundToFrame().Value );
                                                                       } ) ),
                              CreatePopupNewSectionWithDurationWidget( LOCTEXT( "section.create-shot-after-with-duration-label", "New Shot of" ) ),
                              NAME_None,
                              LOCTEXT( "section.create-shot-after-with-duration-tooltip", "Create a new section containing a new shot after this section with the specified duration" ) );

    menuBuilder.AddMenuEntry( FUIAction( FExecuteAction::CreateLambda( [this]()
                                                                       {
                                                                           ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
                                                                           FFrameRate tick_resolution = sequencer->GetFocusedTickResolution();
                                                                           FFrameTime time = GetDefault<UEposTracksEditorSettings>()->DefaultDuration * tick_resolution;
                                                                           CinematicBoardTrackTools::InsertBoard( sequencer, mBoardSection.Pin()->GetSectionObject()->GetExclusiveEndFrame() - 1, time.RoundToFrame().Value );
                                                                       } ) ),
                              CreatePopupNewSectionWithDurationWidget( LOCTEXT( "section.create-board-after-with-duration-label", "New Board of" ) ),
                              NAME_None,
                              LOCTEXT( "section.create-board-after-with-duration-tooltip", "Create a new section containing a new board after this section with the specified duration" ) );

    return menuBuilder.MakeWidget();
}

TSharedRef<SWidget>
SCinematicBoardSectionThumbnails::CreatePopupNewSectionWithDurationWidget( FText iLabel )
{
    auto OnDurationChanged = [=]( double iNewValue )
    {
        FFrameTime time = FFrameTime::FromDecimal( iNewValue );

        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        FFrameRate tick_resolution = sequencer->GetFocusedTickResolution();
        FFrameRate display_rate = sequencer->GetFocusedDisplayRate();
        time = FFrameRate::TransformTime( FFrameRate::TransformTime( time, tick_resolution, display_rate ).FloorToFrame(), display_rate, tick_resolution );

        double new_value_as_seconds = tick_resolution.AsSeconds( time.RoundToFrame() );

        GetMutableDefault<UEposTracksEditorSettings>()->SetDefaultDuration( new_value_as_seconds );
    };

    ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
    FFrameRate tick_resolution = sequencer->GetFocusedTickResolution(); // For the example below: 24000 fps
    FFrameRate display_rate = sequencer->GetFocusedDisplayRate();       // For the example below: 24fps

    return SNew( SHorizontalBox )
        + SHorizontalBox::Slot()
        .AutoWidth()
        .Padding( FEposTracksEditorStyle::Get()->GetMargin( "EposSection.ToolBar.Block.IndentedPadding" ) )
        [
            SNew( STextBlock )
            .Text( iLabel )
        ]
        + SHorizontalBox::Slot()
        [
            SNew(SSpacer)
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew( SSpinBox<double> )
            .TypeInterface( mBoardSection.Pin()->GetSequencer()->GetNumericTypeInterface() )
            .Style(&FEditorStyle::GetWidgetStyle<FSpinBoxStyle>( "Sequencer.HyperlinkSpinBox" ) )
            .OnValueCommitted_Lambda( [=] (double Value, ETextCommit::Type) { OnDurationChanged(Value); } )
            .OnValueChanged_Lambda( [=] (double Value) { OnDurationChanged(Value); } )
            .MinValue( FFrameRate::TransformTime( 1, display_rate, tick_resolution ).GetFrame().Value ) // Convert 1 frame in display rate (24fps) to 1000 frames in tick resolution (24000fps)
            .Value_Lambda( [=]() -> double
            {
                ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
                FFrameRate tick_resolution = sequencer->GetFocusedTickResolution();
                FFrameRate display_rate = sequencer->GetFocusedDisplayRate();
                // Convert default duration in seconds (fe: 3.2 seconds) to 76800 frames in tick resolution (3.2 * 24000)
                FFrameTime time = GetDefault<UEposTracksEditorSettings>()->DefaultDuration * tick_resolution;
                // Convert the duration to display rate (-> 76) and back again to tick resolution (-> 76000) to have a multiple of 1000 (scale between display rate and tick resolution)
                // And in this case, as the gui is displayed in display rate, 76000 is displayed as "00076" (and not "00076*" for 76800)
                time = FFrameRate::TransformTime( FFrameRate::TransformTime( time, tick_resolution, display_rate ).FloorToFrame(), display_rate, tick_resolution );

                return time.GetFrame().Value;
            } )
        ];
}

FVector2D
SCinematicBoardSectionThumbnails::ComputeDesiredSize( float ) const //override
{
    FVector2D size = GetDesiredSize();

    auto* Settings = GetDefault<UMovieSceneUserThumbnailSettings>();
    size.Y = Settings->bDrawThumbnails ? Settings->ThumbnailSize.Y : SequencerSectionConstants::DefaultSectionHeight;
    size.Y = size.Y + 2 * SequencerSectionConstants::DefaultSectionHeight /* top/bottom film-border */;

    return size;
}

//---

static
FTimeToPixel
ConstructTimeConverterForSection( const FGeometry& InSectionGeometry, const UMovieSceneSection& InSection )
{
    FFrameRate     TickResolution = InSection.GetTypedOuter<UMovieScene>()->GetTickResolution();
    double         LowerTime = InSection.GetInclusiveStartFrame() / TickResolution;
    double         UpperTime = InSection.GetExclusiveEndFrame() / TickResolution;

    return FTimeToPixel( InSectionGeometry, TRange<double>( LowerTime, UpperTime ), TickResolution );
}

struct FSequencerSectionPainterImpl
    : FSequencerSectionPainter
{
public:
    /** Constructor */
    FSequencerSectionPainterImpl( UMovieSceneSection& InSection, FSlateWindowElementList& _OutDrawElements, const FGeometry& InSectionGeometry );

    /** Virtual destructor */
    virtual ~FSequencerSectionPainterImpl();

    /** Paint the section background with the specified tint override */
    virtual int32 PaintSectionBackground( const FLinearColor& Tint );

public:
    /** Get a time-to-pixel converter for the section */
    virtual const FTimeToPixel& GetTimeConverter() const;

public:
    FTimeToPixel TimeToPixelConverter;
};

FSequencerSectionPainterImpl::FSequencerSectionPainterImpl( UMovieSceneSection& InSection, FSlateWindowElementList& _OutDrawElements, const FGeometry& InSectionGeometry )
    : FSequencerSectionPainter( _OutDrawElements, InSectionGeometry, InSection )
    , TimeToPixelConverter( ConstructTimeConverterForSection( SectionGeometry, InSection ) )
{
}

FSequencerSectionPainterImpl::~FSequencerSectionPainterImpl()
{
}

int32
FSequencerSectionPainterImpl::PaintSectionBackground( const FLinearColor& Tint )
{
    return 0;
}

const FTimeToPixel&
FSequencerSectionPainterImpl::GetTimeConverter() const
{
    return TimeToPixelConverter;
}

//---

int32
SCinematicBoardSectionThumbnails::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const //override
{
    if( !mBoardSection.IsValid() )
        return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );

    //---

    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    UMovieSceneSubSection*  subsection_object = &board_section->GetSubSectionObject();

    FSequencerSectionPainterImpl painter( *subsection_object, OutDrawElements, AllottedGeometry );
    //painter.KeyAreaElements = ;
    painter.SectionClippingRect = board_section->GetRootPainter( Args )->SectionClippingRect.IntersectionWith( MyCullingRect );
    painter.LayerId = LayerId;
    painter.bParentEnabled = board_section->GetRootPainter( Args )->bParentEnabled;
    painter.bIsHighlighted = board_section->GetRootPainter( Args )->bIsHighlighted;
    painter.bIsSelected = board_section->GetRootPainter( Args )->bIsSelected;

    //---

    static const FSlateBrush* filmBorder = FEditorStyle::GetBrush( "Sequencer.Section.FilmBorder" );

    FVector2D localSectionSize = painter.SectionGeometry.GetLocalSize();

    // Paint fancy-looking film border.
    FSlateDrawElement::MakeBox(
        painter.DrawElements,
        painter.LayerId++,
        painter.SectionGeometry.ToPaintGeometry( FVector2D( localSectionSize.X - 2.f, 7.f ), FSlateLayoutTransform( FVector2D( 1.f, 4.f ) ) ),
        filmBorder
    );

    FSlateDrawElement::MakeBox(
        painter.DrawElements,
        painter.LayerId++,
        painter.SectionGeometry.ToPaintGeometry( FVector2D( localSectionSize.X - 2.f, 7.f ), FSlateLayoutTransform( FVector2D( 1.f, localSectionSize.Y - 11.f ) ) ),
        filmBorder
    );

    //---

    painter.LayerId = board_section->FKeyThumbnailSection::OnPaintSection( painter );

    //---

    // Paint the sub-sequence information/looping boundaries/etc.

    FSubSectionPainterParams subSectionPainterParams( board_section->GetContentPadding() );
    subSectionPainterParams.bShowTrackNum = false;

    FSubSectionPainterUtil::PaintSection( board_section->GetSequencer(), *subsection_object, painter, subSectionPainterParams );

    //---

    return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, painter.LayerId, InWidgetStyle, bParentEnabled );
}

#undef LOCTEXT_NAMESPACE
