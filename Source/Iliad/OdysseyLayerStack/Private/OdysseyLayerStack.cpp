// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyLayerStack.h"

#include "OdysseyLayer.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/ScopedSlowTask.h"
#include "OdysseyLayerStackFunctionLibrary.h"
#include "Misc/TransactionObjectEvent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "CanvasTypes.h"


UOdysseyLayerStack::UOdysseyLayerStack()
    : mCellSelection(MakeShared<FOdysseyLayerCellSelection>(this))
{
}

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
    if (SupportedLayerClasses.Contains(iClass))
        return true;

    return false;
}

FInt32Range
UOdysseyLayerStack::GetFrameRange() const
{
    return LayerRoot->GetFrameRange();
}

TSharedRef<FOdysseyLayerCellSelection>
UOdysseyLayerStack::GetCellSelection() const
{
    return mCellSelection;
}

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

    if (layersDuplicates.Num() != 0)
        SetCurrentLayer(layersDuplicates[0]);

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

    for(UOdysseyLayer* child : iLayer->GetChildren() )
    {
        CopyLayerInternal(child, duplicatedLayer, iLayer->GetChildren().Num());
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
    CurrentLayer = Layer;
    OnCurrentLayerChanged().Broadcast(this);
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

TArray<TSubclassOf<UOdysseyLayer>>
UOdysseyLayerStack::GetSupportedLayerClasses() const
{
    return SupportedLayerClasses;
}

UOdysseyLayer*
UOdysseyLayerStack::GetCurrentLayer() const
{
    return CurrentLayer;
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
UOdysseyLayerStack::IsSRGB() const
{
    return bIsSRGB;
}

void
UOdysseyLayerStack::SetIsSRGB(bool Value)
{
    bIsSRGB = Value;
    RenderingChanged();
}

void
UOdysseyLayerStack::RenderToTexture_RenderThread(FRDGBuilder& iGraphBuilder, FRDGTextureRef iDestinationTexture, ERHIFeatureLevel::Type iFeatureLevel, FFrameNumber iFrame, const FIntRect& iSrcRect, const FIntRect& iDstRect) const
{
    //FRDGTextureDesc desc = FRDGTextureDesc::Create2D(iDestinationTexture->Desc.Extent, PF_FloatRGBA, FClearValueBinding::Transparent, TexCreate_ShaderResource | TexCreate_RenderTargetable | NoTiling);
    //FRDGTextureRef renderTarget = graphBuilder.CreateTexture(desc, TEXT("OdysseyLayerStack::RenderTarget"));

    GetLayerRoot()->RenderToTexture_RenderThread(iGraphBuilder, iDestinationTexture, iFeatureLevel, iFrame, iSrcRect, iDstRect);

    /* if (!iRenderTarget)
        return;

    if (!mSurface)
        mSurface = MakeShared<FOdysseySurfaceTexture2DEditable>( GetWidth(), GetHeight(), ::ULIS::Format_BGRA8, SRGB ); //layerstack data are considered as SRGB

    TSharedPtr<IOdysseyImageRenderer> renderer = LayerRoot->BuildImageRenderer(EOdysseyRenderingType::Render, iFrame.Value);
    renderer->Init();

    FOdysseyImageRendererCopyParams params(mSurface->Block(), {iSrcRect}, ::ULIS::FVec2I(iPos.X, iPos.Y));
    renderer->Copy(params, {});

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(mSurface->Block()->Format());
    ctx.Finish();

    mSurface->Invalidate(::ULISUtils::ToULISRectIs({iSrcRect}));

    FTextureResource* srcResource = mSurface->Texture()->GetResource();
    double x = iDstRect.Min.X;
    double y = iDstRect.Min.Y;
    double w = iDstRect.Width();
    double h = iDstRect.Height();
    float u = float(iSrcRect.Min.X) / GetWidth();
    float v = float(iSrcRect.Min.Y) / GetHeight();
    float sizeU = float(iSrcRect.Max.X) / GetWidth();
    float sizeV = float(iSrcRect.Max.Y) / GetHeight();
    iCanvas->DrawTile(x, y, w, h, u, v, sizeU, sizeV, FLinearColor::Transparent, srcResource, SE_BLEND_Opaque);
    iCanvas->DrawTile(x, y, w, h, u, v, sizeU, sizeV, FLinearColor::White, srcResource, SE_BLEND_AlphaBlend);
    iCanvas->Flush_GameThread(true); */
}

TArray<FGuid>
UOdysseyLayerStack::GetRenderingComposition(EOdysseyRenderingType iRenderType, int iFrameIndex) const
{
    TArray<FGuid> idComposition = { GetRenderingId() };

    UOdysseyLayer* layerRoot = Cast<UOdysseyLayer>(LayerRoot);
    if ( !layerRoot )
        return idComposition;

    idComposition.Append(layerRoot->GetRenderingComposition(iRenderType, iFrameIndex));
    return idComposition;
}
