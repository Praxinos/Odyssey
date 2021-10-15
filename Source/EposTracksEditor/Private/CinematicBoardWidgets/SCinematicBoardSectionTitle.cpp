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

class SInlineEditableTextBlockOnDoubleClick2
    : public SInlineEditableTextBlock
{
    virtual FReply OnMouseButtonDoubleClick( const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent ) override;
};

FReply
SInlineEditableTextBlockOnDoubleClick2::OnMouseButtonDoubleClick( const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent ) //override
{
    SInlineEditableTextBlock::OnMouseButtonDoubleClick( InMyGeometry, InMouseEvent );

    EnterEditingMode();

    return FReply::Handled();
}

//---

void
SCinematicBoardSectionTitle::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;

    mOptionalWidgetsVisibility = InArgs._OptionalWidgetsVisibility;

    //---

    FToolBarBuilder LeftToolbarBuilder( nullptr, FMultiBoxCustomization::None );
    LeftToolbarBuilder.SetLabelVisibility( EVisibility::Collapsed );
    LeftToolbarBuilder.SetStyle( &*FEposTracksEditorStyle::Get(), "EposSectionTitle.ToolBar" );

    auto PilotEject = [this]()
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;

        if( BoardSequenceTools::IsPilotingCamera( sequencer, subsection_object ) )
            BoardSequenceTools::EjectCamera( sequencer, subsection_object, local_frame );
        else
            BoardSequenceTools::PilotCamera( sequencer, subsection_object, local_frame );
    };

    auto CanPilotEject = [this]() -> bool
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;

        return BoardSequenceTools::CanPilotCamera( sequencer, subsection_object, local_frame )
                || BoardSequenceTools::CanEjectCamera( sequencer, subsection_object, local_frame );
    };

    auto IsPilotChecked = [this]() -> bool
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;

        return BoardSequenceTools::IsPilotingCamera( sequencer, subsection_object );
    };

    auto IsPilotEjectVisible = [this]() -> bool
    {
        return mOptionalWidgetsVisibility.Get() == EVisibility::Visible
            && mBoardSection.Pin()->GetSubSectionObject().GetSequence()->IsA<UShotSequence>();
    };

    TAttribute<FText> GetTooltip = MakeAttributeLambda(
        [this]() -> FText
        {
            ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
            const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
            FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;

            if( BoardSequenceTools::IsPilotingCamera( sequencer, subsection_object ) )
                return LOCTEXT( "eject-camera-tooltip", "Eject the existing camera" );
            else
                return LOCTEXT( "pilot-camera-tooltip", "Pilot the existing camera (create a camera and set the current frame where to create the camera keyframe)" );
        } );

    TAttribute<FSlateIcon> GetIcon = MakeAttributeLambda(
        [this]() -> FSlateIcon
        {
            ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
            const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
            FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;

            if( BoardSequenceTools::IsPilotingCamera( sequencer, subsection_object ) )
                return FSlateIcon( FEposTracksEditorStyle::Get()->GetStyleSetName(), "EposTracksEditor.EjectCamera" );
            else
                return FSlateIcon( FEposTracksEditorStyle::Get()->GetStyleSetName(), "EposTracksEditor.PilotCamera" );
        } );

    LeftToolbarBuilder.AddToolBarButton(
        FUIAction(
            FExecuteAction::CreateLambda( PilotEject ),
            FCanExecuteAction::CreateLambda( CanPilotEject ),
            FIsActionChecked::CreateLambda( IsPilotChecked ),
            FIsActionButtonVisible::CreateLambda( IsPilotEjectVisible )
        ),
        NAME_None,
        FText::GetEmpty(),
        GetTooltip,
        GetIcon,
        EUserInterfaceActionType::ToggleButton );

    //-

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
            .FillWidth( .5f )
            [
                LeftToolbarBuilder.MakeWidget()
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .HAlign( HAlign_Center )
            .VAlign( VAlign_Center )
            [
                SAssignNew( mWidgetName, SInlineEditableTextBlockOnDoubleClick2 )
                .Text( this, &SCinematicBoardSectionTitle::HandleTitleText )
                .ColorAndOpacity( this, &SCinematicBoardSectionTitle::HandleTitleTextColor )
                .ShadowOffset( FVector2D( 1, 1 ) )
                .OnTextCommitted( mBoardSection.Pin().ToSharedRef(), &FCinematicBoardSection::HandleThumbnailTextBlockTextCommitted )
            ]
            + SHorizontalBox::Slot()
            .FillWidth( .5f )
            [
                SNew( SSpacer )
            ]
        ]
    ];
}

FText
SCinematicBoardSectionTitle::HandleTitleText() const
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

FSlateColor
SCinematicBoardSectionTitle::HandleTitleTextColor() const
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
