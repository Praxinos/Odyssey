// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Helpers/SectionsHelpersMove.h"

#include "MovieSceneCommonHelpers.h"
#include "MovieSceneSection.h"
#include "MovieSceneTimeHelpers.h"

//---

FMoveResult::FMoveResult()
    : mForward( false )
    , mBackward( false )
    , mReferenceFrame( 0 )
    , mInitialGap( TRange<FFrameNumber>::Empty() )
    , mInitialGapMiddle( 0 )
    , mInitialGapSize( 0 )
    , mSectionsBeforeGap()
    , mSectionsAfterGap()
{
}

//---

//static
FMoveResult
SectionsHelpersMove::GetMoveInfo( TArray< UMovieSceneSection* > iSections, TRange<FFrameNumber> iPreviousMove, TRange<FFrameNumber> iLastGapMove, const UMovieSceneSection* iSection )
{
    FMoveResult move_result;

    if( iSection->GetInclusiveStartFrame() > iPreviousMove.GetLowerBoundValue() )
        move_result.mForward = true;
    else if( iSection->GetInclusiveStartFrame() < iPreviousMove.GetLowerBoundValue() )
        move_result.mBackward = true;
    else
        return move_result;

    if( move_result.mForward )
        move_result.mReferenceFrame = iSection->GetExclusiveEndFrame();
    else
        move_result.mReferenceFrame = iSection->GetInclusiveStartFrame();

    //---

    move_result.mInitialGap = iLastGapMove;
    move_result.mInitialGapMiddle = ( move_result.mInitialGap.GetLowerBoundValue() + move_result.mInitialGap.GetUpperBoundValue() ) / 2;
    move_result.mInitialGapSize = UE::MovieScene::DiscreteSize( move_result.mInitialGap );

    //---

    for( int i = 0; i < iSections.Num(); i++ )
    {
        if( iSections[i] == iSection )
            continue;

        UMovieSceneSection* section = iSections[i];
        FFrameNumber section_middle = ( section->GetInclusiveStartFrame() + section->GetExclusiveEndFrame() ) / 2;

        if( section_middle < move_result.mInitialGapMiddle )
            move_result.mSectionsBeforeGap.Add( section );
        else
            move_result.mSectionsAfterGap.Add( section );
    }


    // So we can use for( auto section : mSectionsBeforeGap )
    Algo::Reverse( move_result.mSectionsBeforeGap );

    return move_result;
}

//static
void
SectionsHelpersMove::FixMoveSections( TArray< UMovieSceneSection* >& ioSections, TRange<FFrameNumber>* ioLastGapMove, UMovieSceneSection* iSection, FMoveResult iMoveResult )
{
    // If the reference frame is at the right of the gap,
    // process only every sections 'AfterGap'
    if( iMoveResult.mReferenceFrame > iMoveResult.mInitialGapMiddle )
    {
        // Find the section (if any) corresponding to the reference frame
        UMovieSceneSection* section_containing_reference_frame = nullptr;
        TArray<TRange<FFrameNumber>> ranges;
        for( auto section : iMoveResult.mSectionsAfterGap )
        {
            if( section->IsTimeWithinSection( iMoveResult.mReferenceFrame ) )
            {
                section_containing_reference_frame = section;
                ranges = section->GetTrueRange().Split( ( section->GetInclusiveStartFrame() + section->GetExclusiveEndFrame() ) / 2 );
                break;
            }
        }

        if( ranges.Num() && ranges[0].Contains( iMoveResult.mReferenceFrame ) )
        {
            // Move every 'after gap' sections before (and excluding) the referenced one
            for( auto section_after_gap : iMoveResult.mSectionsAfterGap )
            {
                if( section_after_gap == section_containing_reference_frame )
                    break;

                FFrameNumber gap_shift = UE::MovieScene::DiscreteSize( section_after_gap->GetTrueRange() );
                *ioLastGapMove = TRange<FFrameNumber>( ioLastGapMove->GetLowerBoundValue() + gap_shift, ioLastGapMove->GetUpperBoundValue() + gap_shift );

                section_after_gap->MoveSection( -iMoveResult.mInitialGapSize );
            }
        }
        else if( ranges.Num() >= 2 && ranges[1].Contains( iMoveResult.mReferenceFrame ) )
        {
            // Move every 'after gap' sections before (and including) the referenced one
            for( auto section_after_gap : iMoveResult.mSectionsAfterGap )
            {
                FFrameNumber gap_shift = UE::MovieScene::DiscreteSize( section_after_gap->GetTrueRange() );
                *ioLastGapMove = TRange<FFrameNumber>( ioLastGapMove->GetLowerBoundValue() + gap_shift, ioLastGapMove->GetUpperBoundValue() + gap_shift );

                section_after_gap->MoveSection( -iMoveResult.mInitialGapSize );

                if( section_after_gap == section_containing_reference_frame )
                    break;
            }
        }
    }
    // If the reference frame is at the left of the gap,
    // process only every sections 'BeforeGap'
    else if( iMoveResult.mReferenceFrame < iMoveResult.mInitialGapMiddle )
    {
        // Find the section (if any) corresponding to the reference frame
        UMovieSceneSection* section_containing_reference_frame = nullptr;
        TArray<TRange<FFrameNumber>> ranges;
        for( auto section : iMoveResult.mSectionsBeforeGap )
        {
            if( section->IsTimeWithinSection( iMoveResult.mReferenceFrame ) )
            {
                section_containing_reference_frame = section;
                ranges = section->GetTrueRange().Split( ( section->GetInclusiveStartFrame() + section->GetExclusiveEndFrame() ) / 2 );
                break;
            }
        }

        if( ranges.Num() && ranges[0].Contains( iMoveResult.mReferenceFrame ) )
        {
            // Move every 'before gap' sections after (and including) the referenced one
            for( auto section_before_gap : iMoveResult.mSectionsBeforeGap )
            {
                FFrameNumber gap_shift = UE::MovieScene::DiscreteSize( section_before_gap->GetTrueRange() );
                *ioLastGapMove = TRange<FFrameNumber>( ioLastGapMove->GetLowerBoundValue() - gap_shift, ioLastGapMove->GetUpperBoundValue() - gap_shift );

                section_before_gap->MoveSection( iMoveResult.mInitialGapSize );

                if( section_before_gap == section_containing_reference_frame )
                    break;
            }
        }
        else if( ranges.Num() >= 2 && ranges[1].Contains( iMoveResult.mReferenceFrame ) )
        {
            // Move every 'before gap' sections after (and excluding) the referenced one
            for( auto section_before_gap : iMoveResult.mSectionsBeforeGap )
            {
                if( section_before_gap == section_containing_reference_frame )
                    break;

                FFrameNumber gap_shift = UE::MovieScene::DiscreteSize( section_before_gap->GetTrueRange() );
                *ioLastGapMove = TRange<FFrameNumber>( ioLastGapMove->GetLowerBoundValue() - gap_shift, ioLastGapMove->GetUpperBoundValue() - gap_shift );

                section_before_gap->MoveSection( iMoveResult.mInitialGapSize );
            }
        }
    }

    MovieSceneHelpers::SortConsecutiveSections( ioSections );
}

//static
void
SectionsHelpersMove::FixPostMoveSections( TArray< UMovieSceneSection* >& ioSections, TRange<FFrameNumber> iLastGapMove, UMovieSceneSection* iSection, FMoveResult iMoveResult )
{
    check( UE::MovieScene::DiscreteSize( iSection->GetTrueRange() ) == UE::MovieScene::DiscreteSize( iLastGapMove ) );

    iSection->SetRange( iLastGapMove );

    MovieSceneHelpers::SortConsecutiveSections( ioSections );
}
