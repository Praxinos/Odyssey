// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPaletteUndoRemovePaletteEntry.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorLayer.h"

#define LOCTEXT_NAMESPACE "Palette"

FOdysseyPaletteUndoRemovePaletteEntry::FOdysseyPaletteUndoRemovePaletteEntry(TArray<FOdysseyVectorCell*> iCells)
    : mCells(iCells)
{
}

void FOdysseyPaletteUndoRemovePaletteEntry::Refresh()
{
    for (FOdysseyVectorCell* cell : mCells)
    {
        cell->GetLayer()->RequestRedraw(cell, 0);
    }
}

/** Called when redoing */
void FOdysseyPaletteUndoRemovePaletteEntry::Apply(UObject* iIgnored)
{
    Refresh();
}

/** called when undoing */
void FOdysseyPaletteUndoRemovePaletteEntry::Revert(UObject* iIgnored)
{
    Refresh();
}

FString FOdysseyPaletteUndoRemovePaletteEntry::ToString() const
{
    return FString("FOdysseyPaletteUndoRemovePaletteEntry");
}

#undef LOCTEXT_NAMESPACE
