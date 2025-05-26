// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "LayerFolder/OdysseyAnimationLayerFolder.h"
#include "OdysseyLayerStack.h"

#if WITH_EDITOR
#include "EditorStyleSet.h"
#endif

#define LOCTEXT_NAMESPACE "Animation"

UOdysseyAnimationLayerFolder::UOdysseyAnimationLayerFolder()
{
    LayerTypeName = LOCTEXT("layer-folder.type", "Folder Layer");
    DefaultName = LOCTEXT("layer-folder.default-name", "Folder");
    bCanHaveChildren = true;

#if WITH_EDITOR
    Icon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "ContentBrowser.AssetTreeFolderClosed");
    IconExpanded = FSlateIcon(FAppStyle::GetAppStyleSetName(), "ContentBrowser.AssetTreeFolderOpen");
    bHasLighttable = false;
#endif
}

#if WITH_EDITOR
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
#endif

#undef LOCTEXT_NAMESPACE
