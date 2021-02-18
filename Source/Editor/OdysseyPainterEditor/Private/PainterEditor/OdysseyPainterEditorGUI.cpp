// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorGUI.h"

#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorTab.h"
#include "OdysseyPainterEditorBrushSelectorTab.h"
#include "OdysseyPainterEditorBrushExposedParametersTab.h"
#include "OdysseyPainterEditorColorSlidersTab.h"
#include "OdysseyPainterEditorColorWheelTab.h"
#include "OdysseyPainterEditorMeshSelectorTab.h"
#include "OdysseyPainterEditorStrokeOptionsTab.h"
#include "OdysseyPainterEditorTopTab.h"
#include "OdysseyPainterEditorToolsTab.h"
#include "OdysseyPainterEditorViewportTab.h"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorGUI
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorGUI::~FOdysseyPainterEditorGUI()
{
}

FOdysseyPainterEditorGUI::FOdysseyPainterEditorGUI(FOdysseyPainterEditor* iEditor, const FName iLayoutName)
	: mEditor(iEditor)
	, mLayoutName(iLayoutName)
	, mLayout(nullptr)
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
	mToolsTab = MakeShareable(new FOdysseyPainterEditorToolsTab(mEditor));
	mTopTab = MakeShareable(new FOdysseyPainterEditorTopTab(mEditor));
	mStrokeOptionsTab = MakeShareable(new FOdysseyPainterEditorStrokeOptionsTab(mEditor));
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
	mToolsTab->Init();
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
	mToolsTab->OnToolkitInitialized();
	mTopTab->OnToolkitInitialized();
	mStrokeOptionsTab->OnToolkitInitialized();
}

void
FOdysseyPainterEditorGUI::RegisterTabSpawners( const TSharedRef< class FTabManager >& iTabManager, TSharedRef<FWorkspaceItem>& iWorkspaceMenuCategoryRef)
{
	mViewportTab->RegisterTabSpawner(iTabManager, iWorkspaceMenuCategoryRef);
	mBrushSelectorTab->RegisterTabSpawner(iTabManager, iWorkspaceMenuCategoryRef);
	mMeshSelectorTab->RegisterTabSpawner(iTabManager, iWorkspaceMenuCategoryRef);
	mBrushExposedParametersTab->RegisterTabSpawner(iTabManager, iWorkspaceMenuCategoryRef);
	mColorWheelTab->RegisterTabSpawner(iTabManager, iWorkspaceMenuCategoryRef);
	mColorSlidersTab->RegisterTabSpawner(iTabManager, iWorkspaceMenuCategoryRef);
	mTopTab->RegisterTabSpawner(iTabManager, iWorkspaceMenuCategoryRef);
	mStrokeOptionsTab->RegisterTabSpawner(iTabManager, iWorkspaceMenuCategoryRef);
	mToolsTab->RegisterTabSpawner(iTabManager, iWorkspaceMenuCategoryRef);
}

void
FOdysseyPainterEditorGUI::UnregisterTabSpawners( const TSharedRef< class FTabManager >& iTabManager )
{
	mViewportTab->UnregisterTabSpawner(iTabManager);
	mBrushSelectorTab->UnregisterTabSpawner(iTabManager);
	mMeshSelectorTab->UnregisterTabSpawner(iTabManager);
	mBrushExposedParametersTab->UnregisterTabSpawner(iTabManager);
	mColorWheelTab->UnregisterTabSpawner(iTabManager);
	mColorSlidersTab->UnregisterTabSpawner(iTabManager);
	mTopTab->UnregisterTabSpawner(iTabManager);
	mStrokeOptionsTab->UnregisterTabSpawner(iTabManager);
	mToolsTab->UnregisterTabSpawner(iTabManager);
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
			->AddTab(mBrushSelectorTab->ID(), ETabState::OpenedTab)
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.1f)
		)
		// Brush Preview
		->Split
		(
			FTabManager::NewStack()
			// Brush Params
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
			->AddTab(mStrokeOptionsTab->ID(), ETabState::OpenedTab)
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.3f)
			// Mesh Selector
			->AddTab(mMeshSelectorTab->ID(), ETabState::OpenedTab)
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.3f)
		)
		// Tools
		->Split
		(
			FTabManager::NewStack()
			->AddTab(mToolsTab->ID(), ETabState::OpenedTab)
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.2f)
		);
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
			->AddTab(mColorWheelTab->ID(), ETabState::OpenedTab)
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.2f)
		)
		// ColorSliders
		->Split
		(
			FTabManager::NewStack()
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
			->AddTab(mTopTab->ID(), ETabState::OpenedTab)
			->SetHideTabWell(true)
			->SetSizeCoefficient(0.1f)
		)
		// Viewport
		->Split
		(
			FTabManager::NewStack()
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

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

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

TSharedPtr<FOdysseyPainterEditorTopTab>&
FOdysseyPainterEditorGUI::GetTopTab()
{
	return mTopTab;
}

TSharedPtr<FOdysseyPainterEditorToolsTab>&
FOdysseyPainterEditorGUI::GetToolsTab()
{
	return mToolsTab;
}
