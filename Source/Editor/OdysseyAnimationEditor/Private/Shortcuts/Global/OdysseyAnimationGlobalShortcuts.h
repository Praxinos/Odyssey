// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

class SOdysseyAnimationLayerStackTreeView;
class FUICommandList;
class FOdysseyAnimationGlobalCellsShortcuts;
class FOdysseyAnimationGlobalTimelineShortcuts;

class ODYSSEYANIMATIONEDITOR_API FOdysseyAnimationGlobalShortcuts
{
public:
    FOdysseyAnimationGlobalShortcuts(TSharedPtr<FOdysseyAnimationEditorExtension> iExtension);

public:
    //Shortcuts
    void MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList);

private:
    TSharedRef<FOdysseyAnimationGlobalCellsShortcuts> mCellsShortcuts;
    TSharedRef<FOdysseyAnimationGlobalTimelineShortcuts> mTimelineShortcuts;
};