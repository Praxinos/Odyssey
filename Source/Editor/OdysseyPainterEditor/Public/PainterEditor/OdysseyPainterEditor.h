// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditor.h"
#include "OdysseyPainterEditorGUI.h"
#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingTool.h"
#include "Tools/VectorPrimitiveDrawingTool/OdysseyPainterEditorVectorPrimitiveDrawingTool.h"
#include "Tools/VectorPathDrawingTool/OdysseyPainterEditorVectorPathDrawingTool.h"
#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditTool.h"
#include "Tools/VectorPathCutTool/OdysseyPainterEditorVectorPathCutTool.h"
#include "Tools/VectorObjectPickTool/OdysseyPainterEditorVectorObjectPickTool.h"
#include "Tools/VectorObjectMoveTool/OdysseyPainterEditorVectorObjectMoveTool.h"
#include "Tools/VectorObjectRotateTool/OdysseyPainterEditorVectorObjectRotateTool.h"
#include "Tools/VectorObjectScaleTool/OdysseyPainterEditorVectorObjectScaleTool.h"
#include "Tools/VectorSceneScaleTool/OdysseyPainterEditorVectorSceneScaleTool.h"
#include "Tools/VectorScenePanTool/OdysseyPainterEditorVectorScenePanTool.h"
#include "Tools/VectorEraserTool/OdysseyPainterEditorVectorEraserTool.h"
#include "Tools/VectorPathPushTool/OdysseyPainterEditorVectorPathPushTool.h"
#include "Tools/VectorPathWidthTool/OdysseyPainterEditorVectorPathWidthTool.h"
#include "Tools/VectorPathSmoothTool/OdysseyPainterEditorVectorPathSmoothTool.h"
#include "Tools/VectorPathKnotTool/OdysseyPainterEditorVectorPathKnotTool.h"
#include "Tools/PaintBucketTool/OdysseyPainterEditorPaintBucketTool.h"
#include "Tools/ColorPickerTool/OdysseyPainterEditorColorPickerTool.h"
#include "Tools/VectorGridTool/OdysseyPainterEditorVectorGridTool.h"
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
    virtual UOdysseyPainterEditorVectorPrimitiveDrawingTool*         GetVectorPrimitiveDrawingTool() const = 0;
    virtual UOdysseyPainterEditorVectorPathDrawingTool*              GetVectorPathDrawingTool() const = 0;
    virtual UOdysseyPainterEditorVectorPathEditTool*                 GetVectorPathEditTool() const = 0;
    virtual UOdysseyPainterEditorVectorPathCutTool*                  GetVectorPathCutTool() const = 0;
    virtual UOdysseyPainterEditorVectorObjectPickTool*               GetVectorObjectPickTool() const = 0;
    virtual UOdysseyPainterEditorVectorObjectMoveTool*               GetVectorObjectMoveTool() const = 0;
    virtual UOdysseyPainterEditorVectorObjectRotateTool*             GetVectorObjectRotateTool() const = 0;
    virtual UOdysseyPainterEditorVectorObjectScaleTool*              GetVectorObjectScaleTool() const = 0;
    virtual UOdysseyPainterEditorVectorGridTool*                     GetVectorGridTool() const = 0;
    virtual UOdysseyPainterEditorVectorSceneScaleTool*               GetVectorSceneScaleTool() const = 0;
    virtual UOdysseyPainterEditorVectorScenePanTool*                 GetVectorScenePanTool() const = 0;
    virtual UOdysseyPainterEditorVectorEraserTool*                   GetVectorEraserTool() const = 0;
    virtual UOdysseyPainterEditorVectorPathPushTool*                 GetVectorPathPushTool() const = 0;
    virtual UOdysseyPainterEditorVectorPathWidthTool*                GetVectorPathWidthTool() const = 0;
    virtual UOdysseyPainterEditorVectorPathSmoothTool*               GetVectorPathSmoothTool() const = 0;
    virtual UOdysseyPainterEditorVectorPathKnotTool*                 GetVectorPathKnotTool() const = 0;
    virtual UOdysseyPainterEditorPaintBucketTool*                    GetPaintBucketTool() const = 0;
    virtual UOdysseyPainterEditorColorPickerTool*                    GetColorPickerTool() const = 0;

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
