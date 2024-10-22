// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Layers/LayerFolder/OdysseyAnimationLayerFolder.h"
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

#ifdef WITH_EDITOR

FOptionalSize
UOdysseyAnimationLayerFolder::GetRowHeight(FName iSubRowName) const
{
	if (iSubRowName == "Lighttable")
		return 0; //No lighttable visible for the folder
		
	return UOdysseyLayer::GetRowHeight(iSubRowName);
}

#endif

#undef LOCTEXT_NAMESPACE
