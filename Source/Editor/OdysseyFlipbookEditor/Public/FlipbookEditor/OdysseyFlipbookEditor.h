// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyPainterEditor.h"
#include "PaperFlipbook.h"

#include "OdysseyFlipbookEditorGUI.h"
#include "OdysseyFlipbookWrapper.h"
#include "OdysseyTextureWrapper.h"

class UPaperFlipbook;
class UTexture2D;
class UOdysseyBrush;
class UOdysseyTextureAssetUserData;
class FOdysseyPainterEditorToolkit;
class FOdysseyFlipbookEditorGUI;
class FOdysseySurfaceReadOnly;
class FOdysseyFlipbookEditorController;

/**
 * Implements an Editor for flipbooks.
 */
class ODYSSEYFLIPBOOKEDITOR_API FOdysseyFlipbookEditor
    : public FOdysseyPainterEditor
{
public:
    // Construction / Destruction
    virtual ~FOdysseyFlipbookEditor();
    FOdysseyFlipbookEditor(TSharedPtr<FOdysseyPainterEditorToolkit> iToolkit); //Non-Initialized Contructor
    FOdysseyFlipbookEditor(UPaperFlipbook* iFlipbook, TSharedPtr<FOdysseyPainterEditorToolkit> iToolkit);

public:
    //Initialization
    virtual void Init() override;

public:
    // Getters
	TSharedPtr<FOdysseyFlipbookWrapper>&    FlipbookWrapper();
    FOdysseyTextureWrapper&                 TextureWrapper();
    UTexture2D*							    Texture();
    FOdysseyLayerStack*					    LayerStack() const;
	virtual FOdysseySurfaceEditable*        DisplaySurface() override;
	FOdysseySurfaceReadOnly*			    PreviewSurface();
    ::ul3::eAlphaMode	                    SelectedAlphaMode() const;

public:
    // Setters
    void Texture(UTexture2D* iTexture);
    void SelectedAlphaMode(::ul3::eAlphaMode iMode);

public:
    //Overrides
    virtual FOdysseyFlipbookEditorGUI* GetGUI() override;
    virtual TSharedPtr<FWorkspaceItem> RegisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) override;
    virtual bool OnCloseRequested();

private:
    TSharedPtr<FOdysseyFlipbookWrapper> mFlipbookWrapper; //TODO: No need for a SharedPtr here, and no need for a pointer here
    FOdysseyTextureWrapper                  mTextureWrapper;
	FOdysseySurfaceReadOnly*                mPreviewSurface; //TODO: No need for a pointer here I guess
    ::ul3::eAlphaMode                       mSelectedAlphaMode;

	TSharedPtr<FOdysseyFlipbookEditorGUI> mGUI;
	TSharedPtr<FOdysseyFlipbookEditorController> mController;
};

