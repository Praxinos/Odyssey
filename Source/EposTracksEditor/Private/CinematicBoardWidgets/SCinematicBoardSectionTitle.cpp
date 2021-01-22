// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "CinematicBoardWidgets/SCinematicBoardSectionTitle.h"

#include "Brushes/SlateColorBrush.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

#include "CinematicBoardTrack/CinematicBoardSection.h"

#define LOCTEXT_NAMESPACE "SCinematicBoardSectionTitle"

//---

//static
float
SCinematicBoardSectionTitle::GetHeight( TSharedRef<const FCinematicBoardSection> iBoardSection )
{
    return FEditorStyle::GetFontStyle( "NormalFont" ).Size + 8.f;
}

void
SCinematicBoardSectionTitle::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;

    mName = InArgs._Name;

    ChildSlot
    [
        SNew( SBorder )
        .HAlign( EHorizontalAlignment::HAlign_Center )
        .BorderImage( FEditorStyle::GetBrush( "ToolPanel.GroupBorder" ) )
        .BorderBackgroundColor( FLinearColor( .5f, .5f, .5f ) )
        .Padding( 3 )
        [
            SAssignNew( mWidgetName, SInlineEditableTextBlock )
            .Text( mName )
            .ColorAndOpacity( FLinearColor( .75f, .75f, .75f ) )
            .ShadowOffset( FVector2D( 1, 1 ) )
            .OnTextCommitted( mBoardSection.ToSharedRef(), &FCinematicBoardSection::HandleThumbnailTextBlockTextCommitted )
        ]
    ];
}

void
SCinematicBoardSectionTitle::EnterRename()
{
    mWidgetName->EnterEditingMode();
}

//---

#undef LOCTEXT_NAMESPACE
