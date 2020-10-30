// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "Settings/EposEditorSettings.h"

UEposEditorSettings::UEposEditorSettings( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
{
    BoardTrackSettings.ArrangeShots = EArrangeSections::OnTwoRowsShifted;
}
