// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyPainterEditorBrushSelectorTab.h"

class FOdysseyTextureEditor;

class ODYSSEYTEXTUREEDITOR_API FOdysseyTextureEditorBrushSelectorTab :
	public FOdysseyPainterEditorBrushSelectorTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyTextureEditorBrushSelectorTab();
    FOdysseyTextureEditorBrushSelectorTab(FOdysseyTextureEditor* iEditor);

protected:
    // Event Listeners
    virtual void OnBrushSelected( UOdysseyBrush* iBrush ) override;

private:
    FOdysseyTextureEditor* mEditor;
};

