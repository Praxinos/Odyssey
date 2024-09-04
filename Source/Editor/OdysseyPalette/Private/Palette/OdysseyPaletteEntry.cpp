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

void UOdysseyPaletteEntry::OnCreated_Implementation()
{
    //newName works like a unique ID for this entry
    //FName newName = FName( GetPackage()->GetPersistentGuid().ToString() + GetFName().ToString() );
    //LowLevelRename(newName);
}

UOdysseyPalette* UOdysseyPaletteEntry::GetPalette() const
{
    return Cast<UOdysseyPalette>(GetOuter());
}

uint8 UOdysseyPaletteEntry::GetUsedSetInPalete() const
{
    return Cast<UOdysseyPalette>(GetOuter())->UsedSet;
}

TArray<UOdysseyPaletteEntry*> UOdysseyPaletteEntry::GetChildrenRecursively(EGetEntryChildrenMethod iMethod /*= EGetEntryChildrenMethod::DepthFirst*/) const
{
    TArray<UOdysseyPaletteEntry*> children;
    if (iMethod == EGetEntryChildrenMethod::DepthFirst)
    {
        for (UOdysseyPaletteEntry* child : Children)
        {
            children.Add(child);
            children.Append(child->GetChildrenRecursively(iMethod));
        }
    }
    else if (iMethod == EGetEntryChildrenMethod::BreadthFirst)
    {
        children = Children;
        for (UOdysseyPaletteEntry* child : Children)
            children.Append(child->GetChildrenRecursively(iMethod));
    }
    return children;
}

int UOdysseyPaletteEntry::GetIndexInParent() const
{
    if ( !Parent )
        return INDEX_NONE;
    return Parent->Children.Find(const_cast<UOdysseyPaletteEntry*>(this));
}

bool UOdysseyPaletteEntry::IsChildOf(UOdysseyPaletteEntry* iEntry) const
{
    return GetParents().Find(iEntry) != INDEX_NONE;
}

UOdysseyPaletteEntry* UOdysseyPaletteEntry::GetParent() const
{
    return Parent;
}

TArray<UOdysseyPaletteEntry*> UOdysseyPaletteEntry::GetParents() const
{
    TArray<UOdysseyPaletteEntry*> parents;
    UOdysseyPaletteEntry* parent = Parent;
    while ( parent )
    {
        parents.Add(parent);
        parent = parent->Parent;
    }
    return parents;
}

void UOdysseyPaletteEntry::AddSet()
{
    
}

void UOdysseyPaletteEntry::DuplicateSetAt(int iIndex /*= -1 */)
{

}

void UOdysseyPaletteEntry::RemoveSet(int iIndex /*= -1 */)
{

}

const TArray<UOdysseyPaletteEntry*>& UOdysseyPaletteEntry::GetChildren() const
{
    return Children;
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
    UOdysseyPalette* palette = GetPalette();
    if (!palette)
        return;

    OnParentChanged().Broadcast(this);
    palette->HierarchyChanged();
}

void UOdysseyPaletteEntry::ChildrenChanged()
{
    UOdysseyPalette* palette = GetPalette();
    if (!palette)
        return;

    OnChildrenChanged().Broadcast(this);
    palette->HierarchyChanged();
}

void UOdysseyPaletteEntry::PropertyChanged(const FName& iPropertyName)
{
    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPaletteEntry, EntryName))
        NameChanged();
    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPaletteEntry, IsActivated))
        IsActivatedChanged();
    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPaletteEntry, IsExpanded))
        IsExpandedChanged();
    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPaletteEntry, Parent))
        ParentChanged();
    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPaletteEntry, Children))
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
