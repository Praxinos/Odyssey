// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorGUI.h"

#include "OdysseyEditorTab.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorBrushSelectorTab.h"
#include "OdysseyPainterEditorBrushExposedParametersTab.h"
#include "OdysseyPainterEditorColorSlidersTab.h"
#include "OdysseyPainterEditorColorWheelTab.h"
#include "OdysseyPainterEditorMeshSelectorTab.h"
#include "OdysseyPainterEditorStrokeOptionsTab.h"
#include "OdysseyPainterEditorTopTab.h"
#include "OdysseyPainterEditorToolsTab.h"
#include "OdysseyPainterEditorViewportTab.h"

#include "SOdysseyAboutScreen.h"
#include "SOdysseyTabletAPISwitcher.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorGUI"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorGUI
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorGUI::~FOdysseyPainterEditorGUI()
{
}

FOdysseyPainterEditorGUI::FOdysseyPainterEditorGUI(FOdysseyPainterEditor* iEditor)
	: FOdysseyEditorGUI(iEditor)
	, mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------- Tabs

void
FOdysseyPainterEditorGUI::CreateTabs()
{
	FOdysseyEditorGUI::CreateTabs();

	ODYSSEY_ADD_TAB(mMeshSelectorTab, FOdysseyPainterEditorMeshSelectorTab, mEditor)
	ODYSSEY_ADD_TAB(mViewportTab, FOdysseyPainterEditorViewportTab, mEditor);
	ODYSSEY_ADD_TAB(mBrushSelectorTab, FOdysseyPainterEditorBrushSelectorTab, mEditor);
	ODYSSEY_ADD_TAB(mBrushExposedParametersTab, FOdysseyPainterEditorBrushExposedParametersTab, mEditor);
	ODYSSEY_ADD_TAB(mColorWheelTab, FOdysseyPainterEditorColorWheelTab, mEditor);
	ODYSSEY_ADD_TAB(mColorSlidersTab, FOdysseyPainterEditorColorSlidersTab, mEditor);
	ODYSSEY_ADD_TAB(mToolsTab, FOdysseyPainterEditorToolsTab, mEditor);
	ODYSSEY_ADD_TAB(mTopTab, FOdysseyPainterEditorTopTab, mEditor);
	ODYSSEY_ADD_TAB(mStrokeOptionsTab, FOdysseyPainterEditorStrokeOptionsTab, mEditor);
}

void
FOdysseyPainterEditorGUI::BindShortcuts(FBaseToolkit* iToolkit)
{
	FOdysseyEditorGUI::BindShortcuts(iToolkit);

	//---

	const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

	#define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyPainterEditorGUI::__VA_ARGS__ ), FCanExecuteAction() );

	MAP_ACTION(painterEditorCommands.AboutIliad, AboutIliad )
	MAP_ACTION(painterEditorCommands.VisitPraxinosWebsite, VisitPraxinosWebsite )
	MAP_ACTION(painterEditorCommands.VisitPraxinosForums, VisitPraxinosForums )
	MAP_ACTION(painterEditorCommands.SwitchTabletAPI, VisitPraxinosForums )

	#undef MAP_ACTION
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------- Menu and Toolbar

void
FOdysseyPainterEditorGUI::FillExtender(FBaseToolkit* iToolkit, TSharedPtr<FExtender>& ioExtender)
{
	FOdysseyEditorGUI::FillExtender(iToolkit, ioExtender);

	//---

	ioExtender->AddMenuExtension(
        "HelpApplication",
        EExtensionHook::After,
		iToolkit->GetToolkitCommands(),
        FMenuExtensionDelegate::CreateRaw( this, &FOdysseyPainterEditorGUI::ExtendMenuAbout ) );
}

//static
void
FOdysseyPainterEditorGUI::ExtendMenuAbout( FMenuBuilder& ioMenuBuilder )
{
    ioMenuBuilder.BeginSection( "About", LOCTEXT( "OdysseyPainter", "ILIAD" ) );
    {
        ioMenuBuilder.AddMenuEntry( FOdysseyPainterEditorCommands::Get().AboutIliad );
        ioMenuBuilder.AddMenuEntry( FOdysseyPainterEditorCommands::Get().VisitPraxinosWebsite );
        ioMenuBuilder.AddMenuEntry( FOdysseyPainterEditorCommands::Get().VisitPraxinosForums );
    }
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------- Layout

TSharedPtr<FTabManager::FLayout>
FOdysseyPainterEditorGUI::CreateLayout()
{
	return FOdysseyEditorGUI::CreateLayout()
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

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Shortcuts

void
FOdysseyPainterEditorGUI::VisitPraxinosWebsite()
{
    FString URL = "https://praxinos.coop/";
    FPlatformProcess::LaunchURL( *URL, NULL, NULL );
}

void
FOdysseyPainterEditorGUI::VisitPraxinosForums()
{
    FString URL = "https://praxinos.coop/forum";
    FPlatformProcess::LaunchURL( *URL, NULL, NULL );
}

void
FOdysseyPainterEditorGUI::AboutIliad()
{
	TSharedPtr<SWindow> parentWindow = FGlobalTabmanager::Get()->GetRootWindow();
	SOdysseyAboutScreen::Open(parentWindow);
}

void
FOdysseyPainterEditorGUI::SwitchTabletAPI()
{
    SOdysseyTabletAPISwitcher::Open();
}

#undef LOCTEXT_NAMESPACE