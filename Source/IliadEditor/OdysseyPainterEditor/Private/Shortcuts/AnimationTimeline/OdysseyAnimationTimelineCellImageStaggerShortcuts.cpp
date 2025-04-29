// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Shortcuts/AnimationTimeline/OdysseyAnimationTimelineCellImageStaggerShortcuts.h"

#include "Widgets/Animation/Timeline/Cells/CellImageStagger/SOdysseyAnimationCellImageStagger.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorAnimationCommands.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyLayerStack.h"
#include "OdysseyAnimation.h"
#include "ULISLoaderModule.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyAnimationCellSelection.h"

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

    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerStack->CurrentLayer.Get());
    if (!layer)
        return;

    if (layer->IsLockedRecursively())
        return;

    TArray<UOdysseyAnimationCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return;

    TArray<UOdysseyAnimationCell*> staggerCells = selectedCells.FilterByPredicate(
        [](UOdysseyAnimationCell* iCell)
        {
            return iCell->IsA<UOdysseyAnimationCellImageStagger>();
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

    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerStack->CurrentLayer.Get());
    if (!layer)
        return false;

    if (layer->IsLockedRecursively())
        return false;

    TArray<UOdysseyAnimationCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return false;

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
