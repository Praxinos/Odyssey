// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyEditor.h"
#include "OdysseyPainterEditorGUI.h"
#include <ULIS>

class FOdysseyPaintEngine;
class FOdysseyToolSystem;
class FOdysseyHUDSystem;
class FOdysseyUndoHistory;
class UOdysseyBrush;
class UOdysseyBrushAssetBase;
class IOdysseySurfaceEditable;

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

    virtual FOdysseyPaintEngine*                        PaintEngine() const;
    virtual FOdysseyToolSystem*                         ToolSystem() const;
    virtual FOdysseyHUDSystem*                          HUDSystem() const;
    virtual IOdysseySurfaceEditable*                    DisplaySurface() const = 0;
    virtual FOdysseyUndoHistory*                        UndoHistory() const;
    virtual bool                                        DrawBrushPreview() const;
    virtual ::ULIS::FColor                              PaintColor() const;
    virtual eGUISelectedTool                            GetGUISelectedTool() const;

public:
    // Setters
    void                         DrawBrushPreview(bool iDrawBrushPreview);
    void                         PaintColor(::ULIS::FColor iColor);
    void                         SetGUISelectedTool( eGUISelectedTool iGUISelectedTool );

public:
    // Interface
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;

private:
    FOdysseyUndoHistory*        mUndoHistory;
    FOdysseyPaintEngine*        mPaintEngine; //TODO: no need for a pointer here
    FOdysseyToolSystem*         mToolSystem;
    FOdysseyHUDSystem*          mHUDSystem;
    ::ULIS::FColor              mPaintColor;

    bool                        mDrawBrushPreview;
    eGUISelectedTool            mGUISelectedTool;
};
