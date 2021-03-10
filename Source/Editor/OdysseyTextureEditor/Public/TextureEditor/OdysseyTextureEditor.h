// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyPainterEditor.h"
#include "Engine/Texture2D.h"
#include "OdysseyTextureEditorGUI.h"
#include "OdysseyTextureWrapper.h"

class FOdysseyTextureEditorController;
class IOdysseyLayer;

/**
 * Implements an Editor for textures.
 */
class ODYSSEYTEXTUREEDITOR_API FOdysseyTextureEditor
    : public FOdysseyPainterEditor
{
public:
    // Construction / Destruction
    virtual ~FOdysseyTextureEditor();
    FOdysseyTextureEditor(); //Non Initialized constructor
    FOdysseyTextureEditor(UTexture2D* iTexture);

public:
    // Initialization
    virtual void InitData() override;

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
    virtual TSharedPtr<FWorkspaceItem> RegisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) override;
    virtual bool OnCloseRequested() override;
    
protected:
    // Attributes
    virtual bool PaintEngineIsLocked() const;

protected:
    // Listeners
    virtual void OnPreTextureChange(UTexture2D* iNewTexture);
    virtual void OnPostTextureChange(UTexture2D* iOldTexture);

	virtual void OnLayerStackCurrentLayerChanged(TSharedPtr<IOdysseyLayer> iOldValue);
	virtual void OnLayerStackStructureChanged();
	virtual void OnLayerStackImageResultChanged(const ::ul3::FRect& iRect);
	virtual void OnCurrentLayerIsAlphaLockedChanged(bool iOldValue);

    virtual void OnPaintEnginePaintEnd(const TArray<::ul3::FRect>& iChangedTiles);

private:
    FOdysseyTextureWrapper     mTextureWrapper;
    ::ul3::eAlphaMode                       mSelectedAlphaMode;

	TSharedPtr<FOdysseyTextureEditorGUI> mGUI;
};

