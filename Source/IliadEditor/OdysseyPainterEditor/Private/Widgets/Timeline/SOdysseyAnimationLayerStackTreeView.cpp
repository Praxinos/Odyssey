// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyAnimationLayerStackTreeView.h"

#include "Framework/Commands/UICommandList.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "ScopedTransaction.h"

#include "OdysseyAnimationCellImageRaster.h"
#include "OdysseyAnimationCellImageVector.h"
#include "OdysseyAnimationLayerFolder.h"
#include "OdysseyAnimationLayerImageRaster.h"
#include "OdysseyAnimationLayerImageVector.h"
#include "OdysseyAnimationLayerStack.h"
#include "OdysseyCommandList.h"
#include "OdysseyLayerStackSelection.h"
#include "Shortcuts/AnimationTimeline/OdysseyAnimationTimelineShortcuts.h"
#include "SOdysseyAnimationLayerFolderRow.h"
#include "SOdysseyAnimationLayerImageRasterRow.h"
#include "SOdysseyAnimationLayerImageVectorRow.h"
#include "SOdysseyAnimationTimelineToolSelector.h"
#include "Widgets/SOdysseyLayerStackAddLayerButton.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

SOdysseyAnimationLayerStackTreeView::SOdysseyAnimationLayerStackTreeView()
{
}

void
SOdysseyAnimationLayerStackTreeView::Construct(const FArguments& InArgs)
{
    mTimelinePosition = InArgs._TimelinePosition;
    mCurrentFrame = InArgs._CurrentFrame;
    mLayerStack = InArgs._LayerStack;
    if (!mLayerStack)
        return;

    SOdysseyLayerStackTreeView::Construct(
        SOdysseyLayerStackTreeView::FArguments()
        .LayerStack(mLayerStack)
        .OnGenerateRow(this, &SOdysseyAnimationLayerStackTreeView::OnGenerateRow)
        .Columns(InArgs._Columns)
        .ExternalScrollbar(InArgs._ExternalScrollbar)
        .OnTreeViewScrolled(InArgs._OnTreeViewScrolled)
        .HeaderContent()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SOdysseyLayerStackAddLayerButton)
                .LayerStack(mLayerStack)
                .OnAdded( this, &SOdysseyAnimationLayerStackTreeView::OnLayerAdded)
            ]
            + SHorizontalBox::Slot()
            [
                SNullWidget::NullWidget
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(VAlign_Center)
            [
                SNew(SOdysseyAnimationTimelineToolSelector)
            ]
        ]
    );
}

TSharedRef<ITableRow>
SOdysseyAnimationLayerStackTreeView::OnGenerateRow(UOdysseyLayer* iLayer, const TSharedRef<STableViewBase>& iOwnerTable)
{
    check(iLayer);

    UClass* layerClass = iLayer->GetClass();
    if (layerClass == UOdysseyAnimationLayerFolder::StaticClass())
    {
        return SNew(SOdysseyAnimationLayerFolderRow, SharedThis(this), Cast<UOdysseyAnimationLayerFolder>(iLayer))
            .TimelinePosition(mTimelinePosition);
    }
    else if (layerClass == UOdysseyAnimationLayerImageRaster::StaticClass())
    {
        return SNew(SOdysseyAnimationLayerImageRasterRow, SharedThis(this), Cast<UOdysseyAnimationLayerImageRaster>(iLayer))
            .TimelinePosition(mTimelinePosition);
    }
    else if (layerClass == UOdysseyAnimationLayerImageVector::StaticClass())
    {
        return SNew(SOdysseyAnimationLayerImageVectorRow, SharedThis(this), Cast<UOdysseyAnimationLayerImageVector>(iLayer))
            .TimelinePosition(mTimelinePosition);
    }

    return SNew(STableRow<UOdysseyLayer*>, iOwnerTable);
}

TArray<TSharedPtr<FExtender>>
SOdysseyAnimationLayerStackTreeView::ExtendContextMenu()
{
    TSharedPtr<FExtender> extender = MakeShared<FExtender>();
    extender->AddMenuExtension(
        "LayerSection"
        , EExtensionHook::Position::After
        , nullptr
        , FMenuExtensionDelegate::CreateRaw(this, &SOdysseyAnimationLayerStackTreeView::ExtendContextMenuLayerSection)
    );

    return { extender };
}

void
SOdysseyAnimationLayerStackTreeView::ExtendContextMenuLayerSection(FMenuBuilder& iMenuBuilder)
{
    TArray<UOdysseyLayer*> selectedLayers = GetSelectedItems();
    if (selectedLayers.Num() <= 0)
        return;

    bool canConvert = selectedLayers.ContainsByPredicate(
        [](UOdysseyLayer* iLayer) -> bool
        {
            return iLayer->GetClass() != UOdysseyAnimationLayerImageRaster::StaticClass();
        }
    );

    if (!canConvert)
        return;

    iMenuBuilder.BeginSection("Convertions", LOCTEXT("animation.layerstack.context-menu.convertions-section", "Convertions"));
    {
        iMenuBuilder.AddMenuEntry(
            LOCTEXT("animation.layerstack.context-menu.convert-layer-to-raster-layer.name", "Convert to Raster")
            , LOCTEXT("animation.layerstack.context-menu.convert-layer-to-raster-layer.tooltip", "Converts the selected layers to raster layers")
            , FSlateIcon()
            , FUIAction(
                FExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerStackTreeView::Action_ConvertLayerToRasterLayer)
            )
        );
    }
}

void
SOdysseyAnimationLayerStackTreeView::Action_ConvertLayerToRasterLayer()
{
    UOdysseyLayerStack* layerStack = GetLayerStack();

    TArray<UOdysseyLayer*> selectedLayers = GetSelectedItems();
    if (selectedLayers.Num() <= 0)
        return;

    bool shouldConvert = selectedLayers.ContainsByPredicate(
        [](UOdysseyLayer* iLayer) -> bool
        {
            return iLayer->GetClass() != UOdysseyAnimationLayerImageRaster::StaticClass();
        }
    );

    if (!shouldConvert)
        return;

    // Sort all selected layer from bottom to top
    // Otherwise during the merge of each layer, the new layer (via AddLayer()) will return nullptr
    // - as the previous merged layer was removed
    // - but as it was the parent of the layer being merged, the AddLayer() will return nullptr
    // because it doesn't find the old parent in the layer list
    //
    // So sort all selected layers from bottom to top, so each merged (and removed) layer won't never be
    // the parent of a future layer which will be merged
    TArray<UOdysseyLayer*> sortedBottomToTopSelectedLayers;
    TArray<UOdysseyLayer*> layers = layerStack->GetLayers();
    Algo::Reverse( layers );
    for( UOdysseyLayer* layer : layers )
    {
        if( selectedLayers.Contains( layer ) )
            sortedBottomToTopSelectedLayers.Add( layer );
    }

#if WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("animation.layer.transaction.convert-to-raster", "Convert Layer To Raster Layer"));
#endif

    UOdysseyLayer* currentLayer = layerStack->GetCurrentLayer();

    for (UOdysseyLayer* layer : sortedBottomToTopSelectedLayers )
    {
        UOdysseyLayer* parent = layer->GetParent();
        int indexInParent = layer->GetIndexInParent();

        UOdysseyLayer* layerRaster = layerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass(), parent, indexInParent);
        if (layer == currentLayer)
            currentLayer = layerRaster;

        layerRaster->Merge({layer});
        layerStack->RemoveLayer(layer);
    }

    layerStack->SetCurrentLayer(currentLayer);
}

void
SOdysseyAnimationLayerStackTreeView::OnLayerAdded(UOdysseyLayer* iLayer)
{
    if (iLayer->GetClass() == UOdysseyAnimationLayerImageRaster::StaticClass())
    {
        UOdysseyAnimationLayerImageRaster* layer = Cast<UOdysseyAnimationLayerImageRaster>(iLayer);
        layer->AddCell(UOdysseyAnimationCellImageRaster::StaticClass());
        layer->SetCellsOffset(mCurrentFrame.Get());
    }
    else if (iLayer->GetClass() == UOdysseyAnimationLayerImageVector::StaticClass())
    {
        UOdysseyAnimationLayerImageVector* layer = Cast<UOdysseyAnimationLayerImageVector>(iLayer);
        layer->AddCell(UOdysseyAnimationCellImageVector::StaticClass());
        layer->SetCellsOffset(mCurrentFrame.Get());
    }
}

#undef LOCTEXT_NAMESPACE
