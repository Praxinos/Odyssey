// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Shortcuts/Timeline/OdysseyAnimationTimelineCellsShortcuts.h"

#include "Widgets/LayerStack/SOdysseyAnimationLayerStackTreeView.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "OdysseyAnimationEditorTimeline.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "LayerStack/Cells/OdysseyAnimationCellClipboardData.h"
#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStagger.h"
#include "Framework/Commands/GenericCommands.h"
#include "OdysseyEditorModule.h"
#include "Dialogs/Dialogs.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "AnimationEditor/OdysseyAnimationEditorCommands.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationCurrentFrameMutator.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

FOdysseyAnimationTimelineCellsShortcuts::FOdysseyAnimationTimelineCellsShortcuts(UOdysseyLayerStack* iLayerStack, FOdysseyAnimationEditorExtension* iAnimationExtension)
    : mLayerStack(iLayerStack)
    , mAnimationExtension(iAnimationExtension)
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
        FOdysseyAnimationEditorCommands::Get().ConvertToStaggerCell,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::Action_ConvertToStaggerCell),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::CanAction_ConvertToStaggerCell)
    );

    iCommandList->MapAction(
        FOdysseyAnimationEditorCommands::Get().IncreaseCellLength,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::Action_IncreaseCellLength),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::CanAction_IncreaseCellLength)
    );

    iCommandList->MapAction(
        FOdysseyAnimationEditorCommands::Get().DecreaseCellLength,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::Action_DecreaseCellLength),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::CanAction_DecreaseCellLength)
    );

    iCommandList->MapAction(
        FOdysseyAnimationEditorCommands::Get().SetCellLength,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::Action_SetCellLength),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::CanAction_SetCellLength)
    );
}

void
FOdysseyAnimationTimelineCellsShortcuts::Action_Copy()
{
    const TArray<UOdysseyAnimationCell*> selectedCells = mAnimationExtension->Timeline()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return;

    FOdysseyEditorModule& odysseyEditorModule = FModuleManager::Get().LoadModuleChecked<FOdysseyEditorModule>(TEXT("OdysseyEditor"));
    TSharedPtr<FOdysseyAnimationCellClipboardData> clipboardData = MakeShared<FOdysseyAnimationCellClipboardData>(selectedCells);
    odysseyEditorModule.GetClipboard()->SetData(clipboardData);
}

void
FOdysseyAnimationTimelineCellsShortcuts::Action_Cut()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return;

    if (layer->GetIsLockedRecursively())
    {
        Action_Copy();
        return;
    }

    const TArray<UOdysseyAnimationCell*> selectedCells = mAnimationExtension->Timeline()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline.shortcuts.cut-frame", "Cut Frames"));
#endif
    
    FOdysseyEditorModule& odysseyEditorModule = FModuleManager::Get().LoadModuleChecked<FOdysseyEditorModule>(TEXT("OdysseyEditor"));
    TSharedPtr<FOdysseyAnimationCellClipboardData> clipboardData = MakeShared<FOdysseyAnimationCellClipboardData>(selectedCells);
    odysseyEditorModule.GetClipboard()->SetData(clipboardData);
    Action_Delete();
}

void
FOdysseyAnimationTimelineCellsShortcuts::Action_Paste()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return;

    if (layer->GetIsLockedRecursively())
        return;

    FOdysseyEditorModule& odysseyEditorModule = FModuleManager::Get().LoadModuleChecked<FOdysseyEditorModule>(TEXT("OdysseyEditor"));
    TSharedPtr<FOdysseyAnimationCellClipboardData> clipboardData = odysseyEditorModule.GetClipboard()->GetData<FOdysseyAnimationCellClipboardData>();
    if (!clipboardData)
        return;
    
    if (!clipboardData->CanPaste(layer))
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline.shortcuts.paste-frame", "Paste Frames"));
#endif

    UOdysseyAnimation* animation = layer->GetAnimation();
    clipboardData->Paste(layer, animation->CurrentFrame);
    
    FOdysseyAnimationCurrentFrameMutator currentFrameMutator(animation);
    currentFrameMutator.Set(animation->CurrentFrame);
    currentFrameMutator.Commit();
}

void
FOdysseyAnimationTimelineCellsShortcuts::Action_SelectAll()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return;

    mAnimationExtension->Timeline()->SetSelectedCells(layer->GetCells());
}

void
FOdysseyAnimationTimelineCellsShortcuts::Action_Delete()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return;

    if (layer->GetIsLockedRecursively())
        return;

    const TArray<UOdysseyAnimationCell*> selectedCells = mAnimationExtension->Timeline()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline.shortcuts.remove-frame", "Remove Frames"));
#endif

    UOdysseyAnimation* animation = layer->GetAnimation();

    FOdysseyAnimationCurrentFrameMutator currentFrameMutator(animation);
    currentFrameMutator.Set(selectedCells[0]->GetFrameRange().GetLowerBoundValue());
    currentFrameMutator.Commit();

    layer->RemoveCells(selectedCells);
}



void
FOdysseyAnimationTimelineCellsShortcuts::Action_ConvertToStaggerCell()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return;

    if (layer->GetIsLockedRecursively())
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

    selectedCells = selectedCells.FilterByPredicate(
        [](UOdysseyAnimationCell* iCell)
        {
            return iCell->IsA<UOdysseyAnimationCellImageStagger>();
        }
    );

    if (selectedCells.IsEmpty())
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline-cells.transaction.create-stagger-cell", "Stagger Cell"));
#endif
    FOdysseyAnimationCurrentFrameMutator currentFrameMutator(animation);
    currentFrameMutator.Set(selectedCells[0]->GetFrameRange().GetLowerBoundValue());
    currentFrameMutator.Commit();

    for (UOdysseyAnimationCell* cell : selectedCells)
    {
		layer->AddCell(UOdysseyAnimationCellImageStagger::StaticClass(), cell->IndexInLayer);
		FOdysseyObjectEditorUtils::SetPropertyValue(cell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Length), cell->Length);
		layer->RemoveCell(cell);
    }
}

void
FOdysseyAnimationTimelineCellsShortcuts::Action_IncreaseCellLength()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return;

    if (layer->GetIsLockedRecursively())
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
        
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline-cells.transaction.increase-selected-cells-length", "Increase Selected Cells Length"));
#endif
    FOdysseyAnimationCurrentFrameMutator currentFrameMutator(animation);
    currentFrameMutator.Set(selectedCells[0]->GetFrameRange().GetLowerBoundValue());
    currentFrameMutator.Commit();

    for (UOdysseyAnimationCell* selectedCell : selectedCells)
    {
		FOdysseyObjectEditorUtils::SetPropertyValue(selectedCell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Length), selectedCell->Length + 1);
    }
}

void
FOdysseyAnimationTimelineCellsShortcuts::Action_DecreaseCellLength()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return;

    if (layer->GetIsLockedRecursively())
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
        
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline-cells.transaction.decrease-selected-cells-length", "Decrease Selected Cells Length"));
#endif
    FOdysseyAnimationCurrentFrameMutator currentFrameMutator(animation);
    currentFrameMutator.Set(selectedCells[0]->GetFrameRange().GetLowerBoundValue());
    currentFrameMutator.Commit();

    for (UOdysseyAnimationCell* selectedCell : selectedCells)
    {
		FOdysseyObjectEditorUtils::SetPropertyValue(selectedCell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Length), FMath::Max(1, selectedCell->Length - 1));
    }
}

void
FOdysseyAnimationTimelineCellsShortcuts::Action_SetCellLength()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return;

    if (layer->GetIsLockedRecursively())
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

    int value = 1;
    SGenericDialogWidget::OpenDialog(
        LOCTEXT("timeline-cells.dialog.set-selected-cells-length.title", "Set Selected Cells Length"),
        
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
            .Text(LOCTEXT("timeline-cells.dialog.set-selected-cells-length.label", "Length"))
        ],

        SGenericDialogWidget::FArguments()
        .UseScrollBox(false)
        .OnOkPressed_Lambda(
            [&value, &layer, &selectedCells]()
            {
                UOdysseyAnimation* animation = layer->GetAnimation();

                #ifdef WITH_EDITOR
                FScopedTransaction ScopedTransaction(LOCTEXT("timeline-cells.transaction.set-selected-cells-length", "Set Selected Cells Length"));
                #endif

                FOdysseyAnimationCurrentFrameMutator currentFrameMutator(animation);
                currentFrameMutator.Set(selectedCells[0]->GetFrameRange().GetLowerBoundValue());
                currentFrameMutator.Commit();

                for (UOdysseyAnimationCell* selectedCell : selectedCells)
                {
					FOdysseyObjectEditorUtils::SetPropertyValue(selectedCell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Length), FMath::Max(1, value));
                }
            }
        ),

        true //As Modal
    );
}

bool
FOdysseyAnimationTimelineCellsShortcuts::CanAction_Copy()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return false;

    const TArray<UOdysseyAnimationCell*> selectedCells = mAnimationExtension->Timeline()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return false;

    return true;
}

bool
FOdysseyAnimationTimelineCellsShortcuts::CanAction_Cut()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return false;

    if (layer->GetIsLockedRecursively())
        return false;

    const TArray<UOdysseyAnimationCell*> selectedCells = mAnimationExtension->Timeline()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return false;

    return true;
}

bool
FOdysseyAnimationTimelineCellsShortcuts::CanAction_Paste()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return false;

    if (layer->GetIsLockedRecursively())
        return false;

    FOdysseyEditorModule& odysseyEditorModule = FModuleManager::Get().LoadModuleChecked<FOdysseyEditorModule>(TEXT("OdysseyEditor"));
    TSharedPtr<FOdysseyAnimationCellClipboardData> clipboardData = odysseyEditorModule.GetClipboard()->GetData<FOdysseyAnimationCellClipboardData>();
    if (!clipboardData)
        return false;

    return clipboardData->CanPaste(layer);
}

bool
FOdysseyAnimationTimelineCellsShortcuts::CanAction_SelectAll()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return false;
        
    //Authorize SelectAll only if there is already an active selection
    const TArray<UOdysseyAnimationCell*> selectedCells = mAnimationExtension->Timeline()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return false;

    return true;
}

bool
FOdysseyAnimationTimelineCellsShortcuts::CanAction_Delete()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return false;

    if (layer->GetIsLockedRecursively())
        return false;

    const TArray<UOdysseyAnimationCell*> selectedCells = mAnimationExtension->Timeline()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return false;

    return true;
}

bool
FOdysseyAnimationTimelineCellsShortcuts::CanAction_ConvertToStaggerCell()
{
    
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return false;

    if (layer->GetIsLockedRecursively())
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
            
        selectedCells.Add(cell);
    }
    selectedCells = selectedCells.FilterByPredicate(
        [](UOdysseyAnimationCell* iCell)
        {
            return iCell->IsA<UOdysseyAnimationCellImageStagger>();
        }
    );

    if (selectedCells.IsEmpty())
        return false;

    return true;
}

bool
FOdysseyAnimationTimelineCellsShortcuts::CanAction_IncreaseCellLength()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return false;

    if (layer->GetIsLockedRecursively())
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

    return true;
}

bool
FOdysseyAnimationTimelineCellsShortcuts::CanAction_DecreaseCellLength()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return false;

    if (layer->GetIsLockedRecursively())
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

    return true;
}

bool
FOdysseyAnimationTimelineCellsShortcuts::CanAction_SetCellLength()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return false;

    if (layer->GetIsLockedRecursively())
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

    return true;
}

#undef LOCTEXT_NAMESPACE