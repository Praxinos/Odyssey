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

#include <ULIS3>

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
	mSelectedAlphaMode(::ul3::AM_NORMAL),
	mGUI(nullptr),
	mController(nullptr)
{
}

FOdysseyFlipbookEditor::FOdysseyFlipbookEditor(UPaperFlipbook* iFlipbook, TSharedPtr<FOdysseyPainterEditorToolkit> iToolkit) :
	FOdysseyPainterEditor(iToolkit),
	mFlipbookWrapper(MakeShareable(new FOdysseyFlipbookWrapper(iFlipbook))),
	mTextureWrapper( nullptr ),
	mPreviewSurface(new FOdysseySurfaceReadOnly(nullptr)),
	mSelectedAlphaMode(::ul3::AM_NORMAL),
	mGUI(nullptr),
	mController(nullptr)
{
	mGUI = MakeShareable(new FOdysseyFlipbookEditorGUI(this));
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

	mGUI->Init();
	mGUI->InitOdysseyFlipbookEditorGUI(this, mController);
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

::ul3::eAlphaMode
FOdysseyFlipbookEditor::SelectedAlphaMode() const
{
	return mSelectedAlphaMode;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Setters

void
FOdysseyFlipbookEditor::Texture(UTexture2D* iTexture)
{
    mTextureWrapper.Texture(iTexture);
}

void
FOdysseyFlipbookEditor::SelectedAlphaMode(::ul3::eAlphaMode iMode)
{
	mSelectedAlphaMode = iMode;

	//Make sure we set the right value in the Paint Engine accoridng to the editor state
    if (!LayerStack())
        return;

    if (!LayerStack()->GetCurrentLayer())
        return;

    if (LayerStack()->GetCurrentLayer()->GetType() != IOdysseyLayer::eType::kImage)
        return;

    TSharedPtr<FOdysseyImageLayer> imageLayer = StaticCastSharedPtr<FOdysseyImageLayer>(LayerStack()->GetCurrentLayer());
    if (imageLayer && imageLayer->IsAlphaLocked())
    {
        PaintEngine()->SetAlphaModeModifier(::ul3::AM_BACK);
    }
    else
    {
		PaintEngine()->SetAlphaModeModifier(mSelectedAlphaMode);
    }
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Overrides

FOdysseyFlipbookEditorGUI*
FOdysseyFlipbookEditor::GetGUI()
{
	return mGUI.Get();
}

TSharedRef<FTabManager::FLayout>
FOdysseyFlipbookEditor::GetLayout()
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
 * 3) Make GUI -> Tabs classes, containing the creation of GUI and a pointer to a controller specific for this GUI (This one is a BIG one)
 * 3.1) while making 3), the old almighty controller can coexist with the new Tabs classes, so we can make each Tab class + controller one after the other
 * 4) Make FlipbookEditor Inherite TextureEditor and cleanup
 * 5) Test and Debug
 * 6) Hooray !
 */

#undef LOCTEXT_NAMESPACE
