// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyPainterEditorTab.h"

class ODYSSEYTEXTUREEDITOR_API FOdysseyTextureEditorTextureDetailsTab :
	public FOdysseyPainterEditorTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyTextureEditorTextureDetailsTab();
    FOdysseyTextureEditorTextureDetailsTab(FOdysseyTextureEditor* iEditor);

protected:
    // FOdysseyTextureEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;

protected:
    // Widget Getters
    virtual UTexture* Texture() const;

protected:
    // Event Listeners

private:
    FOdysseyTextureEditor* mEditor;
};

