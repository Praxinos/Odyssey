// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
    virtual UTexture* Texture() const override;

private:
    FOdysseyFlipbookEditor* mEditor;
};

