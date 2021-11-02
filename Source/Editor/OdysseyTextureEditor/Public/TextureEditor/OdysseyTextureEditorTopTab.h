// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyPainterEditorTopTab.h"

class FOdysseyTextureEditor;

class ODYSSEYTEXTUREEDITOR_API FOdysseyTextureEditorTopTab :
	public FOdysseyPainterEditorTopTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyTextureEditorTopTab();
    FOdysseyTextureEditorTopTab(FOdysseyTextureEditor* iEditor);

protected:
    // Widget Getters
    virtual ::ULIS::eAlphaMode AlphaMode() const override;

protected:
    // Event Listeners
    virtual void SetAlphaMode( ::ULIS::eAlphaMode iAlphaMode ) override;

private:
    FOdysseyTextureEditor* mEditor;
};

