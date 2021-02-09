// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyFlipbookEditor.h"

#include "OdysseyFlipbookEditorController.h"
#include "OdysseyFlipbookEditorData.h"
#include "OdysseyFlipbookEditorGUI.h"

#include "OdysseyFlipbookEditorToolkit.h"

#include "Types/NavigationMetaData.h"

#define LOCTEXT_NAMESPACE "OdysseyFlipbookEditor"

/////////////////////////////////////////////////////
// FOdysseyFlipbookEditor
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyFlipbookEditor::~FOdysseyFlipbookEditor()
{
	mFlipbookWrapper->OnSpriteTextureChanged().Remove(mOnSpriteTextureChangedHandle);
}

FOdysseyFlipbookEditor::FOdysseyFlipbookEditor(TSharedPtr<FOdysseyPainterEditorToolkit> iToolkit) :
	FOdysseyPainterEditor(iToolkit),
	mFlipbookWrapper(nullptr),
	mData(nullptr),
	mGUI(nullptr),
	mController(nullptr)
{
}

FOdysseyFlipbookEditor::FOdysseyFlipbookEditor(UPaperFlipbook* iFlipbook, TSharedPtr<FOdysseyPainterEditorToolkit> iToolkit) :
	FOdysseyPainterEditor(iToolkit),
	mFlipbookWrapper(MakeShareable(new FOdysseyFlipbookWrapper(iFlipbook))),
	mData(nullptr),
	mGUI(nullptr),
	mController(nullptr)
{
	mData = MakeShareable(new FOdysseyFlipbookEditorData(mFlipbookWrapper));
	mGUI = MakeShareable(new FOdysseyFlipbookEditorGUI());
	mController = MakeShareable(new FOdysseyFlipbookEditorController(this, mGUI));
}

void
FOdysseyFlipbookEditor::Init()
{
	FOdysseyPainterEditor::Init();

	mData->Init();
	mGUI->Init(mData, mController);
	mController->Init();
}

void
FOdysseyFlipbookEditor::OnToolkitInitialized()
{
	mController->OnToolkitInitialized();
}

bool
FOdysseyFlipbookEditor::OnCloseRequested()
{ 
	//FOdysseyPainterEditor::OnCloseRequested();
	GetData()->PaintEngine()->Flush();

	mData->SyncTextureWithSurfaceBlock();
	mData->ApplyPropertiesBackup();

	//TODO: Move in the right place
    if (mData->LayerStack())
	{
		mData->LayerStack()->mDrawingUndo->Clear();
	}
    return true;
}

const TSharedRef<FTabManager::FLayout>&
FOdysseyFlipbookEditor::CreateLayout() const
{
	return mGUI->GetLayout();
}

const TArray<TSharedPtr<FExtender>>&
FOdysseyFlipbookEditor::CreateMenuExtenders() const
{
	return mController->GetMenuExtenders();
}

TSharedPtr<FWorkspaceItem>
FOdysseyFlipbookEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager)
{
    TSharedPtr<FWorkspaceItem> workspaceMenuCategory = iTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_OdysseyFlipbookEditor", "Odyssey Flipbook Editor"));
	TSharedRef<FWorkspaceItem> workspaceMenuCategoryRef = workspaceMenuCategory.ToSharedRef();
	mGUI->RegisterTabSpawners(iTabManager, workspaceMenuCategoryRef);
	return workspaceMenuCategory;
}

void
FOdysseyFlipbookEditor::UnregisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager)
{
	mGUI->UnregisterTabSpawners(iTabManager);
}

/**
 * TODO:
 * 2) Move Data directly in editor (allowing data methods override on Editor inheritance)
 * 3) Make Tabs classes, containing the creation of GUI and a pointer to a controller specific for this GUI (This one is a BIG one)
 * 3.1) while making 3), the old almighty controller can coexist with the new Tabs classes, so we can make each Tab class + controller one after the other
 * 4) Make FlipbookEditor Inherite TextureEditor and cleanup
 * 5) Test and Debug
 * 6) Hooray !
 */

//TEMPORARY
TSharedPtr<FOdysseyFlipbookEditorData>
FOdysseyFlipbookEditor::GetData()
{
	return mData;
}

#undef LOCTEXT_NAMESPACE
