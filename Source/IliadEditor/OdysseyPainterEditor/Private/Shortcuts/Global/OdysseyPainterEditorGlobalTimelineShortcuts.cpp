// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Shortcuts/Global/OdysseyPainterEditorGlobalTimelineShortcuts.h"
#include "OdysseyPainterEditorAnimationCommands.h"
#include "OdysseyPainterEditor.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyAnimationCell.h"
#include "OdysseyAnimationPlayer.h"
#include "OdysseyPainterEditorAnimationUserSettings.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

FOdysseyPainterEditorGlobalTimelineShortcuts::FOdysseyPainterEditorGlobalTimelineShortcuts(FOdysseyPainterEditor* iEditor)
    : mEditor(iEditor)
{
}

void
FOdysseyPainterEditorGlobalTimelineShortcuts::MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList)
{
    //Navigation Actions
    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().NavigateToNextFrame,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalTimelineShortcuts::Action_NavigateToNextFrame)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().NavigateToPreviousFrame,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalTimelineShortcuts::Action_NavigateToPreviousFrame)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().NavigateToNextCell,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalTimelineShortcuts::Action_NavigateToNextCell)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().NavigateToPreviousCell,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalTimelineShortcuts::Action_NavigateToPreviousCell)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().NavigateToAnimationFirstFrame,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalTimelineShortcuts::Action_NavigateToAnimationFirstFrame)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().NavigateToAnimationLastFrame,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalTimelineShortcuts::Action_NavigateToAnimationLastFrame)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().Play,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalTimelineShortcuts::Action_Play)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().PlayStop,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalTimelineShortcuts::Action_PlayStop)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().PlayReversed,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalTimelineShortcuts::Action_PlayReversed)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().Stop,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalTimelineShortcuts::Action_Stop)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().ActivateLooping,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalTimelineShortcuts::Action_ActivateLooping)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().InactivateLooping,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalTimelineShortcuts::Action_InactivateLooping)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().ToggleLooping,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalTimelineShortcuts::Action_ToggleLooping)
    );

    for (int i = 0; i < FOdysseyPainterEditorAnimationCommands::Get().Flip.Num(); i++)
    {
        iCommandList->MapAction(
            FOdysseyPainterEditorAnimationCommands::Get().Flip[i],
            FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalTimelineShortcuts::Action_Flip, i )
        );
    }
}

void
FOdysseyPainterEditorGlobalTimelineShortcuts::Action_NavigateToNextFrame()
{
    UOdysseyAnimationPlayer* player = mEditor->GetAnimationPlayer();
    if (!player)
        return;

    player->SeekToFrame(player->GetCurrentFrame().FloorToFrame() + FFrameTime(1));
}

void
FOdysseyPainterEditorGlobalTimelineShortcuts::Action_NavigateToPreviousFrame()
{
    UOdysseyAnimationPlayer* player = mEditor->GetAnimationPlayer();
    if (!player)
        return;

    FFrameTime frame = player->GetCurrentFrame().FloorToFrame() - FFrameTime(1);
    if (frame < FFrameTime(0))
        return;

    player->SeekToFrame(frame);
}

void
FOdysseyPainterEditorGlobalTimelineShortcuts::Action_NavigateToNextCell()
{
    UOdysseyAnimation* animation = mEditor->GetAnimation();
    if (!animation)
        return;

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(mEditor->LayerStack());
    if (!layerStack)
        return;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->GetCurrentLayer());
    if (!currentLayer)
        return;

    if (currentLayer->GetCells().IsEmpty())
        return;

    UOdysseyAnimationPlayer* player = mEditor->GetAnimationPlayer();
    if (!player)
        return;

    int currentFrame = player->GetCurrentFrame().FrameNumber.Value;
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
        cellIndex = currentLayer->GetCellIndexAtFrame(currentFrame);
        if (cellIndex == INDEX_NONE)
            return;

        cellIndex++;
    }

    while(cellIndex < currentLayer->GetCells().Num() && !currentLayer->GetCells()[cellIndex])
    cellIndex++;

    if (cellIndex >= currentLayer->GetCells().Num())
        return;

    UOdysseyLayerCell* nextCell = currentLayer->GetCells()[cellIndex];
    FFrameTime frame(nextCell->GetFrameRange().GetLowerBoundValue());
    player->SeekToFrame(frame);
}

void
FOdysseyPainterEditorGlobalTimelineShortcuts::Action_NavigateToPreviousCell()
{
    UOdysseyAnimation* animation = mEditor->GetAnimation();
    if (!animation)
        return;

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(mEditor->LayerStack());
    if (!layerStack)
        return;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->GetCurrentLayer());
    if (!currentLayer)
        return;

    if (currentLayer->GetCells().IsEmpty())
        return;

    UOdysseyAnimationPlayer* player = mEditor->GetAnimationPlayer();
    if (!player)
        return;

    int currentFrame = player->GetCurrentFrame().FrameNumber.Value;
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
        cellIndex = currentLayer->GetCellIndexAtFrame(currentFrame);
        if (cellIndex == INDEX_NONE)
            return;

        cellIndex--;
    }

    while(cellIndex >= 0 && !currentLayer->GetCells()[cellIndex])
        cellIndex--;

    if (cellIndex < 0)
        return;

    UOdysseyLayerCell* prevCell = currentLayer->GetCells()[cellIndex];
    FFrameTime frame(prevCell->GetFrameRange().GetLowerBoundValue());
    player->SeekToFrame(frame);
}

void
FOdysseyPainterEditorGlobalTimelineShortcuts::Action_NavigateToAnimationFirstFrame()
{
    UOdysseyAnimation* animation = mEditor->GetAnimation();
    if (!animation)
        return;

    UOdysseyAnimationPlayer* player = mEditor->GetAnimationPlayer();
    if (!player)
        return;

    FInt32Range frameRange = animation->GetFrameRange();
    player->SeekToFrame(FFrameTime(frameRange.GetLowerBoundValue()));
}

void
FOdysseyPainterEditorGlobalTimelineShortcuts::Action_NavigateToAnimationLastFrame()
{
    UOdysseyAnimation* animation = mEditor->GetAnimation();
    if (!animation)
        return;

    UOdysseyAnimationPlayer* player = mEditor->GetAnimationPlayer();
    if (!player)
        return;

    FInt32Range frameRange = animation->GetFrameRange();
    player->SeekToFrame(FFrameTime(frameRange.GetUpperBoundValue()));
}

void
FOdysseyPainterEditorGlobalTimelineShortcuts::Action_Play()
{
    UOdysseyAnimationPlayer* player = mEditor->GetAnimationPlayer();
    if (!player)
        return;

    player->Play();
}

void
FOdysseyPainterEditorGlobalTimelineShortcuts::Action_PlayStop()
{
    UOdysseyAnimationPlayer* player = mEditor->GetAnimationPlayer();
    if (!player)
        return;

    if (player->GetStatus() == EOdysseyAnimationPlayerStatus::Playing )
        player->Stop();
    else
        player->Play();
}

void
FOdysseyPainterEditorGlobalTimelineShortcuts::Action_PlayReversed()
{
    UOdysseyAnimationPlayer* player = mEditor->GetAnimationPlayer();
    if (!player)
        return;
    player->Play(true);
}

void
FOdysseyPainterEditorGlobalTimelineShortcuts::Action_Stop()
{
    UOdysseyAnimationPlayer* player = mEditor->GetAnimationPlayer();
    if (!player)
        return;
    player->Stop();
}

void
FOdysseyPainterEditorGlobalTimelineShortcuts::Action_ActivateLooping()
{
    UOdysseyAnimationPlayer* player = mEditor->GetAnimationPlayer();
    if (!player)
        return;
    player->SetIsLooping(true);
}

void
FOdysseyPainterEditorGlobalTimelineShortcuts::Action_InactivateLooping()
{
    UOdysseyAnimationPlayer* player = mEditor->GetAnimationPlayer();
    if (!player)
        return;
    player->SetIsLooping(false);
}

void
FOdysseyPainterEditorGlobalTimelineShortcuts::Action_ToggleLooping()
{
    UOdysseyAnimationPlayer* player = mEditor->GetAnimationPlayer();
    if (!player)
        return;

    player->SetIsLooping(!player->IsLooping());
}

void
FOdysseyPainterEditorGlobalTimelineShortcuts::Action_Flip(int iConfigurationIndex)
{
    const UOdysseyPainterEditorAnimationUserSettings* settings = UOdysseyPainterEditorAnimationUserSettings::Get();
    mEditor->GetAnimationFlipSystem()->StartFlipping(settings->FlipConfigurations[iConfigurationIndex]);
}

#undef LOCTEXT_NAMESPACE
