// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyEditorShortcuts.h"

class FOdysseyPainterEditor;
class UOdysseyPainterEditorTool;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorGlobalToolsShortcuts
    : public IOdysseyEditorShortcuts
{
public:
    virtual ~FOdysseyPainterEditorGlobalToolsShortcuts() {};
    FOdysseyPainterEditorGlobalToolsShortcuts(FOdysseyPainterEditor* iEditor);

public:
    //Common Shortcuts
    void Action_ActivateTool(UOdysseyPainterEditorTool* iTool);
    void Action_ActivateTemporaryTool(UOdysseyPainterEditorTool* iTool);
    void Action_ActivateColorPickerTool();
    void Action_ActivateDrawingTool();
    void Action_ActivateSelectionTool();
    void Action_ActivatePaintBucketTool();
    void Action_ActivateTransformTool();
    void Action_ActivatePrimitiveDrawingTool();
    void Action_ActivateEraserTool();
    void Action_ActivateWarpTool();
    void Action_ActivateTemporaryColorPickerTool();
    void Action_SetToolRadius();
    void Action_InactivateTemporaryTool();

    bool CanAction_ActivateTool(UOdysseyPainterEditorTool* iTool);
    bool CanAction_ActivateTemporaryTool(UOdysseyPainterEditorTool* iTool);
    bool CanAction_ActivateColorPickerTool();
    bool CanAction_ActivateDrawingTool();
    bool CanAction_ActivateSelectionTool();
    bool CanAction_ActivatePaintBucketTool();
    bool CanAction_ActivateTransformTool();
    bool CanAction_ActivatePrimitiveDrawingTool();
    bool CanAction_ActivateEraserTool();
    bool CanAction_ActivateWarpTool();
    bool CanAction_ActivateTemporaryColorPickerTool();
    bool CanAction_SetToolRadius();
    bool CanAction_InactivateTemporaryTool();

public:
    //Shortcuts
    virtual void MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList) override;

private:
    FOdysseyPainterEditor* mEditor;
};
