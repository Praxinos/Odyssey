// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyTextureLayerStackTreeView.h"

#include "TextureEditor/OdysseyTextureEditorExtension.h"
#include "LayerStack/OdysseyTextureLayerImageRaster.h"
#include "LayerStack/OdysseyTextureLayerStack.h"

#define LOCTEXT_NAMESPACE "TextureEditor"

SOdysseyTextureLayerStackTreeView::SOdysseyTextureLayerStackTreeView()
    : mExtension(nullptr)
{
}

FOdysseyTextureEditorExtension*
SOdysseyTextureLayerStackTreeView::GetTextureEditorExtension() const
{
    return mExtension;
}

void
SOdysseyTextureLayerStackTreeView::Construct(const FArguments& InArgs, FOdysseyTextureEditorExtension* iTextureExtension)
{
    SOdysseyLayerStackTreeView::Construct(InArgs);
    mExtension = iTextureExtension;
}

TArray<TSharedPtr<FExtender>>
SOdysseyTextureLayerStackTreeView::ExtendContextMenu()
{
    //TODO:
    // Create a Seperated File to manage Vector Specific options
    // Include it here and call the extension

    TSharedPtr<FExtender> extender = MakeShared<FExtender>();
    extender->AddMenuExtension(
        "LayerSection"
        , EExtensionHook::Position::After
        , nullptr
        , FMenuExtensionDelegate::CreateRaw(this, &SOdysseyTextureLayerStackTreeView::ExtendContextMenuLayerSection)
    );

    return { extender };
}

void
SOdysseyTextureLayerStackTreeView::ExtendContextMenuLayerSection(FMenuBuilder& iMenuBuilder)
{
    UOdysseyTextureLayerStack* layerStack = mExtension->GetLayerStack();
    if ( !layerStack )
        return;
    
    TArray<UOdysseyLayer*> selectedLayers = GetSelectedItems();
    if (selectedLayers.Num() <= 0)
        return;

    bool canConvert = selectedLayers.ContainsByPredicate(
        [](UOdysseyLayer* iLayer) -> bool
        {
            return iLayer->GetClass() != UOdysseyTextureLayerImageRaster::StaticClass();
        }
    );

    if (!canConvert)
        return;

    iMenuBuilder.BeginSection("Convertions", LOCTEXT("texture.layerstack.context-menu.convertions-section", "Convertions"));
    {
        iMenuBuilder.AddMenuEntry(
            LOCTEXT("texture.layerstack.context-menu.convert-layer-to-raster-layer.name", "Convert to Raster")
            , LOCTEXT("texture.layerstack.context-menu.convert-layer-to-raster-layer.tooltip", "Converts the selected layers to raster layers")
            , FSlateIcon()
            , FUIAction(
                FExecuteAction::CreateRaw(this, &SOdysseyTextureLayerStackTreeView::Action_ConvertLayerToRasterLayer)
            )
        );
    }
}

void
SOdysseyTextureLayerStackTreeView::Action_ConvertLayerToRasterLayer()
{
    UOdysseyTextureLayerStack* layerStack = mExtension->GetLayerStack();
    if ( !layerStack )
        return;
    
    TArray<UOdysseyLayer*> selectedLayers = GetSelectedItems();
    if (selectedLayers.Num() <= 0)
        return;

    bool canConvert = selectedLayers.ContainsByPredicate(
        [](UOdysseyLayer* iLayer) -> bool
        {
            return iLayer->GetClass() != UOdysseyTextureLayerImageRaster::StaticClass();
        }
    );

    if (!canConvert)
        return;
    
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("texture.layer.transaction.convert-to-raster", "Convert Layer To Raster Layer"));
#endif

    for (UOdysseyLayer* layer : selectedLayers)
    {
        UOdysseyLayer* parent = layer->GetParent();
        int indexInParent = layer->GetIndexInParent();

        UOdysseyLayer* layerRaster = layerStack->AddLayer(UOdysseyTextureLayerImageRaster::StaticClass(), parent, indexInParent);
        layerRaster->Merge({layer});
        layerStack->RemoveLayer(layer);
    }
}

#undef LOCTEXT_NAMESPACE
