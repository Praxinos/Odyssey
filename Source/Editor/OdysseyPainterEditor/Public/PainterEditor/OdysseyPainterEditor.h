// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditor.h"
#include "OdysseyPainterEditorGUI.h"
#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingTool.h"
#include "Tools/PaintBucketTool/OdysseyPainterEditorPaintBucketTool.h"
#include <ULIS>

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

    virtual FOdysseyHUDSystem*                          HUDSystem() const;
	virtual IOdysseySurfaceEditable*                    DisplaySurface() const = 0;
    virtual FOdysseyBrushColor&                         PaintColor();
    virtual UOdysseyTool*                               GetSelectedTool() const;

    virtual UOdysseyPainterEditorRasterDrawingTool*                  GetRasterDrawingTool() const = 0;
    virtual UOdysseyPainterEditorPaintBucketTool*                    GetPaintBucketTool() const = 0;

    void ActivateDefaultTool();

public:
    // Setters
    void  PaintColor(const FOdysseyBrushColor& iColor, bool iIsCommit);
    void  SetSelectedTool( UOdysseyTool* iSelectedTool );

public:
    // Interface
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;
    virtual void ExtendMenu( FToolMenuOwner iOwner, FName iMenuName ) override;

protected:
    //Callbacks
    virtual void OnApplyOverrides(const TMap<FName, UObject*>& iOverrides);
    
    // FTickableEditorObject implementation
	virtual void Tick(float DeltaTime) override;

protected:
    //Tools
    UOdysseyTool*               mSelectedTool;
    TArray<UOdysseyTool*>       mTools;

    FOdysseyHUDSystem*              mHUDSystem;
    TArray<FOdysseyBrushContext*>   mBrushContexts;
    FOdysseyBrushColor              mPaintColor;
};
