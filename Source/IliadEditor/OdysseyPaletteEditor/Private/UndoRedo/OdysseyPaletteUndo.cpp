// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPaletteUndo.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorLayer.h"

#define LOCTEXT_NAMESPACE "Palette"

FOdysseyPaletteUndo::FOdysseyPaletteUndo( const TArray<FOdysseyVectorCell*>& iCells)
    : mCells(iCells)
{
}

void FOdysseyPaletteUndo::Refresh()
{
    for (FOdysseyVectorCell* cell : mCells)
    {
        cell->GetLayer()->RequestRedraw(cell, 0);
    }
}

/** Called when redoing */
void FOdysseyPaletteUndo::Apply(UObject* iIgnored)
{
    Refresh();
}

/** called when undoing */
void FOdysseyPaletteUndo::Revert(UObject* iIgnored)
{
    Refresh();
}

FString FOdysseyPaletteUndo::ToString() const
{
    return FString("FOdysseyPaletteUndo");
}

#undef LOCTEXT_NAMESPACE
