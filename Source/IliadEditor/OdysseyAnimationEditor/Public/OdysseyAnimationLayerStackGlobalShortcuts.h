// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "Shortcuts/OdysseyLayerStackGlobalShortcuts.h"

/** Extends common layer shortcuts with animation lighttable controls. */
class ODYSSEYANIMATIONEDITOR_API FOdysseyAnimationLayerStackGlobalShortcuts : public FOdysseyLayerStackGlobalShortcuts
{
public:
    FOdysseyAnimationLayerStackGlobalShortcuts(TAttribute<UOdysseyLayerStack*> InLayerStack,
        TAttribute<UOdysseyLayer*> InLayer = TAttribute<UOdysseyLayer*>());

    void Action_ActivateLayerLighttable();
    void Action_InactivateLayerLighttable();
    void Action_ToggleLayerLighttable();
    virtual void MapActionsToCommandList(TSharedRef<FUICommandList> InCommandList) override;

private:
    void SetLayerLighttable(bool InValue);
};
