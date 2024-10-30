// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPaletteEntryColor.h"
#include "OdysseyStyleSet.h"
#include "OdysseyPalette.h"
#include "Misc/TransactionObjectEvent.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "ScopedTransaction.h"

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

UOdysseyPaletteEntryColor::FOnEntryColorChanged& UOdysseyPaletteEntryColor::OnEntryColorChanged()
{
    static FOnEntryColorChanged onEntryColorChanged;
    return onEntryColorChanged;
}

FColor& UOdysseyPaletteEntryColor::GetUsedColor()
{
    return EntryColors[ GetPalette()->UsedSet ];
}

void UOdysseyPaletteEntryColor::SetUsedColor(FColor iColor)
{
    const FScopedTransaction transaction(NSLOCTEXT("Palette", "ChangeColorEntry_Transaction", "Change color entry"));

    FOdysseyObjectEditorUtils::PreChangePropertyValue(this, "EntryColors");

    EntryColors[GetPalette()->UsedSet] = iColor;

    FOdysseyObjectEditorUtils::PostChangePropertyValue(this, "EntryColors", EPropertyChangeType::ValueSet);
}

void UOdysseyPaletteEntryColor::AddSet()
{
    //The transaction is in OdysseyPalette, since we're adding a set for all entries, and we want only one transaction to handle all the additions
    FOdysseyObjectEditorUtils::PreChangePropertyValue(this, "EntryColors");

    EntryColors.Add( FColor::Black );

    FOdysseyObjectEditorUtils::PostChangePropertyValue(this, "EntryColors", EPropertyChangeType::ArrayAdd);
}

void UOdysseyPaletteEntryColor::DuplicateSetAt(int iIndex /*= -1 */)
{
    if (iIndex >= 0 && iIndex < EntryColors.Num())
    {   
        //The transaction is in OdysseyPalette, since we're adding a set for all entries, and we want only one transaction to handle all the additions
        FOdysseyObjectEditorUtils::PreChangePropertyValue(this, "EntryColors");

        FColor color = EntryColors[iIndex];
        EntryColors.Add( color );

        FOdysseyObjectEditorUtils::PostChangePropertyValue(this, "EntryColors", EPropertyChangeType::ArrayAdd);
    }
}

void UOdysseyPaletteEntryColor::RemoveSet(int iIndex /*= -1*/)
{
    //The transaction is in OdysseyPalette, since we're adding a set for all entries, and we want only one transaction to handle all the additions
    FOdysseyObjectEditorUtils::PreChangePropertyValue(this, "EntryColors");

    EntryColors.RemoveAt( iIndex );

    FOdysseyObjectEditorUtils::PostChangePropertyValue(this, "EntryColors", EPropertyChangeType::ArrayRemove);
}

void UOdysseyPaletteEntryColor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    PropertyChanged(PropertyChangedEvent.GetPropertyName());
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
    }
}

void UOdysseyPaletteEntryColor::EntryColorChanged()
{
    OnEntryColorChanged().Broadcast(this);
}

void UOdysseyPaletteEntryColor::PropertyChanged(const FName& iPropertyName)
{
    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPaletteEntryColor, EntryColors))
        EntryColorChanged();
}

#undef LOCTEXT_NAMESPACE
