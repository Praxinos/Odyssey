// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditor.h"
#include "OdysseyPainterEditorGUI.h"
#include "OdysseyPaintEngine.h"
#include <ULIS>

class FOdysseyPaintEngine;
class FOdysseyHUDSystem;
class IOdysseySurfaceEditable;
class UOdysseyTool;
class FOdysseyBrushContext;

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
    virtual void InitTools();

public:
    // Getters
    virtual FOdysseyPainterEditorGUI*                   GetGUI() = 0;

    virtual FOdysseyPaintEngine&                        PaintEngine();
    virtual FOdysseyHUDSystem*                          HUDSystem() const;
	virtual IOdysseySurfaceEditable*                    DisplaySurface() const = 0;
    virtual FOdysseyBrushColor&                         PaintColor();
    virtual UOdysseyTool*                               GetSelectedTool() const;

    virtual class UOdysseyRasterDrawingTool*                  GetRasterDrawingTool() const;
    virtual class UOdysseyPaintBucketTool*                    GetPaintBucketTool() const;

public:
    // Setters
    void                         PaintColor(const FOdysseyBrushColor& iColor, bool iIsCommit);
    void                         SetSelectedTool( UOdysseyTool* iSelectedTool );

public:
    // Interface
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;
    virtual void ExtendMenu( FToolMenuOwner iOwner, FName iMenuName ) override;

protected:
    //Callbacks
    virtual void OnApplyOverrides(const TMap<FName, UObject*>& iOverrides);

protected:
    // FGCObject implementation
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
    
    // FTickableEditorObject implementation
	virtual void Tick(float DeltaTime) override;

protected:
    FOdysseyPaintEngine         mPaintEngine; //We declare a single PaintEngine which will be used for any brush we use
    
    //Tools
    UOdysseyTool*               mSelectedTool;

    FOdysseyHUDSystem*              mHUDSystem;
    TArray<FOdysseyBrushContext*>   mBrushContexts;
    FOdysseyBrushColor              mPaintColor;

    UOdysseyRasterDrawingTool*      mRasterDrawingTool;
    UOdysseyPaintBucketTool*        mPaintBucketTool;
};
