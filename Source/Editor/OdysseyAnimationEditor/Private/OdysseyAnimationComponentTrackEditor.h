// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "MovieSceneTrackEditor.h"
#include "TrackEditors/SubTrackEditorBase.h"

class FOdysseyAnimationComponentTrackEditor
	: public FMovieSceneTrackEditor
{
public:

	/** Constructor. */
	FOdysseyAnimationComponentTrackEditor( TSharedRef<ISequencer> InSequencer );

	/** Virtual destructor. */
	virtual ~FOdysseyAnimationComponentTrackEditor();

	static TSharedRef<ISequencerTrackEditor> CreateTrackEditor( TSharedRef<ISequencer> OwningSequencer );
	static void OnNewActorTrackAdded(const AActor& iActor, const FGuid& iBinding, TSharedPtr< ISequencer > iSequencer);

public:

	// ISequencerTrackEditor interface
	virtual TSharedPtr<SWidget> BuildOutlinerEditWidget( const FGuid& ObjectBinding, UMovieSceneTrack* Track, const FBuildEditWidgetParams& Params ) override;
	virtual void BuildObjectBindingTrackMenu(FMenuBuilder& iMenuBuilder, const TArray<FGuid>& iObjectBindings, const UClass* iObjectClass) override;
	virtual TSharedRef<ISequencerSection> MakeSectionInterface( UMovieSceneSection& SectionObject, UMovieSceneTrack& Track, FGuid ObjectBinding ) override;
	virtual bool SupportsType( TSubclassOf<class UMovieSceneTrack> TrackClass ) const override;
	
private:
	void AddAnimationTrack(TArray<FGuid> ObjectBindings);
	FKeyPropertyResult AddAnimationTrackKeyInternal(FFrameNumber KeyTime, TArray<FGuid> ObjectBindings);
};
