// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditorShortcuts.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorGlobalToolsShortcuts
    : public IOdysseyEditorShortcuts
{
public:
    virtual ~FOdysseyPainterEditorGlobalToolsShortcuts() {};
    FOdysseyPainterEditorGlobalToolsShortcuts(TSharedPtr<FOdysseyPainterEditor> iEditor);

public:
    //Common Shortcuts
    void Action_ActivateTool(UOdysseyPainterEditorTool* iTool);
    bool CanAction_ActivateTool(UOdysseyPainterEditorTool* iTool);

public:
    //Shortcuts
    virtual void MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList) override;

private:
    TWeakPtr<FOdysseyPainterEditor> mEditor;
};