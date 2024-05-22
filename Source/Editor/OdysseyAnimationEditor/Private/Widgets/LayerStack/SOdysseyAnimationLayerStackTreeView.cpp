// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/LayerStack/SOdysseyAnimationLayerStackTreeView.h"

#include "Shortcuts/Timeline/OdysseyAnimationTimelineShortcuts.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

SOdysseyAnimationLayerStackTreeView::SOdysseyAnimationLayerStackTreeView()
    : mTimelineShortcuts(nullptr)
    , mExtension(nullptr)
{
}

FOdysseyAnimationEditorExtension*
SOdysseyAnimationLayerStackTreeView::GetAnimationEditorExtension() const
{
    return mExtension;
}

void
SOdysseyAnimationLayerStackTreeView::Construct(const FArguments& InArgs, FOdysseyAnimationEditorExtension* iAnimationExtension)
{
    SOdysseyLayerStackTreeView::Construct(InArgs);
    mExtension = iAnimationExtension;
    mTimelineShortcuts = MakeShared<FOdysseyAnimationTimelineShortcuts>(GetLayerStack(), iAnimationExtension);
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
    mExtension->Timeline()->SetSelectedCells({});
    SOdysseyLayerStackTreeView::Private_SignalSelectionChanged(SelectInfo);
}

FReply
SOdysseyAnimationLayerStackTreeView::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
    mExtension->Timeline()->SetSelectedCells({});
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
    UOdysseyAnimationLayerStack* layerStack = mExtension->LayerStack();
    if ( !layerStack )
        return;
    
    TArray<UOdysseyLayer*> selectedLayers = GetSelectedItems();
    if (selectedLayers.Num() <= 0)
        return;

    bool allLayersAreVectors = !selectedLayers.ContainsByPredicate(
        [](UOdysseyLayer* iLayer) -> bool
        {
            return iLayer->GetClass() != UOdysseyAnimationLayerImageVector::StaticClass();
        }
    );

    if (!allLayersAreVectors)
        return;

    iMenuBuilder.BeginSection("VectorLayer", LOCTEXT("animation.layerstack.context-menu.vector-layer-section", "Vector Layer"));
    {
        iMenuBuilder.AddMenuEntry(
            LOCTEXT("animation.layerstack.context-menu.convert-vector-layer-to-raster-layer.name", "Convert to Raster")
            , LOCTEXT("animation.layerstack.context-menu.convert-vector-layer-to-raster-layer.tooltip", "Converts the selected layers to raster layers")
            , FSlateIcon()
            , FUIAction(
                FExecuteAction::CreateRaw(this, &SOdysseyAnimationLayerStackTreeView::Action_ConvertVectorLayerToRasterLayer)
            )
        );
    }
}

void
SOdysseyAnimationLayerStackTreeView::Action_ConvertVectorLayerToRasterLayer()
{
    UOdysseyAnimationLayerStack* layerStack = mExtension->LayerStack();
    if ( !layerStack )
        return;
    
    TArray<UOdysseyLayer*> selectedLayers = GetSelectedItems();
    if (selectedLayers.Num() <= 0)
        return;

    bool allLayersAreVectors = !selectedLayers.ContainsByPredicate(
        [](UOdysseyLayer* iLayer) -> bool
        {
            return iLayer->GetClass() != UOdysseyAnimationLayerImageVector::StaticClass();
        }
    );

    if (!allLayersAreVectors)
        return;
    
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("animation.layer-image-vector.transaction.convert-to-raster", "Convert Vector Layer To Raster Layer"));
#endif

    for (UOdysseyLayer* layerVector : selectedLayers)
    {
        UOdysseyLayer* parent = layerVector->GetParent();
        int indexInParent = layerVector->GetIndexInParent();

        UOdysseyLayer* layerRaster = layerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass(), parent, indexInParent);
        layerRaster->Merge({layerVector});
        layerStack->RemoveLayer(layerVector);
    }
}

#undef LOCTEXT_NAMESPACE
