// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Misc/InlineValue.h"
#include "Tracks/MovieSceneSubTrack.h"
#include "Compilation/MovieSceneSegmentCompiler.h"
#include "MovieSceneBoardTrack.generated.h"

class UMovieSceneSequence;
class UMovieSceneSubSection;

/**
 * A track that holds consecutive sub sequences.
 */
UCLASS( MinimalAPI )
class UMovieSceneBoardTrack
    : public UMovieSceneSubTrack
{
    GENERATED_BODY()

public:

    UMovieSceneBoardTrack( const FObjectInitializer& ObjectInitializer );

    EPOSTRACKS_API void SortSections();

    // UMovieSceneSubTrack interface

    EPOSTRACKS_API virtual UMovieSceneSubSection* AddSequence( UMovieSceneSequence* Sequence, FFrameNumber StartTime, int32 Duration ) override;
    EPOSTRACKS_API virtual UMovieSceneSubSection* AddSequenceOnRow( UMovieSceneSequence* Sequence, FFrameNumber StartTime, int32 Duration, int32 RowIndex ) override;

    // UMovieSceneTrack interface

    virtual void AddSection( UMovieSceneSection& Section ) override;
    virtual bool SupportsType( TSubclassOf<UMovieSceneSection> SectionClass ) const override;
    virtual UMovieSceneSection* CreateNewSection() override;
    virtual void RemoveSection( UMovieSceneSection& Section ) override;
    virtual void RemoveSectionAt( int32 SectionIndex ) override;
    virtual bool SupportsMultipleRows() const override;
    virtual FMovieSceneTrackRowSegmentBlenderPtr GetRowSegmentBlender() const override;
    virtual FMovieSceneTrackSegmentBlenderPtr GetTrackSegmentBlender() const override;

#if WITH_EDITOR
    virtual void OnSectionMoved( UMovieSceneSection& Section, const FMovieSceneSectionMovedParams& Params ) override;
#endif

#if WITH_EDITORONLY_DATA
    virtual FText GetDefaultDisplayName() const override;
#endif
};
