// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

class FOdysseyViewportDrawingEditorGUI;

struct TabState
{
    FName mName;
    bool bIsOpen;
};
/**
 * The Model of the ModeToolbar
 */
class ODYSSEYVIEWPORTDRAWINGEDITOR_API FOdysseyViewportDrawingEditorModeToolbar :  public TSharedFromThis<FOdysseyViewportDrawingEditorModeToolbar>
{
public:
    // Construction / Destruction
    FOdysseyViewportDrawingEditorModeToolbar( TSharedRef<FTabManager> iTabManage, FOdysseyViewportDrawingEditorGUI* iGUI);
    ~FOdysseyViewportDrawingEditorModeToolbar();

public:
    //Getter / Setter

public:
    //CallBacks
    void ToggleLayerStackTab();

private:
    TSharedRef<FTabManager> mTabManager;
    FOdysseyViewportDrawingEditorGUI* mGUI;
    TArray<TabState> mTabStates;
};
