// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPaletteEntryMaterial.h"

#define LOCTEXT_NAMESPACE "UOdysseyPaletteEntryMaterial"

UOdysseyPaletteEntryMaterial::UOdysseyPaletteEntryMaterial()
{
    EntryTypeName = LOCTEXT("entry-material.type", "Material Entry");
    DefaultName = LOCTEXT("entry-material.default-name", "Material");
    Icon = *FOdysseyStyle::GetBrush("OdysseyPalette.EntryMaterial");
}

#undef LOCTEXT_NAMESPACE