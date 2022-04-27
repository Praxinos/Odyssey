// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "MovieSceneSequence.h"
#include "MovieScene.h"
#include "UObject/SoftObjectPtr.h"

#include "EposMovieSceneSequence.generated.h"

class ULevelSequence;

/*
 * Movie scene animation that represents the hierarchical levels of the storyboard.
 */
UCLASS( BlueprintType )
class EPOSMOVIESCENE_API UEposMovieSceneSequence
    : public UMovieSceneSequence
{
public:
    GENERATED_BODY()

    UEposMovieSceneSequence( const FObjectInitializer& ObjectInitializer );

public:

    virtual bool IsResizable() const PURE_VIRTUAL( UEposMovieSceneSequence::IsResizable, return true; );
    virtual void Resize( int32 iNewDuration ) PURE_VIRTUAL( UEposMovieSceneSequence::Resize, );

    virtual void SectionResized( UMovieSceneSection* iSection ) PURE_VIRTUAL( UEposMovieSceneSequence::SectionResized, );
    virtual void SectionAddedOrRemoved( UMovieSceneSection* iSection ) PURE_VIRTUAL( UEposMovieSceneSequence::SectionAddedOrRemoved, );

public:
    // This is for storing a link to the transient level sequence during rendering
    // When opening the movie render queue window, we create a (transient) level sequence (because movie render queue only works with level sequence)
    // and if this level sequence is not stored somewhere, it will be deleted at the end of the function
    // and when we click on the "render" button, this level sequence is no more valid
    // That's why we store it here, to be able to render a valid level sequence pointer
    UPROPERTY( Transient )
    ULevelSequence* mLevelSequenceRender;
};
