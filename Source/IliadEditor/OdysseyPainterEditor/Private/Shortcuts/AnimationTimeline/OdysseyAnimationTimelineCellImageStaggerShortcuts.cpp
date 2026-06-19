// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Shortcuts/AnimationTimeline/OdysseyAnimationTimelineCellImageStaggerShortcuts.h"

#include "ScopedTransaction.h"
#include "Misc/ScopedSlowTask.h"

#include "SOdysseyAnimationCellImageStagger.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorAnimationCommands.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyLayerStack.h"
#include "OdysseyAnimation.h"
#include "ULISLoaderModule.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyLayerCellSelection.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

FOdysseyAnimationTimelineCellImageStaggerShortcuts::FOdysseyAnimationTimelineCellImageStaggerShortcuts(const TAttribute<UOdysseyAnimation*>& iAnimation)
    : mAnimation(iAnimation)
{
}

void
FOdysseyAnimationTimelineCellImageStaggerShortcuts::MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList)
{
    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().ConvertToReferenceCells,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellImageStaggerShortcuts::Action_ConvertToReferenceCells),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellImageStaggerShortcuts::CanAction_ConvertToReferenceCells)
    );
}

void
FOdysseyAnimationTimelineCellImageStaggerShortcuts::Action_ConvertToReferenceCells()
{
    UOdysseyAnimation* animation = mAnimation.Get();
    if (!animation)
        return;

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(animation->GetLayerStack());
    if (!layerStack)
        return;

    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerStack->GetCurrentLayer());
    if (!layer)
        return;

    if (!layer->IsEditable())
        return;

    TArray<UOdysseyLayerCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return;

    TArray<UOdysseyLayerCell*> staggerCells = selectedCells.FilterByPredicate(
        [](UOdysseyLayerCell* iCell)
        {
            return iCell->IsA<UOdysseyLayerCellImageStagger>();
        }
    );

    if (staggerCells.IsEmpty())
        return;

    ::ULIS::eFormat format = ::ULIS::Format_BGRA8;
    switch(animation->GetFormat())
    {
        case EOdysseyAnimationFormat::BGRA8: format = ::ULIS::Format_BGRA8;
        case EOdysseyAnimationFormat::RGBAF: format = ::ULIS::Format_RGBAF;
    }

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);

#if WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("animation.cell.transaction.convert-to-reference-cell", "Convert To Reference Cell"));
#endif

    FScopedSlowTask progressBar(staggerCells.Num(), LOCTEXT("animation.cell.convert-to-reference-cell.progress-bar.title", "Converting To Reference Cell"));
    progressBar.MakeDialog();

    struct FResultingCell
    {
        UOdysseyLayerCell* mReferenceCell;
        int mExposure;
    };

    TMap<UOdysseyLayerCell*, TArray<FResultingCell>> resultingCellsByCell;
    for (UOdysseyLayerCell* cell : staggerCells)
    {
        progressBar.EnterProgressFrame();

        UOdysseyLayerCellImageStagger* staggerCell = Cast<UOdysseyLayerCellImageStagger>(cell);

        UOdysseyLayerCell* referenceCell = staggerCell->GetReferenceCellAtFrame(0);
        if (!referenceCell)
            continue;

        FResultingCell resultingCell;
        resultingCell.mReferenceCell = referenceCell;
        resultingCell.mExposure = staggerCell->GetExposure();

        TArray<FResultingCell> resultingCells;
        resultingCells.Add(resultingCell);

        FScopedSlowTask loopProgress(staggerCell->GetExposure() - 1);
        for (int i = 1; i < staggerCell->GetExposure(); i++)
        {
            loopProgress.EnterProgressFrame();

            referenceCell = staggerCell->GetReferenceCellAtFrame(i);
            if (!referenceCell || referenceCell == resultingCells.Last().mReferenceCell)
                continue;

            resultingCells.Last().mExposure = i - (staggerCell->GetExposure() - resultingCells.Last().mExposure);

            resultingCell.mReferenceCell = referenceCell;
            resultingCell.mExposure = staggerCell->GetExposure() - i;

            resultingCells.Add(resultingCell);
        }

        resultingCellsByCell.Add(staggerCell, resultingCells);
    }

    for (auto element : resultingCellsByCell)
    {
        UOdysseyLayerCell* originalCell = element.Key;
        TArray<FResultingCell> resultingCells = element.Value;

        int indexInLayer = originalCell->GetIndexInLayer();
        TArray<UOdysseyLayerCell*> referenceCells;

        for (int i = 0; i < resultingCells.Num(); i++)
        {
            const FResultingCell& resultingCell = resultingCells[i];
            referenceCells.Add(resultingCell.mReferenceCell);
        }

        TArray<UOdysseyLayerCell*> newCells = layer->CopyCells(referenceCells, originalCell->GetIndexInLayer());
        layer->RemoveCell(originalCell);

        for (int i = 0; i < resultingCells.Num(); i++)
        {
            newCells[i]->SetExposure(resultingCells[i].mExposure);
        }

        selectedCells.Remove(originalCell);
        selectedCells.Append(newCells);
    }

    layerStack->GetCellSelection()->SetSelectedCells(selectedCells);
}

bool
FOdysseyAnimationTimelineCellImageStaggerShortcuts::CanAction_ConvertToReferenceCells()
{
    UOdysseyAnimation* animation = mAnimation.Get();
    if (!animation)
        return false;

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(animation->GetLayerStack());
    if (!layerStack)
        return false;

    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerStack->GetCurrentLayer());
    if (!layer)
        return false;

    if (!layer->IsEditable())
        return false;

    TArray<UOdysseyLayerCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return false;

    TArray<UOdysseyLayerCell*> staggerCells = selectedCells.FilterByPredicate(
        [](UOdysseyLayerCell* iCell)
        {
            return iCell->IsA<UOdysseyLayerCellImageStagger>();
        }
    );

    if (staggerCells.IsEmpty())
        return false;

    return true;
}

#undef LOCTEXT_NAMESPACE
