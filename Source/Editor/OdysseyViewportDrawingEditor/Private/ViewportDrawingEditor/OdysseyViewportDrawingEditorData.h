// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include <ULIS3>

class FOdysseyPaintEngine3D;
class FOdysseyUndoHistory;
class UOdysseyBrush;
class UOdysseyBrushAssetBase;
class FOdysseySurfaceEditable;

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

    void						        Brush(UOdysseyBrush* iBrush);
    void						        BrushInstance(UOdysseyBrushAssetBase* iBrushInstance);

private:
    UTexture2D*                   mTexture;
    FOdysseyLayerStack*           mLayerStack;
    FOdysseySurfaceEditable*      mDisplaySurface;
    FOdysseyPaintEngine3D*        mPaintEngine;
    UOdysseyBrush*                mBrush;
    UOdysseyBrushAssetBase*       mBrushInstance;
};