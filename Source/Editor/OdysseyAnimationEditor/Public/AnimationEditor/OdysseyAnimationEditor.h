// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyPainterEditor.h"
#include "OdysseyAnimationEditorGUI.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "AnimationEditor/OdysseyAnimationEditorTimeline.h"
#include "Tools/RasterDrawingTool/OdysseyAnimationEditorRasterDrawingTool.h"
#include "Tools/PaintBucketTool/OdysseyAnimationEditorPaintBucketTool.h"
#include "Tools/ColorPickerTool/OdysseyAnimationEditorColorPickerTool.h"
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
    virtual UOdysseyAnimationPlayer*            Player() const;
    virtual FOdysseyAnimationEditorTimeline*    Timeline();
    virtual float                               PlaybackFramesPerSecond() const;

	virtual UTexture*                           DisplayTexture() const override;
    virtual TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> GetDisplayBlock() override;

    virtual UOdysseyAnimationEditorRasterDrawingTool* GetRasterDrawingTool() const override;
    virtual UOdysseyAnimationEditorPaintBucketTool* GetPaintBucketTool() const override;
    virtual UOdysseyAnimationEditorColorPickerTool* GetColorPickerTool() const override;


    virtual UOdysseyPainterEditorVectorPrimitiveDrawingTool* GetVectorPrimitiveDrawingTool() const override { return nullptr; }
    virtual UOdysseyPainterEditorVectorPathDrawingTool* GetVectorPathDrawingTool() const override { return nullptr; }
    virtual UOdysseyPainterEditorVectorPathEditTool* GetVectorPathEditTool() const override { return nullptr; }
    virtual UOdysseyPainterEditorVectorPathCutTool* GetVectorPathCutTool() const override { return nullptr; }
    virtual UOdysseyPainterEditorVectorObjectPickTool* GetVectorObjectPickTool() const override { return nullptr; }
    virtual UOdysseyPainterEditorVectorObjectMoveTool* GetVectorObjectMoveTool() const override { return nullptr; }
    virtual UOdysseyPainterEditorVectorObjectRotateTool* GetVectorObjectRotateTool() const override { return nullptr; }
    virtual UOdysseyPainterEditorVectorObjectScaleTool* GetVectorObjectScaleTool() const override { return nullptr; }
    virtual UOdysseyPainterEditorVectorGridTool* GetVectorGridTool() const override { return nullptr; }
    virtual UOdysseyPainterEditorVectorSceneScaleTool* GetVectorSceneScaleTool() const override { return nullptr; }
    virtual UOdysseyPainterEditorVectorScenePanTool* GetVectorScenePanTool() const override { return nullptr; }
    virtual UOdysseyPainterEditorVectorEraserTool* GetVectorEraserTool() const override { return nullptr; }
    virtual UOdysseyPainterEditorVectorPathPushTool* GetVectorPathPushTool() const override { return nullptr; }
    virtual UOdysseyPainterEditorVectorPathWidthTool* GetVectorPathWidthTool() const override { return nullptr; }
    virtual UOdysseyPainterEditorVectorPathSmoothTool* GetVectorPathSmoothTool() const override { return nullptr; }
    virtual UOdysseyPainterEditorVectorPathKnotTool* GetVectorPathKnotTool() const override { return nullptr; }

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
    void OnImageRenderingCompositionCommited(const FGuid& iFrameId);
    void OnCurrentLayerChanged(class UOdysseyLayerStack* iLayerStack);

private:
    UOdysseyAnimation* mAnimation;
	TSharedPtr<FOdysseyAnimationEditorGUI> mGUI;

    FOdysseyAnimationEditorTimeline mTimeline; //Those are just the editor specific data of the timeline
    
    UOdysseyAnimationEditorRasterDrawingTool* mRasterDrawingTool;
    UOdysseyAnimationEditorPaintBucketTool* mPaintBucketTool;
    UOdysseyAnimationEditorColorPickerTool* mColorPickerTool;

    TSharedPtr<IOdysseyHandle> mLayerStackPreloadHandle;

    UOdysseyAnimationPlayer* mPlayer;
    UOdysseyAnimationTexture* mTexture;

    float mPlaybackFramesPerSecond;
    TArray<FGuid> mImageRenderingComposition;
};
