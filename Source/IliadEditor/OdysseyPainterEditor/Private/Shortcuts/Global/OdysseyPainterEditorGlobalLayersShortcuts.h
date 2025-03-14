// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditorShortcuts.h"

class FOdysseyPainterEditor;
class UOdysseyPainterEditorTool;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorGlobalLayersShortcuts
    : public IOdysseyEditorShortcuts
{
public:
    virtual ~FOdysseyPainterEditorGlobalLayersShortcuts() {};
    FOdysseyPainterEditorGlobalLayersShortcuts(FOdysseyPainterEditor* iEditor);

public:
    void Action_ChangeLayerOpacity(float iOpacity);
    void Action_CreateNewLayer();

    bool CanAction_CreateNewLayer();
    bool CanAction_ChangeLayerOpacity();

public:
    //Shortcuts
    virtual void MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList) override;

private:
    FOdysseyPainterEditor* mEditor;
};
