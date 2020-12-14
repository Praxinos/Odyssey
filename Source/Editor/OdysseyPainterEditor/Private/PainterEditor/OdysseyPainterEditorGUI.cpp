// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorGUI.h"

#include "Widgets/Text/SMultiLineEditableText.h"

#include "Brush/SOdysseyBrushExposedParameters.h"
#include "Brush/SOdysseyBrushSelector.h"
#include "Color/SOdysseyColorSelector.h"
#include "Color/SOdysseyColorSliders.h"
#include "Mesh/SOdysseyMeshSelector.h"
#include "SOdysseyPaintModifiers.h"
#include "SOdysseyPerformanceOptions.h"
#include "SOdysseyStrokeOptions.h"
#include "SOdysseySurfaceViewport.h"
#include "UndoHistory/SOdysseyUndoHistory.h"
#include "Models/OdysseyPainterEditorViewportClient.h"
#include "OdysseyPainterEditorController.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SWrapBox.h"
#include "OdysseyStyleSet.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorToolkit"

/*static*/const FName FOdysseyPainterEditorGUI::smViewportTabId( TEXT( "OdysseyPainterEditor_Viewport" ) );
/*static*/const FName FOdysseyPainterEditorGUI::smBrushSelectorTabId( TEXT( "OdysseyPainterEditor_BrushSelector" ) );
/*static*/const FName FOdysseyPainterEditorGUI::smMeshSelectorTabId( TEXT( "OdysseyPainterEditor_MeshSelector" ) );
/*static*/const FName FOdysseyPainterEditorGUI::smBrushExposedParametersTabId( TEXT( "OdysseyPainterEditor_BrushExposedParameters" ) );
/*static*/const FName FOdysseyPainterEditorGUI::smColorSelectorTabId( TEXT( "OdysseyPainterEditor_ColorSelector" ) );
/*static*/const FName FOdysseyPainterEditorGUI::smColorSlidersTabId( TEXT( "OdysseyPainterEditor_ColorSliders" ) );
/*static*/const FName FOdysseyPainterEditorGUI::smTopBarTabId( TEXT( "OdysseyPainterEditor_TopBar" ) );
/*static*/const FName FOdysseyPainterEditorGUI::smStrokeOptionsTabId( TEXT( "OdysseyPainterEditor_StrokeOptions" ) );
/*static*/const FName FOdysseyPainterEditorGUI::smNotesTabId( TEXT( "OdysseyPainterEditor_Notes" ) );
// /*static*/const FName FOdysseyPainterEditorGUI::smUndoHistoryTabId( TEXT( "OdysseyPainterEditor_UndoHistory" ) );
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

FOdysseyPainterEditorGUI::FOdysseyPainterEditorGUI(const FName iLayoutName)
	: mLayout(FTabManager::NewLayout(iLayoutName))
	, mPerformanceOptions( NULL )
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyPainterEditorGUI::InitOdysseyPainterEditorGUI(TSharedPtr<FOdysseyPainterEditorData> iData, TSharedPtr<FOdysseyPainterEditorController> iController)
{
	CreateWidgets(iData, iController);
	CreateLayout();
}

const TSharedRef<FTabManager::FLayout>&
FOdysseyPainterEditorGUI::GetLayout() const
{
	return mLayout;
}

void
FOdysseyPainterEditorGUI::CreateWidgets(TSharedPtr<FOdysseyPainterEditorData>& iData, TSharedPtr<FOdysseyPainterEditorController>& iController)
{
	// Create tabs contents
	CreateMeshSelectorTab(iData, iController);
	CreateViewportTab(iData, iController);
	CreateBrushSelectorTab(iData, iController);
	CreateBrushExposedParametersTab(iData, iController);
	CreateColorSelectorTab(iData, iController);
	CreateColorSlidersTab(iData, iController);
	CreateTopTab(iData, iController);
	CreateStrokeOptionsTab(iData, iController);
	CreatePerformanceOptionsTab(iData, iController);
	//CreateUndoHistoryTab(iData, iController);
	CreateToolsTab(iData, iController);
}

void
FOdysseyPainterEditorGUI::CreateLayout()
{
    // Build Layout
	mLayout
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
			->AddTab(smBrushSelectorTabId, ETabState::OpenedTab)
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
			->AddTab(smBrushExposedParametersTabId, ETabState::OpenedTab)
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
			->AddTab(smStrokeOptionsTabId, ETabState::OpenedTab)
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.3f)
			// Performance Options
			->AddTab( smPerformanceOptionsTabId, ETabState::ClosedTab )
			->SetHideTabWell( false )
			->SetSizeCoefficient( 0.3f )
			// Mesh Selector
			->AddTab(smMeshSelectorTabId, ETabState::OpenedTab)
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
			->AddTab(smColorSelectorTabId, ETabState::OpenedTab)
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.2f)
		)
		// ColorSliders
		->Split
		(
			FTabManager::NewStack()
			->AddTab(smColorSlidersTabId, ETabState::OpenedTab)
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.2f)
		);
}

TSharedRef<FTabManager::FSplitter>
FOdysseyPainterEditorGUI::CreateMiddleSection()
{

	return FTabManager::NewSplitter()
		->SetOrientation(Orient_Vertical)
		// Top Bar
		->Split
		(
			FTabManager::NewStack()
			->AddTab(smTopBarTabId, ETabState::OpenedTab)
			->SetHideTabWell(true)
			->SetSizeCoefficient(0.1f)
		)
		// Viewport
		->Split
		(
			FTabManager::NewStack()
			->AddTab(smViewportTabId, ETabState::OpenedTab)
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
    iTabManager->RegisterTabSpawner( smViewportTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnViewport ) )
        .SetDisplayName( LOCTEXT( "ViewportTab", "Viewport" ) )
        .SetGroup(iWorkspaceMenuCategoryRef)
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.Viewport16" ) );

    // BrushSelector
    iTabManager->RegisterTabSpawner( smBrushSelectorTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnBrushSelector ) )
        .SetDisplayName( LOCTEXT( "BrushSelectorTab", "Brush Selector" ) )
        .SetGroup(iWorkspaceMenuCategoryRef)
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.BrushSelector16" ) );

    // MeshSelector
    iTabManager->RegisterTabSpawner( smMeshSelectorTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnMeshSelector ) )
        .SetDisplayName( LOCTEXT( "MeshSelectorTab", "Mesh Selector" ) )
        .SetGroup(iWorkspaceMenuCategoryRef)
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.Mesh16" ) );

    // BrushExposedParameters
    iTabManager->RegisterTabSpawner( smBrushExposedParametersTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnBrushExposedParameters ) )
        .SetDisplayName( LOCTEXT( "BrushExposedParametersTab", "Brush Parameters" ) )
        .SetGroup(iWorkspaceMenuCategoryRef)
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.BrushExposedParameters16" ) );

    // ColorSelector
    iTabManager->RegisterTabSpawner( smColorSelectorTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnColorSelector ) )
        .SetDisplayName( LOCTEXT( "ColorSelectorTab", "ColorSelector" ) )
        .SetGroup(iWorkspaceMenuCategoryRef)
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.ColorWheel16" ) );

    // ColorSliders
    iTabManager->RegisterTabSpawner( smColorSlidersTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnColorSliders ) )
        .SetDisplayName( LOCTEXT( "ColorSlidersTab", "ColorSliders" ) )
        .SetGroup(iWorkspaceMenuCategoryRef)
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.ColorSliders_2_16" ) );

    // TopBar
    iTabManager->RegisterTabSpawner( smTopBarTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnTopBar ) )
        .SetDisplayName( LOCTEXT( "TopBarTab", "TopBar" ) )
        .SetGroup(iWorkspaceMenuCategoryRef)
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.TopBar16" ) );

    // StrokeOptions
    iTabManager->RegisterTabSpawner( smStrokeOptionsTabId, FOnSpawnTab::CreateSP( this, &FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnStrokeOptions ) )
        .SetDisplayName( LOCTEXT( "StrokeOptionsTab", "Stroke Options" ) )
        .SetGroup(iWorkspaceMenuCategoryRef)
        .SetIcon( FSlateIcon( "OdysseyStyle", "PainterEditor.StrokeOptions16" ) );

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
    iTabManager->UnregisterTabSpawner( smViewportTabId );
    iTabManager->UnregisterTabSpawner( smBrushSelectorTabId );
    iTabManager->UnregisterTabSpawner( smMeshSelectorTabId );
    iTabManager->UnregisterTabSpawner( smBrushExposedParametersTabId );
    iTabManager->UnregisterTabSpawner( smColorSelectorTabId );
    iTabManager->UnregisterTabSpawner( smColorSlidersTabId );
    iTabManager->UnregisterTabSpawner( smTopBarTabId );
    iTabManager->UnregisterTabSpawner( smStrokeOptionsTabId );
    iTabManager->UnregisterTabSpawner( smPerformanceOptionsTabId );
    iTabManager->UnregisterTabSpawner( smNotesTabId );
    //iTabManager->UnregisterTabSpawner( smUndoHistoryTabId );
    iTabManager->UnregisterTabSpawner( smToolsTabId );
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- Internal widget creation
void
FOdysseyPainterEditorGUI::CreateViewportTab(TSharedPtr<FOdysseyPainterEditorData>& iData, TSharedPtr<FOdysseyPainterEditorController>& iController)
{
    mViewportTab = SNew( SOdysseySurfaceViewport );
    mViewportTab->SetSurface( iData->DisplaySurface() );
	FOdysseyPainterEditorViewportClient* viewportClient = new FOdysseyPainterEditorViewportClient(iData, mViewportTab, mMeshSelectorTab->GetMeshSelectorPtr());
	viewportClient->OnPickColor().AddRaw(iController.Get(), &FOdysseyPainterEditorController::HandleViewportColorPicked);
	mViewportTab->SetViewportClient(MakeShareable(viewportClient));

    mViewportTab->OnParameterChanged().AddRaw( iController.Get(), &FOdysseyPainterEditorController::HandleViewportParameterChanged );
}

void
FOdysseyPainterEditorGUI::CreateBrushSelectorTab(TSharedPtr<FOdysseyPainterEditorData>& iData, TSharedPtr<FOdysseyPainterEditorController>& iController)
{
    mBrushSelectorTab = SNew( SOdysseyBrushSelector )
        .OnBrushChanged_Raw( iController.Get(), &FOdysseyPainterEditorController::OnBrushSelected );
}

void
FOdysseyPainterEditorGUI::CreateMeshSelectorTab(TSharedPtr<FOdysseyPainterEditorData>& iData, TSharedPtr<FOdysseyPainterEditorController>& iController)
{
    mMeshSelectorTab = SNew( SOdysseyMeshSelector )
        .OnMeshChanged_Raw(iController.Get(), &FOdysseyPainterEditorController::OnMeshSelected );
}

void
FOdysseyPainterEditorGUI::CreateBrushExposedParametersTab(TSharedPtr<FOdysseyPainterEditorData>& iData, TSharedPtr<FOdysseyPainterEditorController>& iController)
{
    mBrushExposedParametersTab = SNew( SOdysseyBrushExposedParameters )
        .OnParameterChanged_Raw(iController.Get(), &FOdysseyPainterEditorController::HandleBrushParameterChanged );
}

void
FOdysseyPainterEditorGUI::CreateColorSelectorTab(TSharedPtr<FOdysseyPainterEditorData>& iData, TSharedPtr<FOdysseyPainterEditorController>& iController)
{
    mColorSelectorTab = SNew( SOdysseyColorSelector )
		.Color(iData.ToSharedRef(), &FOdysseyPainterEditorData::PaintColor)
        .OnColorChange_Raw(iController.Get(), &FOdysseyPainterEditorController::HandlePaintColorChange);
}

void
FOdysseyPainterEditorGUI::CreateColorSlidersTab(TSharedPtr<FOdysseyPainterEditorData>& iData, TSharedPtr<FOdysseyPainterEditorController>& iController)
{
    mColorSlidersTab = SNew( SOdysseyColorSliders )
		.Color(iData.ToSharedRef(), &FOdysseyPainterEditorData::PaintColor)
        .OnColorChange_Raw(iController.Get(), &FOdysseyPainterEditorController::HandlePaintColorChange);
}

void
FOdysseyPainterEditorGUI::CreateTopTab(TSharedPtr<FOdysseyPainterEditorData>& iData, TSharedPtr<FOdysseyPainterEditorController>& iController)
{
    mTopTab = SNew( SOdysseyPaintModifiers )
        .OnSizeChanged_Raw(iController.Get(), &FOdysseyPainterEditorController::HandleSizeModifierChanged )
        .OnOpacityChanged_Raw(iController.Get(), &FOdysseyPainterEditorController::HandleOpacityModifierChanged )
        .OnFlowChanged_Raw(iController.Get(), &FOdysseyPainterEditorController::HandleFlowModifierChanged )
        .OnBlendingModeChanged_Raw(iController.Get(), &FOdysseyPainterEditorController::HandleBlendingModeModifierChanged )
        .OnAlphaModeChanged_Raw(iController.Get(), &FOdysseyPainterEditorController::HandleAlphaModeModifierChanged );

	mTopTab->SetAlphaMode(iData->PaintEngine()->GetAlphaMode());
	mTopTab->SetSize(20);
	mTopTab->SetOpacity(100);
	mTopTab->SetFlow(100);
}

void
FOdysseyPainterEditorGUI::CreateStrokeOptionsTab(TSharedPtr<FOdysseyPainterEditorData>& iData, TSharedPtr<FOdysseyPainterEditorController>& iController)
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
}

void
FOdysseyPainterEditorGUI::CreatePerformanceOptionsTab(TSharedPtr<FOdysseyPainterEditorData>& iData, TSharedPtr<FOdysseyPainterEditorController>& iController)
{
	mPerformanceOptions->mOnDrawBrushPreviewChanged.BindRaw(iController.Get(), &FOdysseyPainterEditorController::HandlePerformanceDrawBrushPreviewChanged);
	mPerformanceOptionsTab = SNew(SOdysseyPerformanceOptions)
		.PerformanceOptions(mPerformanceOptions);
}

void
FOdysseyPainterEditorGUI::CreateToolsTab(TSharedPtr<FOdysseyPainterEditorData>& iData, TSharedPtr<FOdysseyPainterEditorController>& iController)
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
				.Text( LOCTEXT( "UndoRedo  (Experimental)", "UndoRedo  (Experimental)" ) )
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
FOdysseyPainterEditorGUI::CreateUndoHistoryTab(TSharedPtr<FOdysseyPainterEditorData>& iData, TSharedPtr<FOdysseyPainterEditorController>& iController)
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

TSharedPtr<SOdysseySurfaceViewport>&
FOdysseyPainterEditorGUI::GetViewportTab()
{
	return mViewportTab;
}

TSharedPtr<SOdysseyBrushSelector>&
FOdysseyPainterEditorGUI::GetBrushSelectorTab()
{
	return mBrushSelectorTab;
}

TSharedPtr<SOdysseyMeshSelector>&
FOdysseyPainterEditorGUI::GetMeshSelectorTab()
{
	return mMeshSelectorTab;
}

TSharedPtr<SOdysseyBrushExposedParameters>&
FOdysseyPainterEditorGUI::GetBrushExposedParametersTab()
{
	return mBrushExposedParametersTab;
}

TSharedPtr<SOdysseyColorSelector>&
FOdysseyPainterEditorGUI::GetColorSelectorTab()
{
	return mColorSelectorTab;
}

TSharedPtr<SOdysseyColorSliders>&
FOdysseyPainterEditorGUI::GetColorSlidersTab()
{
	return mColorSlidersTab;
}

TSharedPtr<SOdysseyStrokeOptions>&
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

TSharedPtr<SOdysseyPaintModifiers>&
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

TSharedRef< SDockTab >
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
}

TSharedRef< SDockTab >
FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnMeshSelector( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == smMeshSelectorTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "MeshSelectorTitle", "MeshSelector" ) )
        [
            mMeshSelectorTab.ToSharedRef()
        ];
}

TSharedRef< SDockTab >
FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnBrushExposedParameters( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == smBrushExposedParametersTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "BrushExposedParametersTitle", "Brush Parameters" ) )
        [
            mBrushExposedParametersTab.ToSharedRef()
        ];
}

TSharedRef<SDockTab>
FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnViewport( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == smViewportTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "ViewportTabTitle", "Viewport" ) )
        [
            mViewportTab.ToSharedRef()
        ];
}

TSharedRef< SDockTab >
FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnColorSelector( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == smColorSelectorTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "ColorSelectorTitle", "Color Selector" ) )
        [
            mColorSelectorTab.ToSharedRef()
        ];
}

TSharedRef< SDockTab >
FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnColorSliders( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == smColorSlidersTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "ColorSlidersTitle", "Color Sliders" ) )
        [
            mColorSlidersTab.ToSharedRef()
        ];
}

TSharedRef< SDockTab >
FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnTopBar( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == smTopBarTabId );

    return SNew( SDockTab )
        .ShouldAutosize( true )
        .Label( LOCTEXT( "TopBarTitle", "TopBar" ) )
        [
            mTopTab.ToSharedRef()
        ];
}

TSharedRef< SDockTab >
FOdysseyPainterEditorGUI::HandleTabSpawnerSpawnStrokeOptions( const FSpawnTabArgs& iArgs )
{
    check( iArgs.GetTabId() == smStrokeOptionsTabId );

    return SNew( SDockTab )
        .Label( LOCTEXT( "StrokeOptionsTitle", "Stroke Options" ) )
        [
            mStrokeOptionsTab.ToSharedRef()
        ];
}

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
