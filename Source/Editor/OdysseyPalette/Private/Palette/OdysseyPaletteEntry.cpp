// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPaletteEntry.h"
#include "OdysseyPalette.h"
#include "Misc/TransactionObjectEvent.h"

UOdysseyPaletteEntry::FOnNameChanged&
UOdysseyPaletteEntry::OnNameChanged()
{
    static FOnNameChanged onNameChanged;
    return onNameChanged;
}

UOdysseyPaletteEntry::FOnIsActivatedChanged&
UOdysseyPaletteEntry::OnIsActivatedChanged()
{
    static FOnIsActivatedChanged onIsActivatedChanged;
    return onIsActivatedChanged;
}

UOdysseyPaletteEntry::FOnIsExpandedChanged&
UOdysseyPaletteEntry::OnIsExpandedChanged()
{
    static FOnIsExpandedChanged onIsExpandedChanged;
    return onIsExpandedChanged;
}

UOdysseyPaletteEntry::FOnParentChanged&
UOdysseyPaletteEntry::OnParentChanged()
{
    static FOnParentChanged onParentChanged;
    return onParentChanged;
}

UOdysseyPaletteEntry::FOnChildrenChanged&
UOdysseyPaletteEntry::OnChildrenChanged()
{
    static FOnChildrenChanged onChildrenChanged;
    return onChildrenChanged;
}

UOdysseyPalette* UOdysseyPaletteEntry::GetPalette() const
{
    return Cast<UOdysseyPalette>(GetOuter());
}

void UOdysseyPaletteEntry::NameChanged()
{
    OnNameChanged().Broadcast(this);
}

void UOdysseyPaletteEntry::IsActivatedChanged()
{
    OnIsActivatedChanged().Broadcast(this);
}

void UOdysseyPaletteEntry::IsExpandedChanged()
{
    OnIsExpandedChanged().Broadcast(this);
}

void UOdysseyPaletteEntry::ParentChanged()
{
    OnParentChanged().Broadcast(this);
}

void UOdysseyPaletteEntry::ChildrenChanged()
{
    OnChildrenChanged().Broadcast(this);
}

void UOdysseyPaletteEntry::PropertyChanged(const FName& iPropertyName)
{
    if (iPropertyName == "Name")
        NameChanged();
    if (iPropertyName == "IsActivated")
        IsActivatedChanged();
    if (iPropertyName == "IsExpanded")
        IsExpandedChanged();
    if (iPropertyName == "Parent")
        ParentChanged();
    if (iPropertyName == "Children")
        ChildrenChanged();
}

void UOdysseyPaletteEntry::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    PropertyChanged(PropertyChangedEvent.GetPropertyName());
}

void UOdysseyPaletteEntry::PostTransacted(const FTransactionObjectEvent& iTransactionEvent)
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
