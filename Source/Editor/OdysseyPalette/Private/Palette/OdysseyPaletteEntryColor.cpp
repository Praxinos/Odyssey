// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPaletteEntryColor.h"

#define LOCTEXT_NAMESPACE "UOdysseyPaletteEntryColor"

UOdysseyPaletteEntryColor::UOdysseyPaletteEntryColor()
{
    EntryTypeName = LOCTEXT( "EntryTypeName", "Color Entry" );
    DefaultName = LOCTEXT("DefaultName", "Color");
    Icon = *FOdysseyStyle::GetBrush("OdysseyPalette.EntryColor");
    
    for( int i = 0; i < 8; i++ )
        EntryColor.Add( FColor::Black );
}

FColor& UOdysseyPaletteEntryColor::GetUsedColor()
{
    return EntryColor[ GetPalette()->UsedSet ];
}

void UOdysseyPaletteEntryColor::SetUsedColor(FColor iColor)
{
    EntryColor[GetPalette()->UsedSet] = iColor;
}

void UOdysseyPaletteEntryColor::AddSet()
{
    EntryColor.Add( FColor::Black );
}

void UOdysseyPaletteEntryColor::RemoveSet(int iIndex /*= -1*/)
{
    EntryColor.RemoveAt( iIndex );
}

#undef LOCTEXT_NAMESPACE