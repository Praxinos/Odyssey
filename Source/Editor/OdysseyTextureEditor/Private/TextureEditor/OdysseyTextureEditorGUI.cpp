// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyTextureEditorGUI.h"

#include "Widgets/Text/SMultiLineEditableText.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SWrapBox.h"
#include "OdysseyStyleSet.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorToolkit"

/*static*/const FName FOdysseyTextureEditorGUI::smLayerStackTabId( TEXT( "OdysseyTextureEditor_LayerStack" ) );

void
FOdysseyTexturePerformanceOptions::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
{
	FString PropertyName = PropertyThatChanged->GetName();
	if (PropertyName == FString(TEXT("LiveUpdate"))) {
		mOnLiveUpdateChanged.ExecuteIfBound(LiveUpdate);
	}
	FOdysseyPerformanceOptions::NotifyPostChange(PropertyChangedEvent, PropertyThatChanged);
}

/////////////////////////////////////////////////////
// FOdysseyTextureEditorGUI
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditorGUI::~FOdysseyTextureEditorGUI()
{
}

FOdysseyTextureEditorGUI::FOdysseyTextureEditorGUI()
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyTextureEditorGUI::Init(TSharedPtr<FOdysseyTextureEditorData>& iData, TSharedPtr<FOdysseyTextureEditorController>& iController)
{
	FOdysseyTexturePerformanceOptions* performanceOptions = new FOdysseyTexturePerformanceOptions();
	performanceOptions->mOnLiveUpdateChanged.BindRaw(iController.Get(), &FOdysseyTextureEditorController::HandlePerformanceLiveUpdateChanged);
	PerformanceOptions(performanceOptions);

	CreateLayerStackTab(iData, iController);
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
			->SetSizeCoefficient(0.6f)
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
}

void
FOdysseyTextureEditorGUI::UnregisterTabSpawners( const TSharedRef< class FTabManager >& iTabManager )
{
    iTabManager->UnregisterTabSpawner( smLayerStackTabId );
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- Internal widget creation

void
FOdysseyTextureEditorGUI::CreateLayerStackTab(TSharedPtr<FOdysseyTextureEditorData>& iData, TSharedPtr<FOdysseyTextureEditorController>& iController)
{
    mLayerStackTab = SNew( SOdysseyLayerStackView )
        .LayerStackData( MakeShareable( iData->LayerStack() ) );
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

TSharedPtr<SOdysseyLayerStackView>&
FOdysseyTextureEditorGUI::GetLayerStackTab()
{
	return mLayerStackTab;
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

#undef LOCTEXT_NAMESPACE
