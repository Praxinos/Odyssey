// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditor.h"
#include "OdysseyPainterEditorGUI.h"
#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingTool.h"
#include "Tools/RasterEraserTool/OdysseyPainterEditorRasterEraserTool.h"
#include "Tools/RasterTransformTool/OdysseyPainterEditorRasterTransformTool.h"
#include "Tools/RasterPrimitiveDrawingTool/OdysseyPainterEditorRasterPrimitiveDrawingTool.h"
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
    FOdysseyPainterEditor(const FName& iId, const FText& iName, UObject* iEditedObject, const FName& iLayoutName);

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
    virtual UOdysseyPainterEditorRasterEraserTool*                   GetRasterEraserTool() const;
    virtual UOdysseyPainterEditorRasterTransformTool*                GetRasterTransformTool() const;
    virtual UOdysseyPainterEditorRasterPrimitiveDrawingTool*         GetRasterPrimitiveDrawingTool() const;
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
    
    TArray<FOdysseyBrushContext*>& GetBrushContexts();

    void SetVectorHUDFlags( uint64 iVectorHUDFlags );
    uint64 GetVectorHUDFlags();

    void SetVectorDrawingFlags(uint64 iVectorDrawingFlags);
    uint64 GetVectorDrawingFlags();

    // generic reusable vector methods. 
    static void BringForward( FOdysseyVectorGroupPaint* iScene );
    static void SendBackward( FOdysseyVectorGroupPaint* iScene );
    static void ApplyTransformations( FOdysseyVectorGroupPaint* iScene );
    static void MakePaintGroup( FOdysseyVectorGroupPaint* iScene );
    static void Ungroup( FOdysseyVectorGroupPaint* iScene );
    static void Group( FOdysseyVectorGroupPaint* iScene );
    static void SelectAllPoints( FOdysseyVectorGroupPaint* iScene );
    static void SelectAllObjects( FOdysseyVectorGroupPaint* iScene );
    static void ResetView( FOdysseyVectorGroupPaint* iScene );
    static void UnalignPointSelection( FOdysseyVectorGroupPaint* iScene );
    static void AlignPointSelection( FOdysseyVectorGroupPaint* iScene );
    static void DeletePointSelectionRecursive( FOdysseyVectorObject* iVectorObject
                                             , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                                             , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray
                                             , std::vector<FOdysseyVectorPath*>& oRemovedPathArray
                                             , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray );
    static void DeletePointSelection( FOdysseyVectorGroupPaint* iScene );
    static void DeleteObjects( FOdysseyVectorGroupPaint* iScene );
    static void FlipHorizontal( FOdysseyVectorGroupPaint* iScene );
    static void ClearColoring( FOdysseyVectorGroupPaint* iScene );
    static void FlipVertical( FOdysseyVectorGroupPaint* iScene );
    static void DeleteBucket( FOdysseyVectorBucket* iBucket );
    static void PropagateBucket( FOdysseyVectorBucket* iBucket );
    static void UnpropagateBucket( FOdysseyVectorBucket* iBucket );
    static void CopyObjects( FOdysseyVectorGroupPaint* iScene );
    static void PasteObjects( FOdysseyVectorGroupPaint* iScene );
    static void StitchVertices( FOdysseyVectorGroupPaint* iScene
                              , FOdysseyVectorVertex* iVertexA
                              , FOdysseyVectorVertex* iVertexB );
    static void CopyTransformation( FOdysseyVectorGroupPaint* iScene );
    static void PasteTransformation( FOdysseyVectorGroupPaint* iScene );
    static void MergeScenes( FOdysseyVectorGroupPaint* iDestinationScene
                           , const TArray<FOdysseyVectorGroupPaint*>& iSourceSceneArray );

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

    uint64                          mVectorHUDFlags;
    uint64                          mVectorDrawingFlags;

    FOdysseyHUDSystem*              mHUDSystem;
    TArray<FOdysseyBrushContext*>   mBrushContexts;
    FOdysseyBrushColor              mPaintColor;
    FSimpleMulticastDelegate        mOnSelectedToolChanged;
    FSimpleMulticastDelegate        mOnSourceChanged;
    
    UOdysseyPainterEditorRasterDrawingTool* mRasterDrawingTool;
    UOdysseyPainterEditorRasterEraserTool* mRasterEraserTool;
    UOdysseyPainterEditorRasterTransformTool* mRasterTransformTool;
    UOdysseyPainterEditorRasterPrimitiveDrawingTool* mRasterPrimitiveDrawingTool;
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
