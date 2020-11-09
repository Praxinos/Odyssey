// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Helpers/SectionsHelpersResize.h"

#include "MovieSceneSection.h"

//---

//static
TRange<FFrameNumber>
SectionsHelpersResize::GetValidRangeLeading( TArray<UMovieSceneSection*> iSections, UMovieSceneSection* iSection, FFrameNumber iNewFrame, int32 iThreshold )
{
    int32 current_index = INDEX_NONE;
    if( !iSections.Find( iSection, current_index ) )
        return iSection->GetTrueRange();

    FFrameNumber maxFrame = iSection->GetExclusiveEndFrame() - iThreshold;
    iNewFrame = FMath::Min( iNewFrame, maxFrame );

    int32 previous_index = current_index - 1;
    if( iSections.IsValidIndex( previous_index ) )
    {
        UMovieSceneSection* previous_section = iSections[previous_index];
        FFrameNumber minFrame = previous_section->GetInclusiveStartFrame() + iThreshold;
        iNewFrame = FMath::Max( minFrame, iNewFrame );
    }

    if( current_index == 0 )
        iNewFrame = 0;

    return TRange<FFrameNumber>( TRangeBound<FFrameNumber>::Inclusive( iNewFrame ), iSection->GetRange().GetUpperBound() );
}

//static
TRange<FFrameNumber>
SectionsHelpersResize::GetValidRangeTrailing( TArray<UMovieSceneSection*> iSections, UMovieSceneSection* iSection, FFrameNumber iNewFrame, int32 iThreshold )
{
    int32 current_index = INDEX_NONE;
    if( !iSections.Find( iSection, current_index ) )
        return iSection->GetTrueRange();

    FFrameNumber minFrame = iSection->GetInclusiveStartFrame() + iThreshold;
    iNewFrame = FMath::Max( iNewFrame, minFrame );

    int32 next_index = current_index + 1;
    if( iSections.IsValidIndex( next_index ) )
    {
        UMovieSceneSection* next_section = iSections[next_index];
        FFrameNumber maxFrame = next_section->GetExclusiveEndFrame() - iThreshold;
        iNewFrame = FMath::Min( iNewFrame, maxFrame );
    }

    return TRange<FFrameNumber>( iSection->GetRange().GetLowerBound(), TRangeBound<FFrameNumber>::Exclusive( iNewFrame ) );
}
