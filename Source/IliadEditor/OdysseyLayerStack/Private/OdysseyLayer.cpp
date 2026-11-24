// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyLayer.h"

#include "Misc/TransactionObjectEvent.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"

#include "OdysseyBlendShader.h"
#include "OdysseyLayerRoot.h"
#include "OdysseyLayerStack.h"
#include "OdysseyLayerStackRenderContext.h"
#include "OdysseyLayerStackSelection.h"
#include "Selection.h"
#include "OdysseyRenderContext.h"

#if WITH_EDITOR

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

#endif

void
UOdysseyLayer::PostLoad()
{
    Super::PostLoad();

    if (BlendMode_DEPRECATED != EOdysseyBlendingMode::kBlendingMode_Count)
    {
        GetBlendModeFromDeprecatedBlendingMode(BlendMode_DEPRECATED, LayerBlendMode);
    }

    for (UOdysseyLayer* child : Children)
    {
        child->Parent = this;
    }
}

void
UOdysseyLayer::PostDuplicate(EDuplicateMode::Type iDuplicateMode)
{
    Super::PostDuplicate(iDuplicateMode);

    // See comment (REALLY) in ...\Plugins\Odyssey\Source\Iliad\OdysseyLayerStack\Private\OdysseyLayerStack.cpp#590: CopyLayerInternal()

    //PATCH---------------------------
#if WITH_EDITOR
    FText old_current_layer;
    if( IsA<UOdysseyLayerRoot>() )
        old_current_layer = GetLayerStack()->GetCurrentLayer()->GetLayerName();
#endif
    //--------------------------------

    TArray<UOdysseyLayer*> children = GetChildren();
    Children.Empty();

    TArray<UOdysseyLayer*> duplicated_children;
    for( UOdysseyLayer* child : children )
    {
        FObjectDuplicationParameters params( child, GetLayerStack() );
        UOdysseyLayer* duplicatedLayer = Cast<UOdysseyLayer>( StaticDuplicateObjectEx( params ) );
        if( !duplicatedLayer )
            continue;

        duplicated_children.Add( duplicatedLayer );
    }

    Children.Append( duplicated_children );
    for( UOdysseyLayer* child : duplicated_children )
        child->Parent = this;

    //PATCH---------------------------
#if WITH_EDITOR
    if( IsA<UOdysseyLayerRoot>() )
    {
        TArray<UOdysseyLayer*> layers = GetLayerStack()->GetLayers();
        for( UOdysseyLayer* layer : layers )
        {
            if( layer->GetLayerName().EqualTo( old_current_layer ) )
                GetLayerStack()->SetCurrentLayer( layer ); // Call delegate ?! or make a new function ?
        }
    }
#endif
    //--------------------------------
}

FInt32Range
UOdysseyLayer::GetFrameRange() const
{
    TArray<FInt32Range> Ranges;
    for (UOdysseyLayer* Child : Children)
    {
        Ranges.Add(Child->GetFrameRange());
    }
    return FInt32Range::Hull(Ranges);
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

FText
UOdysseyLayer::GetLayerTypeName() const
{
    return LayerTypeName;
}

FText
UOdysseyLayer::GetDescription() const
{
    return Description;
}

FText
UOdysseyLayer::GetDefaultName() const
{
    return DefaultName;
}

bool
UOdysseyLayer::CanHaveChildren() const
{
    return bCanHaveChildren;
}

void
UOdysseyLayer::Merge(const TArray<UOdysseyLayer*>& Layers)
{
}

#if WITH_EDITOR

FOdysseyMediaProvider
UOdysseyLayer::GetMediaProvider(uint32 iFrameIndex) const
{
    return FOdysseyMediaProvider();
}
#endif

bool
UOdysseyLayer::GetInheritsAlpha() const
{
    return bInheritsAlpha;
}

FText
UOdysseyLayer::GetLayerName() const
{
    return Name;
}

bool
UOdysseyLayer::IsActivated() const
{
    return bIsActivated;
}

bool
UOdysseyLayer::IsActivatedRecursively() const
{
    const UOdysseyLayer* layer = this;
    while(layer)
    {
        if (!layer->bIsActivated)
            return false;
        layer = layer->Parent;
    }

    return true;
}
bool
UOdysseyLayer::IsEditable() const
{
    return !IsLockedRecursively() && IsActivatedRecursively();
}

bool
UOdysseyLayer::IsLocked() const
{
    return bIsLocked;
}

bool
UOdysseyLayer::IsLockedRecursively() const
{
    const UOdysseyLayer* layer = this;
    while(layer)
    {
        if (layer->bIsLocked)
            return true;
        layer = layer->Parent;
    }

    return false;
}

#if WITH_EDITOR

bool
UOdysseyLayer::ShouldDisplayChildren() const
{
    return bDisplayChildren;
}

bool
UOdysseyLayer::ShouldDisplayOptions() const
{
    return bDisplayOptions;
}

#endif

EOdysseyBlendMode
UOdysseyLayer::GetBlendMode() const
{
    return LayerBlendMode;
}

float
UOdysseyLayer::GetOpacity() const
{
    return Opacity;
}

FIntRect
UOdysseyLayer::GetDefaultRenderRect() const
{
    UOdysseyLayerStack* layerStack = GetLayerStack();
    if(!layerStack)
        return FIntRect(0, 0, 0, 0);

    return layerStack->GetDefaultRenderRect();
}

void
UOdysseyLayer::SetInheritsAlpha(bool InheritsAlpha)
{
    if( !CanSetInheritsAlpha() )
        return;

    Modify();
    bInheritsAlpha = InheritsAlpha;

    RenderingChanged();
}

void
UOdysseyLayer::AddChild(UOdysseyLayer* Layer, int IndexInParent)
{
    AddChildren({Layer}, IndexInParent);
}

void
UOdysseyLayer::AddChildren(TArray<UOdysseyLayer*> Layers, int IndexInParent)
{
    if( !IsEditable() )
        return;

    Modify();

    for (UOdysseyLayer* layer : Layers)
        ensure(!layer->Parent);

    //Sanitize Layers array
    Layers.RemoveAll(
        [this](const UOdysseyLayer* iLayer)
        {
            return !iLayer || iLayer->Parent != nullptr;
        }
    );

    if (IndexInParent == INDEX_NONE)
    {
        Children.Append(Layers);
    }
    else
    {
        IndexInParent = FMath::Clamp(IndexInParent, 0, Children.Num());
        Children.Insert(Layers, IndexInParent);
    }

    for (UOdysseyLayer* layer : Layers)
    {
        // Allow reparenting layers even if they are read only
        layer->Modify();
        layer->Parent = this;
    }

    RenderingCompositionChanged();

    UOdysseyLayerStack* layerStack = GetLayerStack();
    if ( !layerStack )
        return;

    layerStack->HierarchyChanged();
}

void
UOdysseyLayer::RemoveChild(UOdysseyLayer* Layer)
{
    RemoveChildren({Layer});
}

void
UOdysseyLayer::RemoveChildren(TArray<UOdysseyLayer*> Layers)
{
    if( !IsEditable() )
        return;

    //Sanitize Layers array
    Layers.RemoveAll(
        [this](const UOdysseyLayer* iLayer)
        {
            return !iLayer || !Children.Contains(iLayer);
        }
    );

    //No Layers
    if (Layers.Num() <= 0)
        return;

    Modify();

    for (UOdysseyLayer* layer : Layers)
    {
        // Allow unparenting layers even if they are read only
        layer->Modify();
        layer->Parent = nullptr;
        Children.Remove(layer);
    }

    RenderingCompositionChanged();
    UOdysseyLayerStack* layerStack = GetLayerStack();
    if ( !layerStack )
        return;

    layerStack->HierarchyChanged();
}

void
UOdysseyLayer::SetLayerName(FText Value)
{
    if( !IsEditable() )
        return;

    Modify();
    Name = Value;
}

void
UOdysseyLayer::SetIsActivated(bool Value)
{
    Modify();

    bIsActivated = Value;
    if (Parent)
        Parent->RenderingCompositionChanged();
}

void
UOdysseyLayer::SetIsLocked(bool Value)
{
    Modify();
    bIsLocked = Value;
}

#if WITH_EDITOR

const FSlateIcon&
UOdysseyLayer::GetIcon() const
{
    return Icon;
}

const FSlateIcon&
UOdysseyLayer::GetIconExpanded() const
{
    return IconExpanded;
}

void
UOdysseyLayer::SetDisplayChildren(bool Value)
{
    bDisplayChildren = Value;
    OnDisplayChildrenChanged().Broadcast(this);
}

void
UOdysseyLayer::SetDisplayOptions(bool Value)
{
    bDisplayOptions = Value;
    OnDisplayOptionsChanged().Broadcast(this);
}

#endif

bool
UOdysseyLayer::CanSetBlendMode(EOdysseyBlendMode Value) const
{
    if (!IsEditable())
        return false;

    if (!bInheritsAlpha)
        return true;

    return Value != EOdysseyBlendMode::Erase && Value != EOdysseyBlendMode::Stencil;
}

bool
UOdysseyLayer::CanSetInheritsAlpha() const
{
    if (!IsEditable())
        return false;

    return LayerBlendMode != EOdysseyBlendMode::Erase && LayerBlendMode != EOdysseyBlendMode::Stencil;
}

void
UOdysseyLayer::SetBlendMode(EOdysseyBlendMode Value)
{
    if( !CanSetBlendMode(Value) )
        return;

    Modify();

    LayerBlendMode = Value;
    RenderingChanged();
}

void
UOdysseyLayer::SetOpacity(float iValue)
{
    if( !IsEditable() )
        return;

    Modify();

    Opacity = FMath::Clamp( iValue, 0.f, 1.f );
    RenderingChanged();
}

#if WITH_EDITOR
void
UOdysseyLayer::SetOpacityInteractive(float iValue)
{
    if( !IsEditable() )
        return;

    Modify();

    Opacity = FMath::Clamp( iValue, 0.f, 1.f );
    RenderingChanged(true);
}

#endif

UTextureRenderTarget2D*
UOdysseyLayer::CreateRenderingRenderTarget() const
{
    return GetLayerStack()->CreateRenderingRenderTarget();
}

bool
UOdysseyLayer::BuildRenderChildrenPipeline(
    IOdysseyTextureRenderingAbility::FRenderFunction& oRenderFunction,
    const TArray<const IOdysseyTextureRenderingAbility*>& iParents, const FOdysseyRenderContext* RenderContext
) const
{
    struct FChildRenderParams
    {
        IOdysseyTextureRenderingAbility::FRenderFunction RenderFunction;
        EOdysseyColorBlendMode BlendMode;
        EOdysseyAlphaBlendMode AlphaMode;
        float Opacity;
    };

#if WITH_EDITOR
    UOdysseyLayerStack* layerStack = GetLayerStack();
    UOdysseyLayer* currentLayer = layerStack->GetCurrentLayer();
    TArray<UOdysseyLayer*> parents = GetParents();
    bool bIsParentCurrentLayer = currentLayer == this || parents.Contains(currentLayer);
#endif

    TArray<FChildRenderParams> childrenRenderParams;
    for ( int i = Children.Num() - 1; i >= 0; i-- )
    {
        UOdysseyLayer* child = Children[i];
        const FOdysseyLayerStackRenderContext* LayerContext = RenderContext ? RenderContext->GetExtension<FOdysseyLayerStackRenderContext>() : nullptr;
        const FOdysseyLayerStackRenderOverride* LayerOverride = LayerContext ? LayerContext->Find(child->GetRenderingId()) : nullptr;
        if ( !(LayerOverride && LayerOverride->bVisible.IsSet() ? LayerOverride->bVisible.GetValue() : child->IsActivated()) )
            continue;

        IOdysseyTextureRenderingAbility::FRenderFunction childRenderFunction;
        if (child->BuildRenderPipeline(childRenderFunction, iParents, RenderContext))
        {
            float opacity = LayerOverride && LayerOverride->Opacity.IsSet() ? LayerOverride->Opacity.GetValue() : child->GetOpacity();

#if WITH_EDITOR
            bool bIsChildCurrentLayer = child == layerStack->GetCurrentLayer();
            if( (!LayerContext || LayerContext->bUseDisplayOnlyCurrentLayer) && !bIsChildCurrentLayer && !bIsParentCurrentLayer && layerStack->GetDisplayOnlyCurrentLayer() && !child->CanHaveChildren() )
                opacity = opacity * layerStack->GetOtherLayersOpacityNormalized();
#endif

            EOdysseyColorBlendMode ColorBlendMode = GetColorBlendModeFromBlendMode(LayerOverride && LayerOverride->BlendMode.IsSet() ? LayerOverride->BlendMode.GetValue() : child->GetBlendMode());
            EOdysseyAlphaBlendMode AlphaBlendMode = GetAlphaBlendModeFromBlendMode(LayerOverride && LayerOverride->BlendMode.IsSet() ? LayerOverride->BlendMode.GetValue() : child->GetBlendMode());

            childrenRenderParams.Add(
                {
                    childRenderFunction,
                    ColorBlendMode,
                    (LayerOverride && LayerOverride->bInheritsAlpha.IsSet() ? LayerOverride->bInheritsAlpha.GetValue() : child->GetInheritsAlpha()) ? EOdysseyAlphaBlendMode::Back : AlphaBlendMode,
                    opacity
                }
            );
        }
    }

    if (childrenRenderParams.IsEmpty())
        return false;

    oRenderFunction = [childrenRenderParams](
            FRDGBuilder& iGraphBuilder,
            ERHIFeatureLevel::Type iFeatureLevel,
            FRDGTextureRef iDestinationTexture,
            const FIntRect& iSrcRect,
            const FIntRect& iDstRect,
            const FMatrix& iSrcTransform
        )
        {
            AddClearRenderTargetPass(iGraphBuilder, iDestinationTexture, FLinearColor::Transparent, iDstRect);

            FRDGTextureDesc desc = FRDGTextureDesc::Create2D(
                iDestinationTexture->Desc.Extent,
                iDestinationTexture->Desc.Format,
                FClearValueBinding::Transparent,
                ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable
            );

            FRDGTextureRef childTexture = iGraphBuilder.CreateTexture(desc, TEXT("UOdysseyLayerRoot::childTexture"));

            for ( const FChildRenderParams& childRenderParams : childrenRenderParams )
            {
                childRenderParams.RenderFunction(iGraphBuilder, iFeatureLevel, childTexture, iSrcRect, iDstRect, iSrcTransform);

                FOdysseyBlendShader::BlendRect(
                    iGraphBuilder,
                    iFeatureLevel,
                    iDestinationTexture,
                    childTexture,
                    iDestinationTexture,
                    iDstRect,
                    iDstRect,
                    FMatrix::Identity,
                    childRenderParams.BlendMode,
                    childRenderParams.AlphaMode,
                    childRenderParams.Opacity,
                    EOdysseyAntiAliasing::AnisotropicLinear
                );
            }
        };

    return true;
}

#if WITH_EDITOR

TArray<FName>
UOdysseyLayer::GetRows() const
{
    return { "Main", "Blend" };
}

int
UOdysseyLayer::GetRowHeight(FName iSubRowName) const
{
    return iSubRowName == "Main" || iSubRowName == "Blend" ? 20 : 0;
}

bool
UOdysseyLayer::IsRowVisible(FName iSubRowName) const
{
    return iSubRowName == "Main" || (iSubRowName == "Blend" && bDisplayOptions);
}

FMargin
UOdysseyLayer::GetRowPadding(FName iSubRowName) const
{
    return FMargin(0.f, 2.f, 0.f, 2.f);
}

void
UOdysseyLayer::PostTransacted(const FTransactionObjectEvent& iTransactionEvent)
{
    Super::PostTransacted(iTransactionEvent);

    if ( iTransactionEvent.GetEventType() != ETransactionObjectEventType::UndoRedo )
        return;

    const TArray<FName>& changedPropertyNames = iTransactionEvent.GetChangedProperties();

    if ( changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyLayer, Children)))
    {
        UOdysseyLayerStack* layerStack = GetLayerStack();
        if ( !layerStack )
            return;

        layerStack->HierarchyChanged();
    }

    if ( changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyLayer, bIsActivated) ) )
    {
        if (Parent)
            Parent->RenderingCompositionChanged();
    }

    if ( changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyLayer, Children)))
    {
        RenderingCompositionChanged();
    }

    if ( changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyLayer, LayerBlendMode))
        || changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyLayer, Opacity))
        || changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyLayer, bInheritsAlpha)))
    {
        RenderingChanged();
    }

}

#if WITH_EDITOR
bool
UOdysseyLayer::IsSelectedInEditor() const
{
    const USelection* Selection = OdysseyLayerStackSelection::Get();
    return IsValidChecked(this) && Selection && Selection->IsSelected(this);
}
#endif

UTexture2D*
UOdysseyLayer::CreateExportTexture( const FString& iAssetName, const FString& iPackagePath, UClass* iAssetClass, UFactory* iFactory )
{
    return GetLayerStack()->CreateExportTexture(iAssetName, iPackagePath, iAssetClass, iFactory);
}

#endif

/** Builds the static layer content without interpreting animation settings. */
bool
UOdysseyLayer::BuildRenderPipelineInternal(
    FRenderFunction& oRenderFunction,
    const TArray<const IOdysseyTextureRenderingAbility*>& iParents, const FOdysseyRenderContext* RenderContext
) const
{
    const FOdysseyRenderOverride* ContentOverride = RenderContext ? RenderContext->Find(*this) : nullptr;
    FRenderFunction RenderFunction;
    if (ContentOverride && ContentOverride->ContentReplacement)
    {
        RenderFunction = ContentOverride->ContentReplacement;
    }
    else if (!bCanHaveChildren || !BuildRenderChildrenPipeline(RenderFunction, iParents, RenderContext))
    {
        return false;
    }
    if (ContentOverride && ContentOverride->ContentDecorator)
    {
        RenderFunction = ContentOverride->ContentDecorator(MoveTemp(RenderFunction));
    }
    oRenderFunction = MoveTemp(RenderFunction);
    return bool(oRenderFunction);
}

TArray<FGuid>
UOdysseyLayer::GetRenderingComposition(uint64 iRenderType, int iFrame) const
{
    TArray<FGuid> Composition = { GetRenderingId() };
    for (UOdysseyLayer* Child : GetChildren())
    {
        if (Child->IsActivated())
        {
            Composition.Append(Child->GetRenderingComposition(iRenderType, iFrame));
        }
    }
    return Composition;
}
