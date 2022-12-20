// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditor.h"
#include "OdysseyPainterEditorGUI.h"
#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingTool.h"
#include "Tools/VectorDrawingTool/OdysseyPainterEditorVectorDrawingTool.h"
#include "Tools/VectorEditPathTool/OdysseyPainterEditorVectorEditPathTool.h"
#include "Tools/VectorMoveObjectTool/OdysseyPainterEditorVectorMoveObjectTool.h"
#include "Tools/VectorRotateObjectTool/OdysseyPainterEditorVectorRotateObjectTool.h"
#include "Tools/VectorScaleObjectTool/OdysseyPainterEditorVectorScaleObjectTool.h"
#include "Tools/PaintBucketTool/OdysseyPainterEditorPaintBucketTool.h"
#include <ULIS>

class FOdysseyHUDSystem;
class IOdysseySurfaceEditable;
class UOdysseyPainterEditorTool;
class FOdysseyBrushContext;

/**
 * Base class for a Painting Editor
 */
class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditor
    : public FOdysseyEditor
{
public:
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnSelectedToolChange, UOdysseyPainterEditorTool*);

public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditor();
    FOdysseyPainterEditor();

protected:
    // Protected Initialization
    virtual void InitData(UObject* iEditedObject);
    virtual void InitTools();

public:
    // Getters
    FOnSelectedToolChange&                              OnSelectedToolChangedDelegate();
    virtual FOdysseyPainterEditorGUI*                   GetGUI() = 0;

    virtual FOdysseyHUDSystem*                              HUDSystem() const;
	virtual UTexture*                                       DisplayTexture() const = 0;
    virtual const FOdysseyBrushColor&                       PaintColor() const;
    virtual UOdysseyPainterEditorTool*                      GetSelectedTool() const;
    virtual TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> GetDisplayBlock() = 0;

    virtual UOdysseyPainterEditorRasterDrawingTool*                  GetRasterDrawingTool() const = 0;
    virtual UOdysseyPainterEditorVectorDrawingTool*                  GetVectorDrawingTool() const = 0;
    virtual UOdysseyPainterEditorVectorEditPathTool*                 GetVectorEditPathTool() const = 0;
    virtual UOdysseyPainterEditorVectorMoveObjectTool*               GetVectorMoveObjectTool() const = 0;
    virtual UOdysseyPainterEditorVectorRotateObjectTool*             GetVectorRotateObjectTool() const = 0;
    virtual UOdysseyPainterEditorVectorScaleObjectTool*              GetVectorScaleObjectTool() const = 0;
    virtual UOdysseyPainterEditorPaintBucketTool*                    GetPaintBucketTool() const = 0;

    void SelectDefaultTool();

public:
    // Setters
    void  PaintColor(const FOdysseyBrushColor& iColor, bool iIsCommit);
    void  SetSelectedTool( UOdysseyPainterEditorTool* iSelectedTool );

public:
    // Interface
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;
    virtual void ExtendMenu( FToolMenuOwner iOwner, FName iMenuName ) override;
    virtual bool OnCloseRequested() override;

protected:
    //Callbacks
    virtual void OnApplyOverrides(const TMap<FName, UObject*>& iOverrides);
    
    // FTickableEditorObject implementation
	virtual void Tick(float DeltaTime) override;

protected:
    //Tools
    UOdysseyPainterEditorTool*               mSelectedTool;
    TArray<UOdysseyPainterEditorTool*>       mTools;

    FOdysseyHUDSystem*              mHUDSystem;
    TArray<FOdysseyBrushContext*>   mBrushContexts;
    FOdysseyBrushColor              mPaintColor;
    FOnSelectedToolChange mOnSelectedToolChange;
};
