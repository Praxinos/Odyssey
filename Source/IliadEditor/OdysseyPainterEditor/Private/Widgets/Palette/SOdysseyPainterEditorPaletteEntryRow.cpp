// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Palette/SOdysseyPainterEditorPaletteEntryRow.h"
#include "Palette/OdysseyPaletteEntry.h"
#include "OdysseyStyleSet.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow
void SOdysseyPainterEditorPaletteEntryRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& iTreeView)
{
    mEntry = InArgs._Entry;

    SMultiColumnTableRow<TSharedPtr<struct IOdysseyPainterEditorPaletteTreeViewItem>>::FArguments args;
    // args.Style(&FOdysseyStyle::GetWidgetStyle<FTableRowStyle>("OdysseyLayerStack.AlternatedRows"))
    args.Padding(FMargin(0, 2, 0, 2));

    SMultiColumnTableRow<TSharedPtr<struct IOdysseyPainterEditorPaletteTreeViewItem>>::Construct(
        args,
        iTreeView
    );
}

//PRIVATE API-----------------------------------------------------------

TSharedRef<SWidget>
SOdysseyPainterEditorPaletteEntryRow::GenerateWidgetForColumn( const FName& InColumnName )
{
    if (InColumnName == "Header")
    {
        return GenerateHeaderWidget();
    }

    return SNullWidget::NullWidget;
}

const FSlateBrush*
SOdysseyPainterEditorPaletteEntryRow::GetIcon() const
{
    return FOdysseyStyle::Get().GetBrush("OdysseyPalette.EntryColor");
}

FSlateColor
SOdysseyPainterEditorPaletteEntryRow::GetIconColorAndOpacity() const
{
    return FLinearColor::White;
}

TSharedRef<SWidget>
SOdysseyPainterEditorPaletteEntryRow::GenerateHeaderWidget()
{
    return SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .AutoWidth()
        .HAlign(HAlign_Right)
        .VAlign(VAlign_Fill)
        [
            SNew(SExpanderArrow, SharedThis(this) )
            .ShouldDrawWires(true)
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        .VAlign(VAlign_Center)
        [
            SNew(SImage)
            .Image(this, &SOdysseyPainterEditorPaletteEntryRow::GetIcon)
            .ColorAndOpacity(this, &SOdysseyPainterEditorPaletteEntryRow::GetIconColorAndOpacity)
        ]
        + SHorizontalBox::Slot()
        .VAlign(VAlign_Center)
        [
            SNew(STextBlock)
            .Text_Lambda(
                [this]()
                {
                    return mEntry.Get()->EntryName;
                }
            )
        ];
}

#undef LOCTEXT_NAMESPACE
