// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyFlipbookEditorGUI.h"

#include "Widgets/Text/SMultiLineEditableText.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SWrapBox.h"
#include "OdysseyStyleSet.h"
#include "SOdysseySurfaceViewport.h"
#include "SSingleObjectDetailsPanel.h"
#include "OdysseyFlipbookEditorToolkit.h"

#define LOCTEXT_NAMESPACE "OdysseyFlipbookEditorToolkit"

/*static*/const FName FOdysseyFlipbookEditorGUI::smLayerStackTabId( TEXT( "OdysseyFlipbookEditor_LayerStack" ) );
/*static*/const FName FOdysseyFlipbookEditorGUI::smTextureDetailsTabId( TEXT( "OdysseyFlipbookEditor_TextureDetails" ) );
/*static*/const FName FOdysseyFlipbookEditorGUI::smTimelineTabId( TEXT( "OdysseyFlipbookEditor_Timeline" ) );
/*static*/const FName FOdysseyFlipbookEditorGUI::smDetailsTabId( TEXT( "OdysseyFlipbookEditor_Details" ) );

void
FOdysseyFlipbookPerformanceOptions::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
{
	// FString PropertyName = PropertyThatChanged->GetName();
	FOdysseyPerformanceOptions::NotifyPostChange(PropertyChangedEvent, PropertyThatChanged);
}



/////////////////////////////////////////////////////
// SOdysseyFlipbookPropertiesTabBody

class SOdysseyFlipbookPropertiesTabBody : public SSingleObjectDetailsPanel
{
public:
	SLATE_BEGIN_ARGS(SOdysseyFlipbookPropertiesTabBody) {}
	SLATE_END_ARGS()

private:
	// Pointer back to owning sprite editor instance (the keeper of state)
	TSharedPtr<FOdysseyFlipbookEditorData> mData;

public:
	void Construct(const FArguments& InArgs, TSharedPtr<FOdysseyFlipbookEditorData>& iData)
	{
		mData = iData;

		SSingleObjectDetailsPanel::Construct(
			SSingleObjectDetailsPanel::FArguments()
			.HostCommandList(mData->Toolkit().Pin()->GetToolkitCommands())
			.HostTabManager(mData->Toolkit().Pin()->GetTabManager())
			, /*bAutomaticallyObserveViaGetObjectToObserve=*/ true,
			/*bAllowSearch=*/ true);
	}

	// SSingleObjectDetailsPanel interface
	virtual UObject* GetObjectToObserve() const override
	{
		return mData->FlipbookWrapper()->Flipbook();
	}

	virtual TSharedRef<SWidget> PopulateSlot(TSharedRef<SWidget> PropertyEditorWidget) override
	{
		return SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.FillHeight(1)
			[
				PropertyEditorWidget
			];
	}
	// End of SSingleObjectDetailsPanel interface
};

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
	CreateTextureDetailsTab(iData, iController);
	CreateTimelineTab(iData, iController);
	CreateDetailsTab(iData, iController);

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
    iTabManager->RegisterTabSpawner( smTimelineTabId, FOnSpawnTab::CreateSP( this, &FOdysseyFlipbookEditorGUI::HandleTabSpawnerSpawnTimeline ) )
        .SetDisplayName( LOCTEXT( "TimelineTab", "Timeline" ) )
        .SetGroup(iWorkspaceMenuCategoryRef)
        .SetIcon( FSlateIcon( "OdysseyStyle", "FlipbookEditor.Layers16" ) ); //TODO: set timeline icon
    // Details
    /* iTabManager->RegisterTabSpawner( smDetailsTabId, FOnSpawnTab::CreateSP( this, &FOdysseyFlipbookEditorGUI::HandleTabSpawnerSpawnDetails ) )
        .SetDisplayName( LOCTEXT( "DetailsTab", "Details" ) )
        .SetGroup(iWorkspaceMenuCategoryRef)
        .SetIcon( FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details") ); */
}

void
FOdysseyFlipbookEditorGUI::UnregisterTabSpawners( const TSharedRef< class FTabManager >& iTabManager )
{
    iTabManager->UnregisterTabSpawner( smLayerStackTabId );
    iTabManager->UnregisterTabSpawner( smTextureDetailsTabId );
    iTabManager->UnregisterTabSpawner( smTimelineTabId );
    // iTabManager->UnregisterTabSpawner( smDetailsTabId );
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
FOdysseyFlipbookEditorGUI::CreateTextureDetailsTab(TSharedPtr<FOdysseyFlipbookEditorData>& iData, TSharedPtr<FOdysseyFlipbookEditorController>& iController)
{
    mTextureDetailsTab = SNew( SOdysseyTextureDetails )
        .Texture( iData->Texture() );
}

void
FOdysseyFlipbookEditorGUI::CreateTimelineTab(TSharedPtr<FOdysseyFlipbookEditorData>& iData, TSharedPtr<FOdysseyFlipbookEditorController>& iController)
{
	mTimelineTab = SNew(SOdysseyFlipbookTimelineView)
		.FlipbookWrapper(iData->FlipbookWrapper())
		.OnScrubStarted(iController.Get(), &FOdysseyFlipbookEditorController::OnTimelineScrubStarted)
		.OnScrubStopped(iController.Get(), &FOdysseyFlipbookEditorController::OnTimelineScrubStopped)
		.OnCurrentKeyframeChanged(iController.Get(), &FOdysseyFlipbookEditorController::OnTimelineCurrentKeyframeChanged)
		.OnFlipbookChanged(iController.Get(), &FOdysseyFlipbookEditorController::OnFlipbookChanged)
		.OnSpriteCreated(iController->OnSpriteCreated())
		.OnTextureCreated(iController->OnTextureCreated())
		.OnKeyframeRemoved(iController->OnKeyframeRemoved());
		//.OnStructureChanged(iController.Get(), &FOdysseyFlipbookEditorController::OnTimelineStructureChanged);
}

void
FOdysseyFlipbookEditorGUI::CreateDetailsTab(TSharedPtr<FOdysseyFlipbookEditorData>& iData, TSharedPtr<FOdysseyFlipbookEditorController>& iController)
{
	mDetailsTab = SNew(SOdysseyFlipbookPropertiesTabBody, iData);
}

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
FOdysseyFlipbookEditorGUI::HandleTabSpawnerSpawnTextureDetails(const FSpawnTabArgs& iArgs)
{
    check( iArgs.GetTabId() == smTextureDetailsTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "TextureDetailsTitle", "TextureDetails" ) )
        [
            mTextureDetailsTab.ToSharedRef()
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

TSharedRef<SDockTab>
FOdysseyFlipbookEditorGUI::HandleTabSpawnerSpawnDetails(const FSpawnTabArgs& iArgs)
{
    check( iArgs.GetTabId() == smDetailsTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "FlipbookDetailsTitle", "Details" ) )
        [
            mDetailsTab.ToSharedRef()
        ];

}

#undef LOCTEXT_NAMESPACE
