// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditorContextMenu.h"

class FOdysseyPainterEditor;
class UOdysseyPainterEditorTool;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorContextMenu :
    public FOdysseyEditorContextMenu
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorVectorContextMenu();
    FOdysseyPainterEditorVectorContextMenu(FOdysseyPainterEditor* iEditor);

protected:
    // FOdysseyEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;

private:
    FOdysseyPainterEditor* mEditor;
};
