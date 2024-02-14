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
    void Action_ActivateColorPickerTool();
    void Action_ActivateDrawingTool();
    void Action_ActivateSelectionTool();
    void Action_ActivatePaintBucketTool();
    void Action_ActivateTransformTool();
    void Action_ActivatePrimitiveDrawingTool();
    void Action_ActivateEraserTool();
    void Action_ActivateWarpTool();

    bool CanAction_ActivateTool(UOdysseyPainterEditorTool* iTool);
    bool CanAction_ActivateColorPickerTool();
    bool CanAction_ActivateDrawingTool();
    bool CanAction_ActivateSelectionTool();
    bool CanAction_ActivatePaintBucketTool();
    bool CanAction_ActivateTransformTool();
    bool CanAction_ActivatePrimitiveDrawingTool();
    bool CanAction_ActivateEraserTool();
    bool CanAction_ActivateWarpTool();

public:
    //Shortcuts
    virtual void MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList) override;

private:
    TWeakPtr<FOdysseyPainterEditor> mEditor;
};