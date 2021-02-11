// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyFlipbookEditorGUI.h"

#include "Widgets/Text/SMultiLineEditableText.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SWrapBox.h"
#include "OdysseyStyleSet.h"
#include "SOdysseySurfaceViewport.h"
#include "OdysseyFlipbookEditor.h"
#include "OdysseyFlipbookEditorToolkit.h"
#include "OdysseyFlipbookEditorViewportTab.h"
#include "OdysseyFlipbookEditorTimelineTab.h"

#define LOCTEXT_NAMESPACE "OdysseyFlipbookEditorToolkit"

/*static*/const FName FOdysseyFlipbookEditorGUI::smLayerStackTabId( TEXT( "OdysseyFlipbookEditor_LayerStack" ) );
/*static*/const FName FOdysseyFlipbookEditorGUI::smTextureDetailsTabId( TEXT( "OdysseyFlipbookEditor_TextureDetails" ) );
///*static*/const FName FOdysseyFlipbookEditorGUI::smTimelineTabId( TEXT( "OdysseyFlipbookEditor_Timeline" ) );

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

FOdysseyFlipbookEditorGUI::FOdysseyFlipbookEditorGUI(FOdysseyFlipbookEditor* iEditor)
	: FOdysseyPainterEditorGUI(iEditor, "OdysseyFlipbookEditor_Layout")
	, mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization


void
FOdysseyFlipbookEditorGUI::CreateTabs()
{
	FOdysseyPainterEditorGUI::CreateTabs();

	mViewportTab = MakeShareable(new FOdysseyFlipbookEditorViewportTab(mEditor));
	mTimelineTab = MakeShareable(new FOdysseyFlipbookEditorTimelineTab(mEditor));
}

void
FOdysseyFlipbookEditorGUI::InitTabs()
{
	mTimelineTab->Init();//TODO: Initialize Timeline after, FOdysseyPainterEditorGUI::InitTabs()

	FOdysseyPainterEditorGUI::InitTabs();

	//mViewportTab initialized by PainterEditorGUI
}

void
FOdysseyFlipbookEditorGUI::InitOdysseyFlipbookEditorGUI(FOdysseyFlipbookEditor* iEditor, TSharedPtr<FOdysseyFlipbookEditorController>& iController)
{
	FOdysseyFlipbookPerformanceOptions* performanceOptions = new FOdysseyFlipbookPerformanceOptions();
	PerformanceOptions(performanceOptions);

	CreateLayerStackTab(iEditor, iController);
	CreateTextureDetailsTab(iEditor, iController);
	//CreateTimelineTab(iEditor, iController);

	FOdysseyPainterEditorGUI::InitOdysseyPainterEditorGUI(iEditor, iController); //Creates also creates the Layout

	// GetViewportTab()->SetSurface( iEditor->DisplaySurface() );
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
			//->AddTab(smTimelineTabId, ETabState::OpenedTab)
			->AddTab(mTimelineTab->ID(), ETabState::OpenedTab)
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
	// Texture Details
    iTabManager->RegisterTabSpawner( smTextureDetailsTabId, FOnSpawnTab::CreateSP( this, &FOdysseyFlipbookEditorGUI::HandleTabSpawnerSpawnTextureDetails ) )
        .SetDisplayName( LOCTEXT( "TextureDetailsTab", "TextureDetails" ) )
        .SetGroup(iWorkspaceMenuCategoryRef)
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.Layers16" ) );
    // Timeline
	mTimelineTab->RegisterTabSpawner(iTabManager, iWorkspaceMenuCategoryRef);
    /* iTabManager->RegisterTabSpawner( smTimelineTabId, FOnSpawnTab::CreateSP( this, &FOdysseyFlipbookEditorGUI::HandleTabSpawnerSpawnTimeline ) )
        .SetDisplayName( LOCTEXT( "TimelineTab", "Timeline" ) )
        .SetGroup(iWorkspaceMenuCategoryRef)
        .SetIcon( FSlateIcon( "OdysseyStyle", "FlipbookEditor.Layers16" ) ); //TODO: set timeline icon */
}

void
FOdysseyFlipbookEditorGUI::UnregisterTabSpawners( const TSharedRef< class FTabManager >& iTabManager )
{
    iTabManager->UnregisterTabSpawner( smLayerStackTabId );
    iTabManager->UnregisterTabSpawner( smTextureDetailsTabId );
	mTimelineTab->UnregisterTabSpawner(iTabManager);
    // iTabManager->UnregisterTabSpawner( smTimelineTabId );
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- Internal widget creation

void
FOdysseyFlipbookEditorGUI::CreateLayerStackTab(FOdysseyFlipbookEditor* iEditor, TSharedPtr<FOdysseyFlipbookEditorController>& iController)
{
    mLayerStackTab = SNew( SOdysseyLayerStackView )
        .LayerStackData_Raw( iEditor, &FOdysseyFlipbookEditor::LayerStack );
}

void
FOdysseyFlipbookEditorGUI::CreateTextureDetailsTab(FOdysseyFlipbookEditor* iEditor, TSharedPtr<FOdysseyFlipbookEditorController>& iController)
{
    mTextureDetailsTab = SNew( SOdysseyTextureDetails )
        .Texture( iEditor->Texture() );
}

/* void
FOdysseyFlipbookEditorGUI::CreateTimelineTab(FOdysseyFlipbookEditor* iEditor, TSharedPtr<FOdysseyFlipbookEditorController>& iController)
{
	mTimelineTab = SNew(SOdysseyFlipbookTimelineView)
		.FlipbookWrapper(iEditor->FlipbookWrapper())
		.OnScrubStarted(iController.Get(), &FOdysseyFlipbookEditorController::OnTimelineScrubStarted)
		.OnScrubStopped(iController.Get(), &FOdysseyFlipbookEditorController::OnTimelineScrubStopped)
		.OnCurrentKeyframeChanged(iController.Get(), &FOdysseyFlipbookEditorController::OnTimelineCurrentKeyframeChanged)
		.OnFlipbookChanged(iController.Get(), &FOdysseyFlipbookEditorController::OnFlipbookChanged)
		.OnSpriteCreated(iController.Get(), &FOdysseyFlipbookEditorController::OnSpriteCreated)
		.OnTextureCreated(iController.Get(), &FOdysseyFlipbookEditorController::OnTextureCreated)
		.OnKeyframeRemoved(iController.Get(), &FOdysseyFlipbookEditorController::OnKeyframeRemoved);
		//.OnStructureChanged(iController.Get(), &FOdysseyFlipbookEditorController::OnTimelineStructureChanged);
} */

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

TSharedPtr<SOdysseyLayerStackView>&
FOdysseyFlipbookEditorGUI::GetLayerStackTab()
{
	return mLayerStackTab;
}


TSharedPtr<SOdysseyTextureDetails>&
FOdysseyFlipbookEditorGUI::GetTextureDetailsTab()
{
	return mTextureDetailsTab;
}

TSharedPtr<FOdysseyFlipbookEditorTimelineTab>&
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
FOdysseyFlipbookEditorGUI::HandleTabSpawnerSpawnTextureDetails(const FSpawnTabArgs& iArgs)
{
    check( iArgs.GetTabId() == smTextureDetailsTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "TextureDetailsTitle", "TextureDetails" ) )
        [
            mTextureDetailsTab.ToSharedRef()
        ];

}

/* TSharedRef<SDockTab>
FOdysseyFlipbookEditorGUI::HandleTabSpawnerSpawnTimeline(const FSpawnTabArgs& iArgs)
{
    check( iArgs.GetTabId() == smTimelineTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "FlipbookTimelineTitle", "Timeline" ) )
        [
            mTimelineTab.ToSharedRef()
        ];

} */

#undef LOCTEXT_NAMESPACE
