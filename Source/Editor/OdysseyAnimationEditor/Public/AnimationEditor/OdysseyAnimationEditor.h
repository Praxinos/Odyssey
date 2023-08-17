// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

/*
#include "OdysseyPainterEditor.h"
#include "OdysseyAnimationEditorGUI.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "AnimationEditor/OdysseyAnimationEditorTimeline.h"
#include "Misc/OdysseyHandle.h"

class UOdysseyAnimationPlayer;
class UOdysseyAnimationTexture;

class ODYSSEYANIMATIONEDITOR_API FOdysseyAnimationEditor
    : public FOdysseyPainterEditor
{
public:
    // Construction / Destruction
    virtual ~FOdysseyAnimationEditor();
    FOdysseyAnimationEditor(); //Non Initialized constructor

public:
    // Getters
	virtual UOdysseyAnimation*				    Animation() const;
    virtual UOdysseyAnimationLayerStack*	    LayerStack() const;
    virtual UOdysseyAnimationPlayer*            Player() const;
    virtual FOdysseyAnimationEditorTimeline*    Timeline();
    virtual float                               PlaybackFramesPerSecond() const;

    // TSharedPtr<FOdysseyRasterBlock> GetCurrentRasterBlock() const;
    bool CanProvideRasterBlockOnDemand() const;
    void ProvideRasterBlock();
    bool IsRasterBlockReadOnly() const;
    FOdysseyVectorEngine* GetCurrentVectorEngine() const;

public:
    // Overrides
    FOdysseyAnimationEditorGUI* GetGUI();

private:
    //Called when the media player has seeked successfully
    void OnPlayerStop();
    void OnCurrentFrameChanged(UOdysseyAnimation* iAnimation);
    void OnImageRenderingCompositionCommited(const FGuid& iFrameId);
    void OnCurrentLayerChanged(class UOdysseyLayerStack* iLayerStack);
    void OnLayerStackElementMediaChanged();

private:
	TSharedPtr<FOdysseyAnimationEditorGUI> mGUI;
    UOdysseyAnimation* mAnimation;
    FOdysseyAnimationEditorTimeline mTimeline; //Those are just the editor specific data of the timeline    

    float mPlaybackFramesPerSecond;
    TArray<FGuid> mImageRenderingComposition;
};
*/