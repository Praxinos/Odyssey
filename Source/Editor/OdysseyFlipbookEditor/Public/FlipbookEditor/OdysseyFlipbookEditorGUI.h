// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyTextureEditorGUI.h"

class FOdysseyFlipbookEditor;
class FOdysseyFlipbookEditorTimelineTab;

/**
 * Implements an Editor toolkit for Flipbooks.
 */
class ODYSSEYFLIPBOOKEDITOR_API FOdysseyFlipbookEditorGUI :
	public FOdysseyTextureEditorGUI
{
public:
    // Construction / Destruction
    virtual ~FOdysseyFlipbookEditorGUI();
    FOdysseyFlipbookEditorGUI(FOdysseyFlipbookEditor* iEditor);

protected:
    //Init
	virtual void CreateTabs() override;

public:
    // Getters
	virtual FName GetLayoutName() override;
	TSharedPtr<FOdysseyFlipbookEditorTimelineTab>& GetTimelineTab();

protected:
	// Internal widget creation
	virtual TSharedRef<FTabManager::FSplitter>	CreateMainSection();

private:
	FOdysseyFlipbookEditor*						mEditor;

protected:
    //Tabs
	TSharedPtr<FOdysseyFlipbookEditorTimelineTab>    mTimelineTab;
};

