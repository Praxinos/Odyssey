// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyTextureEditorGUI.h"

class FOdysseyTexture2DEditor;

/**
 * Implements an Editor toolkit for textures.
 */
class ODYSSEYTEXTURE2DEDITOR_API FOdysseyTexture2DEditorGUI :
	public FOdysseyTextureEditorGUI
{
public:
    // Construction / Destruction
    virtual ~FOdysseyTexture2DEditorGUI();
    FOdysseyTexture2DEditorGUI(FOdysseyTexture2DEditor* iEditor);

public:
    // GettersFName
	virtual FName GetLayoutName() override;

private:
	FOdysseyTexture2DEditor* mEditor;

private:
    //Tabs
};
