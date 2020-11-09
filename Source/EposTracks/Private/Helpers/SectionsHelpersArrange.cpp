// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Helpers/SectionsHelpersArrange.h"

#include "MovieSceneTrack.h"

//---

//static
void
SectionsHelpersArrange::Arrange( UMovieSceneTrack* iTrack, EArrangeSections iArrangeShots )
{
    auto sections = iTrack->GetAllSections();

    for( int i = 0; i < sections.Num(); i++ )
    {
        auto section = sections[i];

        section->Modify();

        switch( iArrangeShots )
        {
            case EArrangeSections::OnOneRow:
                section->SetRowIndex( 0 );
                break;
            case EArrangeSections::OnTwoRowsShifted:
                section->SetRowIndex( i % 2 );
                break;
            default:
                break;
        }
    }
}
