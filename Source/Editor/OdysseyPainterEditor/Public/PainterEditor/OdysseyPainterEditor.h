// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

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
    : public FGCObject //Allows us to register External UObject in Garbage Collector
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditor();
    FOdysseyPainterEditor(TSharedPtr<FOdysseyPainterEditorToolkit> iToolkit);

public:
    // Initialization
    virtual void Init();

public:
    // Getters
    virtual FOdysseyPaintEngine*		                PaintEngine();
	virtual FOdysseySurfaceEditable*                    DisplaySurface() = 0;
	virtual FOdysseyUndoHistory*		                UndoHistory();
	virtual UOdysseyBrush*                              Brush();
	virtual UOdysseyBrushAssetBase*                     BrushInstance();
	virtual bool                                        DrawBrushPreview();
	virtual ::ul3::FPixelValue                          PaintColor() const;
    virtual TSharedPtr<FOdysseyPainterEditorToolkit>    Toolkit();

public:
    // Setters
	void						 Brush(UOdysseyBrush* iBrush);
	void						 BrushInstance(UOdysseyBrushAssetBase* iBrushInstance);
	void                         DrawBrushPreview(bool iDrawBrushPreview);
	void                         PaintColor(::ul3::FPixelValue iColor);

public:
    // Methods
    virtual void OnToolkitInitialized();
    virtual bool OnCloseRequested();

public:
    // Interface
    virtual const TSharedRef<FTabManager::FLayout>& CreateLayout() const = 0;
    virtual const TArray<TSharedPtr<FExtender>>& CreateMenuExtenders() const = 0;
    virtual TSharedPtr<FWorkspaceItem> RegisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) = 0;
    virtual void UnregisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) = 0;

protected:
    // FGCObject interface
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
    virtual FString GetReferencerName() const override;

private:
    TWeakPtr<FOdysseyPainterEditorToolkit> mToolkit;

	FOdysseyUndoHistory*		mUndoHistory;
    FOdysseyPaintEngine*        mPaintEngine; //TODO: no need for a pointer here
	UOdysseyBrush*              mBrush;             // NOT Owned
	UOdysseyBrushAssetBase*     mBrushInstance;     // Owned        // Used by PaintEngine and Brush Parameters and Brush Preview
	::ul3::FPixelValue			mPaintColor;
	bool                        mDrawBrushPreview;
};
