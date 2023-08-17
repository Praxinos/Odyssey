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
#include "Tools/VectorPickTool/OdysseyPainterEditorVectorPickTool.h"
#include "Tools/VectorSceneScaleTool/OdysseyPainterEditorVectorSceneScaleTool.h"
#include "Tools/VectorScenePanTool/OdysseyPainterEditorVectorScenePanTool.h"
#include "Tools/VectorEraserTool/OdysseyPainterEditorVectorEraserTool.h"
#include "Tools/VectorPathPushTool/OdysseyPainterEditorVectorPathPushTool.h"
#include "Tools/VectorPathWidthTool/OdysseyPainterEditorVectorPathWidthTool.h"
#include "Tools/VectorPathSmoothTool/OdysseyPainterEditorVectorPathSmoothTool.h"
#include "Tools/VectorPathStitchTool/OdysseyPainterEditorVectorPathStitchTool.h"
#include "Tools/PaintBucketTool/OdysseyPainterEditorPaintBucketTool.h"
#include "Tools/ColorPickerTool/OdysseyPainterEditorColorPickerTool.h"
#include "Tools/VectorGridTool/OdysseyPainterEditorVectorGridTool.h"
#include "Tools/VectorTransformTool/OdysseyPainterEditorVectorTransformTool.h"
#include "OdysseyMediaProvider.h"
#include <ULIS>

class FOdysseyHUDSystem;
class IOdysseySurfaceEditable;
class UOdysseyPainterEditorTool;
class FOdysseyBrushContext;
class FOdysseyPainterEditorSource;
class UOdysseyLayerStack;

enum class eVectorEditionMode : uint8
{
    Object = 0,
    Vertex = 1
};

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
    virtual void InitData(UObject* iEditedObject);
    virtual void InitTools();

public:
    // Getters
    FSimpleMulticastDelegate& OnSourceChanged();
    FSimpleMulticastDelegate& OnSelectedToolChanged();
    
    TSharedPtr<FOdysseyPainterEditorSource>              GetSource() const;
    virtual FOdysseyPainterEditorGUI*                   GetGUI() = 0;

    virtual FOdysseyHUDSystem*                              HUDSystem() const;
	virtual const FOdysseyBrushColor&                       PaintColor() const;
    virtual UOdysseyPainterEditorTool*                      GetSelectedTool() const;
    virtual FOdysseyMediaProvider                           GetCurrentMediaProvider();
    virtual UOdysseyLayerStack*                             LayerStack() const;
    
    virtual UOdysseyPainterEditorRasterDrawingTool*                  GetRasterDrawingTool() const;
    virtual UOdysseyPainterEditorVectorPrimitiveDrawingTool*         GetVectorPrimitiveDrawingTool() const;
    virtual UOdysseyPainterEditorVectorPathDrawingTool*              GetVectorPathDrawingTool() const;
    virtual UOdysseyPainterEditorVectorPathEditTool*                 GetVectorPathEditTool() const;
    virtual UOdysseyPainterEditorVectorPathCutTool*                  GetVectorPathCutTool() const;
    virtual UOdysseyPainterEditorVectorPickTool*                     GetVectorPickTool() const;
    virtual UOdysseyPainterEditorVectorGridTool*                     GetVectorGridTool() const;
    virtual UOdysseyPainterEditorVectorTransformTool*                GetVectorTransformTool() const;
    virtual UOdysseyPainterEditorVectorSceneScaleTool*               GetVectorSceneScaleTool() const;
    virtual UOdysseyPainterEditorVectorScenePanTool*                 GetVectorScenePanTool() const;
    virtual UOdysseyPainterEditorVectorEraserTool*                   GetVectorEraserTool() const;
    virtual UOdysseyPainterEditorVectorPathPushTool*                 GetVectorPathPushTool() const;
    virtual UOdysseyPainterEditorVectorPathWidthTool*                GetVectorPathWidthTool() const;
    virtual UOdysseyPainterEditorVectorPathSmoothTool*               GetVectorPathSmoothTool() const;
    virtual UOdysseyPainterEditorVectorPathStitchTool*               GetVectorPathStitchTool() const;
    virtual UOdysseyPainterEditorPaintBucketTool*                    GetPaintBucketTool() const;
    virtual UOdysseyPainterEditorColorPickerTool*                    GetColorPickerTool() const;

    // generic reusable vector methods. 
    static void Group( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    static void ResetView( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    static void DeleteSelection( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    static void GroupPaint( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    static void Ungroup( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    static void SendBackward( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    static void BringForward( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    static void FlipHorizontal( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    static void FlipVertical( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    static void SelectAll( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
    static void DeleteBucket( FOdysseyVectorBucket* iBucket );
    static void PropagateBucket( FOdysseyVectorBucket* iBucket );
    static void UnpropagateBucket( FOdysseyVectorBucket* iBucket );
    static void StitchVertices( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );

    void SetVectorEditionMode(eVectorEditionMode iVectorEditionMode);
    eVectorEditionMode GetVectorEditionMode();

public:
    // Setters
    void  SetSource(TSharedPtr<FOdysseyPainterEditorSource> iSource);
    void  PaintColor(const FOdysseyBrushColor& iColor, bool iIsCommit);
    void  SetSelectedTool( UOdysseyPainterEditorTool* iSelectedTool );
    void  RefreshCurrentTool();

public:
    // Interface
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;
    virtual void ExtendMenu( FToolMenuOwner iOwner, FName iMenuName ) override;
    virtual bool OnCloseRequested() override;

protected:
    //Callbacks
    virtual void OnApplyOverrides(const TMap<FName, UObject*>& iOverrides);
    void OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack);
    
    // FTickableEditorObject implementation
	virtual void Tick(float DeltaTime) override;

protected:
    // FGCObject implementation
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

private:
    UOdysseyPainterEditorTool* FindDefaultToolForCurrentLayer();

protected:
    //Tools
    TSharedPtr<FOdysseyPainterEditorSource>  mSource;
    UOdysseyPainterEditorTool*               mSelectedTool;
    TArray<UOdysseyPainterEditorTool*>       mTools;

    eVectorEditionMode                       mVectorEditionMode;

    FOdysseyHUDSystem*              mHUDSystem;
    TArray<FOdysseyBrushContext*>   mBrushContexts;
    FOdysseyBrushColor              mPaintColor;
    FSimpleMulticastDelegate        mOnSelectedToolChanged;
    FSimpleMulticastDelegate        mOnSourceChanged;
    
    UOdysseyPainterEditorRasterDrawingTool* mRasterDrawingTool;
    UOdysseyPainterEditorVectorPrimitiveDrawingTool* mVectorPrimitiveDrawingTool;
    UOdysseyPainterEditorVectorPathDrawingTool* mVectorPathDrawingTool;
    UOdysseyPainterEditorVectorPathEditTool* mVectorPathEditTool;
    UOdysseyPainterEditorVectorPathCutTool* mVectorPathCutTool;
    UOdysseyPainterEditorVectorPickTool* mVectorPickTool;
    UOdysseyPainterEditorVectorSceneScaleTool* mVectorSceneScaleTool;
    UOdysseyPainterEditorVectorScenePanTool* mVectorScenePanTool;
    UOdysseyPainterEditorVectorEraserTool* mVectorEraserTool;
    UOdysseyPainterEditorVectorPathPushTool* mVectorPathPushTool;
    UOdysseyPainterEditorVectorPathWidthTool* mVectorPathWidthTool;
    UOdysseyPainterEditorVectorPathSmoothTool* mVectorPathSmoothTool;
    UOdysseyPainterEditorVectorPathStitchTool* mVectorPathStitchTool;
    UOdysseyPainterEditorPaintBucketTool* mPaintBucketTool;
    UOdysseyPainterEditorColorPickerTool* mColorPickerTool;
    UOdysseyPainterEditorVectorGridTool* mVectorGridTool;
    UOdysseyPainterEditorVectorTransformTool* mVectorTransformTool;

    TMap<UClass*, UOdysseyPainterEditorTool*> mCurrentToolPerLayerClass;
};
