// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyPainterEditorToolsTab.h"

class FOdysseyTextureEditor;

class ODYSSEYTEXTUREEDITOR_API FOdysseyTextureEditorToolsTab :
	public FOdysseyPainterEditorToolsTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyTextureEditorToolsTab();
    FOdysseyTextureEditorToolsTab(FOdysseyTextureEditor* iEditor);

protected:
    // Widget Getters

protected:
    // Event Listeners
    virtual FReply OnClear() override;
    virtual FReply OnFill() override;
    virtual FReply OnUndo() override;
    virtual FReply OnRedo() override;
    virtual FReply OnClearUndo() override;

private:
    FOdysseyTextureEditor* mEditor;
};

