// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyEditorGUI.h"

#include "OdysseyEditor.h"
#include "OdysseyEditorTab.h"

/////////////////////////////////////////////////////
// FOdysseyEditorGUI
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyEditorGUI::~FOdysseyEditorGUI()
{
}

FOdysseyEditorGUI::FOdysseyEditorGUI(FOdysseyEditor* iEditor)
	: mEditor(iEditor)
	, mLayout(nullptr)
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyEditorGUI::Init()
{
	CreateTabs(); //Create Tabs Objects and sets their corresponding controllers
	InitTabs(); //Init Tabs, creating their widgets

	mLayout = CreateLayout();
	mWidget = CreateWidget();
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------- Tabs

void
FOdysseyEditorGUI::CreateTabs()
{
}

void
FOdysseyEditorGUI::InitTabs()
{
	for (int i = 0; i < mTabs.Num(); i++)
	{
		mTabs[i].Get()->Init();
	}
}

void
FOdysseyEditorGUI::BindShortcuts(FBaseToolkit* iToolkit)
{
	for (int i = 0; i < mTabs.Num(); i++)
	{
		mTabs[i].Get()->BindShortcuts(iToolkit);
	}
}

void
FOdysseyEditorGUI::RegisterTabSpawners( const TSharedRef< class FTabManager >& iTabManager, TSharedRef<FWorkspaceItem>& iWorkspaceMenuCategoryRef)
{
	for (int i = 0; i < mTabs.Num(); i++)
	{
		iTabManager->RegisterTabSpawner(mTabs[i].Get()->ID(), FOnSpawnTab::CreateSP( mTabs[i].Get().ToSharedRef(), &FOdysseyEditorTab::SpawnTab ) )
			.SetDisplayName( mTabs[i].Get()->DisplayName() )
			.SetGroup(iWorkspaceMenuCategoryRef)
			.SetIcon( mTabs[i].Get()->Icon() );
	}
}

void
FOdysseyEditorGUI::UnregisterTabSpawners( const TSharedRef< class FTabManager >& iTabManager )
{
	for (int i = 0; i < mTabs.Num(); i++)
	{
		iTabManager->UnregisterTabSpawner( mTabs[i].Get()->ID() );
	}
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------- Menu and Toolbar

void
FOdysseyEditorGUI::FillExtender(FBaseToolkit* iToolkit, TSharedPtr<FExtender>& ioExtender)
{
	for (int i = 0; i < mTabs.Num(); i++)
	{
		mTabs[i].Get()->FillExtender(iToolkit, ioExtender);
	}
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Listeners

void
FOdysseyEditorGUI::OnToolkitInitialized(FBaseToolkit* iToolkit)
{
	for (int i = 0; i < mTabs.Num(); i++)
	{
		mTabs[i].Get()->OnToolkitInitialized(iToolkit);
	}
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------- Layout

TSharedPtr<FTabManager::FLayout>
FOdysseyEditorGUI::CreateLayout()
{
	return FTabManager::NewLayout(GetLayoutName());
}

TSharedPtr<SWidget>
FOdysseyEditorGUI::CreateWidget()
{
	return nullptr;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

TSharedRef<FTabManager::FLayout>
FOdysseyEditorGUI::GetLayout()
{
	return mLayout.ToSharedRef();
}

TSharedRef<SWidget>
FOdysseyEditorGUI::GetWidget()
{
	return mWidget.ToSharedRef();
}
