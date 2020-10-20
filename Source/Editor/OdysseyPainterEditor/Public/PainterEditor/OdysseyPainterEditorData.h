// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include <ULIS3>

class FOdysseyPaintEngine;
class FOdysseyUndoHistory;
class UOdysseyBrush;
class UOdysseyBrushAssetBase;
class FOdysseySurfaceEditable;

/**
 * Implements an Editor toolkit for textures.
 */
class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorData :
	public TSharedFromThis<FOdysseyPainterEditorData>
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorData();
    FOdysseyPainterEditorData();

public:
	virtual void Init();

    FOdysseyPaintEngine*		        PaintEngine();
	virtual FOdysseySurfaceEditable*    DisplaySurface() = 0;
	FOdysseyUndoHistory*		        UndoHistory();
	UOdysseyBrush*                      Brush();
	UOdysseyBrushAssetBase*             BrushInstance();
	bool                                DrawBrushPreview();
	::ul3::FPixelValue                  PaintColor() const;

	void						 Brush(UOdysseyBrush* iBrush);
	void						 BrushInstance(UOdysseyBrushAssetBase* iBrushInstance);
	void                         DrawBrushPreview(bool iDrawBrushPreview);
	void                         PaintColor(::ul3::FPixelValue iColor);

private:
	FOdysseyUndoHistory*		mUndoHistory;
    FOdysseyPaintEngine*        mPaintEngine;       // Owned        
	UOdysseyBrush*              mBrush;             // NOT Owned
	UOdysseyBrushAssetBase*     mBrushInstance;     // Owned        // Used by PaintEngine and Brush Parameters and Brush Preview
	::ul3::FPixelValue			mPaintColor;
	bool                        mDrawBrushPreview;
};

