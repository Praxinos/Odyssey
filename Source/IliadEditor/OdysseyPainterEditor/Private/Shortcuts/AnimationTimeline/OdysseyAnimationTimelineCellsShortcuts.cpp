// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Shortcuts/AnimationTimeline/OdysseyAnimationTimelineCellsShortcuts.h"

#include "Algo/Accumulate.h"

#include "OdysseyAnimationLayerStack.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyAnimationCell.h"
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
        FOdysseyPainterEditorAnimationCommands::Get().CreateStaggerCellLoop,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::Action_CreateStaggerCell, EOdysseyLayerCellImageStaggerBehaviour::Loop),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::CanAction_CreateStaggerCell)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().CreateStaggerCellPingPong,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::Action_CreateStaggerCell, EOdysseyLayerCellImageStaggerBehaviour::PingPong),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::CanAction_CreateStaggerCell)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().CreateStaggerCellRandom,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::Action_CreateStaggerCell, EOdysseyLayerCellImageStaggerBehaviour::Random),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::CanAction_CreateStaggerCell)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().ReverseSelectedCells,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::Action_ReverseSelectedCells),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::CanAction_ReverseSelectedCells)
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

    if (!layer->IsEditable())
    {
        Action_Copy();
        return;
    }

    const TArray<UOdysseyLayerCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return;

#if WITH_EDITOR
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

    if (!layer->IsEditable())
        return;

    FOdysseyCoreEditorModule& odysseyCoreEditorModule = FModuleManager::Get().LoadModuleChecked<FOdysseyCoreEditorModule>(TEXT("OdysseyCoreEditor"));
    TSharedPtr<FOdysseyAnimationCellClipboardData> clipboardData = odysseyCoreEditorModule.GetClipboard()->GetData<FOdysseyAnimationCellClipboardData>();
    if (!clipboardData)
        return;

    if (!clipboardData->CanPaste(layer))
        return;

#if WITH_EDITOR
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

    if (!layer->IsEditable())
        return;

    const TArray<UOdysseyLayerCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return;

#if WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline.shortcuts.remove-frame", "Remove Frames"));
#endif
    mOnTransactCurrentFrame.ExecuteIfBound(selectedCells[0]->GetFrameRange().GetLowerBoundValue());
    layer->RemoveCells(selectedCells);
    if (layer->GetCells().IsEmpty())
        layer->AddCell(layer->GetDefaultCellClass());
}



void
FOdysseyAnimationTimelineCellsShortcuts::Action_CreateStaggerCell( EOdysseyLayerCellImageStaggerBehaviour iBehavior )
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

    TArray<TArray<UOdysseyLayerCell*>> selectedCellGroups = UOdysseyLayer::GetCellsContiguousGroups( selectedCells );

#if WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline-cells.transaction.create-stagger-cell", "Stagger Cell"));
#endif

    mOnTransactCurrentFrame.ExecuteIfBound(selectedCells[0]->GetFrameRange().GetLowerBoundValue());
    for( TArray<UOdysseyLayerCell*> selectedCellGroup : selectedCellGroups )
    {
        int exposure = Algo::TransformAccumulate( selectedCellGroup,
                                                []( UOdysseyLayerCell* iCell )
                                                {
                                                    return iCell->GetExposure();
                                                },
                                                0 );
        int reach = exposure;

        if( iBehavior == EOdysseyLayerCellImageStaggerBehaviour::PingPong )
            // -1 because for ping-pong the last exposure of the cell is not inside the loop (cell exposure == 10, then ping exposure == 9, ...)
            // Max( 1, ...) because exposure can be 0 (if exposure == 1 and due to -1 before) and so to display something in ping-pong, set exposure to 1
            exposure = FMath::Max( 1, ( exposure - 1 ) * 2 );

        UOdysseyLayerCellImageStagger* staggerCell = Cast<UOdysseyLayerCellImageStagger>( layer->AddCell(UOdysseyLayerCellImageStagger::StaticClass(), selectedCellGroup.Last()->GetIndexInLayer() + 1) );
        staggerCell->SetExposure( exposure );
        staggerCell->SetBehaviour( iBehavior );
        staggerCell->SetReach( reach, false );
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

    if (!layer->IsEditable())
        return;

    TArray<UOdysseyLayerCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
    {
        UOdysseyLayerCell* cell = layer->GetCellAtFrame(mCurrentFrame.Get());
        if (!cell)
            return;

        selectedCells.Add(cell);
    }

#if WITH_EDITOR
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

    if (!layer->IsEditable())
        return;

    TArray<UOdysseyLayerCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
    {
        UOdysseyLayerCell* cell = layer->GetCellAtFrame(mCurrentFrame.Get());
        if (!cell)
            return;

        selectedCells.Add(cell);
    }

#if WITH_EDITOR
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

    if (!layer->IsEditable())
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
#if WITH_EDITOR
                FScopedTransaction ScopedTransaction(LOCTEXT("timeline-cells.transaction.set-selected-cells-exposure", "Set Selected Cells Exposure"));
#endif

                mOnTransactCurrentFrame.ExecuteIfBound(selectedCells[0]->GetFrameRange().GetLowerBoundValue());
                for (UOdysseyLayerCell* selectedCell : selectedCells)
                {
                    selectedCell->SetExposure( FMath::Max( 1, value ) );
                }
            }
        ),

        true //As Modal
    );
}

void
FOdysseyAnimationTimelineCellsShortcuts::Action_ReverseSelectedCells()
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

    TSharedRef<FOdysseyLayerCellSelection> cellSelection = layer->GetLayerStack()->GetCellSelection();
    TArray<UOdysseyLayerCell*> selectedCells = cellSelection.Get().GetSelectedCells();

#if WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline.shortcuts.reverse-selected-cells", "Reverse Selected Cells"));
#endif
    layer->ReverseCells( selectedCells );
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

    if (!layer->IsEditable())
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

    if (!layer->IsEditable())
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

    if (!layer->IsEditable())
        return false;

    const TArray<UOdysseyLayerCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return false;

    return true;
}

bool
FOdysseyAnimationTimelineCellsShortcuts::CanAction_CreateStaggerCell()
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

    if (!layer->IsEditable())
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

    if (!layer->IsEditable())
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

    if (!layer->IsEditable())
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
FOdysseyAnimationTimelineCellsShortcuts::CanAction_ReverseSelectedCells()
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

    TSharedRef<FOdysseyLayerCellSelection> cellSelection = layer->GetLayerStack()->GetCellSelection();
    TArray<UOdysseyLayerCell*> selectedCells = cellSelection.Get().GetSelectedCells();

    return UOdysseyAnimationLayer::AreCellsContiguous( selectedCells ) && ( selectedCells.Num() > 1 ) ? true
                                                                                                      : false;
}

#undef LOCTEXT_NAMESPACE
