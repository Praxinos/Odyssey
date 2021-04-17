// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "CinematicBoardTrack/MovieSceneCinematicBoardTrackHelpers.h"

#include "MovieSceneTrack.h"
#include "Settings/EposTracksSettings.h"

//---

//static
void
MovieSceneCinematicBoardTrackHelpers::Arrange( TArray< UMovieSceneSection* > ioSections, EArrangeSections iArrangeShots )
{
    if( !ioSections.Num() )
        return;

    if( iArrangeShots == EArrangeSections::Manually )
    {
    }
    else if( iArrangeShots == EArrangeSections::OnOneRow )
    {
        for( auto section : ioSections )
        {
            section->Modify();
            section->SetRowIndex( 0 );
        }
    }
    else if( iArrangeShots == EArrangeSections::OnTwoRowsShifted )
    {
        int start = ioSections[0]->GetRowIndex();
        for( int i = 0; i < ioSections.Num(); i++ )
        {
            auto section = ioSections[i];
            section->Modify();

            section->SetRowIndex( ( start + i ) % 2 );
        }
    }
}

//static
void
MovieSceneCinematicBoardTrackHelpers::Arrange( TArray< UMovieSceneSection* > ioSections )
{
    const UEposTracksSettings* settings = GetDefault<UEposTracksSettings>();
    Arrange( ioSections, settings->BoardTrackSettings.ArrangeShots );
}

//---

//static
void
MovieSceneCinematicBoardTrackHelpers::OrganizeSections( TArray< UMovieSceneSection* > iSections )
{
    UMovieSceneSection* first_section = nullptr;
    if( iSections.Num() )
    {
        first_section = iSections[0];
        first_section->MoveSection( -first_section->GetInclusiveStartFrame() );
    }

    UMovieSceneSection* previous_section = nullptr;
    for( auto section : iSections )
    {
        if( section == first_section )
        {
            previous_section = section;
            continue;
        }

        FFrameNumber offset = section->GetInclusiveStartFrame() - previous_section->GetExclusiveEndFrame();

        section->MoveSection( -offset );

        //---

        previous_section = section;
    }
}
