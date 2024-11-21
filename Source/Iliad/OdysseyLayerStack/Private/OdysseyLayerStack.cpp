// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyLayerStack.h"

#include "OdysseyLayer.h"
#include "OdysseyLayerStackImageRenderer.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/ScopedSlowTask.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyLayerStackFunctionLibrary.h"
#include "Misc/TransactionObjectEvent.h"

void
UOdysseyLayerStack::PostInitProperties()
{
    Super::PostInitProperties();

    if (HasAnyFlags(RF_ClassDefaultObject))
        return;

    if ( LayerRootClass )
        LayerRoot = NewObject<UOdysseyLayer>(this, LayerRootClass, NAME_None, RF_Public | RF_Transactional);
}

void
UOdysseyLayerStack::PostLoad()
{
    Super::PostLoad();

    const TArray<UOdysseyLayer*>& rootLayers = GetRootLayers();
    bool hasLayers = !rootLayers.IsEmpty();
    bool currentLayerIsInvalid = !CurrentLayer || !GetLayers().Contains(CurrentLayer);
    if (hasLayers && currentLayerIsInvalid)
    {
        CurrentLayer = rootLayers[0];
    }
}

//--- Delegates

UOdysseyLayerStack::FOnHierarchyChanged&
UOdysseyLayerStack::OnHierarchyChanged()
{
    static FOnHierarchyChanged onHierarchyChanged;
    return onHierarchyChanged;
}

UOdysseyLayerStack::FOnCurrentLayerChanged&
UOdysseyLayerStack::OnCurrentLayerChanged()
{
    static FOnCurrentLayerChanged onCurrentLayerChanged;
    return onCurrentLayerChanged;
}

bool
UOdysseyLayerStack::SupportsLayerClass(UClass* iClass) const
{
    if (CompatibleLayers.Contains(iClass))
        return true;

    return false;
}

//--- Layers management

void
UOdysseyLayerStack::CurrentLayerBlueprintSetter(UOdysseyLayer* Layer)
{
    FObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyLayerStack, CurrentLayer), Layer);
}

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

    if (!ParentLayer->CanHaveChildren || !ContainsLayer(ParentLayer))
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

    //Add the layer to the hierarchy
    AddLayersToHierarchy(layers, ParentLayer, IndexInParent);

    return layers;
}

void
UOdysseyLayerStack::RemoveLayer(UOdysseyLayer* Layer)
{
    //No Layer or not contained by the layerstack
    if (!Layer || !ContainsLayer(Layer))
        return;

    RemoveLayersFromHierarchy({Layer});
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

    RemoveLayersFromHierarchy(Layers);
}

UOdysseyLayer*
UOdysseyLayerStack::DuplicateLayer(UOdysseyLayer* Layer)
{
    //No Layer or not contained by the layerstack
    if(!Layer || !ContainsLayer(Layer))
        return nullptr;

    //Duplicate the layer
    UOdysseyLayer* layerDuplicate = CopyLayerInternal(Layer, Layer->Parent, Layer->Parent->Children.Find(Layer));

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
        UOdysseyLayer* layerCopy = CopyLayerInternal(layer, layer->Parent, layer->Parent->Children.Find(layer));
        layersDuplicates.Add(layerCopy);
    }

    if (layersDuplicates.Num() != 0)
        FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyLayerStack, CurrentLayer), layersDuplicates[0]);

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
    if (ParentLayer && (!ParentLayer->CanHaveChildren || !ContainsLayer(ParentLayer)) )
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
    if (ParentLayer && (!ParentLayer->CanHaveChildren || !ContainsLayer(ParentLayer)))
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
    UOdysseyLayer* parent = layersToMerge.Last()->Parent;
    int indexInParent = layersToMerge.Last()->GetIndexInParent();

    //Merge layers in the new layer
    mergedLayer->Merge(layersToMerge);
    AddLayersToHierarchy({ mergedLayer }, parent, indexInParent);

    //Remove merged layers from the hierarchy
    RemoveLayersFromHierarchy(layersToMerge);

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
    if (!ParentLayer->CanHaveChildren || !ContainsLayer(ParentLayer))
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
    if (!ParentLayer->CanHaveChildren || !ContainsLayer(ParentLayer))
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

    int oldIndex = Layer->Parent->Children.Find(Layer);
    if (Layer->Parent == ParentLayer && oldIndex == IndexInParent)
        return;

    bool bChangeParent = Layer->Parent != ParentLayer;

    if (bChangeParent)
    {
        FOdysseyObjectEditorUtils::PreChangePropertyValue(Layer, "Parent");
        FOdysseyObjectEditorUtils::PreChangePropertyValue(Layer->Parent, "Children");
    }
    FOdysseyObjectEditorUtils::PreChangePropertyValue(ParentLayer, "Children");

    int index = FMath::Clamp(IndexInParent, 0, ParentLayer->Children.Num());
    Layer->Parent->Children.Remove(Layer);
    ParentLayer->Children.Insert(Layer, (Layer->Parent == ParentLayer && oldIndex < index) ? index - 1 : index);
    Layer->Parent = ParentLayer;

    if (bChangeParent)
    {
        FOdysseyObjectEditorUtils::PostChangePropertyValue(Layer, "Parent", EPropertyChangeType::ValueSet);
        FOdysseyObjectEditorUtils::PostChangePropertyValue(Layer, "Children", EPropertyChangeType::ArrayRemove);
    }
    FOdysseyObjectEditorUtils::PostChangePropertyValue(ParentLayer, "Children", EPropertyChangeType::ArrayAdd);
}

void
UOdysseyLayerStack::MoveLayers(TArray<UOdysseyLayer*> Layers, UOdysseyLayer* ParentLayer, int IndexInParent)
{
    if ( !ParentLayer )
        ParentLayer = LayerRoot;

    //If the given parent can't have children or isn't contained in this layerstack
    if ( !ParentLayer->CanHaveChildren || !ContainsLayer(ParentLayer))
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

    TArray<UOdysseyLayer*> layersParentChanged;
    TArray<UOdysseyLayer*> layersChildrenChanged;

    layersChildrenChanged.AddUnique(ParentLayer);
    FOdysseyObjectEditorUtils::PreChangePropertyValue(ParentLayer, "Children");

    int index = FMath::Clamp(IndexInParent, 0, ParentLayer->Children.Num());
    for (UOdysseyLayer* layer : Layers)
    {
        UOdysseyLayer* oldParent = layer->Parent;
        bool bChangeParent = oldParent != ParentLayer;

        if (bChangeParent)
        {
            FOdysseyObjectEditorUtils::PreChangePropertyValue(layer, "Parent");
            FOdysseyObjectEditorUtils::PreChangePropertyValue(oldParent, "Children");
            layersParentChanged.AddUnique(layer);
            layersChildrenChanged.AddUnique(oldParent);
        }

        int oldIndex = oldParent->Children.Find(layer);
        if (!bChangeParent && oldIndex < index)
            index--;

        oldParent->Children.Remove(layer);
    }
    index = FMath::Clamp(IndexInParent, 0, ParentLayer->Children.Num());

    for (UOdysseyLayer* layer : Layers)
    {
        ParentLayer->Children.Insert(layer, index);
        layer->Parent = ParentLayer;
    }

    for (UOdysseyLayer* layer : layersParentChanged)
    {
        FOdysseyObjectEditorUtils::PostChangePropertyValue(layer, "Parent", EPropertyChangeType::ValueSet);
    }

    for (UOdysseyLayer* layer : layersChildrenChanged)
    {
        FOdysseyObjectEditorUtils::PostChangePropertyValue(layer, "Children", EPropertyChangeType::ArrayRemove);
    }

    FOdysseyObjectEditorUtils::PostChangePropertyValue(ParentLayer, "Children", EPropertyChangeType::ArrayAdd);
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
    return LayerRoot->Children;
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
    FString name = layer->DefaultName.ToString() + TEXT(" ") + FString::FromInt(GetLayers().Num() + 1);
    layer->Name = FText::FromString(name);

    return layer;
}

void
UOdysseyLayerStack::AddLayersToHierarchy(TArray<UOdysseyLayer*> iLayers, UOdysseyLayer* iParent, int iIndexInParent)
{
    //Call propertyPreChange in a stable state of the layerstack
    FOdysseyObjectEditorUtils::PreChangePropertyValue(iParent, "Children");
    for ( UOdysseyLayer* layer : iLayers )
    {
        FOdysseyObjectEditorUtils::PreChangePropertyValue(layer, "Parent");
    }

    for (UOdysseyLayer* layer : iLayers)
    {
        layer->Children.Empty();
    }

    //Add Layers to parent's children
    iParent->Children.Insert(iLayers, FMath::Clamp(iIndexInParent, 0, iParent->Children.Num()));

    for ( UOdysseyLayer* layer : iLayers )
    {
        //Make sure layer is accessible in the hierarchy map
        layer->Parent = iParent;
    }

    //Call propertyPostChange in a stable state of the layerstack
    FOdysseyObjectEditorUtils::PostChangePropertyValue(iParent, "Children", EPropertyChangeType::ArrayAdd);
    for ( UOdysseyLayer* layer : iLayers )
    {
        FOdysseyObjectEditorUtils::PostChangePropertyValue(layer, "Parent", EPropertyChangeType::ValueSet);
    }
}

void
UOdysseyLayerStack::GetLayersUniqueParents(TArray<UOdysseyLayer*> iLayers, TArray<UOdysseyLayer*>& oParents)
{
    for (UOdysseyLayer* layer : iLayers)
    {
        oParents.AddUnique(layer->Parent);
    }
}

void
UOdysseyLayerStack::RemoveLayersFromHierarchy(TArray<UOdysseyLayer*> iLayers)
{
    TArray<UOdysseyLayer*> parents;
    TArray<UOdysseyLayer*> layersToRemove = UOdysseyLayerStackFunctionLibrary::FilterTopmostLayers(iLayers);
    GetLayersUniqueParents(layersToRemove, parents);

    //Call propertyPreChange in a stable state of the layerstack
    for ( UOdysseyLayer* layer : layersToRemove )
        FOdysseyObjectEditorUtils::PreChangePropertyValue(layer, "Parent");

    for ( UOdysseyLayer* parent : parents)
        FOdysseyObjectEditorUtils::PreChangePropertyValue(parent, "Children");

    for ( UOdysseyLayer* layer : layersToRemove )
        layer->Parent = nullptr;

    for ( UOdysseyLayer* parent : parents )
        for ( UOdysseyLayer* layer : layersToRemove )
            parent->Children.Remove(layer);

    //Call propertyPostChange in a stable state of the layerstack
    for ( UOdysseyLayer* layer : layersToRemove )
        FOdysseyObjectEditorUtils::PostChangePropertyValue(layer, "Parent", EPropertyChangeType::ValueSet);

    for ( UOdysseyLayer* parent : parents )
        FOdysseyObjectEditorUtils::PostChangePropertyValue(parent, "Children", EPropertyChangeType::ArrayRemove);
}

UOdysseyLayer*
UOdysseyLayerStack::CopyLayerInternal(UOdysseyLayer* iLayer, UOdysseyLayer* iParent , int iIndexInParent)
{
    FObjectDuplicationParameters params(iLayer, this);
    UOdysseyLayer* duplicatedLayer = Cast<UOdysseyLayer>(StaticDuplicateObjectEx(params));
    if (!duplicatedLayer)
        return nullptr;

    AddLayersToHierarchy({duplicatedLayer}, iParent, iIndexInParent);

    for(UOdysseyLayer* child : iLayer->Children )
    {
        CopyLayerInternal(child, duplicatedLayer, iLayer->Children.Num());
    }

    return duplicatedLayer;
}

//--- UObject overrides

void
UOdysseyLayerStack::HierarchyChanged()
{
    OnHierarchyChanged().Broadcast(this);
}

void
UOdysseyLayerStack::CurrentLayerChanged()
{
    OnCurrentLayerChanged().Broadcast(this);
}

void
UOdysseyLayerStack::PropertyChanged(const FName& iPropertyName)
{
    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyLayerStack, CurrentLayer) )
        CurrentLayerChanged();
}

void
UOdysseyLayerStack::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent)
{
    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    PropertyChanged(PropertyChangedEvent.GetPropertyName());
}

void
UOdysseyLayerStack::PostTransacted(const FTransactionObjectEvent& iTransactionEvent)
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


TSharedPtr<IOdysseyImageRenderer>
UOdysseyLayerStack::BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame, FImageRendererFilter iFilter) const
{
    if (iFilter.IsBound() && !iFilter.Execute(this))
        return nullptr;

    return MakeShared<FOdysseyLayerStackImageRenderer>(this, iFrame, iRenderType, GetImageRenderingRects(), iFilter);
}

TArray<FGuid>
UOdysseyLayerStack::GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex) const
{
    TArray<FGuid> idComposition = { GetImageRenderingId() };

    UOdysseyLayer* layerRoot = Cast<UOdysseyLayer>(LayerRoot);
    if ( !layerRoot )
        return idComposition;

    idComposition.Append(layerRoot->GetImageRenderingComposition(iRenderType, iFrameIndex));
    return idComposition;
}
