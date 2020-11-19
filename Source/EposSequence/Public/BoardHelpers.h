// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

class UMovieSceneSequence;

/**
 * Implements actions for UBoardSequence assets.
 */
class EPOSSEQUENCE_API BoardHelpers
{
public:

    /**
     * Find all board parents of a board sequence.
     *
     * @param iSequence The child sequence
     * @return All the parent hierarchy
     */
    static TArray< UMovieSceneSequence* > FindParents( UMovieSceneSequence* iSequence );

private:

    /**
     * Find the parent of a board sequence.
     *
     * @param iSequence The child sequence
     * @return Its parent
     */
    static UMovieSceneSequence* FindParent( UMovieSceneSequence* iSequence );
};
