// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Helpers/SectionsHelpersArrange.h"

#include "MovieSceneTrack.h"
#include "Settings/EposTracksSettings.h"

//---

//static
void
SectionsHelpersArrange::Arrange( const TArray< UMovieSceneSection* >& ioSections, EArrangeSections iArrangeShots )
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
SectionsHelpersArrange::Arrange( const TArray< UMovieSceneSection* >& ioSections )
{
    const UEposTracksSettings* settings = GetDefault<UEposTracksSettings>();
    Arrange( ioSections, settings->BoardTrackSettings.ArrangeShots );
}
