// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Palette/OdysseyPaletteEntryColor.h"
#include "OdysseyStyle.h"
#include "OdysseyPalette.h"
#include "Misc/TransactionObjectEvent.h"
#include "ScopedTransaction.h"

#define LOCTEXT_NAMESPACE "Palette"

UOdysseyPaletteEntryColor::UOdysseyPaletteEntryColor()
{
    EntryTypeName = LOCTEXT( "entry-color.type", "Color" );
    DefaultName = LOCTEXT("entry-color.default-name", "Color");
    Icon = *FOdysseyStyle::GetBrush("OdysseyPalette.EntryColor");
}

void
UOdysseyPaletteEntryColor::PostInitProperties()
{
    Super::PostInitProperties();
    if( GetPalette() )
    {
        for (int i = 0; i < GetPalette()->GetSets().Num(); i++)
            AddSet();
    }
}

UOdysseyPaletteEntryColor::FOnEntryColorChanged& UOdysseyPaletteEntryColor::OnEntryColorChanged()
{
    static FOnEntryColorChanged onEntryColorChanged;
    return onEntryColorChanged;
}

FColor& UOdysseyPaletteEntryColor::GetColor(int iSet)
{
    if( EntryColors.Num() <= iSet )
        return EntryColors[0];

    return EntryColors[ iSet ];
}

void UOdysseyPaletteEntryColor::SetColor(FColor iColor, int iSet)
{
    if (EntryColors.Num() <= iSet)
        return;

    EntryColors[iSet] = iColor;
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
