// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

class UOdysseyAnimation;
class FUICommandList;
class FOdysseyAnimationTimelineCellsShortcuts;
class FOdysseyAnimationTimelineCellImageRasterShortcuts;
class FOdysseyAnimationTimelineCellImageStaggerShortcuts;

class ODYSSEYPAINTEREDITOR_API FOdysseyAnimationTimelineShortcuts
{
public:
    DECLARE_DELEGATE_OneParam(FOnTransactCurrentFrame, TOptional<int> /*iFrame*/)

public:
    FOdysseyAnimationTimelineShortcuts(
        const TAttribute<UOdysseyAnimation*>& iAnimation,
        const TAttribute<int>& iCurrentFrame,
        const FOnTransactCurrentFrame& iOnTransactCurrentFrame
    );

public:
    TSharedRef<FUICommandList> GetCommandList() const;

private:
    //Shortcuts
    void MapActionsToCommandList();

private:
    TSharedRef<FUICommandList> mCommandList;
    TSharedRef<FOdysseyAnimationTimelineCellsShortcuts> mCellsShortcuts;
    TSharedRef<FOdysseyAnimationTimelineCellImageRasterShortcuts> mCellImageRasterShortcuts;
    TSharedRef<FOdysseyAnimationTimelineCellImageStaggerShortcuts> mCellImageStaggerShortcuts;
};
