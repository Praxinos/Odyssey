// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Shortcuts/Global/OdysseyAnimationGlobalTimelineShortcuts.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

FOdysseyAnimationGlobalTimelineShortcuts::FOdysseyAnimationGlobalTimelineShortcuts(TSharedPtr<FOdysseyAnimationEditorExtension> iExtension)
    : mExtension(iExtension)
{
}

void
FOdysseyAnimationGlobalTimelineShortcuts::MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList)
{
    //Tools actions
    iCommandList->MapAction(
        FOdysseyAnimationEditorCommands::Get().ActivateTimelineSelectionTool,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_ActivateTimelineSelectionTool)
    );

    iCommandList->MapAction(
        FOdysseyAnimationEditorCommands::Get().ActivateTimelineMoveTool,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_ActivateTimelineMoveTool)
    );
    
    iCommandList->MapAction(
        FOdysseyAnimationEditorCommands::Get().ActivateTimelineCutTool,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_ActivateTimelineCutTool)
    );

    //Navigation Actions

    
    iCommandList->MapAction(
        FOdysseyAnimationEditorCommands::Get().NavigateToNextFrame,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_NavigateToNextFrame)
    );

    iCommandList->MapAction(
        FOdysseyAnimationEditorCommands::Get().NavigateToPreviousFrame,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_NavigateToPreviousFrame)
    );
    
    iCommandList->MapAction(
        FOdysseyAnimationEditorCommands::Get().NavigateToNextCell,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_NavigateToNextCell)
    );
    
    iCommandList->MapAction(
        FOdysseyAnimationEditorCommands::Get().NavigateToPreviousCell,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_NavigateToPreviousCell)
    );
}

void
FOdysseyAnimationGlobalTimelineShortcuts::Action_ActivateTimelineSelectionTool()
{
    TSharedPtr<FOdysseyAnimationEditorExtension> extension = mExtension.Pin();
    if (!extension)
        return;

    extension->Timeline()->SetSelectedTool(EOdysseyTimelineTool::Selection);
}

void
FOdysseyAnimationGlobalTimelineShortcuts::Action_ActivateTimelineMoveTool()
{
    TSharedPtr<FOdysseyAnimationEditorExtension> extension = mExtension.Pin();
    if (!extension)
        return;

    extension->Timeline()->SetSelectedTool(EOdysseyTimelineTool::Move);
}

void
FOdysseyAnimationGlobalTimelineShortcuts::Action_ActivateTimelineCutTool()
{
    TSharedPtr<FOdysseyAnimationEditorExtension> extension = mExtension.Pin();
    if (!extension)
        return;

    extension->Timeline()->SetSelectedTool(EOdysseyTimelineTool::Cut);
}

void
FOdysseyAnimationGlobalTimelineShortcuts::Action_NavigateToNextFrame()
{
    TSharedPtr<FOdysseyAnimationEditorExtension> extension = mExtension.Pin();
    if (!extension)
        return;

    UOdysseyAnimation* animation = extension->Animation();
    if (!animation)
        return;

    int currentFrame = animation->CurrentFrame + 1;
    FOdysseyObjectEditorUtils::SetPropertyValue(animation, "CurrentFrame", currentFrame);
}

void
FOdysseyAnimationGlobalTimelineShortcuts::Action_NavigateToPreviousFrame()
{
    TSharedPtr<FOdysseyAnimationEditorExtension> extension = mExtension.Pin();
    if (!extension)
        return;

    UOdysseyAnimation* animation = extension->Animation();
    if (!animation)
        return;

    int currentFrame = animation->CurrentFrame - 1;
    if (currentFrame < 0)
        return;

    FOdysseyObjectEditorUtils::SetPropertyValue(animation, "CurrentFrame", currentFrame);
}

void
FOdysseyAnimationGlobalTimelineShortcuts::Action_NavigateToNextCell()
{
    TSharedPtr<FOdysseyAnimationEditorExtension> extension = mExtension.Pin();
    if (!extension)
        return;

    UOdysseyAnimation* animation = extension->Animation();
    if (!animation)
        return;

    UOdysseyAnimationLayerStack* layerStack = extension->LayerStack();
    if (!layerStack)
        return;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->CurrentLayer.Get());
    if (!currentLayer)
        return;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = currentLayer->GetCellsContainer();
    if (!cellsContainer)
        return;

    int currentFrame = animation->CurrentFrame;
    FInt32Range frameRange = cellsContainer->GetFrameRange();
    if (currentFrame > frameRange.GetUpperBoundValue())
        return;

    int cellIndex = INDEX_NONE;
    if (currentFrame < frameRange.GetLowerBoundValue())
    {
        cellIndex = 0;
    }
    else
    {
        cellIndex = cellsContainer->GetCellIndexAtFrame(currentFrame);
        if (cellIndex == INDEX_NONE || cellIndex == cellsContainer->GetCells().Num() - 1)
            return;
        
        cellIndex++;
    }

    int nextCellFrame = cellsContainer->GetCellFrame(cellsContainer->GetCells()[cellIndex]);
    FOdysseyObjectEditorUtils::SetPropertyValue(animation, "CurrentFrame", nextCellFrame);
}

void
FOdysseyAnimationGlobalTimelineShortcuts::Action_NavigateToPreviousCell()
{
    TSharedPtr<FOdysseyAnimationEditorExtension> extension = mExtension.Pin();
    if (!extension)
        return;

    UOdysseyAnimation* animation = extension->Animation();
    if (!animation)
        return;

    UOdysseyAnimationLayerStack* layerStack = extension->LayerStack();
    if (!layerStack)
        return;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->CurrentLayer.Get());
    if (!currentLayer)
        return;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = currentLayer->GetCellsContainer();
    if (!cellsContainer)
        return;

    int currentFrame = animation->CurrentFrame;
    FInt32Range frameRange = cellsContainer->GetFrameRange();
    if (currentFrame < frameRange.GetLowerBoundValue())
        return;

    int cellIndex = INDEX_NONE;
    if (currentFrame > frameRange.GetUpperBoundValue())
    {
        cellIndex = cellsContainer->GetCells().Num() - 1;
    }
    else
    {
        cellIndex = cellsContainer->GetCellIndexAtFrame(currentFrame);
        if (cellIndex == INDEX_NONE || cellIndex == 0)
            return;
        
        cellIndex--;
    }

    int previousCellFrame = cellsContainer->GetCellFrame(cellsContainer->GetCells()[cellIndex]);
    FOdysseyObjectEditorUtils::SetPropertyValue(animation, "CurrentFrame", previousCellFrame);
}

#undef LOCTEXT_NAMESPACE