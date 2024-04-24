// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Shortcuts/Timeline/OdysseyAnimationTimelineCellImageRasterShortcuts.h"

#include "Widgets/LayerStack/Cells/CellImageStagger/SOdysseyAnimationCellImageStagger.h"
#include "Widgets/LayerStack/Cells/CellImageRaster/SOdysseyAnimationCellImageRaster.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

FOdysseyAnimationTimelineCellImageRasterShortcuts::FOdysseyAnimationTimelineCellImageRasterShortcuts(UOdysseyLayerStack* iLayerStack, FOdysseyAnimationEditorExtension* iAnimationExtension)
    : mLayerStack(iLayerStack)
    , mAnimationExtension(iAnimationExtension)
{
}

void
FOdysseyAnimationTimelineCellImageRasterShortcuts::MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList)
{
    iCommandList->MapAction(
        FOdysseyAnimationEditorCommands::Get().CrossFade,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellImageRasterShortcuts::Action_CrossFade),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellImageRasterShortcuts::CanAction_CrossFade)
    );

    iCommandList->MapAction(
        FOdysseyAnimationEditorCommands::Get().ConvertToRasterCell,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellImageRasterShortcuts::Action_ConvertToRasterCell),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellImageRasterShortcuts::CanAction_ConvertToRasterCell)
    );
}

void
FOdysseyAnimationTimelineCellImageRasterShortcuts::Action_ConvertToRasterCell()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer || !layer->IsA(UOdysseyAnimationLayerImageRaster::StaticClass()))
        return;

    if (layer->GetIsLocked())
        return;

    UOdysseyAnimation* animation = layer->GetAnimation();
    if (!animation)
        return;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = layer->GetCellsContainer();
    if (!cellsContainer)
        return;

    UOdysseyAnimationLayerImageRaster* layerImageRaster = Cast<UOdysseyAnimationLayerImageRaster>(layer);

    TArray<TSharedPtr<FOdysseyAnimationCell>> selectedCells = mAnimationExtension->Timeline()->GetSelectedCells();
    if (selectedCells.IsEmpty())
    {
        TSharedPtr<FOdysseyAnimationCell> cell = cellsContainer->GetCellAtFrame(animation->CurrentFrame);
        if (!cell)
            return;
            
        selectedCells.Add(cell);
    }
        
    TArray<TSharedPtr<FOdysseyAnimationCell>> filteredCells = selectedCells.FilterByPredicate(
        [](TSharedPtr<FOdysseyAnimationCell> iCell)
        {
            return iCell->GetType() == FOdysseyAnimationCellImageStagger::StaticType();
        }
    );

    if (filteredCells.IsEmpty())
        return;
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(animation->Format());

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("cell-image-raster.transaction.convert-to-raster-cell", "Convert To Raster Cell"));
#endif

    FScopedSlowTask progressBar(filteredCells.Num(), LOCTEXT("cell-image-raster.cross-fade.progress-bar.title", "Converting To Raster Cell"));
    progressBar.MakeDialog();

    struct FResultingCell
    {
        TSharedPtr<::ULIS::FBlock> mBlock;
        int mLength;
    };
    
    TMap<TSharedPtr<FOdysseyAnimationCell>, TArray<FResultingCell>> resultingCellsByCell;
    for (TSharedPtr<FOdysseyAnimationCell> filteredCell : filteredCells)
    {
        progressBar.EnterProgressFrame();

        TArray<FGuid> lastComposition = filteredCell->GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Render, 0);
        TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(animation->Width(), animation->Height(), animation->Format());
        ctx.Clear(*block);
        ctx.Finish();
        TSharedPtr<IOdysseyImageRenderer> renderer = filteredCell->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, 0);
        renderer->Init();

        FOdysseyImageRendererCopyParams params(block, { block->Rect() });
        renderer->Copy(params, {});
        ctx.Finish();

        FResultingCell resultingCell;
        resultingCell.mBlock = block;
        resultingCell.mLength = filteredCell->GetLength();

        TArray<FResultingCell> resultingCells;
        resultingCells.Add(resultingCell);

        FScopedSlowTask loopProgress(filteredCell->GetLength() - 1);
        for (int i = 1; i < filteredCell->GetLength(); i++)
        {
            loopProgress.EnterProgressFrame();
            TArray<FGuid> composition = filteredCell->GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Render, i);
            if (composition == lastComposition)
                continue;

            resultingCells.Last().mLength = i - (filteredCell->GetLength() - resultingCells.Last().mLength);
            lastComposition = composition;
            
            block = MakeShared<::ULIS::FBlock>(animation->Width(), animation->Height(), animation->Format());
            ctx.Clear(*block);
            ctx.Finish();

            renderer = filteredCell->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, i);
            renderer->Init();

            params = FOdysseyImageRendererCopyParams(block, { block->Rect() });
            renderer->Copy(params, {});
            ctx.Finish();

            resultingCell.mBlock = block;
            resultingCell.mLength = filteredCell->GetLength() - i;

            resultingCells.Add(resultingCell);
        }

        resultingCellsByCell.Add(filteredCell, resultingCells);
    }

    FOdysseyAnimationCellsMutator cellsMutator(layer, cellsContainer);

    for (auto element : resultingCellsByCell)
    {
        TSharedPtr<FOdysseyAnimationCell> originalCell = element.Key;
        TArray<FResultingCell> resultingCells = element.Value;

        TArray<TSharedPtr<FOdysseyAnimationCell>> rasterCells;
        for (const FResultingCell& resultingCell : resultingCells)
        {
            TSharedRef<FOdysseyAnimationCellImageRaster> rasterCell = FOdysseyAnimationCellImageRaster::Create(layerImageRaster, resultingCell.mLength, resultingCell.mBlock);
            rasterCells.Add(rasterCell);
        }

        int cellIndex = cellsContainer->GetCellIndex(originalCell);
        cellsMutator.Remove({ originalCell }, false);
        cellsMutator.Add(rasterCells, cellIndex);

        selectedCells.Remove(originalCell);
        selectedCells.Append(rasterCells);
    }

    cellsMutator.Commit();

    mAnimationExtension->Timeline()->SetSelectedCells(selectedCells);
}

void
FOdysseyAnimationTimelineCellImageRasterShortcuts::Action_CrossFade()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer || !layer->IsA(UOdysseyAnimationLayerImageRaster::StaticClass()))
        return;

    if (layer->GetIsLocked())
        return;

    UOdysseyAnimationLayerImageRaster* layerImageRaster = Cast<UOdysseyAnimationLayerImageRaster>(layer);

    TArray<TSharedPtr<FOdysseyAnimationCell>> selectedCells = mAnimationExtension->Timeline()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return;

    bool hasCellsToCrossFade = selectedCells.ContainsByPredicate(
        [](TSharedPtr<FOdysseyAnimationCell> iCell)
        {
            return iCell->GetLength() > 1;
        }
    );
    if (!hasCellsToCrossFade)
        return;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = layer->GetCellsContainer();
    if (!cellsContainer)
        return;

    UOdysseyAnimation* animation = layer->GetAnimation();

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("cell-image-raster.transaction.cross-fade", "Cross Fade"));
#endif

    FScopedSlowTask progressBar(2, LOCTEXT("cell-image-raster.cross-fade.progress-bar.title", "Applying Cross Fade"));
    progressBar.MakeDialog();
    progressBar.EnterProgressFrame();

    //Convert Selected Stagger Cells to ImageRaster Cells
    Action_ConvertToRasterCell();

    progressBar.EnterProgressFrame();

    //Get the selected cells again to ensure having the converted cells
    selectedCells = mAnimationExtension->Timeline()->GetSelectedCells();
    TArray<TSharedPtr<FOdysseyAnimationCell>> cellsToSelect = selectedCells;

    //Cross Fade all selected cells
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(animation->Format());

    FOdysseyAnimationCellsMutator cellsMutator(layer, cellsContainer);
    
    FScopedSlowTask selectedCellsProgress(selectedCells.Num());
    for (TSharedPtr<FOdysseyAnimationCell> selectedCell : selectedCells)
    {
        selectedCellsProgress.EnterProgressFrame();
        TSharedPtr<::ULIS::FBlock> startBlock = MakeShared<::ULIS::FBlock>(animation->Width(), animation->Height(), animation->Format());
        ctx.Clear(*startBlock);
        ctx.Finish();
        TSharedPtr<IOdysseyImageRenderer> renderer = selectedCell->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, 0);
        renderer->Init();

        FOdysseyImageRendererCopyParams startParams(startBlock, { startBlock->Rect() });
        renderer->Copy(startParams, {});
        ctx.Finish();

        int cellIndex = cellsContainer->GetCellIndex(selectedCell);
        int nextCellIndex = cellIndex + 1;
        int crossFadelength = selectedCell->GetLength();

        //Reduce the original cell to 1 frame length
        cellsMutator.SetLength(selectedCell, 1);

        if (nextCellIndex < cellsContainer->GetCells().Num())
        {
            TSharedPtr<FOdysseyAnimationCell> nextCell = cellsContainer->GetCells()[nextCellIndex];
            TSharedPtr<::ULIS::FBlock> endBlock = MakeShared<::ULIS::FBlock>(animation->Width(), animation->Height(), animation->Format());
            ctx.Clear(*endBlock);
            ctx.Finish();
            renderer = nextCell->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, 0);
            renderer->Init();

            FOdysseyImageRendererCopyParams endParams(endBlock, { endBlock->Rect() });
            renderer->Copy(endParams, {});
            ctx.Finish();

            TArray<TSharedPtr<FOdysseyAnimationCell>> rasterCells;

            FScopedSlowTask framesProgress(crossFadelength);
            for (int j = 1; j < crossFadelength; j++)
            {
                framesProgress.EnterProgressFrame();

                TSharedPtr<::ULIS::FBlock> crossFadeBlock = MakeShared<::ULIS::FBlock>(animation->Width(), animation->Height(), animation->Format());
                ctx.Clear(*crossFadeBlock);
                ctx.Finish();

                ctx.Blend(
                    *startBlock
                    , *crossFadeBlock
                    , ::ULIS::FRectI::Auto
                    , ::ULIS::FVec2I(0)
                    , ::ULIS::Blend_Add
                    , ::ULIS::Alpha_Add
                    , 1.f - (float(j) / float(crossFadelength))
                );
                ctx.Finish();

                ctx.Blend(
                    *endBlock
                    , *crossFadeBlock
                    , ::ULIS::FRectI::Auto
                    , ::ULIS::FVec2I(0)
                    , ::ULIS::Blend_Add
                    , ::ULIS::Alpha_Add
                    , float(j) / float(crossFadelength)
                );
                ctx.Finish();

                TSharedRef<FOdysseyAnimationCellImageRaster> rasterCell = FOdysseyAnimationCellImageRaster::Create(layerImageRaster, 1, crossFadeBlock);
                rasterCells.Add(rasterCell);
            }

            //Insert the created cells after selectedcell
            cellsMutator.Add(rasterCells, cellIndex + 1);

            cellsToSelect.Append(rasterCells);
        }
        else
        {
            TArray<TSharedPtr<FOdysseyAnimationCell>> rasterCells;

            FScopedSlowTask framesProgress(crossFadelength);
            for (int j = 1; j < crossFadelength; j++)
            {
                framesProgress.EnterProgressFrame();

                TSharedPtr<::ULIS::FBlock> crossFadeBlock = MakeShared<::ULIS::FBlock>(animation->Width(), animation->Height(), animation->Format());
                ctx.Clear(*crossFadeBlock);
                ctx.Finish();

                ctx.Blend(
                    *startBlock
                    , *crossFadeBlock
                    , ::ULIS::FRectI::Auto
                    , ::ULIS::FVec2I(0)
                    , ::ULIS::Blend_Add
                    , ::ULIS::Alpha_Add
                    , 1.f - (float(j) / float(crossFadelength))
                );
                ctx.Finish();

                TSharedRef<FOdysseyAnimationCellImageRaster> rasterCell = FOdysseyAnimationCellImageRaster::Create(layerImageRaster, 1, crossFadeBlock);
                rasterCells.Add(rasterCell);
            }

            //Insert the created cells after selectedcell
            cellsMutator.Add(rasterCells, cellIndex + 1);

            cellsToSelect.Append(rasterCells);
        }
    }
    cellsMutator.Commit();

    mAnimationExtension->Timeline()->SetSelectedCells(cellsToSelect);
}

bool
FOdysseyAnimationTimelineCellImageRasterShortcuts::CanAction_ConvertToRasterCell()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer || !layer->IsA(UOdysseyAnimationLayerImageRaster::StaticClass()))
        return false;

    if (layer->GetIsLocked())
        return false;

    UOdysseyAnimation* animation = layer->GetAnimation();
    if (!animation)
        return false;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = layer->GetCellsContainer();
    if (!cellsContainer)
        return false;

    UOdysseyAnimationLayerImageRaster* layerImageRaster = Cast<UOdysseyAnimationLayerImageRaster>(mLayerStack->CurrentLayer.Get());

    TArray<TSharedPtr<FOdysseyAnimationCell>> selectedCells = mAnimationExtension->Timeline()->GetSelectedCells();
    if (selectedCells.IsEmpty())
    {
        TSharedPtr<FOdysseyAnimationCell> cell = cellsContainer->GetCellAtFrame(animation->CurrentFrame);
        if (!cell)
            return false;
    }
        
    TArray<TSharedPtr<FOdysseyAnimationCell>> filteredCells = selectedCells.FilterByPredicate(
        [](TSharedPtr<FOdysseyAnimationCell> iCell)
        {
            return iCell->GetType() == FOdysseyAnimationCellImageStagger::StaticType();
        }
    );

    if (filteredCells.IsEmpty())
        return false;

    return true;
}

bool
FOdysseyAnimationTimelineCellImageRasterShortcuts::CanAction_CrossFade()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer || !layer->IsA(UOdysseyAnimationLayerImageRaster::StaticClass()))
        return false;

    if (layer->GetIsLocked())
        return false;

    UOdysseyAnimationLayerImageRaster* layerImageRaster = Cast<UOdysseyAnimationLayerImageRaster>(mLayerStack->CurrentLayer.Get());

    const TArray<TSharedPtr<FOdysseyAnimationCell>> selectedCells = mAnimationExtension->Timeline()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return false;

    bool hasCellsToCrossFade = selectedCells.ContainsByPredicate(
        [](TSharedPtr<FOdysseyAnimationCell> iCell)
        {
            return iCell->GetLength() > 1;
        }
    );
    if (!hasCellsToCrossFade)
        return false;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = layer->GetCellsContainer();
    if (!cellsContainer)
        return false;

    return true;
}


#undef LOCTEXT_NAMESPACE