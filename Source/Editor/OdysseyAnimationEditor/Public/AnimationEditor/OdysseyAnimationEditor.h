// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyPainterEditor.h"
#include "OdysseyAnimationEditorGUI.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "Tools/RasterDrawingTool/OdysseyAnimationEditorRasterDrawingTool.h"
#include "Tools/PaintBucketTool/OdysseyAnimationEditorPaintBucketTool.h"
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
    virtual UOdysseyAnimationEditorPaintBucketTool* GetPaintBucketTool() const override;

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
    void OnRenderImageChanged(UOdysseyAnimation* iAnimation, const TRange<int>& iRange, const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive);

private:
    UOdysseyAnimation* mAnimation;
	TSharedPtr<FOdysseyAnimationEditorGUI> mGUI;
    
    UOdysseyAnimationEditorRasterDrawingTool* mRasterDrawingTool;
    UOdysseyAnimationEditorPaintBucketTool* mPaintBucketTool;

    TSharedPtr<IOdysseyHandle> mLayerStackPreloadHandle;

    UOdysseyAnimationPlayer* mPlayer;
    UOdysseyAnimationTexture* mTexture;

    float mPlaybackFramesPerSecond;
    FString mCurrentFrameId;
};
