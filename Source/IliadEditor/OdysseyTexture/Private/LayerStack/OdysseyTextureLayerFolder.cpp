// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTextureLayerFolder.h"
#include "OdysseyLayerStack.h"

#define LOCTEXT_NAMESPACE "Texture"

UOdysseyTextureLayerFolder::UOdysseyTextureLayerFolder()
{
    LayerTypeName = LOCTEXT("layer-folder.type", "Folder Layer");
    DefaultName = LOCTEXT("layer-folder.default-name", "Folder");
    Icon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "ContentBrowser.AssetTreeFolderClosed");
    IconExpanded = FSlateIcon(FAppStyle::GetAppStyleSetName(), "ContentBrowser.AssetTreeFolderOpen");
    bCanHaveChildren = true;
}

TSet<UClass*>
UOdysseyTextureLayerFolder::GetMergeDefaultLayerTypes() const
{
    TSet<UClass*> types;
    UOdysseyLayerStack* layerStack = GetLayerStack();
    if (!layerStack)
        return types;

    return layerStack->FindLayersMergeTypes(Children);
}

TSet<UClass*>
UOdysseyTextureLayerFolder::GetMergeLayerTypesFromTypes(TSet<UClass*> iLayerTypes) const
{
    TSet<UClass*> types;
    UOdysseyLayerStack* layerStack = GetLayerStack();
    if (!layerStack)
        return types;

    for (UOdysseyLayer* layer : Children )
    {
        TSet<UClass*> layerTypes = layer->GetMergeLayerTypesFromTypes(iLayerTypes);
        types.Append(layerTypes);
    }

    return types;
}

#undef LOCTEXT_NAMESPACE
