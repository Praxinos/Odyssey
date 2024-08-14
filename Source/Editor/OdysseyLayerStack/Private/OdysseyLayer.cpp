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

UOdysseyLayer::FOnDisplayChildrenChanged&
UOdysseyLayer::OnDisplayChildrenChanged()
{
    static FOnDisplayChildrenChanged onDisplayChildrenChanged;
    return onDisplayChildrenChanged;
}

UOdysseyLayer::FOnDisplayOptionsChanged&
UOdysseyLayer::OnDisplayOptionsChanged()
{
    static FOnDisplayOptionsChanged onDisplayOptionsChanged;
    return onDisplayOptionsChanged;
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

FSimpleMulticastDelegate&
UOdysseyLayer::OnMediaChanged()
{
    static FSimpleMulticastDelegate onMediaChanged;
    return onMediaChanged;
}

UOdysseyLayer::FOnBlendModeChanged&
UOdysseyLayer::OnBlendModeChanged()
{
    static FOnBlendModeChanged onBlendModeChanged;
    return onBlendModeChanged;
}

UOdysseyLayer::FOnOpacityChanged&
UOdysseyLayer::OnOpacityChanged()
{
    static FOnOpacityChanged onOpacityChanged;
    return onOpacityChanged;
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
	ImageRenderingCompositionChanged();
    OnIsActivatedChanged().Broadcast(this);
}

void
UOdysseyLayer::IsLockedChanged()
{
    OnIsLockedChanged().Broadcast(this);
}

void
UOdysseyLayer::DisplayChildrenChanged()
{
    OnDisplayChildrenChanged().Broadcast(this);
}

void
UOdysseyLayer::DisplayOptionsChanged()
{
    OnDisplayOptionsChanged().Broadcast(this);
}

void
UOdysseyLayer::ParentChanged()
{
    OnParentChanged().Broadcast(this);
}

void
UOdysseyLayer::ChildrenChanged()
{
	ImageRenderingCompositionChanged();
    OnChildrenChanged().Broadcast(this);
}

void
UOdysseyLayer::OpacityChanged()
{
    OnOpacityChanged().Broadcast(this);
    ImageRenderingChanged();
}

void
UOdysseyLayer::BlendModeChanged()
{
    OnBlendModeChanged().Broadcast(this);
    ImageRenderingChanged();
}

void
UOdysseyLayer::PropertyChanged(const FName& iPropertyName, const FName& iMemberPropertyName, bool iIsInteractive)
{
	if (iIsInteractive)
		return;

    if ( iPropertyName == "Name" )
        NameChanged();
    if ( iPropertyName == "IsActivated" )
        IsActivatedChanged();
    if ( iPropertyName == "IsLocked" )
        IsLockedChanged();
    if ( iPropertyName == "DisplayChildren" )
        DisplayChildrenChanged();
    if ( iPropertyName == "DisplayOptions" )
        DisplayOptionsChanged();
    if ( iPropertyName == "Parent" )
        ParentChanged();
    if ( iPropertyName == "Children" )
        ChildrenChanged();
    if (iPropertyName == "BlendMode")
        BlendModeChanged();
    if (iPropertyName == "Opacity")
        OpacityChanged();
}

void
UOdysseyLayer::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    PropertyChanged(PropertyChangedEvent.GetPropertyName(), PropertyChangedEvent.GetMemberPropertyName(), PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive );
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
        PropertyChanged(propertyName, propertyName, false);
    }
}

FOdysseyMediaProvider
UOdysseyLayer::GetMediaProvider(uint32 iFrameIndex) const
{
    return FOdysseyMediaProvider();
}

bool
UOdysseyLayer::GetIsActivatedRecursively() const
{
    const UOdysseyLayer* layer = this;
    while(layer)
    {
        if (!layer->IsActivated)
            return false;
        layer = layer->Parent;
    }

    return true;
}

bool
UOdysseyLayer::GetIsLockedRecursively() const
{
    const UOdysseyLayer* layer = this;
    while(layer)
    {
        if (layer->IsLocked)
            return true;
        layer = layer->Parent;
    }

    return false;
}

TArray<FGuid>
UOdysseyLayer::GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame) const
{
    TArray<FGuid> idComposition = { GetImageRenderingId() };

    const TArray<UOdysseyLayer*>& children = GetChildren();
    for (UOdysseyLayer* child : children)
    {
        if (!child->IsActivated)
            continue;

        idComposition.Append(child->GetImageRenderingComposition(iRenderType, iFrame));
    }

    return idComposition;
}

TArray<::ULIS::FRectI>
UOdysseyLayer::GetImageRenderingRects() const
{
    UOdysseyLayerStack* layerStack = GetLayerStack();
    if(!layerStack)
        return {};

    return layerStack->GetImageRenderingRects();
}



TSharedPtr<IOdysseyImageRenderer>
UOdysseyLayer::BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame, FImageRendererFilter iFilter) const
{
    if (iFilter.IsBound() && !iFilter.Execute(this))
        return nullptr;
    
    return MakeShared<FOdysseyLayerImageRenderer>(this, iFrame, iRenderType, GetImageRenderingRects(), iFilter);
}
