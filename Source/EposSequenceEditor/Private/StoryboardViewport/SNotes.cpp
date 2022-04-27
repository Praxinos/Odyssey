// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
            .Padding( 0.f, 2.f )
            [
                SAssignNew( mWidgetList, SListView<TWeakObjectPtr<UStoryNote>> )
                .ListViewStyle( &FEposSequenceEditorStyle::Get().GetWidgetStyle<FTableViewStyle>( "Notes.Viewport.ListView" ) )
                .ListItemsSource( InArgs._ListItemsSource )
                .OnGenerateRow( this, &SNotesInViewport::MakeNoteRow )
                .SelectionMode( ESelectionMode::None )
            ]
        ]
        + SWidgetSwitcher::Slot()
        .HAlign( HAlign_Center )
        .VAlign( VAlign_Center )
        [
            SNew( STextBlock )
            .Text( LOCTEXT("notes-in-viewport.no-notes", "No Notes") )
            .TextStyle( FEposSequenceEditorStyle::Get(), "Notes.Viewport.Text.NoNotes" )
        ]
    ];
}

TSharedRef<ITableRow>
SNotesInViewport::MakeNoteRow( TWeakObjectPtr<UStoryNote> iItem, const TSharedRef<STableViewBase>& iOwnerTable )
{
    auto GetFont = [iItem]() -> FSlateFontInfo
    {
        return iItem.IsValid() ? iItem->Font : FSlateFontInfo();
    };

    auto GetColor = [iItem]() -> FLinearColor
    {
        return iItem.IsValid() ? iItem->ColorAndOpacity : FLinearColor::White;
    };

    auto GetShadowColor = [iItem]() -> FLinearColor
    {
        return iItem.IsValid() ? iItem->ShadowColorAndOpacity : FLinearColor::Black;
    };

    auto GetShadowOffset = [iItem]() -> FVector2D
    {
        return iItem.IsValid() ? iItem->ShadowOffset : FVector2D::UnitVector;
    };

    return
        SNew( STableRow<TWeakObjectPtr<UStoryNote>>, iOwnerTable )
        .Style( FEposSequenceEditorStyle::Get(), "Notes.Viewport.TableView.Row" )
        .Padding( FMargin( 10, 5 ) )
        [
            SNew( SHorizontalBox )
            + SHorizontalBox::Slot()
            .HAlign( HAlign_Center )
            [
                SNew( STextBlock )
                .Text_Lambda( [=]() { return FText::FromString( iItem->Text ); } )
                .Font_Lambda( GetFont )
                .ColorAndOpacity_Lambda( GetColor )
                .ShadowColorAndOpacity_Lambda( GetShadowColor )
                .ShadowOffset_Lambda( GetShadowOffset )
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

//---
//---
//---

void
SNotesAsOverlay::Construct(const FArguments& InArgs)
{
    ChildSlot
    .VAlign( VAlign_Bottom )
    .Padding( 20 )
    [
        SAssignNew( mWidgetList, SListView<TWeakObjectPtr<UStoryNote>> )
        .ListViewStyle( &FEposSequenceEditorStyle::Get().GetWidgetStyle<FTableViewStyle>( "Notes.Overlay.ListView" ) )
        .ListItemsSource( InArgs._ListItemsSource )
        .OnGenerateRow( this, &SNotesAsOverlay::MakeNoteRow )
        .SelectionMode( ESelectionMode::None )
    ];
}

TSharedRef<ITableRow>
SNotesAsOverlay::MakeNoteRow( TWeakObjectPtr<UStoryNote> iItem, const TSharedRef<STableViewBase>& iOwnerTable )
{
    auto GetFont = [iItem]() -> FSlateFontInfo
    {
        return iItem.IsValid() ? iItem->Font : FSlateFontInfo();
    };

    auto GetColor = [iItem]() -> FLinearColor
    {
        return iItem.IsValid() ? iItem->ColorAndOpacity : FLinearColor::White;
    };

    auto GetShadowColor = [iItem]() -> FLinearColor
    {
        return iItem.IsValid() ? iItem->ShadowColorAndOpacity : FLinearColor::Black;
    };

    auto GetShadowOffset = [iItem]() -> FVector2D
    {
        return iItem.IsValid() ? iItem->ShadowOffset : FVector2D::UnitVector;
    };

    return
        SNew( STableRow<TWeakObjectPtr<UStoryNote>>, iOwnerTable )
        .Style( FEposSequenceEditorStyle::Get(), "Notes.Overlay.TableView.Row" )
        .Padding( FMargin( 10, 5 ) )
        [
            SNew( SHorizontalBox )
            + SHorizontalBox::Slot()
            .HAlign( HAlign_Center )
            [
                SNew( STextBlock )
                .TextStyle( FEposSequenceEditorStyle::Get(), "Notes.Overlay.Text" )
                .Text_Lambda( [=]() { return FText::FromString( iItem->Text ); } )
                .Font_Lambda( GetFont )
                .ColorAndOpacity_Lambda( GetColor )
                .ShadowColorAndOpacity_Lambda( GetShadowColor )
                .ShadowOffset_Lambda( GetShadowOffset )
            ]
        ];
}

void
SNotesAsOverlay::RefreshList()
{
    if( !mWidgetList.IsValid() )
        return;

    //mWidgetList->RebuildList();
    mWidgetList->RequestListRefresh();
}

#undef LOCTEXT_NAMESPACE
