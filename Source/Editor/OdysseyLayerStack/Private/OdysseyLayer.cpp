// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyLayer.h"

#include "OdysseyLayerStack.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "Misc/TransactionObjectEvent.h"

UOdysseyLayer::FOnNameChanged&
UOdysseyLayer::OnNameChanged()
{
    static FOnNameChanged onNameChanged;
    return onNameChanged;
}

UOdysseyLayer::FOnIsActivatedChanged&
UOdysseyLayer::OnIsActivatedChanged()
{
    static FOnIsActivatedChanged onIsActivatedChanged;
    return onIsActivatedChanged;
}

UOdysseyLayer::FOnIsLockedChanged&
UOdysseyLayer::OnIsLockedChanged()
{
    static FOnIsLockedChanged onIsLockedChanged;
    return onIsLockedChanged;
}

UOdysseyLayer::FOnIsExpandedChanged&
UOdysseyLayer::OnIsExpandedChanged()
{
    static FOnIsExpandedChanged onIsExpandedChanged;
    return onIsExpandedChanged;
}

UOdysseyLayer::FOnParentChanged&
UOdysseyLayer::OnParentChanged()
{
    static FOnParentChanged onParentChanged;
    return onParentChanged;
}

UOdysseyLayer::FOnChildrenChanged&
UOdysseyLayer::OnChildrenChanged()
{
    static FOnChildrenChanged onChildrenChanged;
    return onChildrenChanged;
}

UOdysseyLayer*
UOdysseyLayer::GetParent() const
{
    return Parent;
}

TArray<UOdysseyLayer*>
UOdysseyLayer::GetParents() const
{
    TArray<UOdysseyLayer*> parents;
    UOdysseyLayer* parent = Parent;
    while ( parent )
    {
        parents.Add(parent);
        parent = parent->Parent;
    }
    return parents;
}

const TArray<UOdysseyLayer*>&
UOdysseyLayer::GetChildren() const
{
    return Children;
}

TArray<UOdysseyLayer*>
UOdysseyLayer::GetChildrenRecursively(EGetLayerChildrenMethod Method) const
{
    TArray<UOdysseyLayer*> children;
    if ( Method == EGetLayerChildrenMethod::DepthFirst )
    {
        for ( UOdysseyLayer* child : Children )
        {
            children.Add(child);
            children.Append(child->GetChildrenRecursively(Method));
        }
    }
    else if ( Method == EGetLayerChildrenMethod::BreadthFirst )
    {
        children = Children;
        for ( UOdysseyLayer* child : Children )
            children.Append(child->GetChildrenRecursively(Method));
    }
    return children;
}

int
UOdysseyLayer::GetIndexInParent() const
{
    if ( !Parent )
        return INDEX_NONE;
    return Parent->Children.Find(const_cast<UOdysseyLayer*>(this));
}

bool
UOdysseyLayer::IsChildOf(UOdysseyLayer* Layer) const
{
    return GetParents().Find(Layer) != INDEX_NONE;
}

TSet<UClass*>
UOdysseyLayer::GetMergeDefaultLayerTypes() const
{
    return { GetClass() };
}

TSet<UClass*>
UOdysseyLayer::GetMergeLayerTypesFromTypes(TSet<UClass*> iLayerTypes) const
{
    return { GetClass() };
}

UOdysseyLayerStack*
UOdysseyLayer::GetLayerStack() const
{
    return Cast<UOdysseyLayerStack>(GetOuter());
}

void
UOdysseyLayer::OnCreated_Implementation()
{
}

void
UOdysseyLayer::Merge(const TArray<UOdysseyLayer*>& Layers)
{
}

void
UOdysseyLayer::NameChanged()
{
    OnNameChanged().Broadcast(this);
}

void
UOdysseyLayer::IsActivatedChanged()
{
    OnIsActivatedChanged().Broadcast(this);
}

void
UOdysseyLayer::IsLockedChanged()
{
    OnIsLockedChanged().Broadcast(this);
}

void
UOdysseyLayer::IsExpandedChanged()
{
    OnIsExpandedChanged().Broadcast(this);
}

void
UOdysseyLayer::ParentChanged()
{
    UOdysseyLayerStack* layerStack = GetLayerStack();
    if ( !layerStack )
        return;

    OnParentChanged().Broadcast(this);
    layerStack->HierarchyChanged();
}

void
UOdysseyLayer::ChildrenChanged()
{
    OnChildrenChanged().Broadcast(this);

    UOdysseyLayerStack* layerStack = GetLayerStack();
    if ( !layerStack )
        return;

    layerStack->HierarchyChanged();
}

void
UOdysseyLayer::PropertyChanged(const FName& iPropertyName)
{
    if ( iPropertyName == "Name" )
        NameChanged();
    if ( iPropertyName == "IsActivated" )
        IsActivatedChanged();
    if ( iPropertyName == "IsLocked" )
        IsLockedChanged();
    if ( iPropertyName == "IsExpanded" )
        IsExpandedChanged();
    if ( iPropertyName == "Parent" )
        ParentChanged();
    if ( iPropertyName == "Children" )
        ChildrenChanged();
}

void
UOdysseyLayer::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    PropertyChanged(PropertyChangedEvent.GetPropertyName());
}

void
UOdysseyLayer::PostTransacted(const FTransactionObjectEvent& iTransactionEvent)
{
    Super::PostTransacted(iTransactionEvent);

    if ( iTransactionEvent.GetEventType() != ETransactionObjectEventType::UndoRedo )
        return;

    const TArray<FName>& changedPropertyNames = iTransactionEvent.GetChangedProperties();
    for ( const FName& propertyName : changedPropertyNames )
    {
        PropertyChanged(propertyName);
    }
}