// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditorShortcuts.h"

class FOdysseyAnimationEditorExtension;

class ODYSSEYANIMATIONEDITOR_API FOdysseyAnimationGlobalCellsShortcuts
    : public IOdysseyEditorShortcuts
{
public:
    virtual ~FOdysseyAnimationGlobalCellsShortcuts() {};
    FOdysseyAnimationGlobalCellsShortcuts(TSharedPtr<FOdysseyAnimationEditorExtension> iExtension);

public:
    //Common Shortcuts
    void Action_BreakCell();
    bool CanAction_BreakCell();

    void Action_RemoveCellMark();
    bool CanAction_RemoveCellMark();

    void Action_SetCellMark(int iMarkId);
    bool CanAction_SetCellMark(int iMarkId);

public:
    //Shortcuts
    virtual void MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList) override;

private:
    TWeakPtr<FOdysseyAnimationEditorExtension> mExtension;
};
