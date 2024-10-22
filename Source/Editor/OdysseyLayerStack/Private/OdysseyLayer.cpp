// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyLayer.h"

#include "OdysseyLayerStack.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyLayerImageRenderer.h"
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
	if (Parent)
		Parent->ImageRenderingCompositionChanged();
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
    UOdysseyLayerStack* layerStack = GetLayerStack();
    if ( !layerStack )
        return;

    OnParentChanged().Broadcast(this);
    layerStack->HierarchyChanged();
}

void
UOdysseyLayer::ChildrenChanged()
{
    UOdysseyLayerStack* layerStack = GetLayerStack();
    if ( !layerStack )
        return;

	ImageRenderingCompositionChanged();
    OnChildrenChanged().Broadcast(this);
    layerStack->HierarchyChanged();
}

void
UOdysseyLayer::OpacityChanged(bool iIsInteractive)
{
    OnOpacityChanged().Broadcast(this);
    ImageRenderingChanged(iIsInteractive);
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
    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyLayer, Name) )
        NameChanged();
    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyLayer, IsActivated) )
        IsActivatedChanged();
    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyLayer, IsLocked) )
        IsLockedChanged();
    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyLayer, DisplayChildren) )
        DisplayChildrenChanged();
    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyLayer, DisplayOptions) )
        DisplayOptionsChanged();
    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyLayer, Parent) )
        ParentChanged();
    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyLayer, Children) )
        ChildrenChanged();
    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyLayer, BlendMode) )
        BlendModeChanged();
    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyLayer, Opacity) )
        OpacityChanged(iIsInteractive);
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
UOdysseyLayer::IsActivatedRecursively() const
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
UOdysseyLayer::IsLockedRecursively() const
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

void
UOdysseyLayer::NameBlueprintSetter(FText Value)
{
	FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyLayer, Name), Value);
}

void
UOdysseyLayer::IsActivatedBlueprintSetter(bool Value)
{
	FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyLayer, IsActivated), Value);
}

void
UOdysseyLayer::IsLockedBlueprintSetter(bool Value)
{
	FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyLayer, IsLocked), Value);
}

void
UOdysseyLayer::DisplayChildrenBlueprintSetter(bool Value)
{
	FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyLayer, DisplayChildren), Value);
}

void
UOdysseyLayer::DisplayOptionsBlueprintSetter(bool Value)
{
	FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyLayer, DisplayOptions), Value);
}

void
UOdysseyLayer::BlendModeBlueprintSetter(EOdysseyBlendingMode Value)
{
	FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyLayer, BlendMode), Value);
}

void
UOdysseyLayer::OpacityBlueprintSetter(float Value)
{
	FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyLayer, Opacity), Value);
}

#ifdef WITH_EDITOR

TArray<FName>
UOdysseyLayer::GetRows() const
{
	return {
		"Main",
		"Blend"
	};
}

int
UOdysseyLayer::GetRowHeight(FName iSubRowName) const
{
	if (iSubRowName == "Main")
		return 20;

	if (iSubRowName == "Blend")
		return 20;
		
	return 0;
}

bool
UOdysseyLayer::IsRowVisible(FName iSubRowName) const
{
	if (iSubRowName == "Main")
		return true;

	if (iSubRowName == "Blend")
		return DisplayOptions;
		
	return 0;
}

FMargin
UOdysseyLayer::GetRowPadding(FName iSubRowName) const
{
	return FMargin(0.f, 0.f, 0.f, 2.f);
}

#endif