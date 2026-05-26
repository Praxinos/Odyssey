// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/Palette/SOdysseyPainterEditorPaletteEntryRow.h"

#include "Widgets/Images/SImage.h"
#include "Widgets/Text/STextBlock.h"

#include "Palette/OdysseyPaletteEntry.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- STableRow
void SOdysseyPainterEditorPaletteEntryRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& iTreeView)
{
    mEntry = InArgs._Entry;

    STableRow<TSharedPtr<struct IOdysseyPainterEditorPaletteTreeViewItem>>::FArguments args;
    args.Style(InArgs._Style);

    STableRow<TSharedPtr<struct IOdysseyPainterEditorPaletteTreeViewItem>>::Construct(
        args
        [
            SNew(SBox)
            .Padding(FMargin(0, 2, 0, 2))
            [
                SNew(SHorizontalBox)

                + SHorizontalBox::Slot()
                .AutoWidth()
                .HAlign(HAlign_Right)
                .VAlign(VAlign_Fill)
                [
                    SNew(SExpanderArrow, SharedThis(this) )
                    .ShouldDrawWires(false)
                ]

                + SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign( VAlign_Center )
                .Padding( FMargin( 3.f, 0 ) )
                [
                    SNew( SImage )
                    .Image( this, &SOdysseyPainterEditorPaletteEntryRow::GetIcon )
                    .ColorAndOpacity( this, &SOdysseyPainterEditorPaletteEntryRow::GetIconColorAndOpacity )
                ]

                + SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign(VAlign_Center)
                [
                    GenerateContentWidget()
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
                ]
            ]
        ],
        iTreeView
    );

    if (ExpanderArrowWidget)
        ExpanderArrowWidget->SetVisibility(EVisibility::Collapsed);
}

//PRIVATE API-----------------------------------------------------------

const FSlateBrush*
SOdysseyPainterEditorPaletteEntryRow::GetIcon() const
{
    return nullptr;
}

FSlateColor
SOdysseyPainterEditorPaletteEntryRow::GetIconColorAndOpacity() const
{
    return FLinearColor::White;
}

TSharedRef<SWidget>
SOdysseyPainterEditorPaletteEntryRow::GenerateContentWidget()
{
    return SNullWidget::NullWidget;
}

#undef LOCTEXT_NAMESPACE
