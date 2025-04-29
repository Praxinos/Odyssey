// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerFolder/OdysseyAnimationLayerFolder.h"
#include "OdysseyLayerStack.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "Animation"

UOdysseyAnimationLayerFolder::UOdysseyAnimationLayerFolder()
{
    LayerTypeName = LOCTEXT("layer-folder.type", "Folder Layer");
    DefaultName = LOCTEXT("layer-folder.default-name", "Folder");

    Icon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "ContentBrowser.AssetTreeFolderClosed");
    IconExpanded = FSlateIcon(FAppStyle::GetAppStyleSetName(), "ContentBrowser.AssetTreeFolderOpen");
    CanHaveChildren = true;
    HasLighttable = false;
}

TSet<UClass*>
UOdysseyAnimationLayerFolder::GetMergeDefaultLayerTypes() const
{
    TSet<UClass*> types;
    UOdysseyLayerStack* layerStack = GetLayerStack();
    if (!layerStack)
        return types;

    return layerStack->FindLayersMergeTypes(Children);
}

TSet<UClass*>
UOdysseyAnimationLayerFolder::GetMergeLayerTypesFromTypes(TSet<UClass*> iLayerTypes) const
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
