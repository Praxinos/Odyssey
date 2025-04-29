// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Widgets/Animation/Timeline/SOdysseyAnimationLayerStackTreeView.h"

#include "Shortcuts/AnimationTimeline/OdysseyAnimationTimelineShortcuts.h"
#include "OdysseyAnimationLayerImageRaster.h"
#include "OdysseyAnimationLayerStack.h"
#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineToolSelector.h"
#include "Widgets/SOdysseyLayerStackAddLayerButton.h"
#include "OdysseyAnimationCellImageRaster.h"
#include "OdysseyAnimationCellImageVector.h"
#include "LayerFolder/OdysseyAnimationLayerFolder.h"
#include "Widgets/Animation/Timeline/Layers/LayerFolder/SOdysseyAnimationLayerFolderRow.h"
#include "Widgets/Animation/Timeline/Layers/LayerImageRaster/SOdysseyAnimationLayerImageRasterRow.h"
#include "OdysseyAnimationLayerImageVector.h"
#include "Widgets/Animation/Timeline/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorRow.h"
#include "OdysseyAnimationCellSelection.h"
#include "ScopedTransaction.h"

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
    if (!mLayerStack)
        return;

    mTimelineShortcuts = MakeShared<FOdysseyAnimationTimelineShortcuts>(mLayerStack->GetAnimation(), InArgs._CurrentFrame, InArgs._OnTransactCurrentFrame);

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

void
SOdysseyAnimationLayerStackTreeView::OnLayerAdded(UOdysseyLayer* iLayer)
{
    if (iLayer->GetClass() == UOdysseyAnimationLayerImageRaster::StaticClass())
    {
        UOdysseyAnimationLayerImageRaster* layer = Cast<UOdysseyAnimationLayerImageRaster>(iLayer);
        layer->AddCell(UOdysseyAnimationCellImageRaster::StaticClass());
    }
    else if (iLayer->GetClass() == UOdysseyAnimationLayerImageVector::StaticClass())
    {
        UOdysseyAnimationLayerImageVector* layer = Cast<UOdysseyAnimationLayerImageVector>(iLayer);
        layer->AddCell(UOdysseyAnimationCellImageVector::StaticClass());
    }
}

#undef LOCTEXT_NAMESPACE
