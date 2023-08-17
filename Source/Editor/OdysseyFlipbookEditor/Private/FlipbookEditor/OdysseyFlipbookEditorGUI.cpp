// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyFlipbookEditorGUI.h"

#include "OdysseyFlipbookEditorTimelineTab.h"

#define LOCTEXT_NAMESPACE "OdysseyFlipbookEditorToolkit"

/////////////////////////////////////////////////////
// FOdysseyFlipbookEditorGUI
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyFlipbookEditorGUI::~FOdysseyFlipbookEditorGUI()
{
}

FOdysseyFlipbookEditorGUI::FOdysseyFlipbookEditorGUI(FOdysseyFlipbookEditorExtension* iExtension)
	: mExtension(iExtension)
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization


void
FOdysseyFlipbookEditorGUI::CreateTabs()
{
	//REPLACE TABS
	//mEditor->AddTab(MakeShared<FOdysseyFlipbookEditorViewportTab>());

	//ADD NEW TABS
	mExtension->GetEditor()->AddTab(MakeShared<FOdysseyFlipbookEditorTimelineTab>(mExtension));
}

/* TSharedRef<FTabManager::FSplitter>
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
} */

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

TSharedPtr<FOdysseyFlipbookEditorTimelineTab>&
FOdysseyFlipbookEditorGUI::GetTimelineTab()
{
	return mTimelineTab;
}


#undef LOCTEXT_NAMESPACE
