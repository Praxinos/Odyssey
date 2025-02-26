// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "OdysseyPainterEditorExtension.h"
#include "OdysseyAnimationEditorTimelinePosition.h"
#include "OdysseyImageRenderingAbility.h"

class FOdysseyPainterEditor;
class UOdysseyAnimation;
class UOdysseyAnimationLayerStack;
class UOdysseyAnimationPlayer;
class FOdysseyAnimationEditorSource;
class FOdysseyAnimationEditorGUI;
class FOdysseyLayerStackEditorBrushContext;
class FOdysseyAnimationGlobalShortcuts;
class UOdysseyAnimationEditorOutOfPegsTool;
class FOdysseyAnimationEditorFlipSystem;

class ODYSSEYPAINTEREDITOR_API FOdysseyAnimationEditorExtension
    : public FOdysseyPainterEditorExtension
    , public TSharedFromThis<FOdysseyAnimationEditorExtension>
{
public:
    // Construction / Destruction
    virtual ~FOdysseyAnimationEditorExtension();
    FOdysseyAnimationEditorExtension( FOdysseyPainterEditor* iEditor );

public:
    virtual void Initialize() override;
    virtual void Finalize() override;
    virtual void BuildLayout(FOdysseyEditorLayoutBuilder& iBuilder) override;

public:
    UOdysseyAnimation*                    Animation() const;
    UOdysseyAnimationLayerStack*        LayerStack() const;
    UOdysseyAnimationPlayer*            Player() const;
    TSharedRef<FOdysseyAnimationEditorTimelinePosition>    TimelinePosition();
    float                               PlaybackFramesPerSecond() const;
    TSharedPtr<FOdysseyAnimationEditorFlipSystem> FlipSystem() const;

    UOdysseyAnimationEditorOutOfPegsTool* GetOutOfPegsTool() const;

private:
    void OnSourceChanged();
    void OnPlayerStop();
    void OnCurrentFrameChanged(UOdysseyAnimation* iAnimation);
    void OnImageRenderingChanged(const FOdysseyImageRenderingChangedEvent& iEvent);
    void OnLayerMediaChanged();

public:
    TSharedPtr<FOdysseyAnimationEditorSource> mAnimationSource;
    TSharedPtr<FOdysseyAnimationEditorGUI> mGUI;
    TSharedRef<FOdysseyAnimationEditorTimelinePosition> mTimelinePosition;
    TSharedPtr<FOdysseyAnimationEditorFlipSystem> mFlipSystem;

    float mPlaybackFramesPerSecond;
    TArray<FGuid> mImageRenderingComposition;
    TSharedPtr<FOdysseyLayerStackEditorBrushContext> mLayerStackBrushEditorContext;

    TObjectPtr<UOdysseyAnimationEditorOutOfPegsTool> mOutOfPegsTool;
};
