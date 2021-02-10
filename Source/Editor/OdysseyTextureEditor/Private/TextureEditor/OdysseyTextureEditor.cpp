// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTextureEditor.h"

#include "OdysseyTextureEditorController.h"
#include "OdysseyTextureEditorGUI.h"

#include "OdysseyLayerStack.h"
#include "OdysseyPaintEngine.h"

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
	mTextureWrapper(nullptr),
	mGUI(nullptr),
	mController(nullptr)
{
}

FOdysseyTextureEditor::FOdysseyTextureEditor(UTexture2D* iTexture, TSharedPtr<FOdysseyPainterEditorToolkit> iToolkit) :
	FOdysseyPainterEditor(iToolkit),
    mTextureWrapper( iTexture ),
	mGUI(nullptr),
	mController(nullptr)
{
	mGUI = MakeShareable(new FOdysseyTextureEditorGUI());
	mController = MakeShareable(new FOdysseyTextureEditorController(this, mGUI));
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyTextureEditor::Init()
{
	FOdysseyPainterEditor::Init();

	//----
	
    mTextureWrapper.OnPreSaveDelegate().AddRaw(this, &FOdysseyTextureEditor::OnTexturePreSave);

	//----

	mGUI->Init(this, mController);
	mController->Init();
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

FOdysseyTextureWrapper&
FOdysseyTextureEditor::TextureWrapper()
{
	return mTextureWrapper;
}

UTexture2D*
FOdysseyTextureEditor::Texture()
{
	return mTextureWrapper.Texture();
}

FOdysseySurfaceEditable*
FOdysseyTextureEditor::DisplaySurface()
{
	return mTextureWrapper.Surface();
}

FOdysseyLayerStack*
FOdysseyTextureEditor::LayerStack() const
{
    return mTextureWrapper.LayerStack();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Overrides

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

bool
FOdysseyTextureEditor::OnCloseRequested()
{
	FOdysseyPainterEditor::OnCloseRequested();

    //TODO: Move in the right place
    if (LayerStack())
        LayerStack()->mDrawingUndo->Clear();

    mTextureWrapper.Texture(nullptr);
	return true;
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Overrides

void
FOdysseyTextureEditor::OnTexturePreSave()
{
    PaintEngine()->Flush();
}

#undef LOCTEXT_NAMESPACE