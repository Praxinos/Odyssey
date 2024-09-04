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
#include "Tools/VectorMatchingTool/OdysseyPainterEditorVectorMatchingTool.h"
#include "Tools/VectorChartTool/OdysseyPainterEditorVectorChartTool.h"
#include "Tools/VectorTrajectoryTool/OdysseyPainterEditorVectorTrajectoryTool.h"
#include "OdysseyMediaProvider.h"
#include "OdysseyVectorEngine.h"
#include <ULIS>

class FOdysseyHUDSystem;
class IOdysseySurfaceEditable;
class UOdysseyPainterEditorTool;
class FOdysseyBrushContext;
class FOdysseyPainterEditorSource;
class FOdysseyPainterEditorExtension;
class UOdysseyLayerStack;
class FOdysseyMeshSelector;
class FOdysseyVectorGroupPaint;

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
    //Tools

    /**
     * @brief Returns the current main tool
     */
    virtual UOdysseyPainterEditorTool* GetCurrentMainTool() const;

    /**
     * @brief Returns the current temporary tool
     */
    virtual UOdysseyPainterEditorTool* GetCurrentTemporaryTool() const;

    /**
     * @brief Returns the current tool (main or temporary)
     */
    virtual UOdysseyPainterEditorTool* GetCurrentTool() const;

    /**
     * @brief Inactivates all tools
     */
    void InactivateAllTools();

    /**
     * @brief Inactivates the current main tool
     * and activates the main tool (if any)
     */
    void InactivateMainTool();

    /**
     * @brief Activates a main tool over the main tool
     */
    void ActivateMainTool( UOdysseyPainterEditorTool* iTool );

    /**
     * @brief Inactivates the current temporary tool
     * and activates the main tool (if any)
     */
    void InactivateTemporaryTool();

    /**
     * @brief Activates a temporary tool over the main tool
     */
    void ActivateTemporaryTool( UOdysseyPainterEditorTool* iTool );

    /**
     * @brief Ensures the current active tool is activable
     * And activates the first activable tool available if needed
     */
    void SanitizeCurrentTool();
    
    virtual UOdysseyPainterEditorRasterDrawingTool*                  GetRasterDrawingTool() const;
    virtual UOdysseyPainterEditorRasterEraserTool*                   GetRasterEraserTool() const;
    virtual UOdysseyPainterEditorRasterTransformTool*                GetRasterTransformTool() const;
    virtual UOdysseyPainterEditorRasterPrimitiveDrawingTool*         GetRasterPrimitiveDrawingTool() const;
    virtual UOdysseyPainterEditorRasterPaintBucketTool*              GetRasterPaintBucketTool() const;
    virtual UOdysseyPainterEditorVectorPrimitiveDrawingTool*         GetVectorPrimitiveDrawingTool() const;
    virtual UOdysseyPainterEditorVectorPathDrawingTool*              GetVectorPathDrawingTool() const;
    virtual UOdysseyPainterEditorVectorPathEditTool*                 GetVectorPathEditTool() const;
    virtual UOdysseyPainterEditorVectorSelectionTool*                GetVectorSelectionTool() const;
    virtual UOdysseyPainterEditorVectorGridTool*                     GetVectorGridTool() const;
    virtual UOdysseyPainterEditorVectorTransformTool*                GetVectorTransformTool() const;
    virtual UOdysseyPainterEditorVectorMatchingTool*                 GetVectorMatchingTool() const;
    virtual UOdysseyPainterEditorVectorChartTool*                    GetVectorChartTool() const;
    virtual UOdysseyPainterEditorVectorTrajectoryTool*               GetVectorTrajectoryTool() const;
    virtual UOdysseyPainterEditorVectorScenePanTool*                 GetVectorScenePanTool() const;
    virtual UOdysseyPainterEditorVectorEraserTool*                   GetVectorEraserTool() const;
    virtual UOdysseyPainterEditorVectorPathPushTool*                 GetVectorPathPushTool() const;
    virtual UOdysseyPainterEditorVectorPathSmoothTool*               GetVectorPathSmoothTool() const;
    virtual UOdysseyPainterEditorVectorPathStitchTool*               GetVectorPathStitchTool() const;
    virtual UOdysseyPainterEditorVectorPaintBucketTool*              GetVectorPaintBucketTool() const;
    virtual UOdysseyPainterEditorColorPickerTool*                    GetColorPickerTool() const;

public:
    // Getters
    FSimpleMulticastDelegate& OnSourceChanged();
    FSimpleMulticastDelegate& OnCurrentToolChanged();
    FSimpleMulticastDelegate& OnCurrentMainToolChanged();
    FSimpleMulticastDelegate& OnCurrentTemporaryToolChanged();
    
    TSharedPtr<FOdysseyPainterEditorSource>              GetSource() const;
    virtual FOdysseyPainterEditorGUI*                    GetGUI();

    virtual FOdysseyHUDSystem*                              HUDSystem() const;
	virtual const FOdysseyBrushColor&                       PaintColor() const;
    virtual FOdysseyMediaProvider                           GetCurrentMediaProvider();
    virtual UOdysseyLayerStack*                             LayerStack() const;
    
    TSharedPtr<FOdysseyMeshSelector>                        GetMeshSelector() const;
    
    TArray<FOdysseyBrushContext*>& GetBrushContexts();

    void SetVectorHUDFlags( uint64 iVectorHUDFlags );
    uint64 GetVectorHUDFlags();

    void SetVectorDrawingFlags(uint64 iVectorDrawingFlags);
    uint64 GetVectorDrawingFlags();

    // generic reusable vector methods. 
    static void BringForward( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void SendBackward( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void ApplyTransformations( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void MakePaintGroup( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void Ungroup( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void GroupAndAddInbetweenerTag( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void Group( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static FOdysseyVectorGroup* _Group( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void SelectAllPoints( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void SelectAllObjects( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void ResetView( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void LockPointSelection( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void UnlockPointSelection( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void UnalignPointSelection( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void AlignPointSelection( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void DeletePointSelectionRecursive( FOdysseyPainterEditor* iEditor
                                             , FOdysseyVectorObject* iVectorObject
                                             , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                                             , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray
                                             , std::vector<FOdysseyVectorPath*>& oRemovedPathArray
                                             , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray );
    static void DeletePointSelection( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void DeleteObjects( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void FlipHorizontal( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void ClearColoring( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void FlipVertical( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void DeleteBucket( FOdysseyPainterEditor* iEditor, FOdysseyVectorBucket* iBucket );
    static void PropagateBucket( FOdysseyPainterEditor* iEditor, FOdysseyVectorBucket* iBucket );
    static void UnpropagateBucket( FOdysseyPainterEditor* iEditor, FOdysseyVectorBucket* iBucket );
    static void CopyObjects( FOdysseyVectorGroupPaint* iScene );
    static void PasteObjects( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void StitchVertices( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene
                              , FOdysseyVectorVertex* iVertexA
                              , FOdysseyVectorVertex* iVertexB );
    static void CopyTransformation( FOdysseyVectorGroupPaint* iScene );
    static void PasteTransformation( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void MergeScenes( FOdysseyVectorGroupPaint* iDestinationScene
                           , const TArray<FOdysseyVectorGroupPaint*>& iSourceSceneArray );
    static void AlterContourWidth( FOdysseyVectorGroupPaint* iScene
                                 , double iValue
                                 , bool   iAbsolute );
    static void AddInbetweenerTag( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void RemoveInbetweenerTag( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void ResetSpacingChart( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void CommitInbetweenerTag( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void CopySpacingChart( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void PasteSpacingChart( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void ResetInbetweenerGrid( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );

public:
    static const uint64 UI_UPDATE_SCENETREEVIEW = ( 1ULL << ( FOdysseyVectorEngine::SIGNAL_RESERVED_SHIFT + 0 ) );
    static const uint64 UI_UPDATE_TIMELINE      = ( 1ULL << ( FOdysseyVectorEngine::SIGNAL_RESERVED_SHIFT + 1 ) );
    static const uint64 UI_UPDATE_OBJECTDETAILS = ( 1ULL << ( FOdysseyVectorEngine::SIGNAL_RESERVED_SHIFT + 2 ) );

    // Populates the Edit Menu everytime it is displayed
    void AddEditMenuEntry( FMenuBuilder& iMenuBuilder );

public:
    // Setters
    void  AddExtension(TSharedPtr<FOdysseyPainterEditorExtension> iExtension);
    void  SetSource(TSharedPtr<FOdysseyPainterEditorSource> iSource);
    void  PaintColor(const FOdysseyBrushColor& iColor, bool iIsCommit);



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
    UOdysseyPainterEditorTool*               mCurrentMainTool;
    UOdysseyPainterEditorTool*               mCurrentTemporaryTool;

    TArray<UOdysseyPainterEditorTool*>       mTools;
    TSharedPtr<FOdysseyPainterEditorGUI>     mGUI;

    uint64                          mVectorHUDFlags;
    uint64                          mVectorDrawingFlags;

    FOdysseyHUDSystem*              mHUDSystem;
    TArray<FOdysseyBrushContext*>   mBrushContexts;
    FOdysseyBrushColor              mPaintColor;
    FSimpleMulticastDelegate        mOnCurrentToolChanged;
    FSimpleMulticastDelegate        mOnCurrentMainToolChanged;
    FSimpleMulticastDelegate        mOnCurrentTemporaryToolChanged;
    FSimpleMulticastDelegate        mOnSourceChanged;
    
    UOdysseyPainterEditorRasterDrawingTool* mRasterDrawingTool;
    UOdysseyPainterEditorRasterEraserTool* mRasterEraserTool;
    UOdysseyPainterEditorRasterTransformTool* mRasterTransformTool;
    UOdysseyPainterEditorRasterPrimitiveDrawingTool* mRasterPrimitiveDrawingTool;
    UOdysseyPainterEditorRasterPaintBucketTool* mRasterPaintBucketTool;
    UOdysseyPainterEditorVectorPrimitiveDrawingTool* mVectorPrimitiveDrawingTool;
    UOdysseyPainterEditorVectorPathDrawingTool* mVectorPathDrawingTool;
    UOdysseyPainterEditorVectorPathEditTool* mVectorPathEditTool;
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
    UOdysseyPainterEditorVectorMatchingTool* mVectorMatchingTool;
    UOdysseyPainterEditorVectorChartTool* mVectorChartTool;
    UOdysseyPainterEditorVectorTrajectoryTool* mVectorTrajectoryTool;

    TMap<UClass*, UOdysseyPainterEditorTool*> mCurrentMainToolPerLayerClass;
};
