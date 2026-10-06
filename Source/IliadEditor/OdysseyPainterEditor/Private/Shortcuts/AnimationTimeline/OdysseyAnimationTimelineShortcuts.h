// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Misc/Attribute.h"

class UOdysseyAnimation;
class FOdysseyCommandList;
class FOdysseyAnimationTimelineCellsShortcuts;
class FOdysseyAnimationTimelineCellImageRasterShortcuts;
class FOdysseyAnimationTimelineCellImageStaggerShortcuts;

class ODYSSEYPAINTEREDITOR_API FOdysseyAnimationTimelineShortcuts
{
public:
    FOdysseyAnimationTimelineShortcuts(
        const TAttribute<UOdysseyAnimation*>& iAnimation,
        const TAttribute<int>& iCurrentFrame
    );

public:
    TSharedRef<FOdysseyCommandList> GetCommandList() const;

private:
    //Shortcuts
    void MapActionsToCommandList();

private:
    TSharedRef<FOdysseyCommandList> mCommandList;
    TSharedRef<FOdysseyAnimationTimelineCellsShortcuts> mCellsShortcuts;
    TSharedRef<FOdysseyAnimationTimelineCellImageRasterShortcuts> mCellImageRasterShortcuts;
    TSharedRef<FOdysseyAnimationTimelineCellImageStaggerShortcuts> mCellImageStaggerShortcuts;
};
