// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyEditorTab.h"
#include <ULIS>

class FOdysseyPainterEditor;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorToolsTab :
	public FOdysseyEditorTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorToolsTab();
    FOdysseyPainterEditorToolsTab(FOdysseyPainterEditor* iEditor);

protected:
    // FOdysseyEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;

protected:
    // Widget Getters

protected:
    // Event Listeners
    virtual FReply OnClear();
    virtual FReply OnFill();
    virtual FReply OnClearUndo();

protected:
    // Methods
    virtual void Clear();
    virtual void Fill();

private:
    FOdysseyPainterEditor* mEditor;
};

