// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "PainterEditor/OdysseyPainterEditorExtension.h"
#include "OdysseyAnimationEditorTimeline.h"
#include "OdysseyAnimationEditorFlipSystem.h"
#include "OdysseyAnimationImageRenderingAbility.h"

class FOdysseyPainterEditor;
class UOdysseyAnimation;
class UOdysseyAnimationLayerStack;
class UOdysseyAnimationPlayer;
class FOdysseyAnimationEditorSource;
class FOdysseyAnimationEditorGUI;
class FOdysseyLayerStackEditorBrushContext;
class FOdysseyAnimationGlobalShortcuts;
class UOdysseyAnimationEditorOutOfPegsTool;

class ODYSSEYANIMATIONEDITOR_API FOdysseyAnimationEditorExtension
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
    virtual void AddReferencedObjects(FReferenceCollector& Collector);
    
    virtual void ExtendMenu( FToolMenuOwner iOwnerFName, FName iMenuName ) override;
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;

public:
    UOdysseyAnimation*				    Animation() const;
    UOdysseyAnimationLayerStack*	    LayerStack() const;
    UOdysseyAnimationPlayer*            Player() const;
    FOdysseyAnimationEditorTimeline*    Timeline();
    float                               PlaybackFramesPerSecond() const;
	TSharedPtr<FOdysseyAnimationEditorFlipSystem> FlipSystem() const;

    UOdysseyAnimationEditorOutOfPegsTool* GetOutOfPegsTool() const;

private:
    void OnSourceChanged();
    void OnPlayerStop();
    void OnCurrentFrameChanged(UOdysseyAnimation* iAnimation);
    void OnImageRenderingChanged(const FOdysseyImageRenderingChangedEvent& iEvent);
    void OnLayerMediaChanged();
    
    void ConfigureTools();

public:
    TSharedPtr<FOdysseyAnimationEditorSource> mAnimationSource;
    TSharedPtr<FOdysseyAnimationEditorGUI> mGUI;
    FOdysseyAnimationEditorTimeline mTimeline; //Those are just the editor specific data of the timeline
    TSharedPtr<FOdysseyAnimationEditorFlipSystem> mFlipSystem;

    float mPlaybackFramesPerSecond;
    TArray<FGuid> mImageRenderingComposition;
    TSharedPtr<FOdysseyLayerStackEditorBrushContext> mLayerStackBrushEditorContext;

    TObjectPtr<UOdysseyAnimationEditorOutOfPegsTool> mOutOfPegsTool;
};
