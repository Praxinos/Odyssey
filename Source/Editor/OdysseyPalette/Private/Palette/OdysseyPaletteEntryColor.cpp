// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPaletteEntryColor.h"

#define LOCTEXT_NAMESPACE "Palette"

UOdysseyPaletteEntryColor::UOdysseyPaletteEntryColor()
{
    EntryTypeName = LOCTEXT( "entry-color.type", "Color" );
    DefaultName = LOCTEXT("entry-color.default-name", "Color");
    Icon = *FOdysseyStyle::GetBrush("OdysseyPalette.EntryColor");
    
    if( GetPalette() )
    {
        for (int i = 0; i < GetPalette()->Sets.Num(); i++)
            AddSet();
    }

}

FColor& UOdysseyPaletteEntryColor::GetUsedColor()
{
    return EntryColors[ GetPalette()->UsedSet ];
}

void UOdysseyPaletteEntryColor::SetUsedColor(FColor iColor)
{
    EntryColors[GetPalette()->UsedSet] = iColor;
}

void UOdysseyPaletteEntryColor::AddSet()
{
    EntryColors.Add( FColor::Black );
}

void UOdysseyPaletteEntryColor::DuplicateSetAt(int iIndex /*= -1 */)
{
    if (iIndex >= 0 && iIndex < EntryColors.Num())
    {   
        FColor color = EntryColors[iIndex];
        EntryColors.Add( color );
    }
}

void UOdysseyPaletteEntryColor::RemoveSet(int iIndex /*= -1*/)
{
    EntryColors.RemoveAt( iIndex );
}

#undef LOCTEXT_NAMESPACE