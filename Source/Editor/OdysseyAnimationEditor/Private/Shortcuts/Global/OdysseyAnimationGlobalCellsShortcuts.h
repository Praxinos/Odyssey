// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

class ODYSSEYANIMATIONEDITOR_API FOdysseyAnimationGlobalCellsShortcuts
{
public:
    FOdysseyAnimationGlobalCellsShortcuts(TSharedPtr<FOdysseyAnimationEditorExtension> iExtension);

public:
    //Common Shortcuts
    void Action_BreakCell();
    bool CanAction_BreakCell();

public:
    //Shortcuts
    void MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList); 

private:
    TWeakPtr<FOdysseyAnimationEditorExtension> mExtension;
};