// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTextureEditor.h"

#include "OdysseyTextureEditorController.h"
#include "OdysseyTextureEditorData.h"
#include "OdysseyTextureEditorGUI.h"

#include "OdysseyPainterEditorToolkit.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditor"

/////////////////////////////////////////////////////
// FOdysseyTextureEditor
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditor::~FOdysseyTextureEditor()
{
	//Finalize in reverse order
	mController = nullptr; //calls destructor
	mGUI = nullptr; //calls destructor
	mData = nullptr; //calls destructor
}

FOdysseyTextureEditor::FOdysseyTextureEditor(TSharedPtr<FOdysseyPainterEditorToolkit> iToolkit) :
	FOdysseyPainterEditor(iToolkit),
	mData(nullptr),
	mGUI(nullptr),
	mController(nullptr)
{
}

FOdysseyTextureEditor::FOdysseyTextureEditor(UTexture2D* iTexture, TSharedPtr<FOdysseyPainterEditorToolkit> iToolkit) :
	FOdysseyPainterEditor(iToolkit),
	mData(nullptr),
	mGUI(nullptr),
	mController(nullptr)
{
	mData = MakeShareable(new FOdysseyTextureEditorData(iTexture));
	mGUI = MakeShareable(new FOdysseyTextureEditorGUI());
	mController = MakeShareable(new FOdysseyTextureEditorController(this, mGUI));
}

void
FOdysseyTextureEditor::Init()
{
	FOdysseyPainterEditor::Init();

	//----

	mData->Init();
	mGUI->Init(mData, mController);
	mController->Init();
}

bool
FOdysseyTextureEditor::OnCloseRequested()
{
	mData->OnCloseRequested();
	return true;
}

const TSharedRef<FTabManager::FLayout>&
FOdysseyTextureEditor::CreateLayout() const
{
	return mGUI->GetLayout();
}

const TArray<TSharedPtr<FExtender>>&
FOdysseyTextureEditor::CreateMenuExtenders() const
{
	return mController->GetMenuExtenders();
}

TSharedPtr<FWorkspaceItem>
FOdysseyTextureEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager)
{
    TSharedPtr<FWorkspaceItem> workspaceMenuCategory = iTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_OdysseyTextureEditor", "Odyssey Texture Editor"));
	TSharedRef<FWorkspaceItem> workspaceMenuCategoryRef = workspaceMenuCategory.ToSharedRef();
	mGUI->RegisterTabSpawners(iTabManager, workspaceMenuCategoryRef);
	return workspaceMenuCategory;
}

void
FOdysseyTextureEditor::UnregisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager)
{
	mGUI->UnregisterTabSpawners(iTabManager);
}

//TEMPORARY
TSharedPtr<FOdysseyTextureEditorData>
FOdysseyTextureEditor::GetData()
{
	return mData;
}
#undef LOCTEXT_NAMESPACE