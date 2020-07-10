// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include <ULIS3>

class FOdysseyPaintEngine;
class FOdysseyUndoHistory;
class FOdysseyBlock;
class UOdysseyBrush;
class UOdysseyBrushAssetBase;
class FOdysseySurface;

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

    FOdysseyPaintEngine*		 PaintEngine();
	virtual FOdysseySurface*     DisplaySurface() = 0;
	FOdysseyUndoHistory*		 UndoHistory();
	UOdysseyBrush*               Brush();
	UOdysseyBrushAssetBase*      BrushInstance();
	bool                         DrawBrushPreview();

	void						 Brush(UOdysseyBrush* iBrush);
	void						 BrushInstance(UOdysseyBrushAssetBase* iBrushInstance);
	void                         DrawBrushPreview(bool iDrawBrushPreview);

private:
	FOdysseyUndoHistory*		mUndoHistory;
    FOdysseyPaintEngine*        mPaintEngine;        // Owned        // Used by SOdysseyLayerStack
	FOdysseyBlock*				mPaintEngineBlock;        // Owned   // Used by PaintEngine
	UOdysseyBrush*              mBrush;              // NOT Owned
	UOdysseyBrushAssetBase*     mBrushInstance;     // Owned        // Used by PaintEngine and Brush Exposed Parameters and Brush Preview
	bool                        mDrawBrushPreview;
};

