// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Framework/Commands/UICommandList.h"

class UOdysseyLayerStack;
class FOdysseyAnimationEditorExtension;

class ODYSSEYANIMATIONEDITOR_API FOdysseyAnimationTimelineCellImageStaggerShortcuts
{
public:
    FOdysseyAnimationTimelineCellImageStaggerShortcuts(UOdysseyLayerStack* iLayerStack, FOdysseyAnimationEditorExtension* iAnimationExtension);

public:
    //Shortcuts
    void MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList);

public:
    //Common Shortcuts
    void Action_ConvertToReferenceCells();
    bool CanAction_ConvertToReferenceCells();

private:
    UOdysseyLayerStack* mLayerStack;
    FOdysseyAnimationEditorExtension* mAnimationExtension;
};
