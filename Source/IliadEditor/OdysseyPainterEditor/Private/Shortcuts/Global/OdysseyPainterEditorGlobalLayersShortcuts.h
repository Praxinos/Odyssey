// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyEditorShortcuts.h"
#include "OdysseyBlendingMode.h"

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
    void Action_SetCurrentLayerBlendMode(EOdysseyBlendingMode iBlendMode);
    void Action_SetCurrentLayerBlendModeToNextBlendMode();
    void Action_SetCurrentLayerBlendModeToPreviousBlendMode();

    bool CanAction_CreateNewLayer();
    bool CanAction_ChangeLayerOpacity();
    bool CanAction_AlterLayer();

public:
    //Shortcuts
    virtual void MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList) override;

private:
    FOdysseyPainterEditor* mEditor;
};
