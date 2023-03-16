// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "AnimationEditor/OdysseyAnimationEditorGUI.h"

#include "AnimationEditor/OdysseyAnimationEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyAnimationEditorGUI"

/////////////////////////////////////////////////////
// FOdysseyAnimationEditorGUI
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyAnimationEditorGUI::~FOdysseyAnimationEditorGUI()
{
}

FOdysseyAnimationEditorGUI::FOdysseyAnimationEditorGUI(FOdysseyAnimationEditor* iEditor) :
	FOdysseyPainterEditorGUI(iEditor),
	mEditor( iEditor )
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyAnimationEditorGUI::CreateTabs()
{
	FOdysseyPainterEditorGUI::CreateTabs();

	//ADD NEW TABS
	ODYSSEY_ADD_TAB(mLayerStackTab, FOdysseyAnimationEditorLayerStackTab, mEditor);
}

TSharedRef<FTabManager::FSplitter>
FOdysseyAnimationEditorGUI::CreateBottomSection()
{
	return FOdysseyPainterEditorGUI::CreateBottomSection()
		// Timeline
		->Split
		(
			FTabManager::NewStack()
			->AddTab(mLayerStackTab->ID(), ETabState::OpenedTab)
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.35f)
		);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

FName
FOdysseyAnimationEditorGUI::GetLayoutName()
{
	return "OdysseyAnimationEditor_Layout";
}

TSharedPtr<FOdysseyAnimationEditorLayerStackTab>&
FOdysseyAnimationEditorGUI::GetLayerStackTab()
{
	return mLayerStackTab;
}

#undef LOCTEXT_NAMESPACE
