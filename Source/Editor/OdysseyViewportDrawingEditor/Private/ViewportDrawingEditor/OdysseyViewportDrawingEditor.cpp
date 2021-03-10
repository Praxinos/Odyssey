// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyViewportDrawingEditor.h"

#include "OdysseyViewportDrawingEditorGUI.h"


#define LOCTEXT_NAMESPACE "OdysseyViewportDrawingEditor"

/////////////////////////////////////////////////////
// FOdysseyViewportDrawingEditor
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyViewportDrawingEditor::~FOdysseyViewportDrawingEditor()
{
}

FOdysseyViewportDrawingEditor::FOdysseyViewportDrawingEditor() :
	FOdysseyTextureEditor(),
	mGUI(nullptr)
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyViewportDrawingEditor::InitData()
{
	FOdysseyTextureEditor::InitData();

	//--- Init Data
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

TArray<FPaintableTexture>&
FOdysseyViewportDrawingEditor::PaintableTextures()
{
    return mPaintableTextures;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Setters

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Overrides

FOdysseyViewportDrawingEditorGUI*
FOdysseyViewportDrawingEditor::GetGUI()
{
	if (!mGUI)
		mGUI = MakeShareable(new FOdysseyViewportDrawingEditorGUI(this));
	return mGUI.Get();
}

void
FOdysseyViewportDrawingEditor::OnPreTextureChange(UTexture2D* iNewTexture)
{
	UTexture2D* texture = Texture();
    if (texture)
	{
		RemoveEditedObject(texture);
	}

	FOdysseyTextureEditor::OnPreTextureChange(iNewTexture);
}

void
FOdysseyViewportDrawingEditor::OnPostTextureChange(UTexture2D* iOldTexture)
{
	UTexture2D* texture = Texture();
    if (texture)
	{
		AddEditedObject(texture);
	}

    FOdysseyTextureEditor::OnPostTextureChange(iOldTexture);
}

    

#undef LOCTEXT_NAMESPACE