// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyTextureEditor.h"
#include "Engine/Texture2D.h"
#include "OdysseyTexture2DEditorGUI.h"
#include "OdysseyTexture2DWrapper.h"

/**
 * Implements an Editor for textures.
 */
class ODYSSEYTEXTURE2DEDITOR_API FOdysseyTexture2DEditor
    : public FOdysseyTextureEditor
{
public:
    // Construction / Destruction
    virtual ~FOdysseyTexture2DEditor();
    FOdysseyTexture2DEditor(); //Non Initialized constructor
    FOdysseyTexture2DEditor(UTexture2D* iTexture);

public:
    // Getters
    virtual FOdysseyTexture2DWrapper* TextureWrapper() const override;
	virtual UTexture2D*				  Texture() const override;
    
public:
    // Overrides
    virtual FOdysseyTexture2DEditorGUI* GetGUI() override;
    virtual TSharedPtr<FWorkspaceItem> RegisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) override;
    
private:
    FOdysseyTexture2DWrapper*     mTextureWrapper;
    
	TSharedPtr<FOdysseyTexture2DEditorGUI> mGUI;
};
