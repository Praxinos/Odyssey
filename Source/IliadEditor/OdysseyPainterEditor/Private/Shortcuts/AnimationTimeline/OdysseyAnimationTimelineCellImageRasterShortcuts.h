// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Framework/Commands/UICommandList.h"

class UOdysseyAnimation;
class FOdysseyPainterEditor;

class ODYSSEYPAINTEREDITOR_API FOdysseyAnimationTimelineCellImageRasterShortcuts
{
public:
    FOdysseyAnimationTimelineCellImageRasterShortcuts(const TAttribute<UOdysseyAnimation*>& iAnimation);

public:
    //Shortcuts
    void MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList);

public:
    //Common Shortcuts
    void Action_CrossFade();
    bool CanAction_CrossFade();

private:
    TAttribute<UOdysseyAnimation*> mAnimation;
};
