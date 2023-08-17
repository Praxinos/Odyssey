// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "TextureEditor/OdysseyTextureEditorGUI.h"

class FOdysseyFlipbookEditorExtension;
class FOdysseyFlipbookEditorTimelineTab;

/**
 * Implements an Editor toolkit for Flipbooks.
 */
class ODYSSEYFLIPBOOKEDITOR_API FOdysseyFlipbookEditorGUI
{
public:
    // Construction / Destruction
    virtual ~FOdysseyFlipbookEditorGUI();
    FOdysseyFlipbookEditorGUI(FOdysseyFlipbookEditorExtension* iExtension);

public:
    void Initialize();
    void Finalize();

    void OnFlipbookChanged();

protected:
    //Init
	void CreateTabs();

public:
    // Getters
	TSharedPtr<FOdysseyFlipbookEditorTimelineTab>& GetTimelineTab();

protected:
	// Internal widget creation
	//TSharedRef<FTabManager::FSplitter>	CreateMainSection();

private:
	FOdysseyFlipbookEditorExtension*				mExtension;

protected:
    //Tabs
	TSharedPtr<FOdysseyFlipbookEditorTimelineTab>   mTimelineTab;
};

