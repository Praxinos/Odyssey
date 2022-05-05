// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyTexture2DEditorGUI.h"

class FOdysseyFlipbookEditor;
class FOdysseyFlipbookEditorTimelineTab;

/**
 * Implements an Editor toolkit for Flipbooks.
 */
class ODYSSEYFLIPBOOKEDITOR_API FOdysseyFlipbookEditorGUI :
	public FOdysseyTexture2DEditorGUI
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
	virtual TSharedRef<FTabManager::FSplitter>	CreateMainSection() override;

private:
	FOdysseyFlipbookEditor*						mEditor;

protected:
    //Tabs
	TSharedPtr<FOdysseyFlipbookEditorTimelineTab>    mTimelineTab;
};

