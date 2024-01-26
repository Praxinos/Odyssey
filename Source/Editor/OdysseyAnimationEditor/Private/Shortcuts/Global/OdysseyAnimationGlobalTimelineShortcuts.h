// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditorShortcuts.h"

class ODYSSEYANIMATIONEDITOR_API FOdysseyAnimationGlobalTimelineShortcuts
    : public IOdysseyEditorShortcuts
{
public:
    virtual ~FOdysseyAnimationGlobalTimelineShortcuts() {};
    FOdysseyAnimationGlobalTimelineShortcuts(TSharedPtr<FOdysseyAnimationEditorExtension> iExtension);

public:
    //Common Shortcuts
    void Action_ActivateTimelineSelectionTool();
    void Action_ActivateTimelineMoveTool();
    void Action_ActivateTimelineCutTool();

    void Action_NavigateToNextFrame();
    void Action_NavigateToPreviousFrame();
    void Action_NavigateToNextCell();
    void Action_NavigateToPreviousCell();

public:
    //Shortcuts
    virtual void MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList) override;

private:
    TWeakPtr<FOdysseyAnimationEditorExtension> mExtension;
};