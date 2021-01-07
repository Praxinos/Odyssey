// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include <ULIS3>

class FOdysseyPaintEngine3D;
class FOdysseyUndoHistory;
class UOdysseyBrush;
class UOdysseyBrushAssetBase;
class FOdysseySurfaceEditable;

struct FTexturePropertiesBackup
{
    uint8 mTextureCompressionNone;
};

/**
 * Implements an Editor toolkit for textures.
 */
class ODYSSEYVIEWPORTDRAWINGEDITOR_API FOdysseyViewportDrawingEditorData :
	public TSharedFromThis<FOdysseyViewportDrawingEditorData>
{
public:
    // Construction / Destruction
    virtual ~FOdysseyViewportDrawingEditorData();
    FOdysseyViewportDrawingEditorData();

public:
    void Init(UTexture2D* iTexture);

    UTexture2D*                         Texture();
    FOdysseyLayerStack*					LayerStack();
    FOdysseySurfaceEditable*            DisplaySurface();
    FOdysseyPaintEngine3D*		        PaintEngine();
    UOdysseyBrush*                      Brush();
    UOdysseyBrushAssetBase*             BrushInstance();
    ::ul3::FPixelValue                  PaintColor() const;

    void						        Brush(UOdysseyBrush* iBrush);
    void						        BrushInstance(UOdysseyBrushAssetBase* iBrushInstance);
    void                                PaintColor(::ul3::FPixelValue iColor);

public:
    void SyncTextureAndInvalidate();
    void PrepareTextureProperties();
    void ApplyPropertiesBackup();

    void OnPreGlobalObjectPropertyChanged(UObject* iObject,const FEditPropertyChain& iEditPropertyChain);
    void OnPackagePreSave(UPackage* iPackage);
    void OnPackageSaved(const FString& iPackageFilename,UObject* iOuter);

private:
    UTexture2D*                   mTexture;
    FOdysseyLayerStack*           mLayerStack;
    FOdysseySurfaceEditable*      mDisplaySurface;
    FOdysseyPaintEngine3D*        mPaintEngine;
    UOdysseyBrush*                mBrush;
    UOdysseyBrushAssetBase*       mBrushInstance;
    ::ul3::FPixelValue			  mPaintColor;
    FTexturePropertiesBackup      mPropertiesBackup;

    FDelegateHandle mOnPrePropertyChangedDelegateHandle;
    FDelegateHandle mOnPackagePreSaveHandle;
    FDelegateHandle mOnPackageSavedHandle;
};