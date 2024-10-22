// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Shortcuts/Timeline/OdysseyAnimationTimelineCellImageStaggerShortcuts.h"

#include "Widgets/LayerStack/Cells/CellImageStagger/SOdysseyAnimationCellImageStagger.h"
#include "AnimationEditor/OdysseyAnimationEditorCommands.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "OdysseyLayerStack.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "OdysseyAnimation.h"
#include "ULISLoaderModule.h"
#include "UObject/OdysseyObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

FOdysseyAnimationTimelineCellImageStaggerShortcuts::FOdysseyAnimationTimelineCellImageStaggerShortcuts(UOdysseyLayerStack* iLayerStack, FOdysseyAnimationEditorExtension* iAnimationExtension)
    : mLayerStack(iLayerStack)
    , mAnimationExtension(iAnimationExtension)
{
}

void
FOdysseyAnimationTimelineCellImageStaggerShortcuts::MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList)
{
    iCommandList->MapAction(
        FOdysseyAnimationEditorCommands::Get().ConvertToReferenceCells,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellImageStaggerShortcuts::Action_ConvertToReferenceCells),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellImageStaggerShortcuts::CanAction_ConvertToReferenceCells)
    );
}

void
FOdysseyAnimationTimelineCellImageStaggerShortcuts::Action_ConvertToReferenceCells()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return;

    if (layer->IsLockedRecursively())
        return;

    UOdysseyAnimation* animation = layer->GetAnimation();
    if (!animation)
        return;

    TArray<UOdysseyAnimationCell*> selectedCells = mAnimationExtension->Timeline()->GetSelectedCells();
    if (selectedCells.IsEmpty())
    {
        UOdysseyAnimationCell* cell = layer->GetCellAtFrame(animation->CurrentFrame);
        if (!cell)
            return;
            
        selectedCells.Add(cell);
    }
        
    TArray<UOdysseyAnimationCell*> staggerCells = selectedCells.FilterByPredicate(
        [](UOdysseyAnimationCell* iCell)
        {
            return iCell->IsA<UOdysseyAnimationCellImageStagger>();
        }
    );

    if (staggerCells.IsEmpty())
        return;
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(animation->GetFormat());

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("animation.cell.transaction.convert-to-reference-cell", "Convert To Reference Cell"));
#endif

    FScopedSlowTask progressBar(staggerCells.Num(), LOCTEXT("animation.cell.convert-to-reference-cell.progress-bar.title", "Converting To Reference Cell"));
    progressBar.MakeDialog();

    struct FResultingCell
    {
		UOdysseyAnimationCell* mReferenceCell;
        int mExposure;
    };
    
    TMap<UOdysseyAnimationCell*, TArray<FResultingCell>> resultingCellsByCell;
    for (UOdysseyAnimationCell* cell : staggerCells)
    {
        progressBar.EnterProgressFrame();

		UOdysseyAnimationCellImageStagger* staggerCell = Cast<UOdysseyAnimationCellImageStagger>(cell);

		UOdysseyAnimationCell* referenceCell = staggerCell->GetReferenceCellAtFrame(0);
		if (!referenceCell)
			continue;

        FResultingCell resultingCell;
        resultingCell.mReferenceCell = referenceCell;
        resultingCell.mExposure = staggerCell->Exposure;

        TArray<FResultingCell> resultingCells;
        resultingCells.Add(resultingCell);

        FScopedSlowTask loopProgress(staggerCell->Exposure - 1);
        for (int i = 1; i < staggerCell->Exposure; i++)
        {
            loopProgress.EnterProgressFrame();

			referenceCell = staggerCell->GetReferenceCellAtFrame(i);
			if (!referenceCell || referenceCell == resultingCells.Last().mReferenceCell)
				continue;

            resultingCells.Last().mExposure = i - (staggerCell->Exposure - resultingCells.Last().mExposure);

            resultingCell.mReferenceCell = referenceCell;
            resultingCell.mExposure = staggerCell->Exposure - i;

            resultingCells.Add(resultingCell);
        }

        resultingCellsByCell.Add(staggerCell, resultingCells);
    }

    for (auto element : resultingCellsByCell)
    {
        UOdysseyAnimationCell* originalCell = element.Key;
        TArray<FResultingCell> resultingCells = element.Value;
        
		int indexInLayer = originalCell->IndexInLayer;
		TArray<UOdysseyAnimationCell*> referenceCells;
		
        for (int i = 0; i < resultingCells.Num(); i++)
        {
			const FResultingCell& resultingCell = resultingCells[i];
			referenceCells.Add(resultingCell.mReferenceCell);
		}

		TArray<UOdysseyAnimationCell*> newCells = layer->CopyCells(referenceCells, originalCell->IndexInLayer);
		layer->RemoveCell(originalCell);

		for (int i = 0; i < resultingCells.Num(); i++)
        {
			FOdysseyObjectEditorUtils::SetPropertyValue(newCells[i], GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), resultingCells[i].mExposure);
		}

        selectedCells.Remove(originalCell);
        selectedCells.Append(newCells);
    }

    mAnimationExtension->Timeline()->SetSelectedCells(selectedCells);
}

bool
FOdysseyAnimationTimelineCellImageStaggerShortcuts::CanAction_ConvertToReferenceCells()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return false;

    if (layer->IsLockedRecursively())
        return false;

    UOdysseyAnimation* animation = layer->GetAnimation();
    if (!animation)
        return false;

    TArray<UOdysseyAnimationCell*> selectedCells = mAnimationExtension->Timeline()->GetSelectedCells();
    if (selectedCells.IsEmpty())
    {
        UOdysseyAnimationCell* cell = layer->GetCellAtFrame(animation->CurrentFrame);
        if (!cell)
            return false;
    }
        
    TArray<UOdysseyAnimationCell*> staggerCells = selectedCells.FilterByPredicate(
        [](UOdysseyAnimationCell* iCell)
        {
            return iCell->IsA<UOdysseyAnimationCellImageStagger>();
        }
    );

    if (staggerCells.IsEmpty())
        return false;

    return true;
}

#undef LOCTEXT_NAMESPACE