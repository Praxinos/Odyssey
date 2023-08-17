// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "PainterEditor/OdysseyPainterEditorExtension.h"

class ODYSSEYANIMATIONEDITOR_API FOdysseyAnimationEditorExtension
    : public FOdysseyPainterEditorExtension
{   
public:
    // Construction / Destruction
    virtual ~FOdysseyAnimationEditorExtension();
    FOdysseyAnimationEditorExtension( FOdysseyPainterEditor* iEditor );

public:
    virtual void Initialize() override;
    virtual void Finalize() override;

public:
    UOdysseyAnimation*				    Animation() const;
    UOdysseyAnimationLayerStack*	    LayerStack() const;
    UOdysseyAnimationPlayer*            Player() const;
    FOdysseyAnimationEditorTimeline*    Timeline();
    float                               PlaybackFramesPerSecond() const;

public:
    // Overrides
    //virtual FOdysseyAnimationEditorGUI* GetGUI() override;
    //virtual TSharedPtr<FWorkspaceItem> RegisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) override;

private:
    void OnSourceChanged();
    void OnPlayerStop();
    void OnCurrentFrameChanged(UOdysseyAnimation* iAnimation);
    void OnImageRenderingCompositionCommited(const FGuid& iFrameId);
    void OnCurrentLayerChanged(class UOdysseyLayerStack* iLayerStack);
    void OnLayerStackElementMediaChanged();

public:
    TSharedPtr<FOdysseyAnimationEditorSource> mAnimationSource;
    TSharedPtr<FOdysseyAnimationEditorGUI> mGUI;
    FOdysseyAnimationEditorTimeline mTimeline; //Those are just the editor specific data of the timeline    

    float mPlaybackFramesPerSecond;
    TArray<FGuid> mImageRenderingComposition;
};
