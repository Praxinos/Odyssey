// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/Palette/SOdysseyPainterEditorPaletteRow.h"

#include "Widgets/Input/SButton.h"

#include "OdysseyStyle.h"
#include "OdysseyPalette.h"
#include "Widgets/SOdysseyPaletteSetComboBox.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow
void SOdysseyPainterEditorPaletteRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& iTreeView)
{
    mPalette = InArgs._Palette;
    mSet = InArgs._Set;
    mOnSetChanged = InArgs._OnSetChanged;
    mOnDeleteButtonClicked = InArgs._OnDeleteButtonClicked;

    STableRow<TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem>>::FArguments args;
    args.Padding(FMargin(0, 2, 0, 2))
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .HAlign(HAlign_Fill)
                .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                    .Text(this, &SOdysseyPainterEditorPaletteRow::GetPaletteName)
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign(VAlign_Center)
                [
                    SNew(SButton)
                    .ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("SimpleButton.NoPadding"))
                    .ToolTipText( LOCTEXT( "PainterEditor.Palette.RemoveFromList", "Remove this palette from the list" ) )
                    .OnClicked(mOnDeleteButtonClicked)
                    [
                        SNew( SImage )
                        .Image( FAppStyle::GetBrush("Icons.Delete") )
                        .ColorAndOpacity( FSlateColor::UseForeground() )
                    ]
                ]
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            .HAlign( HAlign_Fill )
            [
                SNew(SOdysseyPaletteSetComboBox)
                .Palette(mPalette)
                .CurrentSet(mSet)
                .OnCurrentSetSelected(mOnSetChanged)
            ]
        ];

    STableRow<TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem>>::Construct(
        args,
        iTreeView
    );
}

//PRIVATE API-----------------------------------------------------------

FText
SOdysseyPainterEditorPaletteRow::GetPaletteName() const
{
    return FText::FromName(mPalette.Get()->GetFName());
}

bool
SOdysseyPainterEditorPaletteRow::IsItemSelected() const
{
    return false;
}

ESelectionMode::Type
SOdysseyPainterEditorPaletteRow::GetSelectionMode() const
{
    return ESelectionMode::None;
}

#undef LOCTEXT_NAMESPACE
