// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyFlipbookEditorGUI.h"

#include "Widgets/Text/SMultiLineEditableText.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SWrapBox.h"
#include "OdysseyStyleSet.h"
#include "SOdysseySurfaceViewport.h"

#define LOCTEXT_NAMESPACE "OdysseyFlipbookEditorToolkit"

/*static*/const FName FOdysseyFlipbookEditorGUI::smLayerStackTabId( TEXT( "OdysseyFlipbookEditor_LayerStack" ) );
/*static*/const FName FOdysseyFlipbookEditorGUI::smTimelineTabId( TEXT( "OdysseyFlipbookEditor_Timeline" ) );

void
FOdysseyFlipbookPerformanceOptions::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
{
	// FString PropertyName = PropertyThatChanged->GetName();
	FOdysseyPerformanceOptions::NotifyPostChange(PropertyChangedEvent, PropertyThatChanged);
}

/////////////////////////////////////////////////////
// FOdysseyFlipbookEditorGUI
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyFlipbookEditorGUI::~FOdysseyFlipbookEditorGUI()
{
}

FOdysseyFlipbookEditorGUI::FOdysseyFlipbookEditorGUI() :
	FOdysseyPainterEditorGUI("OdysseyFlipbookEditor_Layout")
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyFlipbookEditorGUI::Init(TSharedPtr<FOdysseyFlipbookEditorData>& iData, TSharedPtr<FOdysseyFlipbookEditorController>& iController)
{
	FOdysseyFlipbookPerformanceOptions* performanceOptions = new FOdysseyFlipbookPerformanceOptions();
	PerformanceOptions(performanceOptions);

	CreateLayerStackTab(iData, iController);
	CreateTimelineTab(iData, iController);

	FOdysseyPainterEditorGUI::InitOdysseyPainterEditorGUI(iData, iController); //Creates also creates the Layout

	GetViewportTab()->SetSurface( iData->DisplaySurface() );
}
 
TSharedRef<FTabManager::FSplitter>
FOdysseyFlipbookEditorGUI::CreateMainSection()
{
	return FOdysseyPainterEditorGUI::CreateMainSection()
		// Timeline + Notes
		->Split
		(
			FTabManager::NewStack()
			// Timeline
			->AddTab(smTimelineTabId, ETabState::OpenedTab)
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.2f)
		);
}


TSharedRef<FTabManager::FSplitter>
FOdysseyFlipbookEditorGUI::CreateLeftSection()
{
	return FOdysseyPainterEditorGUI::CreateLeftSection();
}

TSharedRef<FTabManager::FSplitter>
FOdysseyFlipbookEditorGUI::CreateRightSection()
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
FOdysseyFlipbookEditorGUI::CreateMiddleSection()
{
	return FOdysseyPainterEditorGUI::CreateMiddleSection();
}

void
FOdysseyFlipbookEditorGUI::RegisterTabSpawners( const TSharedRef< class FTabManager >& iTabManager, TSharedRef<FWorkspaceItem>& iWorkspaceMenuCategoryRef)
{
    FOdysseyPainterEditorGUI::RegisterTabSpawners(iTabManager, iWorkspaceMenuCategoryRef);

    // LayerStack
    iTabManager->RegisterTabSpawner( smLayerStackTabId, FOnSpawnTab::CreateSP( this, &FOdysseyFlipbookEditorGUI::HandleTabSpawnerSpawnLayerStack ) )
        .SetDisplayName( LOCTEXT( "LayerStackTab", "LayerStack" ) )
        .SetGroup(iWorkspaceMenuCategoryRef)
        .SetIcon( FSlateIcon( "OdysseyStyle", "FlipbookEditor.Layers16" ) );
    // Timeline
    iTabManager->RegisterTabSpawner( smTimelineTabId, FOnSpawnTab::CreateSP( this, &FOdysseyFlipbookEditorGUI::HandleTabSpawnerSpawnTimeline ) )
        .SetDisplayName( LOCTEXT( "TimelineTab", "Timeline" ) )
        .SetGroup(iWorkspaceMenuCategoryRef)
        .SetIcon( FSlateIcon( "OdysseyStyle", "FlipbookEditor.Layers16" ) ); //TODO: set timeline icon
}

void
FOdysseyFlipbookEditorGUI::UnregisterTabSpawners( const TSharedRef< class FTabManager >& iTabManager )
{
    iTabManager->UnregisterTabSpawner( smLayerStackTabId );
    iTabManager->UnregisterTabSpawner( smTimelineTabId );
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- Internal widget creation

void
FOdysseyFlipbookEditorGUI::CreateLayerStackTab(TSharedPtr<FOdysseyFlipbookEditorData>& iData, TSharedPtr<FOdysseyFlipbookEditorController>& iController)
{
    mLayerStackTab = SNew( SOdysseyLayerStackView )
        .LayerStackData_Raw( iData.Get(), &FOdysseyFlipbookEditorData::LayerStack );
}

void
FOdysseyFlipbookEditorGUI::CreateTimelineTab(TSharedPtr<FOdysseyFlipbookEditorData>& iData, TSharedPtr<FOdysseyFlipbookEditorController>& iController)
{
	mTimelineTab = SNew(SOdysseyFlipbookTimelineView)
		.Flipbook(iData->Flipbook())
		.OnScrubStarted(iController.Get(), &FOdysseyFlipbookEditorController::OnTimelineScrubStarted)
		.OnScrubStopped(iController.Get(), &FOdysseyFlipbookEditorController::OnTimelineScrubStopped)
		.OnCurrentKeyframeChanged(iController.Get(), &FOdysseyFlipbookEditorController::OnTimelineCurrentKeyframeChanged)
		.OnFlipbookChanged(iController.Get(), &FOdysseyFlipbookEditorController::OnFlipbookChanged)
		.OnSpriteCreated(iController->OnSpriteCreated())
		.OnTextureCreated(iController->OnTextureCreated())
		.OnKeyframeRemoved(iController->OnKeyframeRemoved());
		//.OnStructureChanged(iController.Get(), &FOdysseyFlipbookEditorController::OnTimelineStructureChanged);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

TSharedPtr<SOdysseyLayerStackView>&
FOdysseyFlipbookEditorGUI::GetLayerStackTab()
{
	return mLayerStackTab;
}

TSharedPtr<SOdysseyFlipbookTimelineView>&
FOdysseyFlipbookEditorGUI::GetTimelineTab()
{
	return mTimelineTab;
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Spawner callbacks

TSharedRef<SDockTab>
FOdysseyFlipbookEditorGUI::HandleTabSpawnerSpawnLayerStack(const FSpawnTabArgs& iArgs)
{
    check( iArgs.GetTabId() == smLayerStackTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "LayerStackTitle", "LayerStack" ) )
        [
            mLayerStackTab.ToSharedRef()
        ];

}

TSharedRef<SDockTab>
FOdysseyFlipbookEditorGUI::HandleTabSpawnerSpawnTimeline(const FSpawnTabArgs& iArgs)
{
    check( iArgs.GetTabId() == smTimelineTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "FlipbookTimelineTitle", "Timeline" ) )
        [
            mTimelineTab.ToSharedRef()
        ];

}

#undef LOCTEXT_NAMESPACE
