// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/Palette/SOdysseyPainterEditorPaletteRow.h"

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
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .VAlign(VAlign_Center)
            [
                SNew(STextBlock)
                .Text(this, &SOdysseyPainterEditorPaletteRow::GetPaletteName)
            ]
            + SHorizontalBox::Slot()
            .VAlign(VAlign_Center)
            [
                SNew(SOdysseyPaletteSetComboBox)
                .Palette(mPalette)
                .CurrentSet(mSet)
                .OnCurrentSetSelected(mOnSetChanged)
            ]
            + SHorizontalBox::Slot()
            .VAlign(VAlign_Center)
            .AutoWidth()
            [
                SNew(SButton)
                .ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("Button.TransparentNoPadding"))
                .OnClicked(mOnDeleteButtonClicked)
                [
                    SNew( SImage )
                    .Image( FAppStyle::GetBrush("Icons.Delete") )
                    .ColorAndOpacity( FSlateColor::UseForeground() )
                ]
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
