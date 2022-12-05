// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyPainterEditorGUI.h"

#include "OdysseyTextureEditorLayerStackTab.h"
#include "OdysseyTextureEditorTextureDetailsTab.h"

class FOdysseyTextureEditor;

/**
 * Implements an Editor toolkit for textures.
 */
class ODYSSEYTEXTUREEDITOR_API FOdysseyTextureEditorGUI :
	public FOdysseyPainterEditorGUI
{
public:
    // Construction / Destruction
    virtual ~FOdysseyTextureEditorGUI();
    FOdysseyTextureEditorGUI(FOdysseyTextureEditor* iEditor);

protected:
    //Init
	virtual void CreateTabs() override;

public:
    // GettersFName
	virtual FName GetLayoutName() override;
    TSharedPtr<FOdysseyTextureEditorLayerStackTab>& GetLayerStackTab();
	TSharedPtr<FOdysseyTextureEditorTextureDetailsTab>& GetTextureDetailsTab();

protected:
	virtual TSharedRef<FTabManager::FSplitter>	CreateRightSection() override;

private:
	FOdysseyTextureEditor* mEditor;

private:
    //Tabs
    TSharedPtr<FOdysseyTextureEditorLayerStackTab>          mLayerStackTab;
	TSharedPtr<FOdysseyTextureEditorTextureDetailsTab>      mTextureDetailsTab;
};
