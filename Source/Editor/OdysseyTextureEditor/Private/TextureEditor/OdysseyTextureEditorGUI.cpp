// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTextureEditorGUI.h"

#include "Widgets/Text/SMultiLineEditableText.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SWrapBox.h"
#include "OdysseyStyleSet.h"
#include "OdysseyTextureEditor.h"

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

	//REPLACE TABS
	ODYSSEY_SET_TAB(mTopTab, FOdysseyTextureEditorTopTab, mEditor)
    ODYSSEY_SET_TAB(mBrushSelectorTab, FOdysseyTextureEditorBrushSelectorTab, mEditor)

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
