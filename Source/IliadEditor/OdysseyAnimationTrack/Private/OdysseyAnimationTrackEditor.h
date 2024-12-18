// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "MovieSceneTrackEditor.h"

class UOdysseyAnimationLayerStack;
class UOdysseyAnimationComponent;

class FOdysseyAnimationTrackEditor
    : public FMovieSceneTrackEditor
{
public:

    /** Constructor. */
    FOdysseyAnimationTrackEditor( TSharedRef<ISequencer> InSequencer );

    /** Virtual destructor. */
    virtual ~FOdysseyAnimationTrackEditor();

    static TSharedRef<ISequencerTrackEditor> CreateTrackEditor( TSharedRef<ISequencer> OwningSequencer );
    static void OnNewActorTrackAdded(const AActor& iActor, const FGuid& iBinding, TSharedPtr< ISequencer > iSequencer);

public:

    // ISequencerTrackEditor interface
    virtual TSharedPtr<SWidget> BuildOutlinerColumnWidget(const FBuildColumnWidgetParams& Params, const FName& ColumnName) override;
    //virtual TSharedPtr<SWidget> BuildOutlinerEditWidget( const FGuid& ObjectBinding, UMovieSceneTrack* Track, const FBuildEditWidgetParams& Params ) override;
    virtual void BuildObjectBindingTrackMenu(FMenuBuilder& iMenuBuilder, const TArray<FGuid>& iObjectBindings, const UClass* iObjectClass) override;
    virtual TSharedRef<ISequencerSection> MakeSectionInterface( UMovieSceneSection& SectionObject, UMovieSceneTrack& Track, FGuid ObjectBinding ) override;
    virtual bool SupportsType( TSubclassOf<class UMovieSceneTrack> TrackClass ) const override;

private:
    void AddAnimationTrack(TArray<FGuid> ObjectBindings);
    FKeyPropertyResult AddAnimationTrackKeyInternal(FFrameNumber KeyTime, TArray<FGuid> ObjectBindings);
    static float GetDefaultSectionDuration(UOdysseyAnimationComponent* iComponent);
    FReply OnAddButtonClicked(FGuid iObjectBinding);
};
