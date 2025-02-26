// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Shortcuts/Global/OdysseyAnimationGlobalTimelineShortcuts.h"
#include "OdysseyPainterEditorAnimationCommands.h"
#include "OdysseyPainterEditor.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyAnimation.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "OdysseyAnimationPlayer.h"
#include "OdysseyPainterEditorAnimationUserSettings.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

FOdysseyAnimationGlobalTimelineShortcuts::FOdysseyAnimationGlobalTimelineShortcuts(FOdysseyPainterEditor* iEditor)
    : mEditor(iEditor)
{
}

void
FOdysseyAnimationGlobalTimelineShortcuts::MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList)
{
    //Navigation Actions
    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().NavigateToNextFrame,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_NavigateToNextFrame)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().NavigateToPreviousFrame,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_NavigateToPreviousFrame)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().NavigateToNextCell,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_NavigateToNextCell)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().NavigateToPreviousCell,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_NavigateToPreviousCell)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().NavigateToAnimationFirstFrame,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_NavigateToAnimationFirstFrame)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().NavigateToAnimationLastFrame,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_NavigateToAnimationLastFrame)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().Play,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_Play)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().PlayStop,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_PlayStop)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().PlayReversed,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_PlayReversed)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().Stop,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_Stop)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().ActivateLooping,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_ActivateLooping)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().InactivateLooping,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_InactivateLooping)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().ToggleLooping,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_ToggleLooping)
    );

    for (int i = 0; i < FOdysseyPainterEditorAnimationCommands::Get().Flip.Num(); i++)
    {
        iCommandList->MapAction(
            FOdysseyPainterEditorAnimationCommands::Get().Flip[i],
            FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_Flip, i )
        );
    }
}

void
FOdysseyAnimationGlobalTimelineShortcuts::Action_NavigateToNextFrame()
{
    UOdysseyAnimation* animation = mEditor->GetAnimation();
    if (!animation)
        return;

    int currentFrame = animation->CurrentFrame + 1;
    FOdysseyObjectEditorUtils::SetPropertyValue(animation, GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, CurrentFrame), currentFrame);
}

void
FOdysseyAnimationGlobalTimelineShortcuts::Action_NavigateToPreviousFrame()
{
    UOdysseyAnimation* animation = mEditor->GetAnimation();
    if (!animation)
        return;

    int currentFrame = animation->CurrentFrame - 1;
    if (currentFrame < 0)
        return;

    FOdysseyObjectEditorUtils::SetPropertyValue(animation, GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, CurrentFrame), currentFrame);
}

void
FOdysseyAnimationGlobalTimelineShortcuts::Action_NavigateToNextCell()
{
    UOdysseyAnimation* animation = mEditor->GetAnimation();
    if (!animation)
        return;

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(mEditor->LayerStack());
    if (!layerStack)
        return;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->CurrentLayer.Get());
    if (!currentLayer)
        return;

    if (currentLayer->GetCells().IsEmpty())
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
    FOdysseyObjectEditorUtils::SetPropertyValue(animation, GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, CurrentFrame), nextCell->GetFrameRange().GetLowerBoundValue());
}

void
FOdysseyAnimationGlobalTimelineShortcuts::Action_NavigateToPreviousCell()
{
    UOdysseyAnimation* animation = mEditor->GetAnimation();
    if (!animation)
        return;

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(mEditor->LayerStack());
    if (!layerStack)
        return;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->CurrentLayer.Get());
    if (!currentLayer)
        return;

    if (currentLayer->GetCells().IsEmpty())
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
    FOdysseyObjectEditorUtils::SetPropertyValue(animation, GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, CurrentFrame), prevCell->GetFrameRange().GetLowerBoundValue());
}

void
FOdysseyAnimationGlobalTimelineShortcuts::Action_NavigateToAnimationFirstFrame()
{
    UOdysseyAnimation* animation = mEditor->GetAnimation();
    if (!animation)
        return;

    FInt32Range frameRange = animation->GetFrameRange();
    FOdysseyObjectEditorUtils::SetPropertyValue(animation, GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, CurrentFrame), frameRange.GetLowerBoundValue());
}

void
FOdysseyAnimationGlobalTimelineShortcuts::Action_NavigateToAnimationLastFrame()
{
    UOdysseyAnimation* animation = mEditor->GetAnimation();
    if (!animation)
        return;

    FInt32Range frameRange = animation->GetFrameRange();
    FOdysseyObjectEditorUtils::SetPropertyValue(animation, GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, CurrentFrame), frameRange.GetUpperBoundValue());
}

void
FOdysseyAnimationGlobalTimelineShortcuts::Action_Play()
{
    UOdysseyAnimationPlayer* player = mEditor->GetAnimationPlayer();
    if (!player)
        return;

    player->Play();
}

void
FOdysseyAnimationGlobalTimelineShortcuts::Action_PlayStop()
{
    UOdysseyAnimationPlayer* player = mEditor->GetAnimationPlayer();
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
    UOdysseyAnimationPlayer* player = mEditor->GetAnimationPlayer();
    if (!player)
        return;
    player->Play(true);
}

void
FOdysseyAnimationGlobalTimelineShortcuts::Action_Stop()
{
    UOdysseyAnimationPlayer* player = mEditor->GetAnimationPlayer();
    if (!player)
        return;
    player->Stop();
}

void
FOdysseyAnimationGlobalTimelineShortcuts::Action_ActivateLooping()
{
    UOdysseyAnimationPlayer* player = mEditor->GetAnimationPlayer();
    if (!player)
        return;
    FOdysseyObjectEditorUtils::SetPropertyValue(player, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationPlayer, IsLooping), true);
}

void
FOdysseyAnimationGlobalTimelineShortcuts::Action_InactivateLooping()
{
    UOdysseyAnimationPlayer* player = mEditor->GetAnimationPlayer();
    if (!player)
        return;
    FOdysseyObjectEditorUtils::SetPropertyValue(player, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationPlayer, IsLooping), false);
}

void
FOdysseyAnimationGlobalTimelineShortcuts::Action_ToggleLooping()
{
    UOdysseyAnimationPlayer* player = mEditor->GetAnimationPlayer();
    if (!player)
        return;
    FOdysseyObjectEditorUtils::SetPropertyValue(player, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationPlayer, IsLooping), !player->IsLooping);
}

void
FOdysseyAnimationGlobalTimelineShortcuts::Action_Flip(int iConfigurationIndex)
{
    const UOdysseyPainterEditorAnimationUserSettings* settings = UOdysseyPainterEditorAnimationUserSettings::Get();
    mEditor->GetAnimationFlipSystem()->StartFlipping(settings->FlipConfigurations[iConfigurationIndex]);
}




#undef LOCTEXT_NAMESPACE
