// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyPainterEditor.h"
#include "PaperFlipbook.h"

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

public:
    // Setters
    void Texture(UTexture2D* iTexture);

public:
    //Overrides
    virtual TSharedRef<FTabManager::FLayout> GetLayout() override;
    virtual const TArray<TSharedPtr<FExtender>>& CreateMenuExtenders() const override;
    
    virtual TSharedPtr<FWorkspaceItem> RegisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) override;
    virtual void UnregisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) override;

    virtual void OnToolkitInitialized() override;
    virtual bool OnCloseRequested();

protected:
    virtual void OnTexturePreSave();

private:
    TSharedPtr<FOdysseyFlipbookWrapper> mFlipbookWrapper; //TODO: No need for a SharedPtr here, and no need for a pointer here
    FOdysseyTextureWrapper                  mTextureWrapper;
	FOdysseySurfaceReadOnly*                mPreviewSurface; //TODO: No need for a pointer here I guess

	TSharedPtr<FOdysseyFlipbookEditorGUI> mGUI;
	TSharedPtr<FOdysseyFlipbookEditorController> mController;
};

