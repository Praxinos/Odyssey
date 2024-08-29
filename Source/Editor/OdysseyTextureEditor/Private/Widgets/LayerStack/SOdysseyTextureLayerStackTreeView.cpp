// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyTextureLayerStackTreeView.h"

#include "TextureEditor/OdysseyTextureEditorExtension.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
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

    bool allLayersAreVectors = !selectedLayers.ContainsByPredicate(
        [](UOdysseyLayer* iLayer) -> bool
        {
            return iLayer->GetClass() != UOdysseyTextureLayerImageVector::StaticClass();
        }
    );

    if (!allLayersAreVectors)
        return;

    iMenuBuilder.BeginSection("VectorLayer", LOCTEXT("texture.layerstack.context-menu.vector-layer-section", "Vector Layer"));
    {
        iMenuBuilder.AddMenuEntry(
            LOCTEXT("texture.layerstack.context-menu.convert-vector-layer-to-raster-layer.name", "Convert to Raster")
            , LOCTEXT("texture.layerstack.context-menu.convert-vector-layer-to-raster-layer.tooltip", "Converts the selected layers to raster layers")
            , FSlateIcon()
            , FUIAction(
                FExecuteAction::CreateRaw(this, &SOdysseyTextureLayerStackTreeView::Action_ConvertVectorLayerToRasterLayer)
            )
        );
    }
}

void
SOdysseyTextureLayerStackTreeView::Action_ConvertVectorLayerToRasterLayer()
{
    UOdysseyTextureLayerStack* layerStack = mExtension->GetLayerStack();
    if ( !layerStack )
        return;
    
    TArray<UOdysseyLayer*> selectedLayers = GetSelectedItems();
    if (selectedLayers.Num() <= 0)
        return;

    bool allLayersAreVectors = !selectedLayers.ContainsByPredicate(
        [](UOdysseyLayer* iLayer) -> bool
        {
            return iLayer->GetClass() != UOdysseyTextureLayerImageVector::StaticClass();
        }
    );

    if (!allLayersAreVectors)
        return;
    
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("texture.layer-image-vector.transaction.convert-to-raster", "Convert Vector Layer To Raster Layer"));
#endif

    for (UOdysseyLayer* layerVector : selectedLayers)
    {
        UOdysseyLayer* parent = layerVector->GetParent();
        int indexInParent = layerVector->GetIndexInParent();

        UOdysseyLayer* layerRaster = layerStack->AddLayer(UOdysseyTextureLayerImageRaster::StaticClass(), parent, indexInParent);
        layerRaster->Merge({layerVector});
        layerStack->RemoveLayer(layerVector);
    }
}

#undef LOCTEXT_NAMESPACE
