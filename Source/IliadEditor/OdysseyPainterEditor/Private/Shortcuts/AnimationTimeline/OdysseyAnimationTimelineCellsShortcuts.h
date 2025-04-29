// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

class UOdysseyAnimation;
class ODYSSEYPAINTEREDITOR_API FOdysseyAnimationTimelineCellsShortcuts
{
public:
    DECLARE_DELEGATE_OneParam(FOnTransactCurrentFrame, TOptional<int> /*iFrame*/)

public:
    FOdysseyAnimationTimelineCellsShortcuts(
        const TAttribute<UOdysseyAnimation*>& iAnimation,
        const TAttribute<int>& iCurrentFrame,
        const FOnTransactCurrentFrame& iOnTransactCurrentFrame
    );

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
    void Action_IncreaseCellExposure();
    void Action_DecreaseCellExposure();
    void Action_SetCellExposure();

    bool CanAction_Copy();
    bool CanAction_Cut();
    bool CanAction_Paste();
    bool CanAction_SelectAll();
    bool CanAction_Delete();
    bool CanAction_ConvertToStaggerCell();
    bool CanAction_IncreaseCellExposure();
    bool CanAction_DecreaseCellExposure();
    bool CanAction_SetCellExposure();

private:
    TAttribute<UOdysseyAnimation*> mAnimation;
    TAttribute<int> mCurrentFrame;
    FOnTransactCurrentFrame mOnTransactCurrentFrame;
};
