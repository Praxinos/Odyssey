// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTextureEditorGUI.h"

#include "Widgets/Text/SMultiLineEditableText.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SWrapBox.h"
#include "OdysseyStyleSet.h"
#include "OdysseyTextureEditor.h"

#include "OdysseyTextureEditorLayerStackTab.h"
#include "OdysseyTextureEditorTextureDetailsTab.h"
#include "OdysseyTextureEditorTopTab.h"
#include "OdysseyTextureEditorToolsTab.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorToolkit"

///*static*/const FName FOdysseyTextureEditorGUI::smLayerStackTabId( TEXT( "OdysseyTextureEditor_LayerStack" ) );
///*static*/const FName FOdysseyTextureEditorGUI::smTextureDetailsTabId( TEXT( "OdysseyTextureEditor_TextureDetails" ) );

/////////////////////////////////////////////////////
// FOdysseyTextureEditorGUI
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditorGUI::~FOdysseyTextureEditorGUI()
{
}

FOdysseyTextureEditorGUI::FOdysseyTextureEditorGUI(FOdysseyTextureEditor* iEditor) :
	FOdysseyPainterEditorGUI(iEditor, "OdysseyTextureEditor_Layout"),
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
	ODYSSEY_SET_TAB(mToolsTab, FOdysseyTextureEditorToolsTab, mEditor);

	//ADD NEW TABS
	ODYSSEY_ADD_TAB(mLayerStackTab, FOdysseyTextureEditorLayerStackTab, mEditor);
	ODYSSEY_ADD_TAB(mTextureDetailsTab, FOdysseyTextureEditorTextureDetailsTab, mEditor);
}

/* void
FOdysseyTextureEditorGUI::InitOdysseyTextureEditorGUI(FOdysseyTextureEditor* iEditor)
{
	// CreateLayerStackTab(iEditor);
	// CreateTextureDetailsTab(iEditor);
} */

TSharedRef<FTabManager::FSplitter>
FOdysseyTextureEditorGUI::CreateRightSection()
{
	return FOdysseyPainterEditorGUI::CreateRightSection()
		// LayerStack + Notes
		->Split
		(
			FTabManager::NewStack()
			// Undo History
			//->AddTab( UndoHistoryTabId, ETabState::ClosedTab )
			//->SetHideTabWell( false )
			//->SetSizeCoefficient( 0.6f )
			// Layer Stack
			//->AddTab(smLayerStackTabId, ETabState::OpenedTab)
			->AddTab(mLayerStackTab->ID(), ETabState::OpenedTab)
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.2f)
		)
		->Split
		(
			FTabManager::NewStack()
			// Undo History
			//->AddTab( UndoHistoryTabId, ETabState::ClosedTab )
			//->SetHideTabWell( false )
			//->SetSizeCoefficient( 0.6f )
			// Layer Stack
			->AddTab(mTextureDetailsTab->ID(), ETabState::OpenedTab)
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.4f)
		);
}

/* void
FOdysseyTextureEditorGUI::RegisterTabSpawners( const TSharedRef< class FTabManager >& iTabManager, TSharedRef<FWorkspaceItem>& iWorkspaceMenuCategoryRef)
{
} */

/* void
FOdysseyTextureEditorGUI::UnregisterTabSpawners( const TSharedRef< class FTabManager >& iTabManager )
{
	FOdysseyPainterEditorGUI::UnregisterTabSpawners(iTabManager);

    // iTabManager->UnregisterTabSpawner( smTextureDetailsTabId );
	// iTabManager->UnregisterTabSpawner( smLayerStackTabId );
} */

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- Internal widget creation

/* void
FOdysseyTextureEditorGUI::CreateLayerStackTab(FOdysseyTextureEditor* iEditor, TSharedPtr<FOdysseyTextureEditorController>& iController)
{
    mLayerStackTab = SNew( SOdysseyLayerStackView )
        .LayerStackData_Raw( iEditor, &FOdysseyTextureEditor::LayerStack );
} */

/* void
FOdysseyTextureEditorGUI::CreateTextureDetailsTab(FOdysseyTextureEditor* iEditor)
{
    mTextureDetailsTab = SNew( SOdysseyTextureDetails )
        .Texture( iEditor->Texture() );
} */

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

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

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Spawner callbacks

/* TSharedRef<SDockTab>
FOdysseyTextureEditorGUI::HandleTabSpawnerSpawnLayerStack(const FSpawnTabArgs& iArgs)
{
    check( iArgs.GetTabId() == smLayerStackTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "LayerStackTitle", "LayerStack" ) )
        [
            mLayerStackTab.ToSharedRef()
        ];

}*/

/* TSharedRef<SDockTab>
FOdysseyTextureEditorGUI::HandleTabSpawnerSpawnTextureDetails(const FSpawnTabArgs& iArgs)
{
    check( iArgs.GetTabId() == smTextureDetailsTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "TextureDetailsTitle", "TextureDetails" ) )
        [
            mTextureDetailsTab.ToSharedRef()
        ];

} */

#undef LOCTEXT_NAMESPACE
