// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Helpers/SectionsHelpersResize.h"

#include "MovieSceneCommonHelpers.h"
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

    // This don't move leading edge further the previous section start
    // Otherwise it can raise some problem, for example if we want to move the start of the last section to 0
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

    // This would be useful if we DON'T want to move trailing edge further the next section end
    // But as all next sections will be pushed, it's not a problem to set a big new end frame
    //int32 next_index = current_index + 1;
    //if( iSections.IsValidIndex( next_index ) )
    //{
    //    UMovieSceneSection* next_section = iSections[next_index];
    //    FFrameNumber maxFrame = next_section->GetExclusiveEndFrame() - iThreshold;
    //    iNewFrame = FMath::Min( iNewFrame, maxFrame );
    //}

    return TRange<FFrameNumber>( iSection->GetRange().GetLowerBound(), TRangeBound<FFrameNumber>::Exclusive( iNewFrame ) );
}


//static
UMovieSceneSection*
SectionsHelpersResize::FixupConsecutiveSections( TArray<UMovieSceneSection*> iSections, UMovieSceneSection* iSection )
{
    MovieSceneHelpers::SortConsecutiveSections( iSections );

    int32 current_index = INDEX_NONE;
    if( !iSections.Find( iSection, current_index ) )
        return nullptr;

    UMovieSceneSection* previous_section = nullptr;
    UMovieSceneSection* next_section = nullptr;

    if( iSections.IsValidIndex( current_index - 1 ) )
        previous_section = iSections[current_index - 1];
    if( iSections.IsValidIndex( current_index + 1 ) )
        next_section = iSections[current_index + 1];

    if( previous_section && previous_section->GetExclusiveEndFrame() != iSection->GetInclusiveStartFrame() )
    {
        auto previous_new_range = TRange<FFrameNumber>( previous_section->GetRange().GetLowerBound(), TRangeBound<FFrameNumber>::Exclusive( iSection->GetInclusiveStartFrame() ) );
        auto previous_board_section = Cast<UMovieSceneCinematicBoardSection>( previous_section );
        previous_board_section->SetRange( previous_new_range );
    }

    if( next_section )
    {
        FFrameNumber diff = iSection->GetExclusiveEndFrame() - next_section->GetInclusiveStartFrame();
        for( int i = current_index + 1; i < iSections.Num(); i++ )
        {
            auto section = iSections[i];
            section->MoveSection( diff );
        }
    }

    return previous_section;
}
