// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyLayerStack.h"
#include "OdysseyPaintEngine.h"
#include "OdysseyPainterEditorData.h"
#include "OdysseySurfaceReadOnly.h"
#include "OdysseyTextureWrapper.h"

class UPaperFlipbook;
class UTexture2D;
class UOdysseyBrush;
class UOdysseyTextureAssetUserData;
class FOdysseyPainterEditorToolkit;
class FOdysseyFlipbookWrapper;

struct FTexturePropertiesBackup
{
    uint8 mTextureCompressionNone;
};

/**
 * Implements an Editor toolkit for Flipbooks.
 */
class ODYSSEYFLIPBOOKEDITOR_API FOdysseyFlipbookEditorData :
	public FOdysseyPainterEditorData
{
public:
    // Construction / Destruction
    virtual ~FOdysseyFlipbookEditorData();
    FOdysseyFlipbookEditorData(TSharedPtr<FOdysseyFlipbookWrapper>& iFlipbookWrapper);

public:
    //Overrides
	virtual void Init() override;

public:
    //Getters

    //Currently Edited Flipbook
	TSharedPtr<FOdysseyFlipbookWrapper>& FlipbookWrapper();
    
    FOdysseyTextureWrapper& TextureWrapper();

    //Currently Edited Texture
    UTexture2D*							Texture();

    //Currently Edited LayerStack
    FOdysseyLayerStack*					LayerStack() const;

	virtual FOdysseySurfaceEditable*    DisplaySurface() override;
	FOdysseySurfaceReadOnly*			PreviewSurface();

public:
    // Setters

    // Sets the currently edited texture
    void Texture(UTexture2D* iTexture);
    
public:
    //TEMPORARY
    void OnCloseRequested();

private:
    void OnTexturePreSave();

private:
    TSharedPtr<FOdysseyFlipbookWrapper>     mFlipbookWrapper;
    FOdysseyTextureWrapper                  mTextureWrapper;

	//Surface for previewing in viewport
	FOdysseySurfaceReadOnly*                mPreviewSurface;
};

