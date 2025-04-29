// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditorShortcuts.h"

class FOdysseyPainterEditor;
class UOdysseyAnimation;

class ODYSSEYPAINTEREDITOR_API FOdysseyAnimationGlobalCellsShortcuts
    : public IOdysseyEditorShortcuts
{
public:
    DECLARE_DELEGATE_OneParam(FOnTransactCurrentFrame, TOptional<int> /*iFrame*/)

public:
    virtual ~FOdysseyAnimationGlobalCellsShortcuts() {};
    FOdysseyAnimationGlobalCellsShortcuts(
        const TAttribute<UOdysseyAnimation*>& iAnimation,
        const TAttribute<int>& iCurrentFrame,
        const FOnTransactCurrentFrame& iOnTransactCurrentFrame
    );

public:
    //Common Shortcuts
    void Action_BreakCell();
    bool CanAction_BreakCell();

    void Action_BreakAndClearCell();
    bool CanAction_BreakAndClearCell();

    void Action_RemoveCellMark();
    bool CanAction_RemoveCellMark();

    void Action_SetCellMark(int iMarkId);
    bool CanAction_SetCellMark(int iMarkId);

public:
    //Shortcuts
    virtual void MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList) override;

private:
    TAttribute<UOdysseyAnimation*> mAnimation;
    TAttribute<int> mCurrentFrame;
    FOnTransactCurrentFrame mOnTransactCurrentFrame;
};
