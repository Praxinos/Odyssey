// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyPainterEditorGUI.h"

#include "OdysseyAnimationEditorLayerStackTab.h"

class FOdysseyAnimationEditor;

/**
 * Implements an Editor toolkit for textures.
 */
class ODYSSEYANIMATIONEDITOR_API FOdysseyAnimationEditorGUI :
	public FOdysseyPainterEditorGUI
{
public:
    // Construction / Destruction
    virtual ~FOdysseyAnimationEditorGUI();
    FOdysseyAnimationEditorGUI(FOdysseyAnimationEditor* iEditor);

protected:
    //Init
	virtual void CreateTabs() override;

public:
    // GettersFName
	virtual FName GetLayoutName() override;
    TSharedPtr<FOdysseyAnimationEditorLayerStackTab>& GetLayerStackTab();

protected:
	virtual TSharedRef<FTabManager::FSplitter>	CreateBottomSection() override;

private:
	FOdysseyAnimationEditor* mEditor;

private:
    //Tabs
    TSharedPtr<FOdysseyAnimationEditorLayerStackTab>          mLayerStackTab;
};
