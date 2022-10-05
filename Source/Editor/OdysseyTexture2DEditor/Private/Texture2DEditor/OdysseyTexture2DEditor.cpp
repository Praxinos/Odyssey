// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTexture2DEditor.h"

#include "OdysseyTexture2DEditorGUI.h"


#define LOCTEXT_NAMESPACE "OdysseyTexture2DEditor"

/////////////////////////////////////////////////////
// FOdysseyTexture2DEditor
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTexture2DEditor::~FOdysseyTexture2DEditor()
{
	delete mTextureWrapper;
}

FOdysseyTexture2DEditor::FOdysseyTexture2DEditor() :
	FOdysseyTextureEditor(),
	mTextureWrapper(new FOdysseyTexture2DWrapper(nullptr)),
	mGUI(nullptr)
{
}

FOdysseyTexture2DEditor::FOdysseyTexture2DEditor(UTexture2D* iTexture) :
	FOdysseyTextureEditor(),
    mTextureWrapper( new FOdysseyTexture2DWrapper(iTexture) ),
	mGUI(nullptr)
{
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

FOdysseyTexture2DWrapper*
FOdysseyTexture2DEditor::TextureWrapper() const
{
	return mTextureWrapper;
}

UTexture2D*
FOdysseyTexture2DEditor::Texture() const
{
	return mTextureWrapper->Texture();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Overrides

FOdysseyTexture2DEditorGUI*
FOdysseyTexture2DEditor::GetGUI()
{
	if (!mGUI)
		mGUI = MakeShareable(new FOdysseyTexture2DEditorGUI(this));
	return mGUI.Get();
}

TSharedPtr<FWorkspaceItem>
FOdysseyTexture2DEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager)
{
    TSharedPtr<FWorkspaceItem> workspaceMenuCategory = iTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_OdysseyTexture2DEditor", "Odyssey Texture2D Editor"));
	TSharedRef<FWorkspaceItem> workspaceMenuCategoryRef = workspaceMenuCategory.ToSharedRef();
	GetGUI()->RegisterTabSpawners(iTabManager, workspaceMenuCategoryRef);
	return workspaceMenuCategory;
}

#undef LOCTEXT_NAMESPACE