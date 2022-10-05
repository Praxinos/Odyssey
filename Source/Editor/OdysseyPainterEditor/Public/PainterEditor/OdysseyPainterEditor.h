// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditor.h"
#include "OdysseyPainterEditorGUI.h"
#include "OdysseyPaintEngine.h"
#include <ULIS>

class FOdysseyPaintEngine;
class FOdysseyHUDSystem;
class FOdysseyUndoHistory;
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

public:
    // Undo
    virtual void Undo() override;
    virtual void Redo() override;
    virtual void ClearUndo() override;

public:
    // Getters
    virtual FOdysseyPainterEditorGUI*                   GetGUI() = 0;

    virtual FOdysseyPaintEngine&                        PaintEngine();
    virtual FOdysseyHUDSystem*                          HUDSystem() const;
    //virtual UOdysseyStrokeEngine*		                StrokeEngine();
	virtual IOdysseySurfaceEditable*                    DisplaySurface() const = 0;
	virtual FOdysseyUndoHistory*		                UndoHistory() const;
    virtual FOdysseyBrushColor&                         PaintColor();
    virtual UOdysseyTool*                               GetSelectedTool() const;

public:
    // Setters
    void                         PaintColor(const FOdysseyBrushColor& iColor);
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
	FOdysseyUndoHistory*		mUndoHistory;
    FOdysseyPaintEngine         mPaintEngine; //We declare a single PaintEngine which will be used for any brush we use
    UOdysseyTool*               mSelectedTool;
    //TMap<EOdysseyTool, UOdysseyTool*> mTools;
    FOdysseyHUDSystem*          mHUDSystem;
    //UOdysseyStrokeEngine*       mStrokeEngine; //TODO: Should actually be a tool => FreeHandTool
    TArray<FOdysseyBrushContext*> mBrushContexts;
    FOdysseyBrushColor mPaintColor;
};
