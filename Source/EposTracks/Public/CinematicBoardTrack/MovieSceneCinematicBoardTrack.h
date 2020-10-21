// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Misc/InlineValue.h"
#include "Tracks/MovieSceneSubTrack.h"
#include "Compilation/MovieSceneSegmentCompiler.h"
#include "MovieSceneCinematicBoardTrack.generated.h"

class UMovieSceneSequence;
class UMovieSceneSubSection;

/**
 * A track that holds consecutive sub sequences.
 */
UCLASS( MinimalAPI )
class UMovieSceneCinematicBoardTrack
    : public UMovieSceneSubTrack
{
    GENERATED_BODY()

public:

    UMovieSceneCinematicBoardTrack( const FObjectInitializer& iObjectInitializer );

    EPOSTRACKS_API void SortSections();

    // UMovieSceneSubTrack interface

    EPOSTRACKS_API virtual UMovieSceneSubSection* AddSequence( UMovieSceneSequence* iSequence, FFrameNumber iStartTime, int32 iDuration ) override;
    EPOSTRACKS_API virtual UMovieSceneSubSection* AddSequenceOnRow( UMovieSceneSequence* iSequence, FFrameNumber iStartTime, int32 iDuration, int32 iRowIndex ) override;

    // UMovieSceneTrack interface

    virtual void AddSection( UMovieSceneSection& ioSection ) override;
    virtual bool SupportsType( TSubclassOf<UMovieSceneSection> iSectionClass ) const override;
    virtual UMovieSceneSection* CreateNewSection() override;
    virtual void RemoveSection( UMovieSceneSection& ioSection ) override;
    virtual void RemoveSectionAt( int32 iSectionIndex ) override;
    virtual bool SupportsMultipleRows() const override;
    virtual FMovieSceneTrackRowSegmentBlenderPtr GetRowSegmentBlender() const override;
    virtual FMovieSceneTrackSegmentBlenderPtr GetTrackSegmentBlender() const override;

#if WITH_EDITOR
    virtual void OnSectionMoved( UMovieSceneSection& ioSection, const FMovieSceneSectionMovedParams& iParams ) override;
#endif

#if WITH_EDITORONLY_DATA
    virtual FText GetDefaultDisplayName() const override;
#endif

private:
    TMap<UMovieSceneSection*, TRange<FFrameNumber>> mPreviousMove;
    TMap<UMovieSceneSection*, TRange<FFrameNumber>> mLastGapMove;
    TMap<UMovieSceneSection*, int32>                mCacheOverlapPriority;

    friend uint32 GetTypeHash( const UMovieSceneSection* iSection )
    {
        return GetTypeHash( iSection->GetFullName() );
    }
};
