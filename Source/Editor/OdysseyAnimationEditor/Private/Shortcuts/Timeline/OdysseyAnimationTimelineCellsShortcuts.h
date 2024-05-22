// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

class FOdysseyAnimationEditorExtension;
class UOdysseyLayerStack;

class ODYSSEYANIMATIONEDITOR_API FOdysseyAnimationTimelineCellsShortcuts
{
public:
    FOdysseyAnimationTimelineCellsShortcuts(UOdysseyLayerStack* iLayerStack, FOdysseyAnimationEditorExtension* iAnimationExtension);

public:
    //Shortcuts
    void MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList);

public:
    //Common Shortcuts
    void Action_Copy();
    void Action_Cut();
    void Action_Paste();
    void Action_SelectAll();
    void Action_Delete();
    void Action_ConvertToStaggerCell();
    void Action_IncreaseCellLength();
    void Action_DecreaseCellLength();
    void Action_SetCellLength();

    bool CanAction_Copy();
    bool CanAction_Cut();
    bool CanAction_Paste();
    bool CanAction_SelectAll();
    bool CanAction_Delete();
    bool CanAction_ConvertToStaggerCell();
    bool CanAction_IncreaseCellLength();
    bool CanAction_DecreaseCellLength();
    bool CanAction_SetCellLength();

private:
    UOdysseyLayerStack* mLayerStack;
    FOdysseyAnimationEditorExtension* mAnimationExtension;
};