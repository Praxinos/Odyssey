// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyLayerStack.h"

#include "OdysseyLayer.h"
#include "OdysseyLayerRoot.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/ScopedSlowTask.h"
#include "OdysseyLayerStackFunctionLibrary.h"
#include "Misc/TransactionObjectEvent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "CanvasTypes.h"

UOdysseyLayerStack::~UOdysseyLayerStack()
{
}

UOdysseyLayerStack::UOdysseyLayerStack()
#if WITH_EDITOR
    : mCellSelection(MakeShared<FOdysseyLayerCellSelection>(this))
    , mLayerSelection( nullptr )
#endif
{
}

void
UOdysseyLayerStack::PostInitProperties()
{
    Super::PostInitProperties();

    if (HasAnyFlags(RF_ClassDefaultObject))
        return;

    LayerRoot = NewObject<UOdysseyLayerRoot>(this, NAME_None, RF_Public | RF_Transactional);
}

void
UOdysseyLayerStack::PostLoad()
{
    Super::PostLoad();

#if WITH_EDITOR
    const TArray<UOdysseyLayer*>& rootLayers = GetRootLayers();
    bool hasLayers = !rootLayers.IsEmpty();
    bool currentLayerIsInvalid = !CurrentLayer || !GetLayers().Contains(CurrentLayer);
    if (hasLayers && currentLayerIsInvalid)
    {
        CurrentLayer = rootLayers[0];
    }

    mLayerSelection = USelection::CreateObjectSelection(GetTransientPackage(), NAME_None, RF_Transactional);
    mLayerSelection->SetElementSelectionSet(NewObject<UTypedElementSelectionSet>(mLayerSelection, NAME_None, RF_Transactional));
    mLayerSelection->AddToRoot();
#endif
}

//--- Delegates

UOdysseyLayerStack::FOnHierarchyChanged&
UOdysseyLayerStack::OnHierarchyChanged()
{
    static FOnHierarchyChanged onHierarchyChanged;
    return onHierarchyChanged;
}


#if WITH_EDITOR
UOdysseyLayerStack::FOnCurrentLayerChanged&
UOdysseyLayerStack::OnCurrentLayerChanged()
{
    static FOnCurrentLayerChanged onCurrentLayerChanged;
    return onCurrentLayerChanged;
}
#endif

bool
UOdysseyLayerStack::SupportsLayerClass(UClass* iClass) const
{
    if (SupportedLayerClasses.Contains(iClass))
        return true;

    return false;
}

FInt32Range
UOdysseyLayerStack::GetFrameRange() const
{
    return LayerRoot->GetFrameRange();
}


#if WITH_EDITOR
TSharedRef<FOdysseyLayerCellSelection>
UOdysseyLayerStack::GetCellSelection() const
{
    return mCellSelection;
}
#endif

//--- Layers management

UOdysseyLayer*
UOdysseyLayerStack::AddLayer(TSubclassOf<UOdysseyLayer> LayerType, UOdysseyLayer* ParentLayer, int IndexInParent)
{
    TArray<UOdysseyLayer*> layers = AddLayers(LayerType, ParentLayer, IndexInParent, 1);
    if (layers.IsEmpty())
        return nullptr;

    return layers[0];
}

TArray<UOdysseyLayer*>
UOdysseyLayerStack::AddLayers(TSubclassOf<UOdysseyLayer> LayerType, UOdysseyLayer* ParentLayer, int IndexInParent, int Count)
{
    UClass* layerType = LayerType.Get();

    //No LayerType
    if ( !layerType )
        return {};

    //LayerType Not supported
    if (!SupportsLayerClass(layerType))
        return {};

    //If the given parent can't have children or isn't contained in this layerstack
    if (!ParentLayer)
        ParentLayer = LayerRoot;

    if (!ParentLayer->CanHaveChildren() || !ContainsLayer(ParentLayer))
        return {};

    //Create the Layer
    TArray<UOdysseyLayer*> layers;
    for (int i = 0; i < Count; i++)
    {
        UOdysseyLayer* layer = CreateLayer(LayerType);
        if (!layer )
            return {};

        layers.Add(layer);
    }

    ParentLayer->AddChildren(layers, IndexInParent);

    return layers;
}

void
UOdysseyLayerStack::RemoveLayer(UOdysseyLayer* Layer)
{
    //No Layer or not contained by the layerstack
    if (!Layer || !ContainsLayer(Layer))
        return;

    Layer->GetParent()->RemoveChild(Layer);
}

void
UOdysseyLayerStack::RemoveLayers(TArray<UOdysseyLayer*> Layers)
{
    //Sanitize Layers array
    Layers.RemoveAll(
        [this](const UOdysseyLayer* iLayer)
        {
            return !iLayer || !ContainsLayer(iLayer);
        }
    );

    //No Layers
    if (Layers.Num() <= 0)
        return;

    for (UOdysseyLayer* layer : Layers)
    {
        //Remove the layer
        layer->GetParent()->RemoveChild(layer);
    }
}

UOdysseyLayer*
UOdysseyLayerStack::DuplicateLayer(UOdysseyLayer* Layer)
{
    //No Layer or not contained by the layerstack
    if(!Layer || !ContainsLayer(Layer))
        return nullptr;

    //Duplicate the layer
    UOdysseyLayer* layerDuplicate = CopyLayerInternal(Layer, Layer->GetParent(), Layer->GetParent()->GetChildren().Find(Layer));

    return layerDuplicate;
}

TArray<UOdysseyLayer*>
UOdysseyLayerStack::DuplicateLayers(TArray<UOdysseyLayer*> Layers)
{
    TArray<UOdysseyLayer*> layersDuplicates;

    //Sanitize Layers array
    Layers.RemoveAll(
        [this](const UOdysseyLayer* iLayer)
        {
            return !iLayer || !ContainsLayer(iLayer);
        }
    );

    //No Layers
    if (Layers.Num()<= 0)
        return layersDuplicates;

    Layers.Sort(
        [this](UOdysseyLayer& iLayerA, UOdysseyLayer& iLayerB)
        {
            //true => iLayerA is before iLayerB
            int depthA = iLayerA.GetParents().Num();
            int depthB = iLayerB.GetParents().Num();

            if (depthA < depthB)
                return true;

            if (depthA == depthB)
            {
                int indexA = iLayerA.GetIndexInParent();
                int indexB = iLayerB.GetIndexInParent();

                return indexA < indexB;
            }

            return false;
        }
    );


    for (UOdysseyLayer* layer : Layers)
    {
        //Duplicate the layer
        UOdysseyLayer* layerCopy = CopyLayerInternal(layer, layer->GetParent(), layer->GetParent()->GetChildren().Find(layer));
        layersDuplicates.Add(layerCopy);
    }

#if WITH_EDITOR
    if (layersDuplicates.Num() != 0)
        SetCurrentLayer(layersDuplicates[0]);
#endif

    return layersDuplicates;
}

UOdysseyLayer*
UOdysseyLayerStack::CopyLayer(UOdysseyLayer* Layer, UOdysseyLayer* ParentLayer, int IndexInParent)
{
    //No Layer
    if(!Layer)
        return nullptr;

    if (!ParentLayer)
        ParentLayer = LayerRoot;

    //If the given parent can't have children or isn't contained in this layerstack
    if (ParentLayer && (!ParentLayer->CanHaveChildren() || !ContainsLayer(ParentLayer)) )
        return nullptr;

    //Duplicate the layer
    UOdysseyLayer* layerCopy = CopyLayerInternal(Layer, ParentLayer, IndexInParent);
    return layerCopy;
}

TArray<UOdysseyLayer*>
UOdysseyLayerStack::CopyLayers(TArray<UOdysseyLayer*> Layers, UOdysseyLayer* ParentLayer, int IndexInParent)
{
    TArray<UOdysseyLayer*> layerCopies;
    if (!ParentLayer)
        ParentLayer = LayerRoot;

    //If the given parent can't have children or isn't contained in this layerstack
    if (ParentLayer && (!ParentLayer->CanHaveChildren() || !ContainsLayer(ParentLayer)))
        return layerCopies;

    //Sanitize Layers array
    Layers.RemoveAll(
        [this](const UOdysseyLayer* iLayer)
        {
            return !iLayer;
        }
    );

    //No Layers
    if (Layers.Num()<= 0)
        return layerCopies;

    Layers.Sort(
        [this](UOdysseyLayer& iLayerA, UOdysseyLayer& iLayerB)
        {
            //true => iLayerA is before iLayerB
            int depthA = iLayerA.GetParents().Num();
            int depthB = iLayerB.GetParents().Num();

            if (depthA > depthB)
                return true;

            if (depthA == depthB)
            {
                int indexA = iLayerA.GetIndexInParent();
                int indexB = iLayerB.GetIndexInParent();

                return indexA > indexB;
            }

            return false;
        }
    );

    for (UOdysseyLayer* layer : Layers)
    {
        //Duplicate the layer
        UOdysseyLayer* layerCopy = CopyLayerInternal(layer, ParentLayer, IndexInParent);
        layerCopies.Insert(layerCopy, 0);
    }

    return layerCopies;
}

TSet<UClass*>
UOdysseyLayerStack::FindLayersMergeTypes(TArray<UOdysseyLayer*> iLayers)
{
    TSet<UClass*> mergeTypes;
    if ( iLayers.Num() <= 0 )
        return mergeTypes;

    //first we ask every layer what are their default merge types
    TSet<UClass*> tmpMergeTypes;
    for (UOdysseyLayer* layer : iLayers)
    {
        TSet<UClass*> layerTypes = layer->GetMergeDefaultLayerTypes();
        tmpMergeTypes.Append(layerTypes);
    }

    //Then we ask each layer what would be their merge types if merged with all the previous merge types
    //This is where we can do things like Vector merged with Raster results with a Raster layer only
    for (UOdysseyLayer* layer : iLayers)
    {
        TSet<UClass*> layerTypes = layer->GetMergeLayerTypesFromTypes(tmpMergeTypes);
        mergeTypes.Append(layerTypes);
    }

    return mergeTypes;
}

bool
UOdysseyLayerStack::CanMergeLayers(TArray<UOdysseyLayer*> iLayers)
{
    if (iLayers.Num() < 2)
        return false;

    //Get only topmost selected layers
    TArray<UOdysseyLayer*> layersToMerge = UOdysseyLayerStackFunctionLibrary::FilterTopmostLayers(iLayers);
    if (layersToMerge.Num() <= 0) //We can have a single layer here if we selected a folder layer and one of its children, and it still works
        return false;

    return FindLayersMergeTypes(layersToMerge).Num() == 1;
}

UOdysseyLayer*
UOdysseyLayerStack::MergeLayers(TArray<UOdysseyLayer*> iLayers)
{
    //Get only topmost selected layers
    TArray<UOdysseyLayer*> layersToMerge = UOdysseyLayerStackFunctionLibrary::FilterTopmostLayers(iLayers);
    if (layersToMerge.Num() <= 0) //We can have a single layer here if we selected a folder layer and one of its children, and it still works
        return nullptr;

    TSet<UClass*> layerMergeTypes = FindLayersMergeTypes(layersToMerge);
    if (layerMergeTypes.Num() != 1)
        return nullptr;

    UClass* layerMergeType = layerMergeTypes.Array()[0];
    if (!layerMergeType)
        return nullptr;

    //sort from bottom to top
    layersToMerge = UOdysseyLayerStackFunctionLibrary::SortLayers(layersToMerge, true);
    if (layersToMerge.Num() <= 0) //We can have a single layer here if we selected a folder layer and one of its children, and it still works
        return nullptr;

    //Create the Layer
    UOdysseyLayer* mergedLayer = CreateLayer(layerMergeType);
    if (!mergedLayer )
        return nullptr;

    //Add the layer to the hierarchy
    UOdysseyLayer* parent = layersToMerge.Last()->GetParent();
    int indexInParent = layersToMerge.Last()->GetIndexInParent();

    //Merge layers in the new layer
    mergedLayer->Merge(layersToMerge);

    parent->AddChild(mergedLayer, indexInParent);
    parent->RemoveChildren(layersToMerge);

    return mergedLayer;
}

bool
UOdysseyLayerStack::CanMoveLayer(UOdysseyLayer* Layer, UOdysseyLayer* ParentLayer) const
{
    //No Layer or not contained by the layerstack
    if(!Layer || !ContainsLayer(Layer))
        return false;

    if ( !ParentLayer )
        ParentLayer = LayerRoot;

    //If the given parent can't have children or isn't contained in this layerstack
    if (!ParentLayer->CanHaveChildren() || !ContainsLayer(ParentLayer))
        return false;

    //Does Layer contain Parent Layer
    if (Layer == ParentLayer || ParentLayer->IsChildOf(Layer))
        return false;

    return true;
}

bool
UOdysseyLayerStack::CanMoveLayers(TArray<UOdysseyLayer*> Layers, UOdysseyLayer* ParentLayer) const
{
    if ( !ParentLayer )
        ParentLayer = LayerRoot;

    //If the given parent can't have children or isn't contained in this layerstack
    if (!ParentLayer->CanHaveChildren() || !ContainsLayer(ParentLayer))
        return false;

    //Sanitize Layers array
    Layers.RemoveAll(
        [this, ParentLayer](UOdysseyLayer* iLayer)
        {
            if(!iLayer || !ContainsLayer(iLayer))
                return true;

            if (iLayer == ParentLayer || ParentLayer->IsChildOf(iLayer))
                return true;

            return false;
        }
    );

    //No Layers
    if (Layers.Num()<= 0)
        return false;

    return true;
}

void
UOdysseyLayerStack::MoveLayer(UOdysseyLayer* Layer, UOdysseyLayer* ParentLayer, int IndexInParent)
{
    if ( !CanMoveLayer(Layer, ParentLayer) )
        return;

    if ( !ParentLayer )
        ParentLayer = LayerRoot;

    UOdysseyLayer* oldParent = Layer->GetParent();
    int oldIndex = oldParent->GetChildren().Find(Layer);
    if (oldParent == ParentLayer && oldIndex == IndexInParent)
        return;

    int index = FMath::Clamp(IndexInParent, 0, ParentLayer->GetChildren().Num());
    oldParent->RemoveChild(Layer);
    ParentLayer->AddChild(Layer, (oldParent == ParentLayer && oldIndex < index) ? index - 1 : index);
}

void
UOdysseyLayerStack::MoveLayers(TArray<UOdysseyLayer*> Layers, UOdysseyLayer* ParentLayer, int IndexInParent)
{
    if ( !ParentLayer )
        ParentLayer = LayerRoot;

    //If the given parent can't have children or isn't contained in this layerstack
    if ( !ParentLayer->CanHaveChildren() || !ContainsLayer(ParentLayer))
        return;

    //Sanitize Layers array
    Layers.RemoveAll(
        [this, ParentLayer](UOdysseyLayer* iLayer)
        {
            if ( !iLayer || !ContainsLayer(iLayer) )
                return true;

            if ( iLayer == ParentLayer || ParentLayer->IsChildOf(iLayer) )
                return true;

            return false;
        }
    );

    //Sort Layers in reverse depth order to ease the insertion of layers in new parent later on
    Layers.Sort(
        [this](UOdysseyLayer& iLayerA, UOdysseyLayer& iLayerB)
        {
            //true => iLayerA is before iLayerB
            int depthA = iLayerA.GetParents().Num();
            int depthB = iLayerB.GetParents().Num();

            if (depthA > depthB)
                return true;

            if (depthA == depthB)
            {
                int indexA = iLayerA.GetIndexInParent();
                int indexB = iLayerB.GetIndexInParent();

                return indexA > indexB;
            }

            return false;
        }
    );

    //No Layers
    if ( Layers.Num() <= 0 )
        return;

    int index = FMath::Clamp(IndexInParent, 0, ParentLayer->GetChildren().Num());
    for (UOdysseyLayer* layer : Layers)
    {
        UOdysseyLayer* oldParent = layer->GetParent();
        bool bChangeParent = oldParent != ParentLayer;
        int oldIndex = oldParent->GetChildren().Find(layer);
        if (!bChangeParent && oldIndex < index)
            index--;

        oldParent->RemoveChild(layer);
    }
    index = FMath::Clamp(IndexInParent, 0, ParentLayer->GetChildren().Num());
;
    ParentLayer->AddChildren(Layers, index);
}

bool
UOdysseyLayerStack::ContainsLayer(const UOdysseyLayer* Layer) const
{
    if (!Layer)
        return false;

    if ( Layer == LayerRoot )
        return true;

    return GetLayers().Contains(Layer);
}

const TArray<UOdysseyLayer*>&
UOdysseyLayerStack::GetRootLayers() const
{
    return LayerRoot->GetChildren();
}

TArray<UOdysseyLayer*>
UOdysseyLayerStack::GetLayers() const
{
    return LayerRoot->GetChildrenRecursively();
}

UOdysseyLayer*
UOdysseyLayerStack::CreateLayer(UClass* iLayerType)
{
    //Create the Layer
    UOdysseyLayer* layer = NewObject<UOdysseyLayer>(this, iLayerType, NAME_None, RF_Public | RF_Transactional);
    if (!layer )
        return nullptr;

    //Name the layer
    FString name = layer->GetDefaultName().ToString() + TEXT(" ") + FString::FromInt(GetLayers().Num() + 1);
    layer->SetLayerName(FText::FromString(name));

    return layer;
}

void
UOdysseyLayerStack::GetLayersUniqueParents(TArray<UOdysseyLayer*> iLayers, TArray<UOdysseyLayer*>& oParents)
{
    for (UOdysseyLayer* layer : iLayers)
    {
        oParents.AddUnique(layer->GetParent());
    }
}

UOdysseyLayer*
UOdysseyLayerStack::CopyLayerInternal(UOdysseyLayer* iLayer, UOdysseyLayer* iParent , int iIndexInParent)
{
    FObjectDuplicationParameters params(iLayer, this);
    UOdysseyLayer* duplicatedLayer = Cast<UOdysseyLayer>(StaticDuplicateObjectEx(params));
    if (!duplicatedLayer)
        return nullptr;

    iParent->AddChild(duplicatedLayer, iIndexInParent);

    // The StaticDuplicateObjectEx() will duplicate the original layer WITHOUT duplicated its Children (only copying pointers)
    //
    // The UE mecanism of duplication:
    // - the original object is always duplicated
    // - all objects inside the original object is also duplicated (and then recursively) ONLY if the Outer of the inner object has already been duplicated
    //
    // Now, as the Outer of ALL layers are always the layer stack
    // (the layer hierarchy is managed by the Parent member)
    // the duplication of Children layers is not processed because the layer stack is never duplicated
    //
    // layer-folder:        Parent = layer_root         outer = layerstack
    //    layer1:           Parent = layer-folder       outer = layerstack
    //    layer2:           Parent = layer-folder       outer = layerstack
    //    layer3:           Parent = layer-folder       outer = layerstack
    //    layer-folderX:    Parent = layer-folder       outer = layerstack
    //        layer8:       Parent = layer-folderX      outer = layerstack
    //        layer9:       Parent = layer-folderX      outer = layerstack
    //
    // All layers inside a layer folder are not duplicated (just "pointer-copied"), because the Outer of the Children layers is NOT the duplicated layer (it's the layerstack)
    //
    // layerX: outer = layerstack
    //    cell1: outer = layerX
    //    cell2: outer = layerX
    //    ...
    //    cellN: outer = layerX
    //
    // All cells inside a layer are automatically duplicated when duplicating the layer, because the Outer of cells (a layer) has been already duplicated
    //
    // To manage this case, the children of a duplicated layer must be emptied
    // But it's not easily doable here as duplicatedLayer->RemoveChildren() will:
    // - empty the Children array: OK
    // - also reset the Parent of the Children layers: NOT OK, as the Parent of all children are still the ORIGINAL layer (because Children are only "pointer-copied")
    //
    // So the best way to solve this, is inside PostDuplicate() of layer which just empty the Children array
    // ...\Plugins\Odyssey\Source\Iliad\OdysseyLayerStack\Private\OdysseyLayer.cpp
    //
    // UPDATE:
    // if the children are only empty inside PostDuplicate(), when duplicate the asset, the duplicated layer root will be just empty
    // So, no other way to recursively duplicate the layer hierarchy inside PostDuplicate() and no more here
    //
    // BEST WAY:
    // Another way to natively manage this case would be to remove Parent member and manage the layer hierarchy via the Outer
    // https://github.com/Praxinos/Odyssey-Plugin/issues/585

    return duplicatedLayer;
}

#if WITH_EDITOR
bool
UOdysseyLayerStack::IsLayerSelected( const UOdysseyLayer* iLayer ) const
{
    return mLayerSelection->IsSelected( iLayer );
}

void
UOdysseyLayerStack::SelectLayer( UOdysseyLayer* iLayer )
{
    mLayerSelection->Select( iLayer );
}

void
UOdysseyLayerStack::DeselectAllLayers()
{
    mLayerSelection->DeselectAll();
}

void
UOdysseyLayerStack::DeselectLayer( UOdysseyLayer* iLayer )
{
    mLayerSelection->Deselect( iLayer );
}
#endif

//--- UObject overrides

void
UOdysseyLayerStack::HierarchyChanged()
{
    OnHierarchyChanged().Broadcast(this);
}

#if WITH_EDITOR
void
UOdysseyLayerStack::PostTransacted(const FTransactionObjectEvent& iTransactionEvent)
{
    Super::PostTransacted(iTransactionEvent);

    if ( iTransactionEvent.GetEventType() != ETransactionObjectEventType::UndoRedo )
        return;

    const TArray<FName>& changedPropertyNames = iTransactionEvent.GetChangedProperties();
    if (changedPropertyNames.Contains(GET_MEMBER_NAME_CHECKED(UOdysseyLayerStack, CurrentLayer)))
        OnCurrentLayerChanged().Broadcast(this);
}

void
UOdysseyLayerStack::SetCurrentLayer(UOdysseyLayer* Layer)
{
    Modify();
    CurrentLayer = Layer;
    OnCurrentLayerChanged().Broadcast(this);
}

UOdysseyLayer*
UOdysseyLayerStack::GetCurrentLayer() const
{
    return CurrentLayer;
}

void
UOdysseyLayerStack::SetTimelineSplitterPosition(float iValue)
{
    TimelineSplitterPosition = iValue;
}

float
UOdysseyLayerStack::GetTimelineSplitterPosition() const
{
    return TimelineSplitterPosition;
}
#endif

TArray<TSubclassOf<UOdysseyLayer>>
UOdysseyLayerStack::GetSupportedLayerClasses() const
{
    return SupportedLayerClasses;
}

UOdysseyLayer*
UOdysseyLayerStack::GetLayerRoot() const
{
    return LayerRoot;
}

TSubclassOf<UOdysseyLayer>
UOdysseyLayerStack::GetLayerRootClass() const
{
    return LayerRootClass;
}

bool
UOdysseyLayerStack::BuildRenderPipelineInternal(
    FFrameNumber iFrame,
    uint64 iType,
    IOdysseyTextureRenderingAbility::FRenderFunction& oRenderFunction,
    const IOdysseyTextureRenderingAbility::FCanRenderFunction& iCanRenderFunction,
    const TArray<const IOdysseyTextureRenderingAbility*>& iParents
) const
{
    return GetLayerRoot()->BuildRenderPipeline(iFrame, iType, oRenderFunction, iCanRenderFunction, iParents);
}

TArray<FGuid>
UOdysseyLayerStack::GetRenderingComposition(uint64 iRenderType, int iFrameIndex) const
{
    TArray<FGuid> idComposition = { GetRenderingId() };

    UOdysseyLayer* layerRoot = Cast<UOdysseyLayer>(LayerRoot);
    if ( !layerRoot )
        return idComposition;

    idComposition.Append(layerRoot->GetRenderingComposition(iRenderType, iFrameIndex));
    return idComposition;
}

UTextureRenderTarget2D*
UOdysseyLayerStack::CreateRenderingRenderTarget() const
{
    checkf(false, TEXT("Must be implemented by child class"));
    return nullptr;
}

#if WITH_EDITOR

UTexture2D*
UOdysseyLayerStack::CreateExportTexture(UObject* Outer, FName Name, EObjectFlags Flags)
{
    checkf(false, TEXT("Must be implemented by child class"));
    return nullptr;
}

#endif
