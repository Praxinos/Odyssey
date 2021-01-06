// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTextureEditorGUI.h"

#include "Widgets/Text/SMultiLineEditableText.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SWrapBox.h"
#include "OdysseyStyleSet.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorToolkit"

/*static*/const FName FOdysseyTextureEditorGUI::smLayerStackTabId( TEXT( "OdysseyTextureEditor_LayerStack" ) );
/*static*/const FName FOdysseyTextureEditorGUI::smTextureDetailsTabId( TEXT( "OdysseyTextureEditor_TextureDetails" ) );

void
FOdysseyTexturePerformanceOptions::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
{
	// FString PropertyName = PropertyThatChanged->GetName();
	FOdysseyPerformanceOptions::NotifyPostChange(PropertyChangedEvent, PropertyThatChanged);
}

/////////////////////////////////////////////////////
// FOdysseyTextureEditorGUI
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditorGUI::~FOdysseyTextureEditorGUI()
{
}

FOdysseyTextureEditorGUI::FOdysseyTextureEditorGUI() :
	FOdysseyPainterEditorGUI("OdysseyTextureEditor_Layout")
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyTextureEditorGUI::Init(TSharedPtr<FOdysseyTextureEditorData>& iData, TSharedPtr<FOdysseyTextureEditorController>& iController)
{
	FOdysseyTexturePerformanceOptions* performanceOptions = new FOdysseyTexturePerformanceOptions();
	PerformanceOptions(performanceOptions);

	CreateLayerStackTab(iData, iController);
	CreateTextureDetailsTab(iData, iController);
	FOdysseyPainterEditorGUI::InitOdysseyPainterEditorGUI(iData, iController); //Creates also creates the Layout
}
 
TSharedRef<FTabManager::FSplitter>
FOdysseyTextureEditorGUI::CreateMainSection()
{
	return FOdysseyPainterEditorGUI::CreateMainSection();
}


TSharedRef<FTabManager::FSplitter>
FOdysseyTextureEditorGUI::CreateLeftSection()
{
	return FOdysseyPainterEditorGUI::CreateLeftSection();
}

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
			->AddTab(smLayerStackTabId, ETabState::OpenedTab)
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
			->AddTab(smTextureDetailsTabId, ETabState::OpenedTab)
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.4f)
		);
}

TSharedRef<FTabManager::FSplitter>
FOdysseyTextureEditorGUI::CreateMiddleSection()
{
	return FOdysseyPainterEditorGUI::CreateMiddleSection();
}

void
FOdysseyTextureEditorGUI::RegisterTabSpawners( const TSharedRef< class FTabManager >& iTabManager, TSharedRef<FWorkspaceItem>& iWorkspaceMenuCategoryRef)
{
    FOdysseyPainterEditorGUI::RegisterTabSpawners(iTabManager, iWorkspaceMenuCategoryRef);

    // LayerStack
    iTabManager->RegisterTabSpawner( smLayerStackTabId, FOnSpawnTab::CreateSP( this, &FOdysseyTextureEditorGUI::HandleTabSpawnerSpawnLayerStack ) )
        .SetDisplayName( LOCTEXT( "LayerStackTab", "LayerStack" ) )
        .SetGroup(iWorkspaceMenuCategoryRef)
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.Layers16" ) );

	// Texture Details
    iTabManager->RegisterTabSpawner( smTextureDetailsTabId, FOnSpawnTab::CreateSP( this, &FOdysseyTextureEditorGUI::HandleTabSpawnerSpawnTextureDetails ) )
        .SetDisplayName( LOCTEXT( "TextureDetailsTab", "TextureDetails" ) )
        .SetGroup(iWorkspaceMenuCategoryRef)
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.Layers16" ) );
}

void
FOdysseyTextureEditorGUI::UnregisterTabSpawners( const TSharedRef< class FTabManager >& iTabManager )
{
    iTabManager->UnregisterTabSpawner( smTextureDetailsTabId );
	iTabManager->UnregisterTabSpawner( smLayerStackTabId );
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- Internal widget creation

void
FOdysseyTextureEditorGUI::CreateLayerStackTab(TSharedPtr<FOdysseyTextureEditorData>& iData, TSharedPtr<FOdysseyTextureEditorController>& iController)
{
    mLayerStackTab = SNew( SOdysseyLayerStackView )
        .LayerStackData( iData->LayerStack() );
}

void
FOdysseyTextureEditorGUI::CreateTextureDetailsTab(TSharedPtr<FOdysseyTextureEditorData>& iData, TSharedPtr<FOdysseyTextureEditorController>& iController)
{
    mTextureDetailsTab = SNew( SOdysseyTextureDetails )
        .Texture( iData->Texture() );
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

TSharedPtr<SOdysseyLayerStackView>&
FOdysseyTextureEditorGUI::GetLayerStackTab()
{
	return mLayerStackTab;
}


TSharedPtr<SOdysseyTextureDetails>&
FOdysseyTextureEditorGUI::GetTextureDetailsTab()
{
	return mTextureDetailsTab;
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Spawner callbacks

TSharedRef<SDockTab>
FOdysseyTextureEditorGUI::HandleTabSpawnerSpawnLayerStack(const FSpawnTabArgs& iArgs)
{
    check( iArgs.GetTabId() == smLayerStackTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "LayerStackTitle", "LayerStack" ) )
        [
            mLayerStackTab.ToSharedRef()
        ];

}

TSharedRef<SDockTab>
FOdysseyTextureEditorGUI::HandleTabSpawnerSpawnTextureDetails(const FSpawnTabArgs& iArgs)
{
    check( iArgs.GetTabId() == smTextureDetailsTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "TextureDetailsTitle", "TextureDetails" ) )
        [
            mTextureDetailsTab.ToSharedRef()
        ];

}

#undef LOCTEXT_NAMESPACE
