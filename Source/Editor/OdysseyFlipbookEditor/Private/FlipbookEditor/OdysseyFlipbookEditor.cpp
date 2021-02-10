// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyFlipbookEditor.h"

#include "OdysseyFlipbookEditorController.h"
#include "OdysseyFlipbookEditorGUI.h"

#include "OdysseyFlipbookEditorToolkit.h"

#include "OdysseySurfaceReadOnly.h"
#include "OdysseyLayerStack.h"
#include "OdysseyPaintEngine.h"

#include "Types/NavigationMetaData.h"

#define LOCTEXT_NAMESPACE "OdysseyFlipbookEditor"

/////////////////////////////////////////////////////
// FOdysseyFlipbookEditor
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyFlipbookEditor::~FOdysseyFlipbookEditor()
{
	if (mPreviewSurface) {
		delete mPreviewSurface;
		mPreviewSurface = nullptr;
	}
}

FOdysseyFlipbookEditor::FOdysseyFlipbookEditor(TSharedPtr<FOdysseyPainterEditorToolkit> iToolkit) :
	FOdysseyPainterEditor(iToolkit),
	mFlipbookWrapper(nullptr),
	mTextureWrapper( nullptr ),
	mPreviewSurface(new FOdysseySurfaceReadOnly(nullptr)),
	mGUI(nullptr),
	mController(nullptr)
{
}

FOdysseyFlipbookEditor::FOdysseyFlipbookEditor(UPaperFlipbook* iFlipbook, TSharedPtr<FOdysseyPainterEditorToolkit> iToolkit) :
	FOdysseyPainterEditor(iToolkit),
	mFlipbookWrapper(MakeShareable(new FOdysseyFlipbookWrapper(iFlipbook))),
	mTextureWrapper( nullptr ),
	mPreviewSurface(new FOdysseySurfaceReadOnly(nullptr)),
	mGUI(nullptr),
	mController(nullptr)
{
	mGUI = MakeShareable(new FOdysseyFlipbookEditorGUI());
	mController = MakeShareable(new FOdysseyFlipbookEditorController(this, mGUI));
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyFlipbookEditor::Init()
{
	FOdysseyPainterEditor::Init();

	//----

	mTextureWrapper.OnPreSaveDelegate().AddRaw(this, &FOdysseyFlipbookEditor::OnTexturePreSave);
    
    //We don't need to initialize anything if there is no keyFrames
    if (mFlipbookWrapper->Flipbook()->GetNumKeyFrames() <= 0)
        return;

    Texture(mFlipbookWrapper->GetKeyframeTexture(0));

	//----

	mGUI->Init(this, mController);
	mController->Init();
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters


TSharedPtr<FOdysseyFlipbookWrapper>&
FOdysseyFlipbookEditor::FlipbookWrapper()
{
	return mFlipbookWrapper;
}

FOdysseyTextureWrapper&
FOdysseyFlipbookEditor::TextureWrapper()
{
	return mTextureWrapper;
}

UTexture2D*
FOdysseyFlipbookEditor::Texture()
{
	return mTextureWrapper.Texture();
}

FOdysseySurfaceEditable*
FOdysseyFlipbookEditor::DisplaySurface()
{
	return mTextureWrapper.Surface();
}

FOdysseyLayerStack*
FOdysseyFlipbookEditor::LayerStack() const
{
    return mTextureWrapper.LayerStack();
}

FOdysseySurfaceReadOnly*
FOdysseyFlipbookEditor::PreviewSurface()
{
	return mPreviewSurface;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Setters

void
FOdysseyFlipbookEditor::Texture(UTexture2D* iTexture)
{
    mTextureWrapper.Texture(iTexture);
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Overrides

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

void
FOdysseyFlipbookEditor::OnToolkitInitialized()
{
	mController->OnToolkitInitialized();
}

bool
FOdysseyFlipbookEditor::OnCloseRequested()
{ 
	FOdysseyPainterEditor::OnCloseRequested();

    //TODO: Move in the right place
    if (LayerStack())
        LayerStack()->mDrawingUndo->Clear();

    mTextureWrapper.Texture(nullptr);
    return true;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Methods

void
FOdysseyFlipbookEditor::OnTexturePreSave()
{
    PaintEngine()->Flush();
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

#undef LOCTEXT_NAMESPACE
