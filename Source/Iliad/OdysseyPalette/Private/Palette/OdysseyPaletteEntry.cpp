// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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

void UOdysseyPaletteEntry::AddSet(FGuid iNewId)
{

}

void UOdysseyPaletteEntry::DuplicateSetAt(FGuid iIndexToCopy, FGuid iNewId)
{

}

void UOdysseyPaletteEntry::RemoveSet(FGuid iIndex)
{

}

const TArray<UOdysseyPaletteEntry*>& UOdysseyPaletteEntry::GetChildren() const
{
    return Children;
}

void UOdysseyPaletteEntry::NameChanged()
{
}

void UOdysseyPaletteEntry::IsActivatedChanged()
{
}

void UOdysseyPaletteEntry::IsExpandedChanged()
{
}

void UOdysseyPaletteEntry::ParentChanged()
{
}

void UOdysseyPaletteEntry::ChildrenChanged()
{
}

void UOdysseyPaletteEntry::PropertyChanged(const FName& iPropertyName)
{
    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPaletteEntry, EntryName))
    {
        OnNameChanged().Broadcast(this);
    }
    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPaletteEntry, IsActivated))
    {
        OnIsActivatedChanged().Broadcast(this);
    }
    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPaletteEntry, IsExpanded))
    {
        OnIsExpandedChanged().Broadcast(this);
    }
    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPaletteEntry, Parent))
    {
        UOdysseyPalette* palette = GetPalette();
        if (!palette)
            return;

        OnParentChanged().Broadcast(this);
        palette->HierarchyChanged();
    }
    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPaletteEntry, Children))
    {
        UOdysseyPalette* palette = GetPalette();
        if (!palette)
            return;

        OnChildrenChanged().Broadcast(this);
        palette->HierarchyChanged();
    }
}

void UOdysseyPaletteEntry::PostPropertyChanged(const FName& iPropertyName)
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

#if WITH_EDITOR
void UOdysseyPaletteEntry::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    PropertyChanged(PropertyChangedEvent.GetPropertyName());
    PostPropertyChanged(PropertyChangedEvent.GetPropertyName());
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
        PostPropertyChanged(propertyName);
    }
}
#endif
