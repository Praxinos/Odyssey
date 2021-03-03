// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyPainterEditorGUI.h"
#include <ULIS3>

class FOdysseyPaintEngine;
class FOdysseyUndoHistory;
class UOdysseyBrush;
class UOdysseyBrushAssetBase;
class FOdysseySurfaceEditable;
class FOdysseyPainterEditorToolkit;

/**
 * Base class for a Painting Editor
 */
class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditor
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditor();
    FOdysseyPainterEditor(TSharedPtr<FOdysseyPainterEditorToolkit> iToolkit);

public:
    // Initialization
    virtual void Init();

protected:
    // Protected Initialization

    // Seperating Init from InitGUI allows us to seperate Data and GUI initialization also in derived classes
    virtual void InitData();
    virtual void InitGUI();

public:
    // Getters
    virtual FOdysseyPaintEngine*		                PaintEngine();
	virtual FOdysseySurfaceEditable*                    DisplaySurface() = 0;
	virtual FOdysseyUndoHistory*		                UndoHistory();
	virtual bool                                        DrawBrushPreview();
	virtual ::ul3::FPixelValue                          PaintColor() const;
    virtual TSharedPtr<FOdysseyPainterEditorToolkit>    Toolkit();

public:
    // Setters
	void                         DrawBrushPreview(bool iDrawBrushPreview);
	void                         PaintColor(::ul3::FPixelValue iColor);

public:
    // Methods
    virtual void OnToolkitInitialized();
    virtual bool OnCloseRequested();
    virtual void FillExtender(TSharedPtr<FExtender>& ioExtender);
    virtual TSharedRef<FTabManager::FLayout> GetLayout();
    virtual void UnregisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager );
public:
    // Interface
    virtual FOdysseyPainterEditorGUI* GetGUI() = 0;
    virtual TSharedPtr<FWorkspaceItem> RegisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) = 0;

private:
    TWeakPtr<FOdysseyPainterEditorToolkit> mToolkit;

	FOdysseyUndoHistory*		mUndoHistory;
    FOdysseyPaintEngine*        mPaintEngine; //TODO: no need for a pointer here
	::ul3::FPixelValue			mPaintColor;
	bool                        mDrawBrushPreview;
};
