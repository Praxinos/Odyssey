// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "CinematicBoardWidgets/SCinematicBoardSectionTitle.h"

#include "Brushes/SlateColorBrush.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

#include "CinematicBoardTrack/CinematicBoardSection.h"
#include "Shot/ShotSequence.h"
#include "Styles/EposTracksEditorStyle.h"
#include "Tools/EposSequenceTools.h"

#define LOCTEXT_NAMESPACE "SCinematicBoardSectionTitle"

//---

void
SCinematicBoardSectionTitle::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;

    mOptionalWidgetsVisibility = InArgs._OptionalWidgetsVisibility;

    //---

    FToolBarBuilder LeftToolbarBuilder( nullptr, FMultiBoxCustomization::None );
    LeftToolbarBuilder.SetStyle( &*FEposTracksEditorStyle::Get(), "EposSectionTitle.ToolBar" );
    LeftToolbarBuilder.SetLabelVisibility( EVisibility::Collapsed );

    auto Snap = [this]()
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;
        BoardSequenceTools::SnapCameraToViewport( sequencer, subsection_object, local_frame );
    };

    auto CanSnap = [this]() -> bool
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;
        return BoardSequenceTools::CanSnapCameraToViewport( sequencer, subsection_object, local_frame );
    };

    auto IsSnapVisible = [this]() -> bool
    {
        return mOptionalWidgetsVisibility.Get() == EVisibility::Visible
                && mBoardSection.Pin()->GetSubSectionObject().GetSequence()->IsA<UShotSequence>();
    };

    LeftToolbarBuilder.AddToolBarButton(
        FUIAction(
            FExecuteAction::CreateLambda( Snap ),
            FCanExecuteAction::CreateLambda( CanSnap ),
            FGetActionCheckState(),
            FIsActionButtonVisible::CreateLambda( IsSnapVisible )
        ),
        NAME_None,
        FText::GetEmpty(),
        LOCTEXT( "snap-camera-to-viewport-tooltip", "Snap the existing camera to the viewport (create a camera and set the current frame where to create the camera keyframe)" ),
        FSlateIcon( FEposTracksEditorStyle::Get()->GetStyleSetName(), "EposTracksEditor.SnapCameraToViewport" ) );

    //---

    ChildSlot
    .HAlign( HAlign_Fill )
    [
        SNew( SBorder )
        .BorderImage( FEditorStyle::GetBrush( "ToolPanel.GroupBorder" ) )
        .BorderBackgroundColor( FLinearColor( .50f, .50f, .50f, 1.0f ) )
        [
            SNew( SHorizontalBox ) // For future buttons
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                LeftToolbarBuilder.MakeWidget()
            ]
            + SHorizontalBox::Slot()
            .HAlign( HAlign_Center )
            [
                SAssignNew( mWidgetName, SInlineEditableTextBlock )
                .Text_Lambda( [this] { return HandleText(); } )
                .ColorAndOpacity_Lambda( [this] { return HandleTextColor(); } )
                .ShadowOffset( FVector2D( 1, 1 ) )
                .OnTextCommitted( mBoardSection.Pin().ToSharedRef(), &FCinematicBoardSection::HandleThumbnailTextBlockTextCommitted )
            ]
        ]
    ];
}

FText
SCinematicBoardSectionTitle::HandleText() const
{
    if( !mBoardSection.IsValid() )
        return FText::GetEmpty();

    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    UMovieSceneSequence*    subsequence = board_section->GetSubSectionObject().GetSequence();

    FText section_text = board_section->HandleThumbnailTextBlockText();
    if( !section_text.IsEmpty() )
        return section_text;

    FText sequence_text = subsequence ? subsequence->GetDisplayName() : FText::GetEmpty();
    if( !sequence_text.IsEmpty() )
        return FText::Format( FText::FromString( "<{0}>" ), sequence_text );

    return FText::GetEmpty();
}

FLinearColor
SCinematicBoardSectionTitle::HandleTextColor() const
{
    if( !mBoardSection.IsValid() )
        return FLinearColor::White;

    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();

    FText section_text = board_section->HandleThumbnailTextBlockText();
    if( !section_text.IsEmpty() )
        return FLinearColor( .75f, .75f, .75f );

    return FLinearColor( .25f, .25f, .25f );
}

//---

void
SCinematicBoardSectionTitle::EnterRename()
{
    mWidgetName->EnterEditingMode();
}

//---

#undef LOCTEXT_NAMESPACE
