// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class UEposMovieSceneSequence;
class UMovieSceneSection;
class UMovieSceneSequence;
class UMovieSceneSubSection;

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
    static TArray< UEposMovieSceneSequence* > FindParents( UEposMovieSceneSequence* iSequence );

private:

    /**
     * Find the parent of a board sequence.
     *
     * @param iSequence The child sequence
     * @return Its parent
     */
    static UEposMovieSceneSequence* FindParent( UEposMovieSceneSequence* iSequence );

public:
    static UMovieSceneSubSection* FindParentSectionOfSequence( UMovieSceneSequence* iParentSequence, UMovieSceneSequence* iChildSequence );

    //---

public:

    static void ResizeChildSequence( UMovieSceneSection* iSection );

    static void ResizeParentSequenceRecursively( UEposMovieSceneSequence* iSequence );
};
