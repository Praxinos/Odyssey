// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyPainterEditor.h"
#include "Engine/Texture.h"
#include "OdysseyTextureEditorGUI.h"
#include "OdysseyTextureWrapper.h"

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

public:
    // Initialization
    virtual void InitData() override;

public:
    // Undo
    virtual void Undo() override;
    virtual void Redo() override;
    virtual void ClearUndo() override;

public:
    // Getters
	virtual UTexture*							Texture() const = 0;
    virtual FOdysseyTextureWrapper*             TextureWrapper() const = 0;
    
	virtual IOdysseySurfaceEditable*            DisplaySurface() const override;
    virtual FOdysseyLayerStack*					LayerStack() const;
    ::ul3::eAlphaMode	                        SelectedAlphaMode() const;

public:
    // Setters
    void SelectedAlphaMode(::ul3::eAlphaMode iMode);

public:
    // Overrides
    virtual bool OnCloseRequested() override;
    
protected:
    // Attributes
    virtual bool PaintEngineIsLocked() const;

protected:
    // Listeners

    //Texture
    virtual void OnPreTextureChange();
    virtual void OnPostTextureChange();

    //LayerStack
	virtual void OnLayerStackCurrentLayerChanged(TSharedPtr<IOdysseyLayer> iOldValue);
	virtual void OnLayerStackStructureChanged();
	virtual void OnLayerStackImageResultChanged(const ::ul3::FRect& iRect);
	virtual void OnCurrentLayerIsAlphaLockedChanged(bool iOldValue);

    //Paint Engine
    virtual void OnPaintEnginePaintEnd(const TArray<::ul3::FRect>& iChangedTiles);

private:
    ::ul3::eAlphaMode                       mSelectedAlphaMode;
};