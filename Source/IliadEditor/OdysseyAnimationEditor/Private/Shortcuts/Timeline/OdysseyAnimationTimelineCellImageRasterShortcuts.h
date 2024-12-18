// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Framework/Commands/UICommandList.h"

class UOdysseyLayerStack;

class ODYSSEYANIMATIONEDITOR_API FOdysseyAnimationTimelineCellImageRasterShortcuts
{
public:
    FOdysseyAnimationTimelineCellImageRasterShortcuts(UOdysseyLayerStack* iLayerStack);

public:
    //Shortcuts
    void MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList);

public:
    //Common Shortcuts
    void Action_CrossFade();
    bool CanAction_CrossFade();

private:
    UOdysseyLayerStack* mLayerStack;
};
