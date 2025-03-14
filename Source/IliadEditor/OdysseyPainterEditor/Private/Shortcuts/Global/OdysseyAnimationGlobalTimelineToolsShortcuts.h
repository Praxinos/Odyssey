// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023
#pragma once

#include "OdysseyEditorShortcuts.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyAnimationGlobalTimelineToolsShortcuts
    : public IOdysseyEditorShortcuts
{
public:
    virtual ~FOdysseyAnimationGlobalTimelineToolsShortcuts() {};
    FOdysseyAnimationGlobalTimelineToolsShortcuts();

public:
    //Common Shortcuts
    void Action_ActivateTimelineSelectionTool();
    void Action_ActivateTimelineMoveTool();
    void Action_ActivateTimelineCutTool();

public:
    //Shortcuts
    virtual void MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList) override;
};
