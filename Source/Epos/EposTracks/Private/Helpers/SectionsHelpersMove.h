// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class UMovieScene;
class UMovieSceneSection;

/**
 * FMoveSection represents a selected section (with its real range) which are moving with a reference frame
 *
 * The reference frame can be its lower bound or its upper bound frame, depending if the section is moving forward or backward
 */
struct FMoveSection
{
    UMovieSceneSection* mSection;

    FFrameNumber                mReferenceFrame;

    friend bool operator< ( const FMoveSection& iA, const FMoveSection& iB )
    {
        return iA.mReferenceFrame < iB.mReferenceFrame;
    }
};

/**
 * FMoveFragment represents all section ranges in the track
 * Except that for selected moving sections, it's its corresponding gap range which are stored
 *
 * If the fragment is:
 * - a gap: its range is stored here
 * - an unselected section: its range is the section range
 */
struct FMoveFragment
{
    UMovieSceneSection* mSection;

    bool                        mIsGap = false;
    TRange<FFrameNumber>        mInitialGap = TRange<FFrameNumber>::Empty();
    FFrameNumber                mInitialGapMiddle;
    FFrameNumber                mInitialGapSize;

    TRange<FFrameNumber> GetEffectiveTrueRange() const;

    friend bool operator< ( const FMoveFragment& iA, const FMoveFragment& iB );
};

/**
 * Store all the fragments of the track and the selected moving sections
 *
 * For example:           --> Move Forward (R if for the reference frame for each selected section)
 *
 * (When moving backward, the reference frame R for each selected section is the lower bound)
 *
 *                                                           s2                                  s5           s6
 *                                             ------------------------------                ---------------------------
 *                                             |                            R                |        R|               R
 *                                             |              2             R                |   5    R|       6       R
 *                                             |                            R                |        R|               R
 *                                             ------------------------------                ---------------------------
 *
 *   ------------------..............................-------------------...........................----------------------------
 *   |    ||          ||                            ||            ||   ||        ||               ||                ||        |
 *   |  0 ||    1     ||             2              ||       3    || 4 ||    5   ||       6       ||       7        ||   8    |
 *   |    ||          ||                            ||            ||   ||        ||               ||                ||        |
 *   ------------------..............................-------------------...........................----------------------------
 *                                 gap2                                    gap5          gap6
 *     s0      s1                                          s3       s4                                    s7            s8
 *
 *
 * After updating all the fragments, this will result as:
 * (As moving forward, the gaps are shifted in this order: gap6 then gap5 than gap2, and a previous gap can't jump over a following gap)
 *
 *                                                           s2                                  s5           s6
 *                                             ------------------------------                ---------------------------
 *                                             |                            R                |        R|               R
 *                                             |              2             R                |   5    R|       6       R
 *                                             |                            R                |        R|               R
 *                                             ------------------------------                ---------------------------
 *
 *   -------------------------------------..............................------------------...........................----------
 *   |    ||          ||            ||   ||                            ||                ||        ||               ||        |
 *   |  0 ||    1     ||       3    || 4 ||             2              ||       7        ||    5   ||       6       ||   8    |
 *   |    ||          ||            ||   ||                            ||                ||        ||               ||        |
 *   -------------------------------------..............................------------------...........................----------
 *                                                    gap2                                   gap5          gap6
 *     s0      s1            s3       s4                                       s7                                       s8
 *
 * Once the drag ends (mouse up), just set the selected section range to its corresponding gap range:
 *
 *   --------------------------------------------------------------------------------------------------------------------------
 *   |    ||          ||            ||   ||                            ||                ||        ||               ||        |
 *   |  0 ||    1     ||       3    || 4 ||              2             ||       7        ||   5    ||       6       ||   8    |
 *   |    ||          ||            ||   ||                            ||                ||        ||               ||        |
 *   --------------------------------------------------------------------------------------------------------------------------
 *     s0      s1            s3       s4              s2                       s7             s5           s6           s8
 *
 * Then sort correctly the Sections of the track to match the new order
 *
 */
struct FMoveResults2
{
    bool                        mForward = false;
    bool                        mBackward = false;
    TArray<FMoveSection>        mMovedSections;

    TArray<FMoveFragment>       mFragments;

    /**
     * Move the gap of the corresponding selected moving (forward) section to the best position which match the real position of the selected moving section
     *
     * @param   iGapIndex   The gap index corresponding to the section
     * @param   iSection    The reference section used to make best-matching the gap
     */
    void MoveForwardGap( int32 iGapIndex, const FMoveSection& iReferenceSection );
    /**
     * Move the gap of the corresponding selected moving (backward) section to the best position which match the real position of the selected moving section
     *
     * @param   iGapIndex   The gap index corresponding to the section
     * @param   iSection    The reference section used to make best-matching the gap
     */
    void MoveBackwardGap( int32 iGapIndex, const FMoveSection& iReferenceSection );

    /**
     * Return the fragment index corresponding to the section
     *
     * @param   iSection    The section to find its fragment
     *
     * @return  The index of the fragment
     */
    int32 FindFragmentFromSection( const UMovieSceneSection* iSection ) const;

    /**
     * Switch 2 consecutives fragments
     *
     * @param   iFragmentIndex1 The first fragment index
     * @param   iFragmentIndex2 The second fragment index
     */
    void SwitchFragments( int32 iFragmentIndex1, int32 iFragmentIndex2 );
};

class EPOSTRACKS_API SectionsHelpersMove
{
public:
    static FMoveResults2 GetMoveInfo2( const TArray<UMovieSceneSection*>& iUnmovedSections, const TMap<UMovieSceneSection*, TRange<FFrameNumber>>& iPreviousMoves, const TMap<UMovieSceneSection*, TRange<FFrameNumber>>& iLastGapMoves, const TArray<UMovieSceneSection*>& iMovedSections );

    static void FixMoveSections2( TArray<UMovieSceneSection*>& ioSections, TMap<UMovieSceneSection*, TRange<FFrameNumber>>* ioLastGapMoves, const FMoveResults2& iMoveResults );
};
