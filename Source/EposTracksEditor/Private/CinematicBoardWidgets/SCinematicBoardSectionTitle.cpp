// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "CinematicBoardWidgets/SCinematicBoardSectionTitle.h"

#include "Brushes/SlateColorBrush.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

#include "CinematicBoardTrack/CinematicBoardSection.h"

#define LOCTEXT_NAMESPACE "SCinematicBoardSectionTitle"

//---

void
SCinematicBoardSectionTitle::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;

    ChildSlot
    .HAlign( HAlign_Fill )
    [
        SNew( SBorder )
        .BorderImage( FEditorStyle::GetBrush( "ToolPanel.GroupBorder" ) )
        .BorderBackgroundColor( FLinearColor( .50f, .50f, .50f, 1.0f ) )
        [
            SNew( SHorizontalBox ) // For future buttons
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
