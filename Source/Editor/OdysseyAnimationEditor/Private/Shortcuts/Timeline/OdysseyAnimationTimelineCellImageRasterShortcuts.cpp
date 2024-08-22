// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Shortcuts/Timeline/OdysseyAnimationTimelineCellImageRasterShortcuts.h"

#include "Widgets/LayerStack/Cells/CellImageStagger/SOdysseyAnimationCellImageStagger.h"
#include "Widgets/LayerStack/Cells/CellImageRaster/SOdysseyAnimationCellImageRaster.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "AnimationEditor/OdysseyAnimationEditorCommands.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "OdysseyLayerStack.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "OdysseyAnimation.h"
#include "ULISLoaderModule.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"

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

    if (layer->IsLockedRecursively())
        return;

    UOdysseyAnimation* animation = layer->GetAnimation();
    if (!animation)
        return;

    UOdysseyAnimationLayerImageRaster* layerImageRaster = Cast<UOdysseyAnimationLayerImageRaster>(layer);

    TArray<UOdysseyAnimationCell*> selectedCells = mAnimationExtension->Timeline()->GetSelectedCells();
    if (selectedCells.IsEmpty())
    {
        UOdysseyAnimationCell* cell = layer->GetCellAtFrame(animation->CurrentFrame);
        if (!cell)
            return;
            
        selectedCells.Add(cell);
    }
        
    TArray<UOdysseyAnimationCell*> filteredCells = selectedCells.FilterByPredicate(
        [](UOdysseyAnimationCell* iCell)
        {
            return iCell->IsA<UOdysseyAnimationCellImageStagger>();
        }
    );

    if (filteredCells.IsEmpty())
        return;
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(animation->GetFormat());

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("cell-image-raster.transaction.convert-to-raster-cell", "Convert To Raster Cell"));
#endif

    FScopedSlowTask progressBar(filteredCells.Num(), LOCTEXT("cell-image-raster.convert-to-raster-cell.progress-bar.title", "Converting To Raster Cell"));
    progressBar.MakeDialog();

    struct FResultingCell
    {
        TSharedPtr<::ULIS::FBlock> mBlock;
        int mLength;
    };
    
    TMap<UOdysseyAnimationCell*, TArray<FResultingCell>> resultingCellsByCell;
    for (UOdysseyAnimationCell* filteredCell : filteredCells)
    {
        progressBar.EnterProgressFrame();

        TArray<FGuid> lastComposition = filteredCell->GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Render, 0);
        TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(animation->GetWidth(), animation->GetHeight(), animation->GetFormat());
        ctx.Clear(*block);
        ctx.Finish();

        TSharedPtr<IOdysseyImageRenderer> renderer = filteredCell->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, 0);
        renderer->Init();
        FOdysseyImageRendererCopyParams params(block, { block->Rect() });
        renderer->Copy(params, {});
        ctx.Finish();

        FResultingCell resultingCell;
        resultingCell.mBlock = block;
        resultingCell.mLength = filteredCell->Length;

        TArray<FResultingCell> resultingCells;
        resultingCells.Add(resultingCell);

        FScopedSlowTask loopProgress(filteredCell->Length - 1);
        for (int i = 1; i < filteredCell->Length; i++)
        {
            loopProgress.EnterProgressFrame();
            TArray<FGuid> composition = filteredCell->GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Render, i);
            if (composition == lastComposition)
                continue;

            resultingCells.Last().mLength = i - (filteredCell->Length - resultingCells.Last().mLength);
            lastComposition = composition;
            
            block = MakeShared<::ULIS::FBlock>(animation->GetWidth(), animation->GetHeight(), animation->GetFormat());
            ctx.Clear(*block);
            ctx.Finish();

            renderer = filteredCell->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, i);
            renderer->Init();

            params = FOdysseyImageRendererCopyParams(block, { block->Rect() });
            renderer->Copy(params, {});
            ctx.Finish();

            resultingCell.mBlock = block;
            resultingCell.mLength = filteredCell->Length - i;

            resultingCells.Add(resultingCell);
        }

        resultingCellsByCell.Add(filteredCell, resultingCells);
    }

    for (auto element : resultingCellsByCell)
    {
        UOdysseyAnimationCell* originalCell = element.Key;
        TArray<FResultingCell> resultingCells = element.Value;

        
		TArray<UOdysseyAnimationCell*> rasterCells = layer->AddCells(UOdysseyAnimationCellImageRaster::StaticClass(), originalCell->IndexInLayer + 1, resultingCells.Num());
		layer->RemoveCell(originalCell);
        for (int i = 0; i < resultingCells.Num(); i++)
        {
			const FResultingCell& resultingCell = resultingCells[i];
			UOdysseyAnimationCellImageRaster* rasterCell = Cast<UOdysseyAnimationCellImageRaster>(rasterCells[i]);
			FOdysseyObjectEditorUtils::SetPropertyValue(rasterCell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Length), resultingCell.mLength);

			FOdysseyRasterBlockMutator mutator(rasterCell->GetRasterBlock(), false);
			mutator.EditTilesFromRects(
				{ rasterCell->GetRasterBlock()->GetRect() },
				[&](TSharedPtr<::ULIS::FBlock> iBlock, const FULISInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
				{
					ctx.Copy(*resultingCell.mBlock, *iBlock);
					ctx.Finish();
					return {};
				}
			);
			mutator.Commit();
        }

        selectedCells.Remove(originalCell);
        selectedCells.Append(rasterCells);
    }

    mAnimationExtension->Timeline()->SetSelectedCells(selectedCells);
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

    TArray<UOdysseyAnimationCell*> selectedCells = mAnimationExtension->Timeline()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return;

    bool hasCellsToCrossFade = selectedCells.ContainsByPredicate(
        [](UOdysseyAnimationCell* iCell)
        {
            return iCell->Length > 1;
        }
    );
    if (!hasCellsToCrossFade)
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
    TArray<UOdysseyAnimationCell*> cellsToSelect = selectedCells;

    //Cross Fade all selected cells
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(animation->GetFormat());

    FScopedSlowTask selectedCellsProgress(selectedCells.Num());
    for (UOdysseyAnimationCell* selectedCell : selectedCells)
    {
        selectedCellsProgress.EnterProgressFrame();
        TSharedPtr<::ULIS::FBlock> startBlock = MakeShared<::ULIS::FBlock>(animation->GetWidth(), animation->GetHeight(), animation->GetFormat());
        ctx.Clear(*startBlock);
        ctx.Finish();
        TSharedPtr<IOdysseyImageRenderer> renderer = selectedCell->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, 0);
        renderer->Init();

        FOdysseyImageRendererCopyParams startParams(startBlock, { startBlock->Rect() });
        renderer->Copy(startParams, {});
        ctx.Finish();

        int cellIndex = selectedCell->IndexInLayer;
        int nextCellIndex = cellIndex + 1;
        int crossFadelength = selectedCell->Length;

        //Reduce the original cell to 1 frame length
		FOdysseyObjectEditorUtils::SetPropertyValue(selectedCell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Length), 1);

        if (nextCellIndex < layer->GetCells().Num())
        {
            UOdysseyAnimationCell* nextCell = layer->GetCells()[nextCellIndex];
            TSharedPtr<::ULIS::FBlock> endBlock = MakeShared<::ULIS::FBlock>(animation->GetWidth(), animation->GetHeight(), animation->GetFormat());
            ctx.Clear(*endBlock);
            ctx.Finish();
            renderer = nextCell->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, 0);
            renderer->Init();

            FOdysseyImageRendererCopyParams endParams(endBlock, { endBlock->Rect() });
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
					[&](TSharedPtr<::ULIS::FBlock> iBlock, const FULISInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
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
            TArray<UOdysseyAnimationCell*> rasterCells = layer->AddCells(UOdysseyAnimationCellImageStagger::StaticClass(), selectedCell->IndexInLayer + 1, crossFadelength - 1);

            FScopedSlowTask framesProgress(crossFadelength);
            for (int j = 1; j < crossFadelength; j++)
            {
                framesProgress.EnterProgressFrame();

				UOdysseyAnimationCellImageRaster* rasterCell = Cast<UOdysseyAnimationCellImageRaster>(rasterCells[j - 1]);

				FOdysseyRasterBlockMutator mutator(rasterCell->GetRasterBlock(), false);
				mutator.EditTilesFromRects(
					{ rasterCell->GetRasterBlock()->GetRect() },
					[&](TSharedPtr<::ULIS::FBlock> iBlock, const FULISInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
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
    mAnimationExtension->Timeline()->SetSelectedCells(cellsToSelect);
}

bool
FOdysseyAnimationTimelineCellImageRasterShortcuts::CanAction_ConvertToRasterCell()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer || !layer->IsA(UOdysseyAnimationLayerImageRaster::StaticClass()))
        return false;

    if (layer->IsLockedRecursively())
        return false;

    UOdysseyAnimation* animation = layer->GetAnimation();
    if (!animation)
        return false;

    UOdysseyAnimationLayerImageRaster* layerImageRaster = Cast<UOdysseyAnimationLayerImageRaster>(layer);

    TArray<UOdysseyAnimationCell*> selectedCells = mAnimationExtension->Timeline()->GetSelectedCells();
    if (selectedCells.IsEmpty())
    {
        UOdysseyAnimationCell* cell = layer->GetCellAtFrame(animation->CurrentFrame);
        if (!cell)
            return false;
    }
        
    TArray<UOdysseyAnimationCell*> filteredCells = selectedCells.FilterByPredicate(
        [](UOdysseyAnimationCell* iCell)
        {
            return iCell->IsA<UOdysseyAnimationCellImageStagger>();
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

    if (layer->IsLockedRecursively())
        return false;

    UOdysseyAnimationLayerImageRaster* layerImageRaster = Cast<UOdysseyAnimationLayerImageRaster>(mLayerStack->CurrentLayer.Get());

    const TArray<UOdysseyAnimationCell*> selectedCells = mAnimationExtension->Timeline()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return false;

    bool hasCellsToCrossFade = selectedCells.ContainsByPredicate(
        [](UOdysseyAnimationCell* iCell)
        {
            return iCell->Length > 1;
        }
    );
    if (!hasCellsToCrossFade)
        return false;

    return true;
}


#undef LOCTEXT_NAMESPACE