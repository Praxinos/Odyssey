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
	mSelectedAlphaMode(::ul3::AM_NORMAL),
	mGUI(nullptr),
	mController(nullptr)
{
}

FOdysseyTextureEditor::FOdysseyTextureEditor(UTexture2D* iTexture, TSharedPtr<FOdysseyPainterEditorToolkit> iToolkit) :
	FOdysseyPainterEditor(iToolkit),
    mTextureWrapper( iTexture ),
	mSelectedAlphaMode(::ul3::AM_NORMAL),
	mGUI(nullptr),
	mController(nullptr)
{
	mGUI = MakeShareable(new FOdysseyTextureEditorGUI(this));
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

	mGUI->Init();
	mGUI->InitOdysseyTextureEditorGUI(this, mController);
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

::ul3::eAlphaMode
FOdysseyTextureEditor::SelectedAlphaMode() const
{
	return mSelectedAlphaMode;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Setters

void
FOdysseyTextureEditor::Texture(UTexture2D* iTexture)
{
    mTextureWrapper.Texture(iTexture);
}

void
FOdysseyTextureEditor::SelectedAlphaMode(::ul3::eAlphaMode iMode)
{
	mSelectedAlphaMode = iMode;

	//Make sure we set the right value in the Paint Engine according to the editor state
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

FOdysseyTextureEditorGUI*
FOdysseyTextureEditor::GetGUI()
{
	return mGUI.Get();
}

TSharedPtr<FWorkspaceItem>
FOdysseyTextureEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager)
{
    TSharedPtr<FWorkspaceItem> workspaceMenuCategory = iTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_OdysseyTextureEditor", "Odyssey Texture Editor"));
	TSharedRef<FWorkspaceItem> workspaceMenuCategoryRef = workspaceMenuCategory.ToSharedRef();
	mGUI->RegisterTabSpawners(iTabManager, workspaceMenuCategoryRef);
	return workspaceMenuCategory;
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