// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyPainterEditor.h"
#include "OdysseyAnimationEditorGUI.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "AnimationEditor/OdysseyAnimationEditorTimeline.h"
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
    virtual void Tick(float iDeltaTime) override;

public:
    // Getters
	virtual UOdysseyAnimation*				    Animation() const;
    virtual UOdysseyAnimationLayerStack*	    LayerStack() const;
    virtual UOdysseyAnimationPlayer*            Player() const;
    virtual FOdysseyAnimationEditorTimeline*    Timeline();
    virtual float                               PlaybackFramesPerSecond() const;

	virtual UTexture*                           DisplayTexture() const override;
    virtual TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> GetDisplayBlock() override;

    TSharedPtr<FOdysseyRasterBlock> GetCurrentRasterBlock() const;
    bool CanProvideRasterBlockOnDemand() const;
    void ProvideRasterBlock();
    bool IsRasterBlockReadOnly() const;
    FOdysseyVectorEngine* GetCurrentVectorEngine() const;

public:
    // Overrides
    virtual FOdysseyAnimationEditorGUI* GetGUI() override;
    virtual TSharedPtr<FWorkspaceItem> RegisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) override;

public:
    //Common Actions
    virtual void Clear() override;

protected:
    // FGCObject implementation
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

private:
    //Called when the media player has seeked successfully
    void OnPlayerStop();
    void OnCurrentFrameChanged(UOdysseyAnimation* iAnimation);
    void OnImageRenderingCompositionCommited(const FGuid& iFrameId);
    void OnCurrentLayerChanged(class UOdysseyLayerStack* iLayerStack);
    
    void UpdateToolContext();

private:
    UOdysseyAnimation* mAnimation;
	TSharedPtr<FOdysseyAnimationEditorGUI> mGUI;

    FOdysseyAnimationEditorTimeline mTimeline; //Those are just the editor specific data of the timeline
    
    UOdysseyAnimationPlayer* mPlayer;
    UOdysseyAnimationTexture* mTexture;

    float mPlaybackFramesPerSecond;
    TArray<FGuid> mImageRenderingComposition;
};
