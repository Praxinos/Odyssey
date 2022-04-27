// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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

    ChildSlot
    .HAlign( HAlign_Fill )
    [
        SNew( SBorder )
        // Same as in ...\Engine\Source\Editor\Sequencer\Private\SAnimationOutlinerTreeNode.cpp::GetNodeBackgroundTint()
        .BorderImage( FAppStyle::Get().GetBrush( "Sequencer.AnimationOutliner.TopLevelBorder_Expanded" ) )
        .BorderBackgroundColor( FStyleColors::Title )
        [
            SNew( SHorizontalBox )
            + SHorizontalBox::Slot()
            .FillWidth( .5f )
            [
                SNew( SSpacer )
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
    // The text is always filled as HandleThumbnailTextBlockText() manages the empty name by returning the asset name
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
    // The text is always filled as HandleThumbnailTextBlockText() manages the empty name by returning the asset name
    if( !section_text.IsEmpty() )
        return FSlateColor::UseForeground();

    return FSlateColor::UseSubduedForeground();
}

//---

void
SCinematicBoardSectionTitle::EnterRename()
{
    mWidgetName->EnterEditingMode();
}

//---

#undef LOCTEXT_NAMESPACE
