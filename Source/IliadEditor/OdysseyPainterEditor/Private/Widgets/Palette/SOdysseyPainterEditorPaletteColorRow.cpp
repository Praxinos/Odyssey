// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/Palette/SOdysseyPainterEditorPaletteColorRow.h"

#include "Widgets/Colors/SColorBlock.h"
#include "Palette/OdysseyPaletteEntryColor.h"
#include "OdysseyStyle.h"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow
void SOdysseyPainterEditorPaletteColorRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& iTreeView)
{
    mIsCurrent = InArgs._IsCurrent;
    mSet = InArgs._Set;
    mEntryColor = InArgs._Entry;

    SOdysseyPainterEditorPaletteEntryRow::Construct(
        SOdysseyPainterEditorPaletteEntryRow::FArguments()
            .Entry_Lambda(
                [this]() -> UOdysseyPaletteEntry*
                {
                    return mEntryColor.Get();
                }
            ),
        iTreeView
    );
}

//PRIVATE API-----------------------------------------------------------

TSharedRef<SWidget>
SOdysseyPainterEditorPaletteColorRow::GenerateWidgetForColumn( const FName& InColumnName )
{
    if (InColumnName == "Color")
    {
        return GenerateColorWidget();
    }

    return SOdysseyPainterEditorPaletteEntryRow::GenerateWidgetForColumn(InColumnName);
}

TSharedRef<SWidget>
SOdysseyPainterEditorPaletteColorRow::GenerateColorWidget()
{
    return SNew(SBox)
        .Padding(FMargin(10, 2, 0, 2))
        [
            SNew(SColorBlock )
            .Color(this, &SOdysseyPainterEditorPaletteColorRow::GetEntryColorAsLinear)
        ];
}

FLinearColor SOdysseyPainterEditorPaletteColorRow::GetEntryColorAsLinear() const
{
    return FLinearColor( mEntryColor.Get()->GetColor( mSet.Get() ) );
}

const FSlateBrush*
SOdysseyPainterEditorPaletteColorRow::GetIcon() const
{
    return FOdysseyStyle::Get().GetBrush("OdysseyPalette.EntryColor");
}

bool
SOdysseyPainterEditorPaletteColorRow::IsItemSelected() const
{
    return mIsCurrent.Get();
}
