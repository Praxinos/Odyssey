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
	mData(MakeShareable(new FOdysseyTextureEditorData(iTexture))),
	mGUI(MakeShareable(new FOdysseyTextureEditorGUI())),
	mController(MakeShareable(new FOdysseyTextureEditorController(mData, mGUI)))
{
}

void
FOdysseyTextureEditor::Init()
{
	FOdysseyPainterEditor::Init();

	//----

	mData->Init();
	mGUI->Init(mData, mController);
	mController->Init(GetToolkit()->GetToolkitCommands());
}

bool
FOdysseyTextureEditor::OnCloseRequested()
{
	mData->PaintEngine()->Flush();
	mData->SyncTextureAndInvalidate();
	mData->ApplyPropertiesBackup();

	//TODO: Move in the right place
    mData->LayerStack()->mDrawingUndo->Clear();
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
#undef LOCTEXT_NAMESPACE