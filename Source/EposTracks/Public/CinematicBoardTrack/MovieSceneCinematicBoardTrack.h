// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Misc/InlineValue.h"
#include "Tracks/MovieSceneSubTrack.h"
#include "Compilation/MovieSceneSegmentCompiler.h"

#include "ArrangeSectionsType.h"

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
    /** Move all sections to make consecutives each other (previous end bound == next start bound) by the order inside the array */
    EPOSTRACKS_API void OrganizeSections();
    /** Move all sections to the right row */
    EPOSTRACKS_API void ArrangeSections();

    EPOSTRACKS_API void             SetArrangeSections( EArrangeSections iArrangeSections );
    EPOSTRACKS_API EArrangeSections GetArrangeSections();

    // UObject interface
    virtual void PostInitProperties();

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

#if WITH_EDITORONLY_DATA
    virtual FText GetDefaultDisplayName() const override;
#endif

#if WITH_EDITOR
    virtual EMovieSceneSectionMovedResult OnSectionMoved( UMovieSceneSection& ioSection, const FMovieSceneSectionMovedParams& iParams ) override;

private:
    TMap<UMovieSceneSection*, TRange<FFrameNumber>> mPreviousMove;
    TMap<UMovieSceneSection*, TRange<FFrameNumber>> mLastGapMove;
    TMap<UMovieSceneSection*, int32>                mCacheOverlapPriority;
#endif

private:
    UPROPERTY()
    EArrangeSections mArrangeSections;
};
