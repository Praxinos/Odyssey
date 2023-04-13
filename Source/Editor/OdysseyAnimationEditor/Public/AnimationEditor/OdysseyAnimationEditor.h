// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyPainterEditor.h"
#include "OdysseyAnimationEditorGUI.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "Tools/RasterDrawingTool/OdysseyAnimationEditorRasterDrawingTool.h"
#include "Tools/VectorPrimitiveDrawingTool/OdysseyAnimationEditorVectorPrimitiveDrawingTool.h"
#include "Tools/VectorPathDrawingTool/OdysseyAnimationEditorVectorPathDrawingTool.h"
#include "Tools/VectorPathEditTool/OdysseyAnimationEditorVectorPathEditTool.h"
#include "Tools/VectorPathCutTool/OdysseyAnimationEditorVectorPathCutTool.h"
#include "Tools/VectorObjectPickTool/OdysseyAnimationEditorVectorObjectPickTool.h"
#include "Tools/VectorObjectMoveTool/OdysseyAnimationEditorVectorObjectMoveTool.h"
#include "Tools/VectorObjectRotateTool/OdysseyAnimationEditorVectorObjectRotateTool.h"
#include "Tools/VectorObjectScaleTool/OdysseyAnimationEditorVectorObjectScaleTool.h"
#include "Tools/VectorSceneScaleTool/OdysseyAnimationEditorVectorSceneScaleTool.h"
#include "Tools/VectorScenePanTool/OdysseyAnimationEditorVectorScenePanTool.h"
#include "Tools/VectorEraserTool/OdysseyAnimationEditorVectorEraserTool.h"
#include "Tools/VectorPathPushTool/OdysseyAnimationEditorVectorPathPushTool.h"
#include "Tools/VectorPathWidthTool/OdysseyAnimationEditorVectorPathWidthTool.h"
#include "Tools/VectorPathSmoothTool/OdysseyAnimationEditorVectorPathSmoothTool.h"
#include "Tools/VectorPathKnotTool/OdysseyAnimationEditorVectorPathKnotTool.h"
#include "Tools/PaintBucketTool/OdysseyAnimationEditorPaintBucketTool.h"
#include "Tools/ColorPickerTool/OdysseyAnimationEditorColorPickerTool.h"
#include "Tools/VectorGridTool/OdysseyAnimationEditorVectorGridTool.h"
#include "Misc/OdysseyHandle.h"

class UOdysseyAnimationPlayer;
class UOdysseyAnimationTexture;

/**
 * Implements an Editor for animations.
 */
class ODYSSEYANIMATIONEDITOR_API FOdysseyAnimationEditor
    : public FOdysseyPainterEditor
{
public:
    // Construction / Destruction
    virtual ~FOdysseyAnimationEditor();
    FOdysseyAnimationEditor(); //Non Initialized constructor

public:
    virtual void InitData(UObject* iEditedObject) override;
    virtual void InitTools() override;
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;
    virtual void ExtendMenu(FToolMenuOwner iOwner, FName iMenuName) override;

public:
    // Getters
	virtual UOdysseyAnimation*				    Animation() const;
    virtual UOdysseyAnimationLayerStack*	    LayerStack() const;
    virtual UOdysseyAnimationPlayer*                   Player() const;
    virtual float                               PlaybackFramesPerSecond() const;

	virtual UTexture*                           DisplayTexture() const override;
    virtual TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> GetDisplayBlock() override;

    virtual UOdysseyAnimationEditorRasterDrawingTool* GetRasterDrawingTool() const override;
    virtual UOdysseyAnimationEditorVectorPrimitiveDrawingTool* GetVectorPrimitiveDrawingTool() const override;
    virtual UOdysseyAnimationEditorVectorPathDrawingTool* GetVectorPathDrawingTool() const override;
    virtual UOdysseyAnimationEditorVectorPathEditTool* GetVectorPathEditTool() const override;
    virtual UOdysseyAnimationEditorVectorPathCutTool* GetVectorPathCutTool() const override;
    virtual UOdysseyAnimationEditorVectorObjectPickTool* GetVectorObjectPickTool() const override;
    virtual UOdysseyAnimationEditorVectorObjectMoveTool* GetVectorObjectMoveTool() const override;
    virtual UOdysseyAnimationEditorVectorObjectRotateTool* GetVectorObjectRotateTool() const override;
    virtual UOdysseyAnimationEditorVectorObjectScaleTool* GetVectorObjectScaleTool() const override;
    virtual UOdysseyAnimationEditorVectorSceneScaleTool* GetVectorSceneScaleTool() const override;
    virtual UOdysseyAnimationEditorVectorScenePanTool* GetVectorScenePanTool() const override;
    virtual UOdysseyAnimationEditorVectorEraserTool* GetVectorEraserTool() const override;
    virtual UOdysseyAnimationEditorVectorPathPushTool* GetVectorPathPushTool() const override;
    virtual UOdysseyAnimationEditorVectorPathWidthTool* GetVectorPathWidthTool() const override;
    virtual UOdysseyAnimationEditorVectorPathSmoothTool* GetVectorPathSmoothTool() const override;
    virtual UOdysseyAnimationEditorVectorPathKnotTool* GetVectorPathKnotTool() const override;
    virtual UOdysseyAnimationEditorPaintBucketTool* GetPaintBucketTool() const override;
    virtual UOdysseyAnimationEditorColorPickerTool* GetColorPickerTool() const override;
    virtual UOdysseyAnimationEditorVectorGridTool* GetVectorGridTool() const override;

public:
    // Overrides
    virtual FOdysseyAnimationEditorGUI* GetGUI() override;
    virtual TSharedPtr<FWorkspaceItem> RegisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) override;

protected:
    // FGCObject implementation
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

private:
    //Called when the media player has seeked successfully
    void OnPlayerStop();
    void OnCurrentFrameChanged(UOdysseyAnimation* iAnimation);
    void OnRenderImageIdCommited(UOdysseyAnimation* iAnimation);
    void OnCurrentLayerChanged(class UOdysseyLayerStack* iLayerStack);

private:
    UOdysseyAnimation* mAnimation;
	TSharedPtr<FOdysseyAnimationEditorGUI> mGUI;
    
    UOdysseyAnimationEditorRasterDrawingTool* mRasterDrawingTool;
    UOdysseyAnimationEditorVectorPrimitiveDrawingTool* mVectorPrimitiveDrawingTool;
    UOdysseyAnimationEditorVectorPathDrawingTool* mVectorPathDrawingTool;
    UOdysseyAnimationEditorVectorPathEditTool* mVectorPathEditTool;
    UOdysseyAnimationEditorVectorPathCutTool* mVectorPathCutTool;
    UOdysseyAnimationEditorVectorObjectPickTool* mVectorObjectPickTool;
    UOdysseyAnimationEditorVectorObjectMoveTool* mVectorObjectMoveTool;
    UOdysseyAnimationEditorVectorObjectRotateTool* mVectorObjectRotateTool;
    UOdysseyAnimationEditorVectorObjectScaleTool* mVectorObjectScaleTool;
    UOdysseyAnimationEditorVectorSceneScaleTool* mVectorSceneScaleTool;
    UOdysseyAnimationEditorVectorScenePanTool* mVectorScenePanTool;
    UOdysseyAnimationEditorVectorEraserTool* mVectorEraserTool;
    UOdysseyAnimationEditorVectorPathPushTool* mVectorPathPushTool;
    UOdysseyAnimationEditorVectorPathWidthTool* mVectorPathWidthTool;
    UOdysseyAnimationEditorVectorPathSmoothTool* mVectorPathSmoothTool;
    UOdysseyAnimationEditorVectorPathKnotTool* mVectorPathKnotTool;
    UOdysseyAnimationEditorPaintBucketTool* mPaintBucketTool;
    UOdysseyAnimationEditorColorPickerTool* mColorPickerTool;
    UOdysseyAnimationEditorVectorGridTool* mVectorGridTool;

    TSharedPtr<IOdysseyHandle> mLayerStackPreloadHandle;

    UOdysseyAnimationPlayer* mPlayer;
    UOdysseyAnimationTexture* mTexture;

    float mPlaybackFramesPerSecond;
    FString mCurrentFrameId;
};
