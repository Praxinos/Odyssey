// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Settings/EposTracksEditorSettings.h"

UEposTracksEditorSettings::UEposTracksEditorSettings( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
{
    BoardTrackSettings.ArrangeShots = EArrangeSections::OnTwoRowsShifted;
}
