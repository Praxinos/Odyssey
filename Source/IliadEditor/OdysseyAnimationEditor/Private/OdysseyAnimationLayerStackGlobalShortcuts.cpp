// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationLayerStackGlobalShortcuts.h"

#include "OdysseyAnimationLayer.h"
#include "OdysseyAnimationLayerStackEditorCommands.h"

FOdysseyAnimationLayerStackGlobalShortcuts::FOdysseyAnimationLayerStackGlobalShortcuts(
    TAttribute<UOdysseyLayerStack*> InLayerStack, TAttribute<UOdysseyLayer*> InLayer)
    : FOdysseyLayerStackGlobalShortcuts(InLayerStack, InLayer)
{
}

void
FOdysseyAnimationLayerStackGlobalShortcuts::MapActionsToCommandList(TSharedRef<FUICommandList> InCommandList)
{
    FOdysseyLayerStackGlobalShortcuts::MapActionsToCommandList(InCommandList);
    InCommandList->MapAction(
        FOdysseyAnimationLayerStackEditorCommands::Get().ActivateLayerLighttable,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationLayerStackGlobalShortcuts::Action_ActivateLayerLighttable)
    );

    InCommandList->MapAction(
        FOdysseyAnimationLayerStackEditorCommands::Get().InactivateLayerLighttable,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationLayerStackGlobalShortcuts::Action_InactivateLayerLighttable)
    );

    InCommandList->MapAction(
        FOdysseyAnimationLayerStackEditorCommands::Get().ToggleLayerLighttable,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationLayerStackGlobalShortcuts::Action_ToggleLayerLighttable)
    );

}

void
FOdysseyAnimationLayerStackGlobalShortcuts::SetLayerLighttable(bool InValue)
{
    TArray<UOdysseyLayer*> selectedLayers;
    GetSelectedLayers(selectedLayers);

    if (selectedLayers.IsEmpty())
        return;

    for( UOdysseyLayer* layer : selectedLayers )
    {
        UOdysseyAnimationLayer* AnimationLayer = Cast<UOdysseyAnimationLayer>(layer);
        if (!AnimationLayer)
            continue;
        FOdysseyLighttable LT = AnimationLayer->GetLighttable();
        LT.bIsActivated = InValue;
        AnimationLayer->SetLighttable(LT);
    }
}

void
FOdysseyAnimationLayerStackGlobalShortcuts::Action_ActivateLayerLighttable()
{
    SetLayerLighttable(true);
}

void
FOdysseyAnimationLayerStackGlobalShortcuts::Action_InactivateLayerLighttable()
{
    SetLayerLighttable(false);
}

void
FOdysseyAnimationLayerStackGlobalShortcuts::Action_ToggleLayerLighttable()
{
    UOdysseyLayer* FocusedLayer = GetFocusedLayer();
    if (!FocusedLayer)
        return;

    UOdysseyAnimationLayer* AnimationLayer = Cast<UOdysseyAnimationLayer>(FocusedLayer);
    if (!AnimationLayer)
        return;
    bool Value = !AnimationLayer->GetLighttable().bIsActivated;
    SetLayerLighttable(Value);
}
