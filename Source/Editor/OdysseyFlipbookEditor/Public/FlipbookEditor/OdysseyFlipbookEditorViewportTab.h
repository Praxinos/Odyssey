// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyPainterEditorViewportTab.h"

class FOdysseyFlipbookEditor;

class ODYSSEYFLIPBOOKEDITOR_API FOdysseyFlipbookEditorViewportTab :
	public FOdysseyPainterEditorViewportTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyFlipbookEditorViewportTab();
    FOdysseyFlipbookEditorViewportTab(FOdysseyFlipbookEditor* iEditor);

protected:
    // Getters
    virtual IOdysseySurface* Surface() const override;

private:
    FOdysseyFlipbookEditor* mEditor;
};

