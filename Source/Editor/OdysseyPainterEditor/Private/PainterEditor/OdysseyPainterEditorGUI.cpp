// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorGUI.h"

#include "Widgets/Text/SMultiLineEditableText.h"

// #include "Brush/SOdysseyBrushExposedParameters.h"
// #include "Brush/SOdysseyBrushSelector.h"
// #include "Color/SOdysseyColorSelector.h"
// #include "Color/SOdysseyColorSliders.h"
// #include "Mesh/SOdysseyMeshSelector.h"
#include "SOdysseyPaintModifiers.h"
#include "SOdysseyPerformanceOptions.h"
// #include "SOdysseyStrokeOptions.h"
#include "UndoHistory/SOdysseyUndoHistory.h"
#include "OdysseyPainterEditorController.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SWrapBox.h"
#include "OdysseyStyleSet.h"

#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorTab.h"
#include "OdysseyPainterEditorMeshSelectorTab.h"
#include "OdysseyPainterEditorStrokeOptionsTab.h"
#include "OdysseyPainterEditorTopTab.h"
#include "OdysseyPainterEditorViewportTab.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorToolkit"

///*static*/const FName FOdysseyPainterEditorGUI::smViewportTabId( TEXT( "OdysseyPainterEditor_Viewport" ) );
///*static*/const FName FOdysseyPainterEditorGUI::smBrushSelectorTabId( TEXT( "OdysseyPainterEditor_BrushSelector" ) );
///*static*/const FName FOdysseyPainterEditorGUI::smMeshSelectorTabId( TEXT( "OdysseyPainterEditor_MeshSelector" ) );
///*static*/const FName FOdysseyPainterEditorGUI::smBrushExposedParametersTabId( TEXT( "OdysseyPainterEditor_BrushExposedParameters" ) );
///*static*/const FName FOdysseyPainterEditorGUI::smColorSelectorTabId( TEXT( "OdysseyPainterEditor_ColorSelector" ) );
///*static*/const FName FOdysseyPainterEditorGUI::smColorSlidersTabId( TEXT( "OdysseyPainterEditor_ColorSliders" ) );
///*static*/const FName FOdysseyPainterEditorGUI::smTopBarTabId( TEXT( "OdysseyPainterEditor_TopBar" ) );
///*static*/const FName FOdysseyPainterEditorGUI::smStrokeOptionsTabId( TEXT( "OdysseyPainterEditor_StrokeOptions" ) );
/*static*/const FName FOdysseyPainterEditorGUI::smNotesTabId( TEXT( "OdysseyPainterEditor_Notes" ) );
///*static*/const FName FOdysseyPainterEditorGUI::smUndoHistoryTabId( TEXT( "OdysseyPainterEditor_UndoHistory" ) );
/*static*/const FName FOdysseyPainterEditorGUI::smPerformanceOptionsTabId( TEXT( "OdysseyPainterEditor_PerformanceOptions" ) );
/*static*/const FName FOdysseyPainterEditorGUI::smToolsTabId( TEXT( "OdysseyPainterEditor_Tools" ) );

/////////////////////////////////////////////////////
// FOdysseyPainterEditorGUI
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorGUI::~FOdysseyPainterEditorGUI()
{
	delete mPerformanceOptions;
}

FOdysseyPainterEditorGUI::FOdysseyPainterEditorGUI(FOdysseyPainterEditor* iEditor, const FName iLayoutName)
	: mEditor(iEditor)
	, mLayoutName(iLayoutName)
	, mLayout(nullptr)
	, mPerformanceOptions( NULL )
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyPainterEditorGUI::Init()
{
	CreateTabs(); //Create Tabs Objects and sets their corresponding controllers
	InitTabs(); //Init Tabs, creating their widgets

	CreateLayout();
}

void
FOdysseyPainterEditorGUI::InitOdysseyPainterEditorGUI(FOdysseyPainterEditor* iEditor, TSharedPtr<FOdysseyPainterEditorController> iController)
{
	CreateWidgets(iEditor, iController);
	// CreateLayout();
}

TSharedRef<FTabManager::FLayout>
FOdysseyPainterEditorGUI::GetLayout()
{
	return mLayout.ToSharedRef();
}

void
FOdysseyPainterEditorGUI::CreateTabs()
{
	mMeshSelectorTab = MakeShareable(new FOdysseyPainterEditorMeshSelectorTab(mEditor));
	mViewportTab = MakeShareable(new FOdysseyPainterEditorViewportTab(mEditor));
	mBrushSelectorTab = MakeShareable(new FOdysseyPainterEditorBrushSelectorTab(mEditor));
	mBrushExposedParametersTab = MakeShareable(new FOdysseyPainterEditorBrushExposedParametersTab(mEditor));
	mColorWheelTab = MakeShareable(new FOdysseyPainterEditorColorWheelTab(mEditor));
	mColorSlidersTab = MakeShareable(new FOdysseyPainterEditorColorSlidersTab(mEditor));
	mTopTab = MakeShareable(new FOdysseyPainterEditorTopTab(mEditor));
	mStrokeOptionsTab = MakeShareable(new FOdysseyPainterEditorStrokeOptionsTab(mEditor));

	/* CreateMeshSelectorTab(iEditor);
	CreateViewportTab(iEditor);
	CreateBrushSelectorTab(iEditor);
	CreateBrushExposedParametersTab(iEditor);
	CreateColorSelectorTab(iEditor);
	CreateColorSlidersTab(iEditor);
	CreateTopTab(iEditor);
	CreateStrokeOptionsTab(iEditor);
	CreatePerformanceOptionsTab(iEditor);
	CreateUndoHistoryTab(iEditor);
	CreateToolsTab(iEditor); */
}

void
FOdysseyPainterEditorGUI::InitTabs()
{
	mMeshSelectorTab->Init();
	mViewportTab->Init();
	mBrushSelectorTab->Init();
	mBrushExposedParametersTab->Init();
	mColorWheelTab->Init();
	mColorSlidersTab->Init();
	mTopTab->Init();
	mStrokeOptionsTab->Init();
}

void
FOdysseyPainterEditorGUI::OnToolkitInitialized()
{
	mMeshSelectorTab->OnToolkitInitialized();
	mViewportTab->OnToolkitInitialized();
	mBrushSelectorTab->OnToolkitInitialized();
	mBrushExposedParametersTab->OnToolkitInitialized();
	mColorWheelTab->OnToolkitInitialized();
	mColorSlidersTab->OnToolkitInitialized();
	mTopTab->OnToolkitInitialized();
	mStrokeOptionsTab->OnToolkitInitialized();
}

void
FOdysseyPainterEditorGUI::CreateLayout()
{
	mLayout = FTabManager::NewLayout(mLayoutName)
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Horizontal)
			->Split
			(
				CreateMainSection()
			)
		);
}



void
FOdysseyPainterEditorGUI::CreateWidgets(FOdysseyPainterEditor* iEditor, TSharedPtr<FOdysseyPainterEditorController>& iController)
{
	// Create tabs contents
	// CreateMeshSelectorTab(iEditor, iController);
	// CreateViewportTab(iEditor, iController);
	// CreateBrushSelectorTab(iEditor, iController);
	// CreateBrushExposedParametersTab(iEditor, iController);
	// CreateColorSelectorTab(iEditor, iController);
	// CreateColorSlidersTab(iEditor, iController);
	// CreateTopTab(iEditor, iController);
	// CreateStrokeOptionsTab(iEditor, iController);
	CreatePerformanceOptionsTab(iEditor, iController);
	//CreateUndoHistoryTab(iEditor, iController);
	CreateToolsTab(iEditor, iController);
}

TSharedRef<FTabManager::FSplitter>
FOdysseyPainterEditorGUI::CreateLeftSection()
{
	return FTabManager::NewSplitter()
		->SetOrientation(Orient_Vertical)
		->SetSizeCoefficient(0.2f)
		// Brush Selector
		->Split
		(
			FTabManager::NewStack()
			// ->AddTab(smBrushSelectorTabId, ETabState::OpenedTab)
			->AddTab(mBrushSelectorTab->ID(), ETabState::OpenedTab)
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.1f)
		)
		// Brush Preview
		->Split
		(
			FTabManager::NewStack()
			/*->AddTab( FName( "BrushPreview" ), ETabState::OpenedTab )
			->SetHideTabWell( false )
			->SetSizeCoefficient( 0.1f )*/
			// Brush Params
			//->AddTab(smBrushExposedParametersTabId, ETabState::OpenedTab)
			->AddTab(mBrushExposedParametersTab->ID(), ETabState::OpenedTab)
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.1f)
		)
		// Brush Params + Stroke Options
		->Split
		(
			FTabManager::NewStack()
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.3f)
			// Stroke Options
			//->AddTab(smStrokeOptionsTabId, ETabState::OpenedTab)
			->AddTab(mStrokeOptionsTab->ID(), ETabState::OpenedTab)
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.3f)
			// Performance Options
			->AddTab( smPerformanceOptionsTabId, ETabState::ClosedTab )
			->SetHideTabWell( false )
			->SetSizeCoefficient( 0.3f )
			// Mesh Selector
			// ->AddTab(smMeshSelectorTabId, ETabState::OpenedTab)
			->AddTab(mMeshSelectorTab->ID(), ETabState::OpenedTab)
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.3f)
		)
		// Tools
		->Split
		(
			FTabManager::NewStack()
			->AddTab(smToolsTabId, ETabState::OpenedTab)
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.2f)
		);
		// Navigator
		/*->Split
		(
			FTabManager::NewStack()
			->AddTab( FName( "Navigator" ), ETabState::OpenedTab )
			->SetHideTabWell( false )
			->SetSizeCoefficient( 0.3f )

			// Notes
			->AddTab( smNotesTabId, ETabState::OpenedTab )
			->SetHideTabWell( false )
			->SetSizeCoefficient( 0.3f )
		)*/
}

TSharedRef<FTabManager::FSplitter>
FOdysseyPainterEditorGUI::CreateRightSection()
{
	return FTabManager::NewSplitter()
		->SetSizeCoefficient(0.16f)
		->SetOrientation(Orient_Vertical)
		// ColorSelector
		->Split
		(
			FTabManager::NewStack()
			//->AddTab(smColorSelectorTabId, ETabState::OpenedTab)
			->AddTab(mColorWheelTab->ID(), ETabState::OpenedTab)
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.2f)
		)
		// ColorSliders
		->Split
		(
			FTabManager::NewStack()
			// ->AddTab(smColorSlidersTabId, ETabState::OpenedTab)
			->AddTab(mColorSlidersTab->ID(), ETabState::OpenedTab)
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.2f)
		);
}

TSharedRef<FTabManager::FSplitter>
FOdysseyPainterEditorGUI::CreateMiddleSection()
{

	return FTabManager::NewSplitter()
		->SetOrientation(Orient_Vertical)
		->SetSizeCoefficient(1.0f)
		// Top Bar
		->Split
		(
			FTabManager::NewStack()
			//->AddTab(smTopBarTabId, ETabState::OpenedTab)
			->AddTab(mTopTab->ID(), ETabState::OpenedTab)
			->SetHideTabWell(true)
			->SetSizeCoefficient(0.1f)
		)
		// Viewport
		->Split
		(
			FTabManager::NewStack()
			//->AddTab(smViewportTabId, ETabState::OpenedTab)
			->AddTab(mViewportTab->ID(), ETabState::OpenedTab)
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.9f)
		);
}



TSharedRef<FTabManager::FSplitter>
FOdysseyPainterEditorGUI::CreateMainSection()
{
	return FTabManager::NewSplitter()
		->SetOrientation(Orient_Vertical)
		->SetSizeCoefficient(1.f)
		// TopMost Part
		->Split
		(
			FTabManager::NewSplitter()
			->SetSizeCoefficient(1.f)
			->SetOrientation(Orient_Horizontal)
			// Left Bar
			->Split
			(
				CreateLeftSection()
			)
			// Middle bar
			->Split
			(
				CreateMiddleSection()
			)
			// Right bar
			->Split
			(
				CreateRightSection()
			)
		);
}

void
FOdysseyPainterEditorGUI::RegisterTabSpawners( const TSharedRef< class FTabManager >& iTabManager, TSharedRef<FWorkspaceItem>& iWorkspaceMenuCategoryRef)
{
    //The viewport: the central area where we can draw on images
    /* iTabManager->RegisterTabSpawner( smViewportTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnViewport ) )
        .SetDisplayName( LOCTEXT( "ViewportTab", "Viewport" ) )
        .SetGroup(iWorkspaceMenuCategoryRef)
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.Viewport16" ) ); */

	mViewportTab->RegisterTabSpawner(iTabManager, iWorkspaceMenuCategoryRef);

    // BrushSelector
	mBrushSelectorTab->RegisterTabSpawner(iTabManager, iWorkspaceMenuCategoryRef);
    /* iTabManager->RegisterTabSpawner( smBrushSelectorTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnBrushSelector ) )
        .SetDisplayName( LOCTEXT( "BrushSelectorTab", "Brush Selector" ) )
        .SetGroup(iWorkspaceMenuCategoryRef)
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.BrushSelector16" ) ); */

    // MeshSelector
	mMeshSelectorTab->RegisterTabSpawner(iTabManager, iWorkspaceMenuCategoryRef);
    /* iTabManager->RegisterTabSpawner( smMeshSelectorTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnMeshSelector ) )
        .SetDisplayName( LOCTEXT( "MeshSelectorTab", "Mesh Selector" ) )
        .SetGroup(iWorkspaceMenuCategoryRef)
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.Mesh16" ) ); */

    // BrushExposedParameters
	mBrushExposedParametersTab->RegisterTabSpawner(iTabManager, iWorkspaceMenuCategoryRef);
    /* iTabManager->RegisterTabSpawner( smBrushExposedParametersTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnBrushExposedParameters ) )
        .SetDisplayName( LOCTEXT( "BrushExposedParametersTab", "Brush Parameters" ) )
        .SetGroup(iWorkspaceMenuCategoryRef)
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.BrushExposedParameters16" ) ); */

    // ColorSelector
	mColorWheelTab->RegisterTabSpawner(iTabManager, iWorkspaceMenuCategoryRef);
    /* iTabManager->RegisterTabSpawner( smColorSelectorTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnColorSelector ) )
        .SetDisplayName( LOCTEXT( "ColorSelectorTab", "ColorSelector" ) )
        .SetGroup(iWorkspaceMenuCategoryRef)
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.ColorWheel16" ) );*/

    // ColorSliders
	mColorSlidersTab->RegisterTabSpawner(iTabManager, iWorkspaceMenuCategoryRef);
    /* iTabManager->RegisterTabSpawner( smColorSlidersTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnColorSliders ) )
        .SetDisplayName( LOCTEXT( "ColorSlidersTab", "ColorSliders" ) )
        .SetGroup(iWorkspaceMenuCategoryRef)
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.ColorSliders_2_16" ) ); */

    // TopBar
	mTopTab->RegisterTabSpawner(iTabManager, iWorkspaceMenuCategoryRef);
    /* iTabManager->RegisterTabSpawner( smTopBarTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnTopBar ) )
        .SetDisplayName( LOCTEXT( "TopBarTab", "TopBar" ) )
        .SetGroup(iWorkspaceMenuCategoryRef)
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.TopBar16" ) ); */

    // StrokeOptions
	mStrokeOptionsTab->RegisterTabSpawner(iTabManager, iWorkspaceMenuCategoryRef);
    /* iTabManager->RegisterTabSpawner( smStrokeOptionsTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnStrokeOptions ) )
        .SetDisplayName( LOCTEXT( "StrokeOptionsTab", "Stroke Options" ) )
        .SetGroup(iWorkspaceMenuCategoryRef)
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.StrokeOptions16" ) ); */

    // PerformanceOptions
    iTabManager->RegisterTabSpawner( smPerformanceOptionsTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnPerformanceOptions ) )
        .SetDisplayName( LOCTEXT( "PerformanceOptionsTab", "Performance Options" ) )
        .SetGroup(iWorkspaceMenuCategoryRef)
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.PerformanceTools16" ) );

    // Notes
    iTabManager->RegisterTabSpawner( smNotesTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnNotes ) )
        .SetDisplayName( LOCTEXT( "NotesTab", "Notes" ) )
        .SetGroup(iWorkspaceMenuCategoryRef)
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.Notes16" ) );

    // Undo History
    /*iTabManager->RegisterTabSpawner( UndoHistoryTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnUndoHistory ) )
        .SetDisplayName( LOCTEXT( "UndoHistoryTab", "UndoHistory" ) )
        .SetGroup( iWorkspaceMenuCategoryRef )
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.UndoHistory16" ) );*/

        // Tools
    iTabManager->RegisterTabSpawner( smToolsTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnTools ) )
        .SetDisplayName( LOCTEXT( "ToolsTab", "Tools" ) )
        .SetGroup(iWorkspaceMenuCategoryRef)
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.Tools16" ) );
}

void
FOdysseyPainterEditorGUI::UnregisterTabSpawners( const TSharedRef< class FTabManager >& iTabManager )
{
    //iTabManager->UnregisterTabSpawner( smViewportTabId );
	mViewportTab->UnregisterTabSpawner(iTabManager);
	mBrushSelectorTab->UnregisterTabSpawner(iTabManager);
    //iTabManager->UnregisterTabSpawner( smBrushSelectorTabId );
	mMeshSelectorTab->UnregisterTabSpawner(iTabManager);
    // iTabManager->UnregisterTabSpawner( smMeshSelectorTabId );
    // iTabManager->UnregisterTabSpawner( smBrushExposedParametersTabId );
	mBrushExposedParametersTab->UnregisterTabSpawner(iTabManager);
	mColorWheelTab->UnregisterTabSpawner(iTabManager);
	mColorSlidersTab->UnregisterTabSpawner(iTabManager);
	mTopTab->UnregisterTabSpawner(iTabManager);
	mStrokeOptionsTab->UnregisterTabSpawner(iTabManager);
    //iTabManager->UnregisterTabSpawner( smColorSelectorTabId );
    // iTabManager->UnregisterTabSpawner( smColorSlidersTabId );
    //iTabManager->UnregisterTabSpawner( smTopBarTabId );
    // iTabManager->UnregisterTabSpawner( smStrokeOptionsTabId );
    iTabManager->UnregisterTabSpawner( smPerformanceOptionsTabId );
    iTabManager->UnregisterTabSpawner( smNotesTabId );
    //iTabManager->UnregisterTabSpawner( smUndoHistoryTabId );
    iTabManager->UnregisterTabSpawner( smToolsTabId );
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- Internal widget creation
/* void
FOdysseyPainterEditorGUI::CreateViewportTab(FOdysseyPainterEditor* iEditor, TSharedPtr<FOdysseyPainterEditorController>& iController)
{
    mViewportTab = SNew( SOdysseySurfaceViewport );
    mViewportTab->SetSurface( iEditor->DisplaySurface() );
	FOdysseyPainterEditorViewportClient* viewportClient = new FOdysseyPainterEditorViewportClient(iEditor, mViewportTab, mMeshSelectorTab->GetMeshSelectorPtr());
	viewportClient->OnPickColor().AddRaw(iController.Get(), &FOdysseyPainterEditorController::HandleViewportColorPicked);
	mViewportTab->SetViewportClient(MakeShareable(viewportClient));

    mViewportTab->OnParameterChanged().AddRaw( iController.Get(), &FOdysseyPainterEditorController::HandleViewportParameterChanged );
} */

/* void
FOdysseyPainterEditorGUI::CreateBrushSelectorTab(FOdysseyPainterEditor* iEditor, TSharedPtr<FOdysseyPainterEditorController>& iController)
{
    mBrushSelectorTab = SNew( SOdysseyBrushSelector )
        .OnBrushChanged_Raw( iController.Get(), &FOdysseyPainterEditorController::OnBrushSelected );
} */

/* void
FOdysseyPainterEditorGUI::CreateMeshSelectorTab(FOdysseyPainterEditor* iEditor, TSharedPtr<FOdysseyPainterEditorController>& iController)
{
    mMeshSelectorTab = SNew( SOdysseyMeshSelector )
        .OnMeshChanged_Raw(iController.Get(), &FOdysseyPainterEditorController::OnMeshSelected );
} */

/* void
FOdysseyPainterEditorGUI::CreateBrushExposedParametersTab(FOdysseyPainterEditor* iEditor, TSharedPtr<FOdysseyPainterEditorController>& iController)
{
    mBrushExposedParametersTab = SNew( SOdysseyBrushExposedParameters )
        .OnParameterChanged_Raw(iController.Get(), &FOdysseyPainterEditorController::HandleBrushParameterChanged );
} */

/* void
FOdysseyPainterEditorGUI::CreateColorSelectorTab(FOdysseyPainterEditor* iEditor, TSharedPtr<FOdysseyPainterEditorController>& iController)
{
    mColorSelectorTab = SNew( SOdysseyColorSelector )
		.Color_Raw(iEditor, &FOdysseyPainterEditor::PaintColor)
        .OnColorChange_Raw(iController.Get(), &FOdysseyPainterEditorController::HandlePaintColorChange);
} */

/* void
FOdysseyPainterEditorGUI::CreateColorSlidersTab(FOdysseyPainterEditor* iEditor, TSharedPtr<FOdysseyPainterEditorController>& iController)
{
    mColorSlidersTab = SNew( SOdysseyColorSliders )
		.Color_Raw(iEditor, &FOdysseyPainterEditor::PaintColor)
        .OnColorChange_Raw(iController.Get(), &FOdysseyPainterEditorController::HandlePaintColorChange);
} */

/* void
FOdysseyPainterEditorGUI::CreateTopTab(FOdysseyPainterEditor* iEditor, TSharedPtr<FOdysseyPainterEditorController>& iController)
{
    mTopTab = SNew( SOdysseyPaintModifiers )
        .OnSizeChanged_Raw(iController.Get(), &FOdysseyPainterEditorController::HandleSizeModifierChanged )
        .OnOpacityChanged_Raw(iController.Get(), &FOdysseyPainterEditorController::HandleOpacityModifierChanged )
        .OnFlowChanged_Raw(iController.Get(), &FOdysseyPainterEditorController::HandleFlowModifierChanged )
        .OnBlendingModeChanged_Raw(iController.Get(), &FOdysseyPainterEditorController::HandleBlendingModeModifierChanged )
        .OnAlphaModeChanged_Raw(iController.Get(), &FOdysseyPainterEditorController::HandleAlphaModeModifierChanged );

	mTopTab->SetAlphaMode(iEditor->PaintEngine()->GetAlphaMode());
	mTopTab->SetSize(20);
	mTopTab->SetOpacity(100);
	mTopTab->SetFlow(100);
} */

/* void
FOdysseyPainterEditorGUI::CreateStrokeOptionsTab(FOdysseyPainterEditor* iEditor, TSharedPtr<FOdysseyPainterEditorController>& iController)
{
    mStrokeOptionsTab = SNew( SOdysseyStrokeOptions )
        .OnStrokeStepChanged_Raw(iController.Get(), &FOdysseyPainterEditorController::HandleStrokeStepChanged )
        .OnStrokeAdaptativeChanged_Raw(iController.Get(), &FOdysseyPainterEditorController::HandleStrokeAdaptativeChanged )
        .OnStrokePaintOnTickChanged_Raw(iController.Get(), &FOdysseyPainterEditorController::HandleStrokePaintOnTickChanged )
        .OnInterpolationTypeChanged_Raw(iController.Get(), &FOdysseyPainterEditorController::HandleInterpolationTypeChanged )
        .OnSmoothingMethodChanged_Raw(iController.Get(), &FOdysseyPainterEditorController::HandleSmoothingMethodChanged )
        .OnSmoothingStrengthChanged_Raw(iController.Get(), &FOdysseyPainterEditorController::HandleSmoothingStrengthChanged )
        .OnSmoothingEnabledChanged_Raw(iController.Get(), &FOdysseyPainterEditorController::HandleSmoothingEnabledChanged )
        .OnSmoothingRealTimeChanged_Raw(iController.Get(), &FOdysseyPainterEditorController::HandleSmoothingRealTimeChanged )
        .OnSmoothingCatchUpChanged_Raw(iController.Get(), &FOdysseyPainterEditorController::HandleSmoothingCatchUpChanged );
		

	mStrokeOptionsTab->SetStrokeStep(20);
	mStrokeOptionsTab->SetStrokeAdaptative(true);
	mStrokeOptionsTab->SetStrokePaintOnTick(false);
	mStrokeOptionsTab->SetInterpolationType((int32)EOdysseyInterpolationType::kCatmullRom);
	mStrokeOptionsTab->SetSmoothingMethod((int32)EOdysseySmoothingMethod::kAverage);
	mStrokeOptionsTab->SetSmoothingStrength(10);
	mStrokeOptionsTab->SetSmoothingEnabled(true);
	mStrokeOptionsTab->SetSmoothingRealTime(true);
	mStrokeOptionsTab->SetSmoothingCatchUp(true);
} */

void
FOdysseyPainterEditorGUI::CreatePerformanceOptionsTab(FOdysseyPainterEditor* iEditor, TSharedPtr<FOdysseyPainterEditorController>& iController)
{
	mPerformanceOptions->mOnDrawBrushPreviewChanged.BindRaw(iController.Get(), &FOdysseyPainterEditorController::HandlePerformanceDrawBrushPreviewChanged);
	mPerformanceOptionsTab = SNew(SOdysseyPerformanceOptions)
		.PerformanceOptions(mPerformanceOptions);
}

void
FOdysseyPainterEditorGUI::CreateToolsTab(FOdysseyPainterEditor* iEditor, TSharedPtr<FOdysseyPainterEditorController>& iController)
{
    mToolsTab = SNew( SScrollBox )
		.Orientation( Orient_Vertical )
		.ScrollBarAlwaysVisible( false )
		+SScrollBox::Slot()
		[
			SNew( SExpandableArea )
			.HeaderContent()
			[
				SNew( STextBlock )
				.Text( LOCTEXT( "Utils", "Utils" ) )
				.Font( FEditorStyle::GetFontStyle( "DetailsView.CategoryFontStyle" ) )
				.ShadowOffset( FVector2D( 1.0f, 1.0f ) )
			]
			.BodyContent()
			[
				SNew( SWrapBox )
				.UseAllottedWidth( true )
				+SWrapBox::Slot()
				[
					SNew( SButton )
					.ButtonStyle( FCoreStyle::Get(), "NoBorder" )
					.OnClicked_Raw(iController.Get(), &FOdysseyPainterEditorController::OnClear)
					[
						SNew( SImage )
						.Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Shredder32" ) )
					]
				]
				+SWrapBox::Slot()
				[
					SNew( SButton )
					.ButtonStyle( FCoreStyle::Get(), "NoBorder" )
					.OnClicked_Raw(iController.Get(), &FOdysseyPainterEditorController::OnFill)
					[
						SNew( SImage )
						.Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PaintBucket32" ) )
					]
				]
				/*+SWrapBox::Slot()
				[
					SNew( SButton )
					.ButtonStyle( FCoreStyle::Get(), "NoBorder" )
					//.OnClicked_Raw( iController.Get(), &FOdysseyPainterEditorController::OnFillCurrentLayer )
					[
						SNew( SImage )
						.Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.ColorPicker32" ) )
					]
				]*/
			]
		]
		/*+SScrollBox::Slot()
		[
			SNew( SExpandableArea )
			.HeaderContent()
			[
				SNew( STextBlock )
				.Text( LOCTEXT( "Shapes", "Shapes" ) )
				.Font( FEditorStyle::GetFontStyle( "DetailsView.CategoryFontStyle" ) )
				.ShadowOffset( FVector2D( 1.0f, 1.0f ) )
			]
			.BodyContent()
			[
				SNew( SWrapBox )
				.UseAllottedWidth( true )
				+SWrapBox::Slot()
				[
					SNew( SButton )
					.ButtonStyle( FCoreStyle::Get(), "NoBorder" )
					//.OnClicked_Raw( iController.Get(), &FOdysseyPainterEditorController::OnFillCurrentLayer )
					[
						SNew( SImage )
						.Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Line32" ) )
					]
				]
				+SWrapBox::Slot()
				[
					SNew( SButton )
					.ButtonStyle( FCoreStyle::Get(), "NoBorder" )
					//.OnClicked_Raw( iController.Get(), &FOdysseyPainterEditorController::OnFillCurrentLayer )
					[
						SNew( SImage )
						.Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Square32" ) )
					]
				]
				+SWrapBox::Slot()
				[
					SNew( SButton )
					.ButtonStyle( FCoreStyle::Get(), "NoBorder" )
					//.OnClicked_Raw( iController.Get(), &FOdysseyPainterEditorController::OnFillCurrentLayer )
					[
						SNew( SImage )
						.Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Circle32" ) )
					]
				]
				+SWrapBox::Slot()
				[
					SNew( SButton )
					.ButtonStyle( FCoreStyle::Get(), "NoBorder" )
					//.OnClicked_Raw( iController.Get(), &FOdysseyPainterEditorController::OnFillCurrentLayer )
					[
						SNew( SImage )
						.Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Ellipse32" ) )
					]
				]
				+SWrapBox::Slot()
				[
					SNew( SButton )
					.ButtonStyle( FCoreStyle::Get(), "NoBorder" )
					//.OnClicked_Raw( iController.Get(), &FOdysseyPainterEditorController::OnFillCurrentLayer )
					[
						SNew( SImage )
						.Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Curve32" ) )
					]
				]
				+SWrapBox::Slot()
				[
					SNew( SButton )
					.ButtonStyle( FCoreStyle::Get(), "NoBorder" )
					//.OnClicked_Raw( iController.Get(), &FOdysseyPainterEditorController::OnFillCurrentLayer )
					[
						SNew( SImage )
						.Image( FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Bezier32" ) )
					]
				]
			]
		]*/
		+SScrollBox::Slot()
		[
			SNew( SExpandableArea )
			.HeaderContent()
			[
				SNew( STextBlock )
				.Text( LOCTEXT( "UndoRedo", "UndoRedo" ) )
				.Font( FEditorStyle::GetFontStyle( "DetailsView.CategoryFontStyle" ) )
				.ShadowOffset( FVector2D( 1.0f, 1.0f ) )
			]
			.BodyContent()
			[
				SNew( SWrapBox )
				.UseAllottedWidth( true )
				+SWrapBox::Slot()
				[
					SNew( SButton )
					.ButtonStyle( FCoreStyle::Get(), "NoBorder" )
					.OnClicked_Raw(iController.Get(), &FOdysseyPainterEditorController::OnUndoIliad)
					[
						SNew(SImage) .Image(FOdysseyStyle::GetBrush("PainterEditor.ToolsTab.Undo32"))
					]
				]
				+SWrapBox::Slot()
				[
					SNew( SButton )
					.ButtonStyle( FCoreStyle::Get(), "NoBorder" )
					.OnClicked_Raw(iController.Get(), &FOdysseyPainterEditorController::OnRedoIliad)
					[
						SNew(SImage) .Image(FOdysseyStyle::GetBrush("PainterEditor.ToolsTab.Redo32"))
					]
				]
				//Debug Purposes
				/*+SWrapBox::Slot()
				[
					SNew( SButton )
					.Text( LOCTEXT( "Check", "Check" ) )
					.OnClicked_Raw(iController.Get(), &FOdysseyPainterEditorController::OnCheck)
				]*/
				+SWrapBox::Slot()
				[
					SNew( SButton )
					.Text( LOCTEXT( "Clear Undo History", "Clear Undo History" ) )
					.ToolTipText( LOCTEXT( "Clear Undos tooltip", "If the undo/redo is slow, clear the cache by clicking this button" ))
                    .VAlign(EVerticalAlignment::VAlign_Center)
					.OnClicked_Raw(iController.Get(), &FOdysseyPainterEditorController::OnClearUndo)
				]
				]
		];
}

/*
void
FOdysseyPainterEditorGUI::CreateUndoHistoryTab(FOdysseyPainterEditor* iEditor, TSharedPtr<FOdysseyPainterEditorController>& iController)
{
    mUndoHistoryTab = SNew( SOdysseyUndoHistory, &mUndoHistory );
}*/

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

void
FOdysseyPainterEditorGUI::PerformanceOptions(FOdysseyPerformanceOptions* iPerformanceOptions)
{
	mPerformanceOptions = iPerformanceOptions;
}

TSharedPtr<FOdysseyPainterEditorViewportTab>&
FOdysseyPainterEditorGUI::GetViewportTab()
{
	return mViewportTab;
}

TSharedPtr<FOdysseyPainterEditorBrushSelectorTab>&
FOdysseyPainterEditorGUI::GetBrushSelectorTab()
{
	return mBrushSelectorTab;
}

TSharedPtr<FOdysseyPainterEditorMeshSelectorTab>&
FOdysseyPainterEditorGUI::GetMeshSelectorTab()
{
	return mMeshSelectorTab;
}

TSharedPtr<FOdysseyPainterEditorBrushExposedParametersTab>&
FOdysseyPainterEditorGUI::GetBrushExposedParametersTab()
{
	return mBrushExposedParametersTab;
}

TSharedPtr<FOdysseyPainterEditorColorWheelTab>&
FOdysseyPainterEditorGUI::GetColorWheelTab()
{
	return mColorWheelTab;
}

TSharedPtr<FOdysseyPainterEditorColorSlidersTab>&
FOdysseyPainterEditorGUI::GetColorSlidersTab()
{
	return mColorSlidersTab;
}

TSharedPtr<FOdysseyPainterEditorStrokeOptionsTab>&
FOdysseyPainterEditorGUI::GetStrokeOptionsTab()
{
	return mStrokeOptionsTab;
}

TSharedPtr<SOdysseyPerformanceOptions>&
FOdysseyPainterEditorGUI::GetPerformanceOptionsTab()
{
	return mPerformanceOptionsTab;
}

//TSharedPtr<SOdysseyUndoHistory>&
//FOdysseyPainterEditorGUI::GetUndoHistoryTab()
//{
//	return mUndoHistoryTab;
//}

TSharedPtr<FOdysseyPainterEditorTopTab>&
FOdysseyPainterEditorGUI::GetTopTab()
{
	return mTopTab;
}

TSharedPtr<SWidget>&
FOdysseyPainterEditorGUI::GetToolsTab()
{
	return mToolsTab;
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Spawner callbacks

/* TSharedRef< SDockTab >
FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnBrushSelector( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == smBrushSelectorTabId );

    return SNew( SDockTab )
        .ShouldAutosize( true )
        .Label( LOCTEXT( "BrushSelectorTitle", "BrushSelector" ) )
        [
            SNew( SBox )
            .HeightOverride( 50 )
            [
                mBrushSelectorTab.ToSharedRef()
            ]
        ];
} */

/* TSharedRef< SDockTab >
FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnMeshSelector( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == smMeshSelectorTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "MeshSelectorTitle", "MeshSelector" ) )
        [
            mMeshSelectorTab.ToSharedRef()
        ];
} */

/* TSharedRef< SDockTab >
FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnBrushExposedParameters( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == smBrushExposedParametersTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "BrushExposedParametersTitle", "Brush Parameters" ) )
        [
            mBrushExposedParametersTab.ToSharedRef()
        ];
} */

/* TSharedRef<SDockTab>
FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnViewport( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == smViewportTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "ViewportTabTitle", "Viewport" ) )
        [
            mViewportTab.ToSharedRef()
        ];
} */

/* TSharedRef< SDockTab >
FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnColorSelector( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == smColorSelectorTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "ColorSelectorTitle", "Color Selector" ) )
        [
            mColorSelectorTab.ToSharedRef()
        ];
}*/

/* TSharedRef< SDockTab >
FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnColorSliders( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == smColorSlidersTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "ColorSlidersTitle", "Color Sliders" ) )
        [
            mColorSlidersTab.ToSharedRef()
        ];
} */

/* TSharedRef< SDockTab >
FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnTopBar( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == smTopBarTabId );

    return SNew( SDockTab )
        .ShouldAutosize( true )
        .Label( LOCTEXT( "TopBarTitle", "TopBar" ) )
        [
            mTopTab.ToSharedRef()
        ];
} */

/* TSharedRef< SDockTab >
FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnStrokeOptions( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == smStrokeOptionsTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "StrokeOptionsTitle", "Stroke Options" ) )
        [
            mStrokeOptionsTab.ToSharedRef()
        ];
} */

TSharedRef< SDockTab >
FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnPerformanceOptions( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == smPerformanceOptionsTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "PerformanceOptionsTitle", "Performance Options" ) )
        [
            mPerformanceOptionsTab.ToSharedRef()
        ];
}

TSharedRef< SDockTab >
FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnNotes( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == smNotesTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "Notes Title", "Notes" ) )
        [
            SNew( SMultiLineEditableText )
        ];
}

/*
TSharedRef< SDockTab >
FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnUndoHistory( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == smUndoHistoryTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "Undo History Title", "Undo History" ) )
        [
            mUndoHistoryTab.ToSharedRef()
        ];
}*/

TSharedRef< SDockTab >
FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnTools( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == smToolsTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "Tool Box", "Tools" ) )
        [
			mToolsTab.ToSharedRef()
        ];
}

#undef LOCTEXT_NAMESPACE
