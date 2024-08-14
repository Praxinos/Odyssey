// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Shortcuts/Global/OdysseyAnimationGlobalTimelineShortcuts.h"
#include "AnimationEditor/OdysseyAnimationEditorCommands.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyAnimation.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "OdysseyAnimationPlayer.h"
#include "OdysseyAnimationEditorUserSettings.h"

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
    
    iCommandList->MapAction(
        FOdysseyAnimationEditorCommands::Get().NavigateToAnimationFirstFrame,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_NavigateToAnimationFirstFrame)
    );
    
    iCommandList->MapAction(
        FOdysseyAnimationEditorCommands::Get().NavigateToAnimationLastFrame,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_NavigateToAnimationLastFrame)
    );
    
    iCommandList->MapAction(
        FOdysseyAnimationEditorCommands::Get().Play,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_Play)
    );
    
    iCommandList->MapAction(
        FOdysseyAnimationEditorCommands::Get().PlayStop,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_PlayStop)
    );
    
    iCommandList->MapAction(
        FOdysseyAnimationEditorCommands::Get().PlayReversed,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_PlayReversed)
    );
    
    iCommandList->MapAction(
        FOdysseyAnimationEditorCommands::Get().Stop,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_Stop)
    );
    
    iCommandList->MapAction(
        FOdysseyAnimationEditorCommands::Get().ActivateLooping,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_ActivateLooping)
    );
    
    iCommandList->MapAction(
        FOdysseyAnimationEditorCommands::Get().InactivateLooping,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_InactivateLooping)
    );
    
    iCommandList->MapAction(
        FOdysseyAnimationEditorCommands::Get().ToggleLooping,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_ToggleLooping)
    );

	for (int i = 0; i < FOdysseyAnimationEditorCommands::Get().Flip.Num(); i++)
    {
		iCommandList->MapAction(
			FOdysseyAnimationEditorCommands::Get().Flip[i],
			FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_Flip, i )
		);
	}
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

    int currentFrame = animation->CurrentFrame;
    FInt32Range frameRange = currentLayer->GetFrameRange();
    if (currentFrame > frameRange.GetUpperBoundValue())
        return;

    int cellIndex = INDEX_NONE;
    if (currentFrame < frameRange.GetLowerBoundValue())
    {
        cellIndex = 0;
    }
    else
    {
        UOdysseyAnimationCell* cell = currentLayer->GetCellAtFrame(currentFrame);
        if (!cell || cell == currentLayer->GetCells().Last())
            return;
        
        cellIndex = cell->IndexInLayer + 1;
    }

	UOdysseyAnimationCell* nextCell = currentLayer->GetCells()[cellIndex];
    FOdysseyObjectEditorUtils::SetPropertyValue(animation, "CurrentFrame", nextCell->GetFrameRange().GetLowerBoundValue());
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

    int currentFrame = animation->CurrentFrame;
    FInt32Range frameRange = currentLayer->GetFrameRange();
    if (currentFrame < frameRange.GetLowerBoundValue())
        return;

    int cellIndex = INDEX_NONE;
    if (currentFrame > frameRange.GetUpperBoundValue())
    {
        cellIndex = currentLayer->GetCells().Num() - 1;
    }
    else
    {
        UOdysseyAnimationCell* cell = currentLayer->GetCellAtFrame(currentFrame);
        if (!cell || cell == currentLayer->GetCells()[0])
            return;
        
        cellIndex = cell->IndexInLayer - 1;
    }

    UOdysseyAnimationCell* prevCell = currentLayer->GetCells()[cellIndex];
    FOdysseyObjectEditorUtils::SetPropertyValue(animation, "CurrentFrame", prevCell->GetFrameRange().GetLowerBoundValue());
}

void
FOdysseyAnimationGlobalTimelineShortcuts::Action_NavigateToAnimationFirstFrame()
{
    TSharedPtr<FOdysseyAnimationEditorExtension> extension = mExtension.Pin();
    if (!extension)
        return;

    UOdysseyAnimation* animation = extension->Animation();
    if (!animation)
        return;

    FInt32Range frameRange = animation->GetFrameRange();
    FOdysseyObjectEditorUtils::SetPropertyValue(animation, "CurrentFrame", frameRange.GetLowerBoundValue());
}

void
FOdysseyAnimationGlobalTimelineShortcuts::Action_NavigateToAnimationLastFrame()
{
    TSharedPtr<FOdysseyAnimationEditorExtension> extension = mExtension.Pin();
    if (!extension)
        return;

    UOdysseyAnimation* animation = extension->Animation();
    if (!animation)
        return;

    FInt32Range frameRange = animation->GetFrameRange();
    FOdysseyObjectEditorUtils::SetPropertyValue(animation, "CurrentFrame", frameRange.GetUpperBoundValue());
}

void
FOdysseyAnimationGlobalTimelineShortcuts::Action_Play()
{
    TSharedPtr<FOdysseyAnimationEditorExtension> extension = mExtension.Pin();
    if (!extension)
        return;

    UOdysseyAnimationPlayer* player = extension->Player();
    if (!player)
        return;
        
    player->Play();
}

void
FOdysseyAnimationGlobalTimelineShortcuts::Action_PlayStop()
{
    TSharedPtr<FOdysseyAnimationEditorExtension> extension = mExtension.Pin();
    if (!extension)
        return;

    UOdysseyAnimationPlayer* player = extension->Player();
    if (!player)
        return;

    if (player->Status == EOdysseyAnimationPlayerStatus::Playing )
        player->Stop();
    else
        player->Play();
}

void
FOdysseyAnimationGlobalTimelineShortcuts::Action_PlayReversed()
{
    TSharedPtr<FOdysseyAnimationEditorExtension> extension = mExtension.Pin();
    if (!extension)
        return;

    UOdysseyAnimationPlayer* player = extension->Player();
    if (!player)
        return;
    player->Play(true);
}

void
FOdysseyAnimationGlobalTimelineShortcuts::Action_Stop()
{
    TSharedPtr<FOdysseyAnimationEditorExtension> extension = mExtension.Pin();
    if (!extension)
        return;

    UOdysseyAnimationPlayer* player = extension->Player();
    if (!player)
        return;
    player->Stop();
}

void
FOdysseyAnimationGlobalTimelineShortcuts::Action_ActivateLooping()
{
    TSharedPtr<FOdysseyAnimationEditorExtension> extension = mExtension.Pin();
    if (!extension)
        return;

    UOdysseyAnimationPlayer* player = extension->Player();
    if (!player)
        return;
    FOdysseyObjectEditorUtils::SetPropertyValue(player, "IsLooping", true);
}

void
FOdysseyAnimationGlobalTimelineShortcuts::Action_InactivateLooping()
{
    TSharedPtr<FOdysseyAnimationEditorExtension> extension = mExtension.Pin();
    if (!extension)
        return;

    UOdysseyAnimationPlayer* player = extension->Player();
    if (!player)
        return;
    FOdysseyObjectEditorUtils::SetPropertyValue(player, "IsLooping", false);
}

void
FOdysseyAnimationGlobalTimelineShortcuts::Action_ToggleLooping()
{
    TSharedPtr<FOdysseyAnimationEditorExtension> extension = mExtension.Pin();
    if (!extension)
        return;

    UOdysseyAnimationPlayer* player = extension->Player();
    if (!player)
        return;
    FOdysseyObjectEditorUtils::SetPropertyValue(player, "IsLooping", !player->IsLooping);
}

void
FOdysseyAnimationGlobalTimelineShortcuts::Action_Flip(int iConfigurationIndex)
{
	TSharedPtr<FOdysseyAnimationEditorExtension> extension = mExtension.Pin();
    if (!extension)
        return;

	const UOdysseyAnimationEditorUserSettings* settings = UOdysseyAnimationEditorUserSettings::Get();
	extension->FlipSystem()->StartFlipping(settings->FlipConfigurations[iConfigurationIndex]);
}




#undef LOCTEXT_NAMESPACE
