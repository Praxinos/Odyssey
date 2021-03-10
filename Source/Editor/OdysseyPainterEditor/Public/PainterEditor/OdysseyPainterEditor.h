// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyEditor.h"
#include "OdysseyPainterEditorGUI.h"
#include <ULIS3>

class FOdysseyPaintEngine;
class FOdysseyUndoHistory;
class UOdysseyBrush;
class UOdysseyBrushAssetBase;
class FOdysseySurfaceEditable;

/**
 * Base class for a Painting Editor
 */
class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditor
    : public FOdysseyEditor
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditor();
    FOdysseyPainterEditor();

protected:
    // Protected Initialization
    virtual void InitData();

public:
    // Undo
    virtual void Undo() override;
    virtual void Redo() override;
    virtual void ClearUndo() override;

public:
    // Getters
    virtual FOdysseyPainterEditorGUI*                   GetGUI() = 0;

    virtual FOdysseyPaintEngine*		                PaintEngine();
	virtual FOdysseySurfaceEditable*                    DisplaySurface() = 0;
	virtual FOdysseyUndoHistory*		                UndoHistory();
	virtual bool                                        DrawBrushPreview();
	virtual ::ul3::FPixelValue                          PaintColor() const;

public:
    // Setters
	void                         DrawBrushPreview(bool iDrawBrushPreview);
	void                         PaintColor(::ul3::FPixelValue iColor);

public:
    // Interface
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;

private:
	FOdysseyUndoHistory*		mUndoHistory;
    FOdysseyPaintEngine*        mPaintEngine; //TODO: no need for a pointer here
	::ul3::FPixelValue			mPaintColor;
	bool                        mDrawBrushPreview;
};
