// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditor.h"
#include "OdysseyPainterEditorGUI.h"
#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingTool.h"
#include "Tools/RasterPaintBucketTool/OdysseyPainterEditorRasterPaintBucketTool.h"
#include "Tools/VectorPrimitiveDrawingTool/OdysseyPainterEditorVectorPrimitiveDrawingTool.h"
#include "Tools/VectorPathDrawingTool/OdysseyPainterEditorVectorPathDrawingTool.h"
#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditTool.h"
#include "Tools/VectorPathCutTool/OdysseyPainterEditorVectorPathCutTool.h"
#include "Tools/VectorSelectionTool/OdysseyPainterEditorVectorSelectionTool.h"
#include "Tools/VectorScenePanTool/OdysseyPainterEditorVectorScenePanTool.h"
#include "Tools/VectorEraserTool/OdysseyPainterEditorVectorEraserTool.h"
#include "Tools/VectorPathPushTool/OdysseyPainterEditorVectorPathPushTool.h"
#include "Tools/VectorPathSmoothTool/OdysseyPainterEditorVectorPathSmoothTool.h"
#include "Tools/VectorPathStitchTool/OdysseyPainterEditorVectorPathStitchTool.h"
#include "Tools/VectorPaintBucketTool/OdysseyPainterEditorVectorPaintBucketTool.h"
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
class FOdysseyPainterEditorExtension;
class UOdysseyLayerStack;
class FOdysseyMeshSelector;

/**
 * Base class for a Painting Editor
 */
class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditor
    : public FOdysseyEditor
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditor();
    FOdysseyPainterEditor(const FText& iName, UObject* iEditedObject, const FName& iLayoutName);

public:
    virtual void Initialize() override;
    virtual TSharedRef<FTabManager::FLayout> CreateLayout() override;
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;
    virtual void ExtendMenu( FToolMenuOwner iOwner, FName iMenuName ) override;
    virtual void OnClose() override;

public:
    // Getters
    FSimpleMulticastDelegate& OnSourceChanged();
    FSimpleMulticastDelegate& OnSelectedToolChanged();
    
    TSharedPtr<FOdysseyPainterEditorSource>              GetSource() const;
    virtual FOdysseyPainterEditorGUI*                    GetGUI();

    virtual FOdysseyHUDSystem*                              HUDSystem() const;
	virtual const FOdysseyBrushColor&                       PaintColor() const;
    virtual UOdysseyPainterEditorTool*                      GetSelectedTool() const;
    virtual FOdysseyMediaProvider                           GetCurrentMediaProvider();
    virtual UOdysseyLayerStack*                             LayerStack() const;
    
    TSharedPtr<FOdysseyMeshSelector>                        GetMeshSelector() const;
    
    virtual UOdysseyPainterEditorRasterDrawingTool*                  GetRasterDrawingTool() const;
    virtual UOdysseyPainterEditorRasterPaintBucketTool*              GetRasterPaintBucketTool() const;
    virtual UOdysseyPainterEditorVectorPrimitiveDrawingTool*         GetVectorPrimitiveDrawingTool() const;
    virtual UOdysseyPainterEditorVectorPathDrawingTool*              GetVectorPathDrawingTool() const;
    virtual UOdysseyPainterEditorVectorPathEditTool*                 GetVectorPathEditTool() const;
    virtual UOdysseyPainterEditorVectorPathCutTool*                  GetVectorPathCutTool() const;
    virtual UOdysseyPainterEditorVectorSelectionTool*                GetVectorSelectionTool() const;
    virtual UOdysseyPainterEditorVectorGridTool*                     GetVectorGridTool() const;
    virtual UOdysseyPainterEditorVectorTransformTool*                GetVectorTransformTool() const;
    virtual UOdysseyPainterEditorVectorScenePanTool*                 GetVectorScenePanTool() const;
    virtual UOdysseyPainterEditorVectorEraserTool*                   GetVectorEraserTool() const;
    virtual UOdysseyPainterEditorVectorPathPushTool*                 GetVectorPathPushTool() const;
    virtual UOdysseyPainterEditorVectorPathSmoothTool*               GetVectorPathSmoothTool() const;
    virtual UOdysseyPainterEditorVectorPathStitchTool*               GetVectorPathStitchTool() const;
    virtual UOdysseyPainterEditorVectorPaintBucketTool*              GetVectorPaintBucketTool() const;
    virtual UOdysseyPainterEditorColorPickerTool*                    GetColorPickerTool() const;

    void SetVectorEditionFlags( uint64 iVectorEditionFlags );
    uint64 GetVectorEditionFlags();

    // generic reusable vector methods. 
    static void BringForward( FOdysseyVectorScene* iScene );
    static void SendBackward( FOdysseyVectorScene* iScene );
    static void Ungroup( FOdysseyVectorScene* iScene );
    static void ApplyTransformations( FOdysseyVectorScene* iScene
                                    , std::list<FOdysseyVectorObject*>* iFocusedObjectList );
    static void GroupPaint( FOdysseyVectorScene* iScene );
    static void Group( FOdysseyVectorScene* iScene );
    static void SelectAll( FOdysseyVectorScene* iScene );
    static void ResetView( FOdysseyVectorScene* iScene );
    static void UnalignPointSelection( FOdysseyVectorScene* iScene
                                     , std::list<FOdysseyVectorObject*>* iFocusedObjectList );
    static void AlignPointSelection( FOdysseyVectorScene* iScene
                                   , std::list<FOdysseyVectorObject*>* iFocusedObjectList );
    static void DeletePointSelectionRecursive( FOdysseyVectorObject* iVectorObject
                                             , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                                             , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray
                                             , std::vector<FOdysseyVectorPath*>& oRemovedPathArray
                                             , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray );
    static void DeletePointSelection( FOdysseyVectorScene* iScene
                                    , std::list<FOdysseyVectorObject*>* iFocusedObjectList );
    static void DeleteObjectSelection( FOdysseyVectorScene* iScene );
    static void FlipHorizontal( FOdysseyVectorScene* iScene
                              , std::list<FOdysseyVectorObject*>* iFocusedObjectList );
    static void ClearColoring( FOdysseyVectorScene* iScene
                             , std::list<FOdysseyVectorObject*>* iFocusedObjectList );
    static void FlipVertical( FOdysseyVectorScene* iScene
                            , std::list<FOdysseyVectorObject*>* iFocusedObjectList );
    static void DeleteBucket( FOdysseyVectorBucket* iBucket );
    static void PropagateBucket( FOdysseyVectorBucket* iBucket );
    static void UnpropagateBucket( FOdysseyVectorBucket* iBucket );
    static void CopyObjectSelection( FOdysseyVectorScene* iScene );
    static void PasteObjectSelection( FOdysseyVectorScene* iScene );
    static void StitchVertices( FOdysseyVectorScene* iScene
                              , FOdysseyVectorVertex* iVertexA
                              , FOdysseyVectorVertex* iVertexB );

    // Populates the Edit Menu everytime it is displayed
    void AddEditMenuEntry( FMenuBuilder& iMenuBuilder );

public:
    // Setters
    void  AddExtension(TSharedPtr<FOdysseyPainterEditorExtension> iExtension);
    void  SetSource(TSharedPtr<FOdysseyPainterEditorSource> iSource);
    void  PaintColor(const FOdysseyBrushColor& iColor, bool iIsCommit);
    void  SetSelectedTool( UOdysseyPainterEditorTool* iSelectedTool );
    void  RefreshCurrentTool();



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
    void InitTools();
    UOdysseyPainterEditorTool* FindDefaultToolForCurrentLayer();

protected:
    FText                                    mName;
    FName                                    mLayoutName;
    TSharedPtr<FTabManager::FLayout>         mLayout;

    //Tools
    TSharedPtr<FOdysseyPainterEditorSource>  mSource;
    TSharedPtr<FOdysseyMeshSelector>         mMeshSelector;
    TArray<TSharedPtr<FOdysseyPainterEditorExtension>> mExtensions;
    UOdysseyPainterEditorTool*               mSelectedTool;
    TArray<UOdysseyPainterEditorTool*>       mTools;
    TSharedPtr<FOdysseyPainterEditorGUI>     mGUI;

    uint64                          mVectorEditionFlags;

    FOdysseyHUDSystem*              mHUDSystem;
    TArray<FOdysseyBrushContext*>   mBrushContexts;
    FOdysseyBrushColor              mPaintColor;
    FSimpleMulticastDelegate        mOnSelectedToolChanged;
    FSimpleMulticastDelegate        mOnSourceChanged;
    
    UOdysseyPainterEditorRasterDrawingTool* mRasterDrawingTool;
    UOdysseyPainterEditorRasterPaintBucketTool* mRasterPaintBucketTool;
    UOdysseyPainterEditorVectorPrimitiveDrawingTool* mVectorPrimitiveDrawingTool;
    UOdysseyPainterEditorVectorPathDrawingTool* mVectorPathDrawingTool;
    UOdysseyPainterEditorVectorPathEditTool* mVectorPathEditTool;
    UOdysseyPainterEditorVectorPathCutTool* mVectorPathCutTool;
    UOdysseyPainterEditorVectorSelectionTool* mVectorSelectionTool;
    UOdysseyPainterEditorVectorScenePanTool* mVectorScenePanTool;
    UOdysseyPainterEditorVectorEraserTool* mVectorEraserTool;
    UOdysseyPainterEditorVectorPathPushTool* mVectorPathPushTool;
    UOdysseyPainterEditorVectorPathSmoothTool* mVectorPathSmoothTool;
    UOdysseyPainterEditorVectorPathStitchTool* mVectorPathStitchTool;
    UOdysseyPainterEditorVectorPaintBucketTool* mVectorPaintBucketTool;
    UOdysseyPainterEditorColorPickerTool* mColorPickerTool;
    UOdysseyPainterEditorVectorGridTool* mVectorGridTool;
    UOdysseyPainterEditorVectorTransformTool* mVectorTransformTool;

    TMap<UClass*, UOdysseyPainterEditorTool*> mCurrentToolPerLayerClass;
};
