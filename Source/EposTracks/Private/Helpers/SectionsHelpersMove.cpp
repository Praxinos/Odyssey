// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Helpers/SectionsHelpersMove.h"

#include "MovieSceneCommonHelpers.h"
#include "MovieSceneSection.h"
#include "MovieSceneTimeHelpers.h"

//---

TRange<FFrameNumber>
FMoveFragment::GetEffectiveTrueRange() const
{
    if( mIsGap )
        return mInitialGap;

    return mSection->GetTrueRange();
}

int32
FMoveResults2::FindFragmentFromSection( const UMovieSceneSection* iSection ) const
{
    for( int i = 0; i < mFragments.Num(); i++ )
    {
        FMoveFragment fragment = mFragments[i];
        if( fragment.mSection == iSection )
            return i;
    }

    return INDEX_NONE;
}

static
void
SwitchFragment( FMoveFragment& ioFragment1, FMoveFragment& ioFragment2 )
{
    FMoveFragment& fragment_left = ioFragment1;
    FMoveFragment& fragment_right = ioFragment2;
    if( ioFragment1.GetEffectiveTrueRange().GetLowerBoundValue() > ioFragment2.GetEffectiveTrueRange().GetLowerBoundValue() )
    {
        fragment_left = ioFragment2;
        fragment_right = ioFragment1;
    }

    if( fragment_left.mIsGap && fragment_right.mIsGap )
    {
        FFrameNumber fragment_left_size = fragment_left.mInitialGapSize;
        FFrameNumber fragment_right_size = fragment_right.mInitialGapSize;

        fragment_left.mInitialGap = UE::MovieScene::TranslateRange( fragment_left.mInitialGap, fragment_right_size );
        fragment_left.mInitialGapMiddle += fragment_right_size;
        fragment_right.mInitialGap = UE::MovieScene::TranslateRange( fragment_right.mInitialGap, -fragment_left_size );
        fragment_right.mInitialGapMiddle -= fragment_left_size;
    }
    else if( !fragment_left.mIsGap && fragment_right.mIsGap )
    {
        FFrameNumber fragment_left_size = UE::MovieScene::DiscreteSize( fragment_left.mSection->GetTrueRange() );
        FFrameNumber fragment_right_size = fragment_right.mInitialGapSize;

        fragment_left.mSection->MoveSection( fragment_right_size );
        fragment_right.mInitialGap = UE::MovieScene::TranslateRange( fragment_right.mInitialGap, -fragment_left_size );
        fragment_right.mInitialGapMiddle -= fragment_left_size;
    }
    else if( fragment_left.mIsGap && !fragment_right.mIsGap )
    {
        FFrameNumber fragment_left_size = fragment_left.mInitialGapSize;
        FFrameNumber fragment_right_size = UE::MovieScene::DiscreteSize( fragment_right.mSection->GetTrueRange() );

        fragment_left.mInitialGap = UE::MovieScene::TranslateRange( fragment_left.mInitialGap, fragment_right_size );
        fragment_left.mInitialGapMiddle += fragment_right_size;
        fragment_right.mSection->MoveSection( -fragment_left_size );
    }
    else if( !fragment_left.mIsGap && !fragment_right.mIsGap )
    {
        FFrameNumber fragment_left_size = UE::MovieScene::DiscreteSize( fragment_left.mSection->GetTrueRange() );
        FFrameNumber fragment_right_size = UE::MovieScene::DiscreteSize( fragment_right.mSection->GetTrueRange() );

        fragment_left.mSection->MoveSection( fragment_right_size );
        fragment_right.mSection->MoveSection( -fragment_left_size );
    }
}

void
FMoveResults2::SwitchFragments( int32 iFragmentIndex1, int32 iFragmentIndex2 )
{
    check( FMath::Abs( iFragmentIndex1 - iFragmentIndex2 ) == 1 );

    SwitchFragment( mFragments[iFragmentIndex1], mFragments[iFragmentIndex2] );
}

void
FMoveResults2::MoveForwardGap( int32 iGapIndex, const FMoveSection& iReferenceSection )
{
    check( mFragments.IsValidIndex( iGapIndex ) );
    check( mFragments[iGapIndex].mIsGap );
    check( mFragments[iGapIndex].mSection == iReferenceSection.mSection );

    FMoveFragment& gap_fragment = mFragments[iGapIndex];

    for( int i = iGapIndex + 1; i < mFragments.Num(); i++ )
    {
        FMoveFragment& next_fragment = mFragments[i];

        FFrameNumber next_fragment_middle = next_fragment.GetEffectiveTrueRange().GetLowerBoundValue() + UE::MovieScene::DiscreteSize( next_fragment.GetEffectiveTrueRange() ) / 2;

        if( next_fragment_middle >= iReferenceSection.mReferenceFrame )
        //if( gap_fragment.mInitialGapMiddle >= iReferenceSection.mReferenceFrame )
            break;

        if( next_fragment.mIsGap )
            break;

        check( gap_fragment.GetEffectiveTrueRange().GetUpperBoundValue() == next_fragment.GetEffectiveTrueRange().GetLowerBoundValue() );

        SwitchFragment( gap_fragment, next_fragment );
    }

    //Do it here ? or outside ? when to *physically* re-sort the array ?
    mFragments.StableSort();
}

void
FMoveResults2::MoveBackwardGap( int32 iGapIndex, const FMoveSection& iReferenceSection )
{
    check( mFragments.IsValidIndex( iGapIndex ) );
    check( mFragments[iGapIndex].mIsGap );
    check( mFragments[iGapIndex].mSection == iReferenceSection.mSection );

    FMoveFragment& gap_fragment = mFragments[iGapIndex];

    for( int i = iGapIndex - 1; i >= 0; i-- )
    {
        FMoveFragment& previous_fragment = mFragments[i];

        FFrameNumber previous_fragment_middle = previous_fragment.GetEffectiveTrueRange().GetLowerBoundValue() + UE::MovieScene::DiscreteSize( previous_fragment.GetEffectiveTrueRange() ) / 2;

        if( previous_fragment_middle < iReferenceSection.mReferenceFrame )
            break;

        if( previous_fragment.mIsGap )
            break;

        check( gap_fragment.GetEffectiveTrueRange().GetLowerBoundValue() == previous_fragment.GetEffectiveTrueRange().GetUpperBoundValue() );

        SwitchFragment( previous_fragment, gap_fragment );
    }

    //Do it here ? or outside ? when to *physically* re-sort the array ?
    mFragments.StableSort();
}

//---

//static
FMoveResults2
SectionsHelpersMove::GetMoveInfo2( const TArray<UMovieSceneSection*>& iUnmovedSections, const TMap<UMovieSceneSection*, TRange<FFrameNumber>>& iPreviousMoves, const TMap<UMovieSceneSection*, TRange<FFrameNumber>>& iLastGapMoves, const TArray<UMovieSceneSection*>& iMovedSections )
{
    FMoveResults2 move_result;

    for( UMovieSceneSection* moved_section : iMovedSections )
    {
        TRange<FFrameNumber> previous_move = iPreviousMoves.FindChecked( moved_section );

        if( moved_section->GetInclusiveStartFrame() > previous_move.GetLowerBoundValue() )
            move_result.mForward = true;
        else if( moved_section->GetInclusiveStartFrame() < previous_move.GetLowerBoundValue() )
            move_result.mBackward = true;

        FMoveSection moved_section_to_add;
        moved_section_to_add.mSection = moved_section;
        if( move_result.mForward )
            moved_section_to_add.mReferenceFrame = moved_section->GetExclusiveEndFrame();
        else
            moved_section_to_add.mReferenceFrame = moved_section->GetInclusiveStartFrame();

        move_result.mMovedSections.Add( moved_section_to_add );
    }

    //---

    for( UMovieSceneSection* unmoved_section : iUnmovedSections )
    {
        FMoveFragment fragment;
        fragment.mSection = unmoved_section;
        fragment.mIsGap = false;

        move_result.mFragments.Add( fragment );
    }

    for( TPair<UMovieSceneSection*, TRange<FFrameNumber>> pair : iLastGapMoves )
    {
        FMoveFragment fragment;
        fragment.mSection = pair.Key;
        fragment.mIsGap = true;

        fragment.mInitialGap = pair.Value;
        fragment.mInitialGapMiddle = ( fragment.mInitialGap.GetLowerBoundValue() + fragment.mInitialGap.GetUpperBoundValue() ) / 2;
        fragment.mInitialGapSize = UE::MovieScene::DiscreteSize( fragment.mInitialGap );

        check( fragment.mInitialGapSize == UE::MovieScene::DiscreteSize( fragment.mSection->GetTrueRange() ) );

        move_result.mFragments.Add( fragment );
    }

    //---

    FMoveResults2 move_result_ordered = move_result;

    move_result_ordered.mMovedSections.StableSort();
    move_result_ordered.mFragments.StableSort();

    return move_result_ordered;
}

//static
void
SectionsHelpersMove::FixMoveSections2( TArray<UMovieSceneSection*>& ioSections, TMap<UMovieSceneSection*, TRange<FFrameNumber>>* ioLastGapMoves, const FMoveResults2& iMoveResults )
{
    FMoveResults2 move_results = iMoveResults;

    if( move_results.mForward )
    {
        TArray<FMoveSection> moved_sections = move_results.mMovedSections;
        Algo::Reverse( moved_sections );

        for( FMoveSection current_moved_section : moved_sections )
        {
            int32 gap_index = move_results.FindFragmentFromSection( current_moved_section.mSection );

            move_results.MoveForwardGap( gap_index, current_moved_section );

            int32 new_gap_index = move_results.FindFragmentFromSection( current_moved_section.mSection );
            check( move_results.mFragments[new_gap_index].mIsGap );

            *ioLastGapMoves->Find( current_moved_section.mSection ) = move_results.mFragments[new_gap_index].GetEffectiveTrueRange();
        }
    }
    else
    {
        for( FMoveSection current_moved_section : move_results.mMovedSections )
        {
            int32 gap_index = move_results.FindFragmentFromSection( current_moved_section.mSection );

            move_results.MoveBackwardGap( gap_index, current_moved_section );

            int32 new_gap_index = move_results.FindFragmentFromSection( current_moved_section.mSection );
            check( move_results.mFragments[new_gap_index].mIsGap );

            *ioLastGapMoves->Find( current_moved_section.mSection ) = move_results.mFragments[new_gap_index].GetEffectiveTrueRange();
        }
    }
}

//---
