// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "AnimationEditor/OdysseyAnimationEditorGUI.h"

#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "AnimationEditor/OdysseyAnimationEditorLightTableTab.h"
#include "AnimationEditor/OdysseyAnimationEditorLayerStackTab.h"

#define LOCTEXT_NAMESPACE "OdysseyAnimationEditorGUI"

/////////////////////////////////////////////////////
// FOdysseyAnimationEditorGUI
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyAnimationEditorGUI::~FOdysseyAnimationEditorGUI()
{
}

FOdysseyAnimationEditorGUI::FOdysseyAnimationEditorGUI(FOdysseyAnimationEditorExtension* iExtension)
	: mExtension(iExtension)
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyAnimationEditorGUI::CreateTabs()
{
	//ADD NEW TABS
	mExtension->GetEditor()->AddTab(MakeShared<FOdysseyAnimationEditorLayerStackTab>(mExtension));
	mExtension->GetEditor()->AddTab(MakeShared<FOdysseyAnimationEditorLightTableTab>(mExtension));
}

/* TSharedRef<FTabManager::FSplitter>
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
} */

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

TSharedPtr<FOdysseyAnimationEditorLayerStackTab>&
FOdysseyAnimationEditorGUI::GetLayerStackTab()
{
	return mLayerStackTab;
}

TSharedPtr<FOdysseyAnimationEditorLightTableTab>&
FOdysseyAnimationEditorGUI::GetLightTableTab()
{
	return mLightTableTab;
}

#undef LOCTEXT_NAMESPACE
