// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "MovieSceneSequence.h"
#include "MovieScene.h"
#include "UObject/SoftObjectPtr.h"

#include "EposMovieSceneSequence.generated.h"

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
};
