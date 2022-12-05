// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyFlipbookEditorGUI.h"

#include "OdysseyStyleSet.h"
#include "OdysseyFlipbookEditor.h"
#include "OdysseyFlipbookEditorToolkit.h"
#include "OdysseyFlipbookEditorViewportTab.h"
#include "OdysseyFlipbookEditorTimelineTab.h"

#define LOCTEXT_NAMESPACE "OdysseyFlipbookEditorToolkit"

/////////////////////////////////////////////////////
// FOdysseyFlipbookEditorGUI
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyFlipbookEditorGUI::~FOdysseyFlipbookEditorGUI()
{
}

FOdysseyFlipbookEditorGUI::FOdysseyFlipbookEditorGUI(FOdysseyFlipbookEditor* iEditor)
	: FOdysseyTextureEditorGUI(iEditor)
	, mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization


void
FOdysseyFlipbookEditorGUI::CreateTabs()
{
	FOdysseyTextureEditorGUI::CreateTabs();

	//REPLACE TABS
	ODYSSEY_SET_TAB(mViewportTab, FOdysseyFlipbookEditorViewportTab, mEditor);

	//ADD NEW TABS
	ODYSSEY_ADD_TAB(mTimelineTab, FOdysseyFlipbookEditorTimelineTab, mEditor);
}

TSharedRef<FTabManager::FSplitter>
FOdysseyFlipbookEditorGUI::CreateMainSection()
{
	return FOdysseyTextureEditorGUI::CreateMainSection()
		// Timeline + Notes
		->Split
		(
			FTabManager::NewStack()
			// Timeline
			->AddTab(mTimelineTab->ID(), ETabState::OpenedTab)
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.2f)
		);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

FName
FOdysseyFlipbookEditorGUI::GetLayoutName()
{
	return "OdysseyFlipbookEditor_Layout";
}

TSharedPtr<FOdysseyFlipbookEditorTimelineTab>&
FOdysseyFlipbookEditorGUI::GetTimelineTab()
{
	return mTimelineTab;
}


#undef LOCTEXT_NAMESPACE
