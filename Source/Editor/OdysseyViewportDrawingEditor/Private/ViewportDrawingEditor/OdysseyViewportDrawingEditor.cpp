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
	FOdysseyPainterEditor::InitData();

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

#undef LOCTEXT_NAMESPACE