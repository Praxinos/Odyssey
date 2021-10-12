// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "StoryboardViewport/SNotes.h"
#include "Styling/SlateTypes.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SHeader.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Input/SCheckBox.h"
#include "EditorStyleSet.h"

#include "Settings/EposSequenceEditorSettings.h"
#include "StoryNote.h"
#include "Styles/EposSequenceEditorStyle.h"

#define LOCTEXT_NAMESPACE "EposSequenceEditorNotes"

void
SNotesInViewport::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SNew( SWidgetSwitcher )
        .WidgetIndex_Lambda( [=]() -> int32 { return InArgs._ListItemsSource->Num() ? 0 : 1; } )

        + SWidgetSwitcher::Slot()
        [
            SNew( SVerticalBox )

            + SVerticalBox::Slot()
            .FillHeight( 1 )
            .HAlign( HAlign_Fill )
            [
                SNew( SBorder )
                .BorderImage( FEposSequenceEditorStyle::Get()->GetBrush( "EposSequenceEditor.StoryboardViewportNotesBackground" ) )
                [
                    SAssignNew( mWidgetList, SListView<TWeakObjectPtr<UStoryNote>> )
                    .ListItemsSource( InArgs._ListItemsSource )
                    .OnGenerateRow( this, &SNotesInViewport::MakeNoteRow )
                    .SelectionMode( ESelectionMode::None )
                ]
            ]
        ]
        + SWidgetSwitcher::Slot()
        .HAlign( HAlign_Center )
        .VAlign( VAlign_Center )
        [
            SNew( STextBlock )
            .Text( LOCTEXT("notes-in-viewport.no-notes", "No Notes") )
            .TextStyle( FEposSequenceEditorStyle::Get(), "EposSequenceEditor.NoNotes" )
        ]
    ];
}

TSharedRef<ITableRow>
SNotesInViewport::MakeNoteRow( TWeakObjectPtr<UStoryNote> iItem, const TSharedRef<STableViewBase>& iOwnerTable )
{
    return
        SNew( STableRow<TWeakObjectPtr<UStoryNote>>, iOwnerTable )
        .Padding( FMargin( 5, 2 ) )
        [
            SNew( SBorder )
            .BorderImage( FEposSequenceEditorStyle::Get()->GetBrush( "EposSequenceEditor.StoryboardViewportNoteBackground" ) )
            .Padding( FMargin( 3 ) )
            [
                SNew( SHorizontalBox )
                + SHorizontalBox::Slot()
                .HAlign( HAlign_Center )
                [
                    SNew( STextBlock )
                    .Text_Lambda( [=]() { return FText::FromString( iItem->Text ); } )
                ]
            ]
        ];
}

void
SNotesInViewport::RefreshList()
{
    if( !mWidgetList.IsValid() )
        return;

    //mWidgetList->RebuildList();
    mWidgetList->RequestListRefresh();
}

#undef LOCTEXT_NAMESPACE
