// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "AnimationEditor/OdysseyAnimationEditorGUI.h"

#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "AnimationEditor/OdysseyAnimationEditorLightTableTab.h"
#include "AnimationEditor/OdysseyAnimationEditorLayerStackTab.h"
#include "Framework/Docking/LayoutExtender.h"

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
FOdysseyAnimationEditorGUI::Initialize()
{
	CreateTabs();
}

void
FOdysseyAnimationEditorGUI::Finalize()
{

}

void
FOdysseyAnimationEditorGUI::BuildLayout(FOdysseyEditorLayoutBuilder& iBuilder)
{	
	TSharedRef<FTabManager::FSplitter> mainVerticalSplitter = iBuilder.GetSplitter("MainVerticalSplitter");
	TSharedRef<FTabManager::FStack> animationTimelineStack = iBuilder.CreateStack("AnimationTimelineStack");
	animationTimelineStack->SetHideTabWell(false);
	animationTimelineStack->SetSizeCoefficient(0.2f);
	animationTimelineStack->AddTab(FOdysseyAnimationEditorLayerStackTab::StaticId(), ETabState::OpenedTab);

	mainVerticalSplitter->Split
	(
		animationTimelineStack
	);
}

void
FOdysseyAnimationEditorGUI::ExtendLevelEditorLayout(FLayoutExtender& Extender)
{   
    Extender.ExtendLayout(FTabId(TEXT("LevelEditorSelectionDetails")), ELayoutExtensionPosition::Above, FTabManager::FTab(FOdysseyAnimationEditorLayerStackTab::StaticId(), ETabState::ClosedTab));
}

void
FOdysseyAnimationEditorGUI::CreateTabs()
{
	//ADD NEW TABS
	TSharedRef<FOdysseyAnimationEditorLayerStackTab> layerStackTab = MakeShared<FOdysseyAnimationEditorLayerStackTab>(mExtension);
	TSharedRef<FOdysseyAnimationEditorLightTableTab> lightTableTab = MakeShared<FOdysseyAnimationEditorLightTableTab>(mExtension);

	layerStackTab->ShouldOpenByDefault(true);

	mExtension->GetEditor()->AddTab(layerStackTab);
	mExtension->GetEditor()->AddTab(lightTableTab);
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
