// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Shortcuts/AnimationTimeline/OdysseyAnimationTimelineCellImageRasterShortcuts.h"

#include "Widgets/Animation/Timeline/Cells/CellImageStagger/SOdysseyAnimationCellImageStagger.h"
#include "Widgets/Animation/Timeline/Cells/CellImageRaster/SOdysseyAnimationCellImageRaster.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "OdysseyPainterEditorAnimationCommands.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "OdysseyLayerStack.h"
#include "OdysseyAnimation.h"
#include "ULISLoaderModule.h"
#include "ULISUtils.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "Shortcuts/AnimationTimeline/OdysseyAnimationTimelineCellImageStaggerShortcuts.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyRasterBlockMutator.h"
#include "LayerStack/Cells/OdysseyAnimationCellSelection.h"
#include "ScopedTransaction.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

FOdysseyAnimationTimelineCellImageRasterShortcuts::FOdysseyAnimationTimelineCellImageRasterShortcuts(UOdysseyLayerStack* iLayerStack)
    : mLayerStack(iLayerStack)
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
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer || !layer->IsA(UOdysseyAnimationLayerImageRaster::StaticClass()))
        return;

    if (layer->IsLockedRecursively())
        return;

    UOdysseyAnimationLayerImageRaster* layerImageRaster = Cast<UOdysseyAnimationLayerImageRaster>(layer);

    TArray<UOdysseyAnimationCell*> selectedCells = layerImageRaster->GetLayerStack()->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return;

    bool hasCellsToCrossFade = selectedCells.ContainsByPredicate(
        [](UOdysseyAnimationCell* iCell)
        {
            return iCell->Exposure > 1;
        }
    );
    if (!hasCellsToCrossFade)
        return;

    UOdysseyAnimation* animation = layer->GetAnimation();

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
    FOdysseyAnimationTimelineCellImageStaggerShortcuts staggerShortcuts(mLayerStack);
    staggerShortcuts.Action_ConvertToReferenceCells();

    progressBar.EnterProgressFrame();

    //Get the selected cells again to ensure having the converted cells
    selectedCells = animation->GetLayerStack()->GetCellSelection()->GetSelectedCells();
    TArray<UOdysseyAnimationCell*> cellsToSelect = selectedCells;

    //Cross Fade all selected cells
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);

    FScopedSlowTask selectedCellsProgress(selectedCells.Num());
    for (UOdysseyAnimationCell* selectedCell : selectedCells)
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
            UOdysseyAnimationCell* nextCell = layer->GetCells()[nextCellIndex];
            TSharedPtr<::ULIS::FBlock> endBlock = MakeShared<::ULIS::FBlock>(animation->GetWidth(), animation->GetHeight(), format);
            ctx.Clear(*endBlock);
            ctx.Finish();
            renderer = nextCell->BuildImageRenderer(EOdysseyRenderingType::Render, 0);
            renderer->Init();

            FOdysseyImageRendererCopyParams endParams(endBlock, { ::ULISUtils::ToIntRect(endBlock->Rect()) });
            renderer->Copy(endParams, {});
            ctx.Finish();

            TArray<UOdysseyAnimationCell*> rasterCells = layer->AddCells(UOdysseyAnimationCellImageRaster::StaticClass(), selectedCell->IndexInLayer + 1, crossFadelength - 1);

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
            TArray<UOdysseyAnimationCell*> rasterCells = layer->AddCells(UOdysseyAnimationCellImageRaster::StaticClass(), selectedCell->IndexInLayer + 1, crossFadelength - 1);

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
    animation->GetLayerStack()->GetCellSelection()->SetSelectedCells(cellsToSelect);
}

bool
FOdysseyAnimationTimelineCellImageRasterShortcuts::CanAction_CrossFade()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer || !layer->IsA(UOdysseyAnimationLayerImageRaster::StaticClass()))
        return false;

    if (layer->IsLockedRecursively())
        return false;

    UOdysseyAnimationLayerImageRaster* layerImageRaster = Cast<UOdysseyAnimationLayerImageRaster>(mLayerStack->CurrentLayer.Get());

    const TArray<UOdysseyAnimationCell*> selectedCells = layerImageRaster->GetLayerStack()->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return false;

    bool hasCellsToCrossFade = selectedCells.ContainsByPredicate(
        [](UOdysseyAnimationCell* iCell)
        {
            return iCell->Exposure > 1;
        }
    );
    if (!hasCellsToCrossFade)
        return false;

    return true;
}


#undef LOCTEXT_NAMESPACE
