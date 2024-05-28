// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditor.h"
#include "OdysseyPainterEditorGUI.h"
#include "OdysseyHUDSystem.h"
#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingTool.h"
#include "Tools/RasterEraserTool/OdysseyPainterEditorRasterEraserTool.h"
#include "Tools/RasterSelectionTool/OdysseyPainterEditorRasterSelectionTool.h"
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
#include "OdysseyMediaProvider.h"
#include <ULIS>

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
    virtual UOdysseyPainterEditorRasterSelectionTool*                GetRasterSelectionTool() const;
    virtual UOdysseyPainterEditorRasterTransformTool*                GetRasterTransformTool() const;
    virtual UOdysseyPainterEditorRasterPrimitiveDrawingTool*         GetRasterPrimitiveDrawingTool() const;
    virtual UOdysseyPainterEditorRasterPaintBucketTool*              GetRasterPaintBucketTool() const;
    virtual UOdysseyPainterEditorVectorPrimitiveDrawingTool*         GetVectorPrimitiveDrawingTool() const;
    virtual UOdysseyPainterEditorVectorPathDrawingTool*              GetVectorPathDrawingTool() const;
    virtual UOdysseyPainterEditorVectorPathEditTool*                 GetVectorPathEditTool() const;
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
    virtual FOdysseyMask&                                   EditorMask();

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
    static void Group( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
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

    // Populates the Edit Menu everytime it is displayed
    void AddEditMenuEntry( FMenuBuilder& iMenuBuilder );

    //HUD system related
    //void MakeHUDPersistent( FOdysseyHUDElement* iHUD ); //Make HUD in parameter persistent in this editor, relinquishing ownership to this editor
    //void ClearHUDPersistent(); //Clear persistent HUD, happens on actions like deselect
    //void RemoveHUDPersistent( FString iName) //Removes a particular HUD from the persistent HUD. Todo when we'll be stacking HUDs on top of each other, like selection + guides, things like that

public:
    // Setters
    void  AddExtension(TSharedPtr<FOdysseyPainterEditorExtension> iExtension);
    void  SetSource(TSharedPtr<FOdysseyPainterEditorSource> iSource);
    void  PaintColor(const FOdysseyBrushColor& iColor, bool iIsCommit);
    /* void  RefreshMaskHUD();
    void  ClearMask(); //Clears the mask data and HUD
    void  ClearMaskHUD(); //Only clears the HUD */

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
    void InitHUD();

    void OnDrawHUD(const FOdysseyHUDSystem::FDrawHUDParams& iParams);
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
    FOdysseyMask                    mEditorMask; //The mask created by a user made selection or from a tool that creates mask/stencil on the drawing
    TArray<FOdysseyBrushContext*>   mBrushContexts;
    FOdysseyBrushColor              mPaintColor;
    FSimpleMulticastDelegate        mOnCurrentToolChanged;
    FSimpleMulticastDelegate        mOnCurrentMainToolChanged;
    FSimpleMulticastDelegate        mOnCurrentTemporaryToolChanged;
    FSimpleMulticastDelegate        mOnSourceChanged;
    
    UOdysseyPainterEditorRasterDrawingTool* mRasterDrawingTool;
    UOdysseyPainterEditorRasterEraserTool* mRasterEraserTool;
    UOdysseyPainterEditorRasterSelectionTool* mRasterSelectionTool;
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

    TMap<UClass*, UOdysseyPainterEditorTool*> mCurrentMainToolPerLayerClass;
};
