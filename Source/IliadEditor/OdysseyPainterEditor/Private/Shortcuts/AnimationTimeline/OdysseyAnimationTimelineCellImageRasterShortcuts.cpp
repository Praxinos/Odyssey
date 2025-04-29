// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Shortcuts/AnimationTimeline/OdysseyAnimationTimelineCellImageRasterShortcuts.h"

#include "Widgets/Animation/Timeline/Cells/CellImageStagger/SOdysseyAnimationCellImageStagger.h"
#include "Widgets/Animation/Timeline/Cells/CellImageRaster/SOdysseyAnimationCellImageRaster.h"
#include "OdysseyAnimationLayerImageRaster.h"
#include "OdysseyPainterEditorAnimationCommands.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyLayerStack.h"
#include "OdysseyAnimation.h"
#include "ULISLoaderModule.h"
#include "ULISUtils.h"
#include "OdysseyAnimationCellImageRaster.h"
#include "Shortcuts/AnimationTimeline/OdysseyAnimationTimelineCellImageStaggerShortcuts.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyRasterBlockMutator.h"
#include "OdysseyLayerCellSelection.h"
#include "ScopedTransaction.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

FOdysseyAnimationTimelineCellImageRasterShortcuts::FOdysseyAnimationTimelineCellImageRasterShortcuts(const TAttribute<UOdysseyAnimation*>& iAnimation)
    : mAnimation(iAnimation)
{
}

void
FOdysseyAnimationTimelineCellImageRasterShortcuts::MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList)
{
    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().CrossFade,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellImageRasterShortcuts::Action_CrossFade),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellImageRasterShortcuts::CanAction_CrossFade)
    );
}

void
FOdysseyAnimationTimelineCellImageRasterShortcuts::Action_CrossFade()
{
    /* UOdysseyAnimation* animation = mAnimation.Get();
    if (!animation)
        return;

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(animation->GetLayerStack());
    if (!layerStack)
        return;

    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerStack->GetCurrentLayer());
    if (!layer || !layer->IsA(UOdysseyAnimationLayerImageRaster::StaticClass()))
        return;

    if (layer->IsLockedRecursively())
        return;

    UOdysseyAnimationLayerImageRaster* layerImageRaster = Cast<UOdysseyAnimationLayerImageRaster>(layer);

    TArray<UOdysseyLayerCell*> selectedCells = layerImageRaster->GetLayerStack()->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return;

    bool hasCellsToCrossFade = selectedCells.ContainsByPredicate(
        [](UOdysseyLayerCell* iCell)
        {
            return iCell->Exposure > 1;
        }
    );
    if (!hasCellsToCrossFade)
        return;

    ::ULIS::eFormat format = ::ULIS::Format_BGRA8;
    switch(animation->GetFormat())
    {
        case EOdysseyAnimationFormat::BGRA8: format = ::ULIS::Format_BGRA8;
        case EOdysseyAnimationFormat::RGBAF: format = ::ULIS::Format_RGBAF;
    }

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("cell-image-raster.transaction.cross-fade", "Cross Fade"));
#endif

    FScopedSlowTask progressBar(2, LOCTEXT("cell-image-raster.cross-fade.progress-bar.title", "Applying Cross Fade"));
    progressBar.MakeDialog();
    progressBar.EnterProgressFrame();

    //Convert Selected Stagger Cells to ImageRaster Cells
    FOdysseyAnimationTimelineCellImageStaggerShortcuts staggerShortcuts(animation);
    staggerShortcuts.Action_ConvertToReferenceCells();

    progressBar.EnterProgressFrame();

    //Get the selected cells again to ensure having the converted cells
    selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    TArray<UOdysseyLayerCell*> cellsToSelect = selectedCells;

    //Cross Fade all selected cells
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);

    FScopedSlowTask selectedCellsProgress(selectedCells.Num());
    for (UOdysseyLayerCell* selectedCell : selectedCells)
    {
        selectedCellsProgress.EnterProgressFrame();
        TSharedPtr<::ULIS::FBlock> startBlock = MakeShared<::ULIS::FBlock>(animation->GetWidth(), animation->GetHeight(), format);
        ctx.Clear(*startBlock);
        ctx.Finish();
        TSharedPtr<IOdysseyImageRenderer> renderer = selectedCell->BuildImageRenderer(EOdysseyRenderingType::Render, 0);
        renderer->Init();

        FOdysseyImageRendererCopyParams startParams(startBlock, { ::ULISUtils::ToIntRect(startBlock->Rect()) });
        renderer->Copy(startParams, {});
        ctx.Finish();

        int cellIndex = selectedCell->IndexInLayer;
        int nextCellIndex = cellIndex + 1;
        int crossFadelength = selectedCell->Exposure;

        //Reduce the original cell to 1 frame Exposure
        FOdysseyObjectEditorUtils::SetPropertyValue(selectedCell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), 1);

        if (nextCellIndex < layer->GetCells().Num() || !layer->GetCells()[nextCellIndex])
        {
            UOdysseyLayerCell* nextCell = layer->GetCells()[nextCellIndex];
            TSharedPtr<::ULIS::FBlock> endBlock = MakeShared<::ULIS::FBlock>(animation->GetWidth(), animation->GetHeight(), format);
            ctx.Clear(*endBlock);
            ctx.Finish();
            renderer = nextCell->BuildImageRenderer(EOdysseyRenderingType::Render, 0);
            renderer->Init();

            FOdysseyImageRendererCopyParams endParams(endBlock, { ::ULISUtils::ToIntRect(endBlock->Rect()) });
            renderer->Copy(endParams, {});
            ctx.Finish();

            TArray<UOdysseyLayerCell*> rasterCells = layer->AddCells(UOdysseyAnimationCellImageRaster::StaticClass(), selectedCell->IndexInLayer + 1, crossFadelength - 1);

            FScopedSlowTask framesProgress(crossFadelength);
            for (int j = 1; j < crossFadelength; j++)
            {
                framesProgress.EnterProgressFrame();

                UOdysseyAnimationCellImageRaster* rasterCell = Cast<UOdysseyAnimationCellImageRaster>(rasterCells[j - 1]);

                FOdysseyRasterBlockMutator mutator(rasterCell->GetRasterBlock(), false);
                mutator.EditTilesFromRects(
                    { rasterCell->GetRasterBlock()->GetRect() },
                    [&](TSharedPtr<::ULIS::FBlock> iBlock, const FOdysseyInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
                    {
                        ctx.Clear(*iBlock);
                        ctx.Finish();

                        ctx.Blend(
                            *startBlock
                            , *iBlock
                            , ::ULIS::FRectI::Auto
                            , ::ULIS::FVec2I(0)
                            , ::ULIS::Blend_Add
                            , ::ULIS::Alpha_Add
                            , 1.f - (float(j) / float(crossFadelength))
                        );
                        ctx.Finish();

                        ctx.Blend(
                            *endBlock
                            , *iBlock
                            , ::ULIS::FRectI::Auto
                            , ::ULIS::FVec2I(0)
                            , ::ULIS::Blend_Add
                            , ::ULIS::Alpha_Add
                            , float(j) / float(crossFadelength)
                        );
                        ctx.Finish();
                        return {};
                    }
                );
                mutator.Commit();
            }

            cellsToSelect.Append(rasterCells);
        }
        else
        {
            TArray<UOdysseyLayerCell*> rasterCells = layer->AddCells(UOdysseyAnimationCellImageRaster::StaticClass(), selectedCell->IndexInLayer + 1, crossFadelength - 1);

            FScopedSlowTask framesProgress(crossFadelength);
            for (int j = 1; j < crossFadelength; j++)
            {
                framesProgress.EnterProgressFrame();

                UOdysseyAnimationCellImageRaster* rasterCell = Cast<UOdysseyAnimationCellImageRaster>(rasterCells[j - 1]);

                FOdysseyRasterBlockMutator mutator(rasterCell->GetRasterBlock(), false);
                mutator.EditTilesFromRects(
                    { rasterCell->GetRasterBlock()->GetRect() },
                    [&](TSharedPtr<::ULIS::FBlock> iBlock, const FOdysseyInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
                    {
                        ctx.Clear(*iBlock);
                        ctx.Finish();

                        ctx.Blend(
                            *startBlock
                            , *iBlock
                            , ::ULIS::FRectI::Auto
                            , ::ULIS::FVec2I(0)
                            , ::ULIS::Blend_Add
                            , ::ULIS::Alpha_Add
                            , 1.f - (float(j) / float(crossFadelength))
                        );
                        ctx.Finish();
                        return {};
                    }
                );
                mutator.Commit();
            }
            cellsToSelect.Append(rasterCells);
        }
    }
    layerStack->GetCellSelection()->SetSelectedCells(cellsToSelect); */
}

bool
FOdysseyAnimationTimelineCellImageRasterShortcuts::CanAction_CrossFade()
{
    UOdysseyAnimation* animation = mAnimation.Get();
    if (!animation)
        return false;

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(animation->GetLayerStack());
    if (!layerStack)
        return false;

    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerStack->GetCurrentLayer());
    if (!layer || !layer->IsA(UOdysseyAnimationLayerImageRaster::StaticClass()))
        return false;

    if (layer->IsLockedRecursively())
        return false;

    UOdysseyAnimationLayerImageRaster* layerImageRaster = Cast<UOdysseyAnimationLayerImageRaster>(layerStack->GetCurrentLayer());

    const TArray<UOdysseyLayerCell*> selectedCells = layerImageRaster->GetLayerStack()->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return false;

    bool hasCellsToCrossFade = selectedCells.ContainsByPredicate(
        [](UOdysseyLayerCell* iCell)
        {
            return iCell->GetExposure() > 1;
        }
    );
    if (!hasCellsToCrossFade)
        return false;

    return true;
}


#undef LOCTEXT_NAMESPACE
