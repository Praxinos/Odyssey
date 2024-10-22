// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationLayerStackTreeView.h"

#include "Shortcuts/Timeline/OdysseyAnimationTimelineShortcuts.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

SOdysseyAnimationLayerStackTreeView::SOdysseyAnimationLayerStackTreeView()
    : mTimelineShortcuts(nullptr)
{
}

void
SOdysseyAnimationLayerStackTreeView::Construct(const FArguments& InArgs)
{
	mTimelinePosition = InArgs._TimelinePosition;

	mLayerStack = InArgs._LayerStack;

	mTimelineShortcuts = MakeShared<FOdysseyAnimationTimelineShortcuts>(mLayerStack);

    SOdysseyLayerStackTreeView::Construct(
		SOdysseyLayerStackTreeView::FArguments()
		.LayerStack(mLayerStack)
        .OnGenerateRow(this, &SOdysseyAnimationLayerStackTreeView::OnGenerateRow)
		.Columns(InArgs._Columns)
		.HeaderHeight(InArgs._HeaderHeight)
		.ExternalScrollbar(InArgs._ExternalScrollbar)
		.OnTreeViewScrolled(InArgs._OnTreeViewScrolled)
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

FReply
SOdysseyAnimationLayerStackTreeView::OnKeyDown( const FGeometry& iGeometry, const FKeyEvent& iKeyEvent )
{
	if (mTimelineShortcuts->GetCommandList()->ProcessCommandBindings(iKeyEvent))
        return FReply::Handled();

    return SOdysseyLayerStackTreeView::OnKeyDown(iGeometry, iKeyEvent);
}

void
SOdysseyAnimationLayerStackTreeView::Private_SignalSelectionChanged(ESelectInfo::Type SelectInfo)
{
    mLayerStack->GetCellSelection()->SetSelectedCells({});
    SOdysseyLayerStackTreeView::Private_SignalSelectionChanged(SelectInfo);
}

FReply
SOdysseyAnimationLayerStackTreeView::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
	mLayerStack->GetCellSelection()->SetSelectedCells({});
    return SOdysseyLayerStackTreeView::OnFocusReceived(MyGeometry, InFocusEvent);
}

TArray<TSharedPtr<FExtender>>
SOdysseyAnimationLayerStackTreeView::ExtendContextMenu()
{
    //TODO:
    // Create a Seperated File to manage Vector Specific options
    // Include it here and call the extension

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
    
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("animation.layer.transaction.convert-to-raster", "Convert Layer To Raster Layer"));
#endif

    for (UOdysseyLayer* layer : selectedLayers)
    {
        UOdysseyLayer* parent = layer->GetParent();
        int indexInParent = layer->GetIndexInParent();

        UOdysseyLayer* layerRaster = layerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass(), parent, indexInParent);
        layerRaster->Merge({layer});
        layerStack->RemoveLayer(layer);
    }
}

#undef LOCTEXT_NAMESPACE
