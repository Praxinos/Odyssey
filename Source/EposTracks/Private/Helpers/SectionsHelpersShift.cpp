// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Helpers/SectionsHelpersShift.h"

#include "MovieSceneCommonHelpers.h"
#include "MovieSceneSection.h"
#include "MovieSceneSequence.h"
#include "MovieSceneTimeHelpers.h"

//---

//static
FShiftResult
SectionsHelpersShift::GetShiftInfo( TArray< UMovieSceneSection* > iSections, UMovieSceneSequence* iSequence, FFrameNumber iStartTime, TOptional<int32> iDuration )
{
    iStartTime = FMath::Max( FFrameNumber( 0 ), iStartTime );

    if( !iSections.Num() )
    {
        check( iDuration.IsSet() );

        FShiftResult shift_result;
        shift_result.mNewRange = TRange<FFrameNumber>( 0, 0 + iDuration.GetValue() );
        shift_result.mFillGap = true;
        shift_result.mGap = shift_result.mNewRange; // It doesn't go in the ShiftFollowingSections() loop
        return shift_result;
    }

    //---

    for( int i = 0; i < iSections.Num(); i++ )
    {
        UMovieSceneSection* current_section = iSections[i];
        TRange<FFrameNumber> current_section_range( current_section->GetTrueRange() );

        // If iStartTime is inside a section
        if( current_section->IsTimeWithinSection( iStartTime ) )
        {
            FFrameNumber duration = ( iDuration.IsSet() ? iDuration.GetValue() : UE::MovieScene::DiscreteSize( current_section_range ) );

            TArray<TRange<FFrameNumber>> ranges = current_section_range.Split( ( current_section_range.GetLowerBoundValue().Value + current_section_range.GetUpperBoundValue().Value ) / 2 );
            // Can't split the section -> return the section range
            if( ranges.Num() != 2 )
            {
                FShiftResult shift_result;
                shift_result.mNewRange = TRange<FFrameNumber>( current_section_range.GetLowerBoundValue(), current_section_range.GetLowerBoundValue() + duration );
                shift_result.mFillGap = false;
                return shift_result;
            }

            // If iStartTime is inside the first half -> return the section range
            if( ranges[0].Contains( iStartTime ) )
            {
                FShiftResult shift_result;
                shift_result.mNewRange = TRange<FFrameNumber>( current_section_range.GetLowerBoundValue(), current_section_range.GetLowerBoundValue() + duration );
                shift_result.mFillGap = false;
                return shift_result;
            }

            //if( i == iSections.Num() - 1 )
            //{
            //    FShiftResult shift_result;
            //    shift_result.mNewStartFrame = TRangeBound<FFrameNumber>::FlipInclusion( current_section_range.GetUpperBound() );
            //    return shift_result;
            //}

            // If iStartTime is inside the second half -> compute the range after the section
            FShiftResult shift_result;
            shift_result.mNewRange = TRange<FFrameNumber>( TRangeBound<FFrameNumber>::FlipInclusion( current_section_range.GetUpperBound() ).GetValue(), current_section_range.GetUpperBoundValue() + duration );
            shift_result.mFillGap = false;
            return shift_result;
        }
    }

    //---

    // Get the first and last range from all sections
    TRange<FFrameNumber> first_range = iSections[0]->GetTrueRange();
    TRange<FFrameNumber> last_range = iSections[0]->GetTrueRange();
    for( int i = 1; i < iSections.Num(); i++ )
    {
        TRange<FFrameNumber> section_range = iSections[i]->GetTrueRange();

        if( TRangeBound<FFrameNumber>::MinLower( first_range.GetLowerBound(), section_range.GetLowerBound() ) == section_range.GetLowerBound() )
            first_range = section_range;

        if( TRangeBound<FFrameNumber>::MaxUpper( last_range.GetUpperBound(), section_range.GetUpperBound() ) == section_range.GetUpperBound() )
            last_range = section_range;
    }

    TRangeBound<FFrameNumber> new_start_bound( iStartTime );

    // If iStartTime is outside and before the first range -> compute the range before the first section (and fill arbitrary the gap)
    if( TRangeBound<FFrameNumber>::MinLower( new_start_bound, first_range.GetLowerBound() ) == new_start_bound )
    {
        FShiftResult shift_result;
        FFrameNumber duration = ( iDuration.IsSet() ? iDuration.GetValue() : UE::MovieScene::DiscreteSize( first_range ) );
        shift_result.mNewRange = TRange<FFrameNumber>( first_range.GetLowerBound().GetValue() - duration, TRangeBound<FFrameNumber>::FlipInclusion( first_range.GetLowerBound() ).GetValue() );
        shift_result.mFillGap = true;
        shift_result.mGap = TRange<FFrameNumber>( shift_result.mNewRange.GetLowerBound(), first_range.GetLowerBound().GetValue() );
        return shift_result;
    }

    // If iStartTime is outside and after the last range -> compute the range after the last section (and fill arbitrary the gap)
    if( TRangeBound<FFrameNumber>::MaxUpper( new_start_bound, TRangeBound<FFrameNumber>::FlipInclusion( last_range.GetUpperBound() ) ) == new_start_bound )
    {
        FShiftResult shift_result;
        FFrameNumber duration = ( iDuration.IsSet() ? iDuration.GetValue() : UE::MovieScene::DiscreteSize( last_range ) );
        shift_result.mNewRange = TRange<FFrameNumber>( TRangeBound<FFrameNumber>::FlipInclusion( last_range.GetUpperBound() ).GetValue(), last_range.GetUpperBound().GetValue() + duration );
        shift_result.mFillGap = true;
        shift_result.mGap = TRange<FFrameNumber>( last_range.GetUpperBound().GetValue(), shift_result.mNewRange.GetUpperBound().GetValue() );
        return shift_result;
    }

    //---

    // If iStartTime is inside a gap delimited by 2 sections -> compute the range of the new section and store the gap (to be able to compute offset, if gap is bigger/smaller than the range)
    for( int i = 1; i < iSections.Num(); i++ )
    {
        UMovieSceneSection* previous_section = iSections[i - 1];
        UMovieSceneSection* next_section = iSections[i];
        TRange<FFrameNumber> gap_range( TRangeBound<FFrameNumber>::FlipInclusion( previous_section->GetTrueRange().GetUpperBound() ), TRangeBound<FFrameNumber>::FlipInclusion( next_section->GetTrueRange().GetLowerBound() ) );
        if( gap_range.Contains( iStartTime ) )
        {
            FShiftResult shift_result;
            FFrameNumber duration = ( iDuration.IsSet() ? iDuration.GetValue() : UE::MovieScene::DiscreteSize( previous_section->GetTrueRange() ) );
            shift_result.mNewRange = TRange<FFrameNumber>( gap_range.GetLowerBound().GetValue(), gap_range.GetLowerBound().GetValue() + duration );
            shift_result.mFillGap = true;
            shift_result.mGap = gap_range;
            return shift_result;
        }
    }

    check( false );
    return FShiftResult();
}

//static
void
SectionsHelpersShift::ShiftFollowingSections( TArray< UMovieSceneSection* > iSections, const UMovieSceneSection* iNewSection, FShiftResult iShiftResult )
{
    TArray< UMovieSceneSection* > sections_to_shift;
    for( auto section : iSections )
    {
        if( section == iNewSection )
            continue;

        if( TRangeBound<FFrameNumber>::MaxLower( iNewSection->GetTrueRange().GetLowerBound(), section->GetTrueRange().GetLowerBound() ) == section->GetTrueRange().GetLowerBound() )
            sections_to_shift.Add( section );
    }

    for( auto section : sections_to_shift )
    {
        // Move the following sections forward of the size of the new section
        FFrameNumber offset = UE::MovieScene::DiscreteSize( iNewSection->GetTrueRange() );
        // If the new section is filling a gap
        if( iShiftResult.mFillGap )
            // If the gap is smaller than the new section, adjust the shift offset
            offset -= UE::MovieScene::DiscreteSize( iShiftResult.mGap );
        // If the gap is bigger than the new section, do not modify the next section
        offset = FMath::Max( FFrameNumber( 0 ), offset );

        section->MoveSection( offset );
    }
}
