// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

class SOdysseyAnimationLayerStackTreeView;
class FUICommandList;

class ODYSSEYANIMATIONEDITOR_API FOdysseyAnimationTimelineShortcuts
{
public:
    FOdysseyAnimationTimelineShortcuts(TSharedPtr<SOdysseyAnimationLayerStackTreeView> iTreeView);

public:
    TSharedRef<FUICommandList> GetCommandList() const;

private:
    //Shortcuts
    void MapActionsToCommandList();

private:
    TSharedRef<FUICommandList> mCommandList;
    TWeakPtr<SOdysseyAnimationLayerStackTreeView> mTreeView;

    TSharedRef<FOdysseyAnimationTimelineCellsShortcuts> mCellsShortcuts;
};