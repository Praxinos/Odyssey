// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

class FOdysseyViewportDrawingEditorGUI;

/**
 * The Model of the ModeToolbar
 */
class ODYSSEYVIEWPORTDRAWINGEDITOR_API FOdysseyViewportDrawingEditorModeToolbar :  public TSharedFromThis<FOdysseyViewportDrawingEditorModeToolbar>
{
public:
    // Construction / Destruction
    FOdysseyViewportDrawingEditorModeToolbar( FOdysseyViewportDrawingEditorGUI* iGUI);
    ~FOdysseyViewportDrawingEditorModeToolbar();

public:
    void SaveOpenedTabs();
    void LoadOpenedTabs();

public:
    void OpenLayerStackTab();
    void OpenColorSlidersTab();
    void OpenBrushSelectorTab();
    void OpenColorWheelTab();
    void OpenMeshSelectorTab();
    void OpenStrokeOptionsTab();
    void OpenTextureDetailsTab();
    void OpenToolsTab();
    void OpenTopTab();
    void OpenViewportTab();

private:
    TSharedPtr<FTabManager> mLevelEditorTabManager;
    FOdysseyViewportDrawingEditorGUI* mGUI;
};
