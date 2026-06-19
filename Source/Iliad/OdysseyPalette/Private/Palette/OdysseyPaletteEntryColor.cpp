// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Palette/OdysseyPaletteEntryColor.h"
#include "OdysseyPalette.h"
#include "Misc/TransactionObjectEvent.h"

#define LOCTEXT_NAMESPACE "Palette"

UOdysseyPaletteEntryColor::UOdysseyPaletteEntryColor()
{
    EntryTypeName = LOCTEXT( "entry-color.type", "Color" );
    DefaultName = LOCTEXT("entry-color.default-name", "Color");
}

void
UOdysseyPaletteEntryColor::PostInitProperties()
{
    Super::PostInitProperties();
    if (GetPalette())
    {
        for( auto pair: GetPalette()->GetSetsIDs() )
            AddSet( pair.Key );
    }
}

UOdysseyPaletteEntryColor::FOnEntryColorChanged& UOdysseyPaletteEntryColor::OnEntryColorChanged()
{
    static FOnEntryColorChanged onEntryColorChanged;
    return onEntryColorChanged;
}

FColor& UOdysseyPaletteEntryColor::GetColor(FGuid iSet)
{
    if( !EntryColorsIDs.Contains(iSet) )
    {
        static FColor InvalidColor(0, 0, 0, 0);
        return InvalidColor;
    }
    return EntryColorsIDs[iSet];
}

void UOdysseyPaletteEntryColor::SetColor(FColor iColor, FGuid iSet)
{
    if (!EntryColorsIDs.Contains(iSet))
        return;

    EntryColorsIDs[iSet] = iColor;
}

void UOdysseyPaletteEntryColor::AddSet(FGuid iNewId)
{
    EntryColorsIDs.Add( iNewId, FColor::Black );
}

void UOdysseyPaletteEntryColor::DuplicateSetAt(FGuid iIndexToCopy, FGuid iNewId)
{
    FColor color = EntryColorsIDs[iIndexToCopy];
    EntryColorsIDs.Add(iNewId, color);
}

void UOdysseyPaletteEntryColor::RemoveSet(FGuid iIndex)
{
    EntryColorsIDs.Remove( iIndex );
}

#if WITH_EDITOR
void UOdysseyPaletteEntryColor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    PropertyChanged(PropertyChangedEvent.GetPropertyName());
    PostPropertyChanged(PropertyChangedEvent.GetPropertyName());
}

void UOdysseyPaletteEntryColor::PostTransacted(const FTransactionObjectEvent& iTransactionEvent)
{
    Super::PostTransacted(iTransactionEvent);

    if (iTransactionEvent.GetEventType() != ETransactionObjectEventType::UndoRedo)
        return;

    const TArray<FName>& changedPropertyNames = iTransactionEvent.GetChangedProperties();
    for (const FName& propertyName : changedPropertyNames)
    {
        PropertyChanged(propertyName);
        PostPropertyChanged(propertyName);
    }
}
#endif //WITH_EDITOR

void UOdysseyPaletteEntryColor::PostLoad()
{
    Super::PostLoad();
    // Legacy, to delete next version
    TArray<FName> setsNames = GetPalette()->GetSets();
    for (int i = 0; i < setsNames.Num(); i++)
    {
        // We have to be careful, because we could have a palette with multiple sets with the same name.
        // Colors may be mismatched in that case, but at least the TMap will be consistent with the IDs stored in the UOdysseyPalette
        TArray<FGuid> ids;

        for (const auto& Pair : GetPalette()->GetSetsIDs())
        {
            if (Pair.Value == setsNames[i])
            {
                ids.Add(Pair.Key);
            }
        }

        for( int j = 0; j < ids.Num(); j++ )
        {
            if( EntryColorsIDs.Contains(ids[j]))
                continue;

            EntryColorsIDs.Add(ids[j], EntryColors[i]);
        }
    }

    EntryColors.Empty();
    //---
}

void UOdysseyPaletteEntryColor::EntryColorChanged()
{
}

void UOdysseyPaletteEntryColor::PropertyChanged(const FName& iPropertyName)
{
    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPaletteEntryColor, EntryColors))
        EntryColorChanged();
}

void UOdysseyPaletteEntryColor::PostPropertyChanged(const FName& iPropertyName)
{
    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPaletteEntryColor, EntryColors))
        OnEntryColorChanged().Broadcast(this);
}

#undef LOCTEXT_NAMESPACE
