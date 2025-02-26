// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023
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
