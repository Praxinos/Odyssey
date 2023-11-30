// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTextureLayerFolder.h"
#include "OdysseyTextureLayerStack.h"
#include "OdysseyTextureLayerImageRaster.h"
#include "OdysseyPixelFormat.h"
#include "EditorStyleSet.h"
#include "ULISEventBuilder.h"
#include "ULISLoaderModule.h"
#include "ULISUtils.h"

#define LOCTEXT_NAMESPACE "Texture"

UOdysseyTextureLayerFolder::FOnBlendModeChanged&
UOdysseyTextureLayerFolder::OnBlendModeChanged()
{
    static FOnBlendModeChanged onBlendModeChanged;
    return onBlendModeChanged;
}

UOdysseyTextureLayerFolder::FOnOpacityChanged&
UOdysseyTextureLayerFolder::OnOpacityChanged()
{
    static FOnOpacityChanged onOpacityChanged;
    return onOpacityChanged;
}

UOdysseyTextureLayerFolder::UOdysseyTextureLayerFolder()
{
	LayerTypeName = LOCTEXT("layer-folder.type", "Folder Layer");
    DefaultName = LOCTEXT("layer-folder.default-name", "Folder");
    Icon = *FEditorStyle::GetBrush( "ContentBrowser.AssetTreeFolderClosed" );
    IconExpanded = *FEditorStyle::GetBrush( "ContentBrowser.AssetTreeFolderOpen" );
    CanHaveChildren = true;
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

void
UOdysseyTextureLayerFolder::OpacityChanged()
{
    OnOpacityChanged().Broadcast(this);

    //TODO: react to interactive events by not commiting immediately
    ImageRenderingChanged();
}

void
UOdysseyTextureLayerFolder::BlendModeChanged()
{
    OnBlendModeChanged().Broadcast(this);

    ImageRenderingChanged();
}

void
UOdysseyTextureLayerFolder::PropertyChanged(const FName& iPropertyName)
{
    Super::PropertyChanged(iPropertyName);

    if (iPropertyName == "BlendMode")
        BlendModeChanged();
    if (iPropertyName == "Opacity")
        OpacityChanged();
}

::ULIS::eBlendMode
UOdysseyTextureLayerFolder::GetImageRenderingBlendMode() const
{
    return (::ULIS::eBlendMode)BlendMode;
}

float
UOdysseyTextureLayerFolder::GetImageRenderingOpacity() const
{
    return Opacity;
}

#undef LOCTEXT_NAMESPACE
