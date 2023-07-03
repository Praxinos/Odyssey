// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyPainterEditorContextMenu.h"

class FOdysseyPainterEditor;
class UOdysseyPainterEditorTool;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorPickToolObjectContextMenu :
    public FOdysseyPainterEditorContextMenu
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorVectorPickToolObjectContextMenu();
    FOdysseyPainterEditorVectorPickToolObjectContextMenu(FOdysseyPainterEditor* iEditor);

public:
    // FOdysseyEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;
    //virtual void OnToolkitInitialized( FBaseToolkit* iToolkit ) override;
    //virtual void ExtendMenu( FToolMenuOwner iOwner, FName iMenuName ) override;

private:
    FOdysseyPainterEditor* mEditor;
};
