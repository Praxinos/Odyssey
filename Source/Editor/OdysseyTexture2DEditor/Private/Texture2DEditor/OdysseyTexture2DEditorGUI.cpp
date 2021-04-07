// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTexture2DEditorGUI.h"

#include "OdysseyTexture2DEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyTexture2DEditorGUI"

/////////////////////////////////////////////////////
// FOdysseyTexture2DEditorGUI
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTexture2DEditorGUI::~FOdysseyTexture2DEditorGUI()
{
}

FOdysseyTexture2DEditorGUI::FOdysseyTexture2DEditorGUI(FOdysseyTexture2DEditor* iEditor) :
	FOdysseyTextureEditorGUI(iEditor),
	mEditor( iEditor )
{
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

FName
FOdysseyTexture2DEditorGUI::GetLayoutName()
{
	return "OdysseyTexture2DEditor_Layout";
}

#undef LOCTEXT_NAMESPACE
