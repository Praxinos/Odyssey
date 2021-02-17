// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyPainterEditor.h"
#include "Engine/Texture2D.h"
#include "OdysseyTextureEditorGUI.h"
#include "OdysseyTextureWrapper.h"

class FOdysseyTextureEditorController;

/**
 * Implements an Editor for textures.
 */
class ODYSSEYTEXTUREEDITOR_API FOdysseyTextureEditor
    : public FOdysseyPainterEditor
{
public:
    // Construction / Destruction
    virtual ~FOdysseyTextureEditor();
    FOdysseyTextureEditor(TSharedPtr<FOdysseyPainterEditorToolkit> iToolkit); //Non Initialized constructor
    FOdysseyTextureEditor(UTexture2D* iTexture, TSharedPtr<FOdysseyPainterEditorToolkit> iToolkit);

public:
    // Initialization
    virtual void Init() override;

public:
    // Getters
    virtual FOdysseyTextureWrapper&             TextureWrapper();
	virtual UTexture2D*							Texture();
	virtual FOdysseySurfaceEditable*            DisplaySurface() override;
    virtual FOdysseyLayerStack*					LayerStack() const;
    ::ul3::eAlphaMode	                        SelectedAlphaMode() const;

public:
    // Setters
    void Texture(UTexture2D* iTexture);
    void SelectedAlphaMode(::ul3::eAlphaMode iMode);

public:
    // Overrides
    virtual FOdysseyTextureEditorGUI* GetGUI() override;
    virtual TSharedRef<FTabManager::FLayout> GetLayout() override;
    virtual const TArray<TSharedPtr<FExtender>>& CreateMenuExtenders() const override;
    
    virtual TSharedPtr<FWorkspaceItem> RegisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) override;
    virtual void UnregisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) override;

    virtual bool OnCloseRequested();

protected:
    // Methods
    virtual void OnTexturePreSave();
    
private:
    FOdysseyTextureWrapper     mTextureWrapper;
    ::ul3::eAlphaMode                       mSelectedAlphaMode;

	TSharedPtr<FOdysseyTextureEditorGUI> mGUI;
	TSharedPtr<FOdysseyTextureEditorController> mController;
};

