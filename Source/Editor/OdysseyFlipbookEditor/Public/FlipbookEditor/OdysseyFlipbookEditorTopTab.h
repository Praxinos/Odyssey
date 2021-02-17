// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyPainterEditorTopTab.h"

class FOdysseyFlipbookEditor;

class ODYSSEYFLIPBOOKEDITOR_API FOdysseyFlipbookEditorTopTab :
	public FOdysseyPainterEditorTopTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyFlipbookEditorTopTab();
    FOdysseyFlipbookEditorTopTab(FOdysseyFlipbookEditor* iEditor);

protected:
    // Widget Getters
    virtual ::ul3::eAlphaMode AlphaMode() const override;

protected:
    // Event Listeners
    virtual void OnAlphaModeChanged( int32 iValue ) override;

private:
    FOdysseyFlipbookEditor* mEditor;
};

