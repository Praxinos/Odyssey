// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "Settings/EposSequencerSettings.h"

//---

UEposSequencerSettings::UEposSequencerSettings()
    : Super()
    , ArrangeShots( EArrangeShots::OnTwoRowsShifted )
{
}

EArrangeShots
UEposSequencerSettings::GetArrangeShots() const
{
    return ArrangeShots;
}

void
UEposSequencerSettings::SetArrangeShots( EArrangeShots iArrangeShots )
{
    if( ArrangeShots == iArrangeShots )
        return;

    ArrangeShots = iArrangeShots;
    SaveConfig();
}
