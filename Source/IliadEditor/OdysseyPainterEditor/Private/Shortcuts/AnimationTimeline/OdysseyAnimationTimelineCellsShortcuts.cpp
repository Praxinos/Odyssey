// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Shortcuts/AnimationTimeline/OdysseyAnimationTimelineCellsShortcuts.h"

#include "OdysseyAnimationLayerStack.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyAnimationCellClipboardData.h"
#include "OdysseyLayerCellImageStagger.h"
#include "Framework/Commands/GenericCommands.h"
#include "OdysseyCoreEditorModule.h"
#include "OdysseyPainterEditorModule.h"
#include "Dialogs/Dialogs.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorAnimationCommands.h"
#include "OdysseyAnimation.h"
#include "OdysseyLayerCellSelection.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "ScopedTransaction.h"
#include "OdysseyAnimationCurrentFrameMutator.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

FOdysseyAnimationTimelineCellsShortcuts::FOdysseyAnimationTimelineCellsShortcuts(
    const TAttribute<UOdysseyAnimation*>& iAnimation,
    const TAttribute<int>& iCurrentFrame,
    const FOnTransactCurrentFrame& iOnTransactCurrentFrame
)
    : mAnimation(iAnimation)
    , mCurrentFrame(iCurrentFrame)
    , mOnTransactCurrentFrame(iOnTransactCurrentFrame)
{
}

void
FOdysseyAnimationTimelineCellsShortcuts::MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList)
{
    iCommandList->MapAction(
        FGenericCommands::Get().Copy,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::Action_Copy),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::CanAction_Copy)
    );

    iCommandList->MapAction(
        FGenericCommands::Get().Paste,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::Action_Paste),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::CanAction_Paste)
    );

    iCommandList->MapAction(
        FGenericCommands::Get().Cut,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::Action_Cut),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::CanAction_Cut)
    );

    iCommandList->MapAction(
        FGenericCommands::Get().SelectAll,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::Action_SelectAll),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::CanAction_SelectAll)
    );

    iCommandList->MapAction(
        FGenericCommands::Get().Delete,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::Action_Delete),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::CanAction_Delete)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().ConvertToStaggerCell,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::Action_ConvertToStaggerCell),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::CanAction_ConvertToStaggerCell)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().IncreaseCellExposure,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::Action_IncreaseCellExposure),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::CanAction_IncreaseCellExposure)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().DecreaseCellExposure,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::Action_DecreaseCellExposure),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::CanAction_DecreaseCellExposure)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().SetCellExposure,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::Action_SetCellExposure),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::CanAction_SetCellExposure)
    );
}

void
FOdysseyAnimationTimelineCellsShortcuts::Action_Copy()
{
    UOdysseyAnimation* animation = mAnimation.Get();
    if (!animation)
        return;

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(animation->GetLayerStack());
    if (!layerStack)
        return;

    const TArray<UOdysseyLayerCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return;

    FOdysseyCoreEditorModule& odysseyCoreEditorModule = FModuleManager::Get().LoadModuleChecked<FOdysseyCoreEditorModule>(TEXT("OdysseyCoreEditor"));
    TSharedPtr<FOdysseyAnimationCellClipboardData> clipboardData = MakeShared<FOdysseyAnimationCellClipboardData>(selectedCells);
    odysseyCoreEditorModule.GetClipboard()->SetData(clipboardData);
}

void
FOdysseyAnimationTimelineCellsShortcuts::Action_Cut()
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

    if (layer->IsLockedRecursively())
    {
        Action_Copy();
        return;
    }

    const TArray<UOdysseyLayerCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline.shortcuts.cut-frame", "Cut Frames"));
#endif

    FOdysseyCoreEditorModule& odysseyCoreEditorModule = FModuleManager::Get().LoadModuleChecked<FOdysseyCoreEditorModule>(TEXT("OdysseyCoreEditor"));
    TSharedPtr<FOdysseyAnimationCellClipboardData> clipboardData = MakeShared<FOdysseyAnimationCellClipboardData>(selectedCells);
    odysseyCoreEditorModule.GetClipboard()->SetData(clipboardData);
    Action_Delete();
}

void
FOdysseyAnimationTimelineCellsShortcuts::Action_Paste()
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

    if (layer->IsLockedRecursively())
        return;

    FOdysseyCoreEditorModule& odysseyCoreEditorModule = FModuleManager::Get().LoadModuleChecked<FOdysseyCoreEditorModule>(TEXT("OdysseyCoreEditor"));
    TSharedPtr<FOdysseyAnimationCellClipboardData> clipboardData = odysseyCoreEditorModule.GetClipboard()->GetData<FOdysseyAnimationCellClipboardData>();
    if (!clipboardData)
        return;

    if (!clipboardData->CanPaste(layer))
        return;

    FOdysseyPainterEditorModule& painterEditorModule = FModuleManager::GetModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
    FOdysseyPainterEditor* editor = painterEditorModule.GetOpenedEditorForAsset(layer->GetAnimation());
    if (!editor)
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline.shortcuts.paste-frame", "Paste Frames"));
#endif
    clipboardData->Paste(layer, mCurrentFrame.Get());
    mOnTransactCurrentFrame.ExecuteIfBound(mCurrentFrame.Get());
}

void
FOdysseyAnimationTimelineCellsShortcuts::Action_SelectAll()
{UOdysseyAnimation* animation = mAnimation.Get();
    if (!animation)
        return;

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(animation->GetLayerStack());
    if (!layerStack)
        return;

    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerStack->GetCurrentLayer());
    if (!layer)
        return;

    layerStack->GetCellSelection()->SetSelectedCells(layer->GetCells());
}

void
FOdysseyAnimationTimelineCellsShortcuts::Action_Delete()
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

    if (layer->IsLockedRecursively())
        return;

    const TArray<UOdysseyLayerCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return;

    FOdysseyPainterEditorModule& painterEditorModule = FModuleManager::GetModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
    FOdysseyPainterEditor* editor = painterEditorModule.GetOpenedEditorForAsset(layer->GetAnimation());
    if (!editor)
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline.shortcuts.remove-frame", "Remove Frames"));
#endif
    mOnTransactCurrentFrame.ExecuteIfBound(selectedCells[0]->GetFrameRange().GetLowerBoundValue());
    layer->RemoveCells(selectedCells);
    if (layer->GetCells().IsEmpty())
        layer->AddCell(layer->GetDefaultCellClass());
}



void
FOdysseyAnimationTimelineCellsShortcuts::Action_ConvertToStaggerCell()
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

    if (layer->IsLockedRecursively())
        return;

    FOdysseyPainterEditorModule& painterEditorModule = FModuleManager::GetModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
    FOdysseyPainterEditor* editor = painterEditorModule.GetOpenedEditorForAsset(layer->GetAnimation());
    if (!editor)
        return;

    TArray<UOdysseyLayerCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
    {
        UOdysseyLayerCell* cell = layer->GetCellAtFrame(mCurrentFrame.Get());
        if (!cell)
            return;

        selectedCells.Add(cell);
    }

    selectedCells = selectedCells.FilterByPredicate(
        [](UOdysseyLayerCell* iCell)
        {
            return !iCell->IsA<UOdysseyLayerCellImageStagger>();
        }
    );

    if (selectedCells.IsEmpty())
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline-cells.transaction.create-stagger-cell", "Stagger Cell"));
#endif

    mOnTransactCurrentFrame.ExecuteIfBound(selectedCells[0]->GetFrameRange().GetLowerBoundValue());
    for (UOdysseyLayerCell* cell : selectedCells)
    {
        UOdysseyLayerCell* staggerCell = layer->AddCell(UOdysseyLayerCellImageStagger::StaticClass(), cell->GetIndexInLayer());
        staggerCell->SetExposure(cell->GetExposure());
        layer->RemoveCell(cell);
    }
}

void
FOdysseyAnimationTimelineCellsShortcuts::Action_IncreaseCellExposure()
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

    if (layer->IsLockedRecursively())
        return;

    FOdysseyPainterEditorModule& painterEditorModule = FModuleManager::GetModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
    FOdysseyPainterEditor* editor = painterEditorModule.GetOpenedEditorForAsset(layer->GetAnimation());
    if (!editor)
        return;

    TArray<UOdysseyLayerCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
    {
        UOdysseyLayerCell* cell = layer->GetCellAtFrame(mCurrentFrame.Get());
        if (!cell)
            return;

        selectedCells.Add(cell);
    }

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline-cells.transaction.increase-selected-cells-exposure", "Increase Selected Cells Exposure"));
#endif

    mOnTransactCurrentFrame.ExecuteIfBound(selectedCells[0]->GetFrameRange().GetLowerBoundValue());
    for (UOdysseyLayerCell* selectedCell : selectedCells)
    {
        selectedCell->SetExposure(selectedCell->GetExposure() + 1);
    }
}

void
FOdysseyAnimationTimelineCellsShortcuts::Action_DecreaseCellExposure()
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

    if (layer->IsLockedRecursively())
        return;

    FOdysseyPainterEditorModule& painterEditorModule = FModuleManager::GetModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
    FOdysseyPainterEditor* editor = painterEditorModule.GetOpenedEditorForAsset(layer->GetAnimation());
    if (!editor)
        return;

    TArray<UOdysseyLayerCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
    {
        UOdysseyLayerCell* cell = layer->GetCellAtFrame(mCurrentFrame.Get());
        if (!cell)
            return;

        selectedCells.Add(cell);
    }

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline-cells.transaction.decrease-selected-cells-exposure", "Decrease Selected Cells Exposure"));
#endif
    mOnTransactCurrentFrame.ExecuteIfBound(selectedCells[0]->GetFrameRange().GetLowerBoundValue());
    for (UOdysseyLayerCell* selectedCell : selectedCells)
    {
        selectedCell->SetExposure(FMath::Max(1, selectedCell->GetExposure() - 1));
    }
}

void
FOdysseyAnimationTimelineCellsShortcuts::Action_SetCellExposure()
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

    if (layer->IsLockedRecursively())
        return;

    FOdysseyPainterEditorModule& painterEditorModule = FModuleManager::GetModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
    FOdysseyPainterEditor* editor = painterEditorModule.GetOpenedEditorForAsset(layer->GetAnimation());
    if (!editor)
        return;

    TArray<UOdysseyLayerCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
    {
        UOdysseyLayerCell* cell = layer->GetCellAtFrame(mCurrentFrame.Get());
        if (!cell)
            return;

        selectedCells.Add(cell);
    }

    int value = 1;
    SGenericDialogWidget::OpenDialog(
        LOCTEXT("timeline-cells.dialog.set-selected-cells-exposure.title", "Set Selected Cells Exposure"),

        SNew(SNumericEntryBox<int>)
        .Value_Lambda([&value]() { return value; })
        .AllowSpin(true)
        .Delta(1)
        .LinearDeltaSensitivity(10)
        .MinValue(1)
        .MaxValue(TOptional<int>())
        .MinSliderValue(1)
        .MaxSliderValue(TOptional<int>())
        .OnValueChanged_Lambda([&value](int iValue) { value = iValue; })
        .OnValueCommitted_Lambda([&value](int iValue, ETextCommit::Type iType) { value = iValue; })
        .LabelVAlign(VAlign_Center)
        .Label()
        [
            SNew(STextBlock)
            .Text(LOCTEXT("timeline-cells.dialog.set-selected-cells-exposure.label", "Exposure"))
        ],

        SGenericDialogWidget::FArguments()
        .UseScrollBox(false)
        .OnOkPressed_Lambda(
            [this, &value, &layer, &selectedCells]()
            {
                UOdysseyAnimation* animation = layer->GetAnimation();

                #ifdef WITH_EDITOR
                FScopedTransaction ScopedTransaction(LOCTEXT("timeline-cells.transaction.set-selected-cells-exposure", "Set Selected Cells Exposure"));
                #endif

                mOnTransactCurrentFrame.ExecuteIfBound(selectedCells[0]->GetFrameRange().GetLowerBoundValue());
                for (UOdysseyLayerCell* selectedCell : selectedCells)
                {
                    FOdysseyObjectEditorUtils::SetPropertyValue(selectedCell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Exposure), FMath::Max(1, value));
                }
            }
        ),

        true //As Modal
    );
}

bool
FOdysseyAnimationTimelineCellsShortcuts::CanAction_Copy()
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

    const TArray<UOdysseyLayerCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return false;

    return true;
}

bool
FOdysseyAnimationTimelineCellsShortcuts::CanAction_Cut()
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

    if (layer->IsLockedRecursively())
        return false;

    const TArray<UOdysseyLayerCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return false;

    return true;
}

bool
FOdysseyAnimationTimelineCellsShortcuts::CanAction_Paste()
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

    if (layer->IsLockedRecursively())
        return false;

    FOdysseyCoreEditorModule& odysseyCoreEditorModule = FModuleManager::Get().LoadModuleChecked<FOdysseyCoreEditorModule>(TEXT("OdysseyCoreEditor"));
    TSharedPtr<FOdysseyAnimationCellClipboardData> clipboardData = odysseyCoreEditorModule.GetClipboard()->GetData<FOdysseyAnimationCellClipboardData>();
    if (!clipboardData)
        return false;

    return clipboardData->CanPaste(layer);
}

bool
FOdysseyAnimationTimelineCellsShortcuts::CanAction_SelectAll()
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

    //Authorize SelectAll only if there is already an active selection
    const TArray<UOdysseyLayerCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return false;

    return true;
}

bool
FOdysseyAnimationTimelineCellsShortcuts::CanAction_Delete()
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

    if (layer->IsLockedRecursively())
        return false;

    const TArray<UOdysseyLayerCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return false;

    return true;
}

bool
FOdysseyAnimationTimelineCellsShortcuts::CanAction_ConvertToStaggerCell()
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

    if (layer->IsLockedRecursively())
        return false;

    FOdysseyPainterEditorModule& painterEditorModule = FModuleManager::GetModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
    FOdysseyPainterEditor* editor = painterEditorModule.GetOpenedEditorForAsset(layer->GetAnimation());
    if (!editor)
        return false;

    TArray<UOdysseyLayerCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
    {
        UOdysseyLayerCell* cell = layer->GetCellAtFrame(mCurrentFrame.Get());
        if (!cell)
            return false;

        selectedCells.Add(cell);
    }
    selectedCells = selectedCells.FilterByPredicate(
        [](UOdysseyLayerCell* iCell)
        {
            return !iCell->IsA<UOdysseyLayerCellImageStagger>();
        }
    );

    if (selectedCells.IsEmpty())
        return false;

    return true;
}

bool
FOdysseyAnimationTimelineCellsShortcuts::CanAction_IncreaseCellExposure()
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

    if (layer->IsLockedRecursively())
        return false;

    FOdysseyPainterEditorModule& painterEditorModule = FModuleManager::GetModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
    FOdysseyPainterEditor* editor = painterEditorModule.GetOpenedEditorForAsset(layer->GetAnimation());
    if (!editor)
        return false;

    TArray<UOdysseyLayerCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
    {
        UOdysseyLayerCell* cell = layer->GetCellAtFrame(mCurrentFrame.Get());
        if (!cell)
            return false;
    }

    return true;
}

bool
FOdysseyAnimationTimelineCellsShortcuts::CanAction_DecreaseCellExposure()
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

    if (layer->IsLockedRecursively())
        return false;

    FOdysseyPainterEditorModule& painterEditorModule = FModuleManager::GetModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
    FOdysseyPainterEditor* editor = painterEditorModule.GetOpenedEditorForAsset(layer->GetAnimation());
    if (!editor)
        return false;

    TArray<UOdysseyLayerCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
    {
        UOdysseyLayerCell* cell = layer->GetCellAtFrame(mCurrentFrame.Get());
        if (!cell)
            return false;
    }

    return true;
}

bool
FOdysseyAnimationTimelineCellsShortcuts::CanAction_SetCellExposure()
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

    if (layer->IsLockedRecursively())
        return false;

    FOdysseyPainterEditorModule& painterEditorModule = FModuleManager::GetModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
    FOdysseyPainterEditor* editor = painterEditorModule.GetOpenedEditorForAsset(layer->GetAnimation());
    if (!editor)
        return false;

    TArray<UOdysseyLayerCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
    {
        UOdysseyLayerCell* cell = layer->GetCellAtFrame(mCurrentFrame.Get());
        if (!cell)
            return false;
    }

    return true;
}

#undef LOCTEXT_NAMESPACE
