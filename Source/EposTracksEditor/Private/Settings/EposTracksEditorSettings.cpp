// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "Settings/EposTracksEditorSettings.h"

UEposTracksEditorSettings::UEposTracksEditorSettings( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
{
    BoardTrackSettings.ArrangeShots = EArrangeSections::OnTwoRowsShifted;
}
