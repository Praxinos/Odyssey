// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPaletteUndoRemovePaletteEntry.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorLayer.h"

#define LOCTEXT_NAMESPACE "Palette"

FOdysseyPaletteUndoRemovePaletteEntry::FOdysseyPaletteUndoRemovePaletteEntry( const TArray<FOdysseyVectorCell*>& iCells)
    : FOdysseyPaletteUndo(iCells)
{
}

FString FOdysseyPaletteUndoRemovePaletteEntry::ToString() const
{
    return FString("FOdysseyPaletteUndoRemovePaletteEntry");
}

#undef LOCTEXT_NAMESPACE
