// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

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
