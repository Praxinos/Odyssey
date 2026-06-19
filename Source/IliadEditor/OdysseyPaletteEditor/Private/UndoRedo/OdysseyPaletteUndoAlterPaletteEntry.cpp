// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPaletteUndoAlterPaletteEntry.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorLayer.h"

#define LOCTEXT_NAMESPACE "Palette"

FOdysseyPaletteUndoAlterPaletteEntry::FOdysseyPaletteUndoAlterPaletteEntry( const TArray<FOdysseyVectorCell*>& iCells)
    : FOdysseyPaletteUndo(iCells)
{
}

FString FOdysseyPaletteUndoAlterPaletteEntry::ToString() const
{
    return FString("FOdysseyPaletteUndoAlterPaletteEntry");
}

#undef LOCTEXT_NAMESPACE
