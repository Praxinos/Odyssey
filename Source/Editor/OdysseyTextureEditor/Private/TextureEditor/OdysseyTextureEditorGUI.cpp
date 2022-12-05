// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "TextureEditor/OdysseyTextureEditorGUI.h"

#include "TextureEditor/OdysseyTextureEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorGUI"

/////////////////////////////////////////////////////
// FOdysseyTextureEditorGUI
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditorGUI::~FOdysseyTextureEditorGUI()
{
}

FOdysseyTextureEditorGUI::FOdysseyTextureEditorGUI(FOdysseyTextureEditor* iEditor) :
	FOdysseyPainterEditorGUI(iEditor),
	mEditor( iEditor )
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyTextureEditorGUI::CreateTabs()
{
	FOdysseyPainterEditorGUI::CreateTabs();

	//ADD NEW TABS
	ODYSSEY_ADD_TAB(mLayerStackTab, FOdysseyTextureEditorLayerStackTab, mEditor);
	ODYSSEY_ADD_TAB(mTextureDetailsTab, FOdysseyTextureEditorTextureDetailsTab, mEditor);
}

TSharedRef<FTabManager::FSplitter>
FOdysseyTextureEditorGUI::CreateRightSection()
{
	return FOdysseyPainterEditorGUI::CreateRightSection()
		// LayerStack + Notes
		->Split
		(
			FTabManager::NewStack()
			->AddTab(mLayerStackTab->ID(), ETabState::OpenedTab)
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.35f)
		)
		->Split
		(
			FTabManager::NewStack()
			->AddTab(mTextureDetailsTab->ID(), ETabState::OpenedTab)
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.15f)
		);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

FName
FOdysseyTextureEditorGUI::GetLayoutName()
{
	return "OdysseyTextureEditor_Layout";
}

TSharedPtr<FOdysseyTextureEditorLayerStackTab>&
FOdysseyTextureEditorGUI::GetLayerStackTab()
{
	return mLayerStackTab;
}

TSharedPtr<FOdysseyTextureEditorTextureDetailsTab>&
FOdysseyTextureEditorGUI::GetTextureDetailsTab()
{
	return mTextureDetailsTab;
}

#undef LOCTEXT_NAMESPACE
