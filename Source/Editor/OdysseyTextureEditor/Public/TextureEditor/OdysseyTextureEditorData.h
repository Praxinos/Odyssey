// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyPainterEditorData.h"
#include "OdysseyTextureWrapper.h"

/**
 * Implements an Editor toolkit for textures.
 */
class ODYSSEYTEXTUREEDITOR_API FOdysseyTextureEditorData :
	public FOdysseyPainterEditorData
{
public:
    // Construction / Destruction
    virtual ~FOdysseyTextureEditorData();
    FOdysseyTextureEditorData(UTexture2D* iTexture);

public:
	virtual void Init() override;

    FOdysseyTextureWrapper&             TextureWrapper();
	UTexture2D*							Texture();
    FOdysseyLayerStack*					LayerStack() const;
	virtual FOdysseySurfaceEditable*    DisplaySurface() override;

public:
    //TEMPORARY
    void OnCloseRequested();

private:
    void OnTexturePreSave();

private:
    FOdysseyTextureWrapper     mTextureWrapper;
};

