// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

class SOdysseyAnimationLayerStackTreeView;
class UOdysseyAnimationLayerStack;
class FUICommandList;
class FOdysseyAnimationEditorTimeline;

class ODYSSEYANIMATIONEDITOR_API FOdysseyAnimationTimelineShortcuts
{
public:
    FOdysseyAnimationTimelineShortcuts(TSharedPtr<SOdysseyAnimationLayerStackTreeView> iTreeView, UOdysseyAnimationLayerStack* iLayerStack, FOdysseyAnimationEditorTimeline* iTimelineContext);

public:
    TSharedRef<FUICommandList> GetCommandList() const;

    //Common Shortcuts
    void Action_Copy();
    void Action_Cut();
    void Action_Paste();
    void Action_SelectAll();
    void Action_Delete();

    bool CanAction_Copy();
    bool CanAction_Cut();
    bool CanAction_Paste();
    bool CanAction_SelectAll();
    bool CanAction_Delete();

private:
    //Shortcuts
    void MapActionsToCommandList(); 

private:
    TSharedRef<FUICommandList> mCommandList;
    TWeakPtr<SOdysseyAnimationLayerStackTreeView> mTreeView;
    UOdysseyAnimationLayerStack* mLayerStack;
    FOdysseyAnimationEditorTimeline* mTimelineContext;
};