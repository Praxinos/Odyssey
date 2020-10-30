// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "TracksCustomizationManager.h"

//---

FDelegateHandle
FTracksCustomizationManager::Register( FOnArrangeSections iOnArrangeSections )
{
    mArrangeSections = iOnArrangeSections;

    return mArrangeSections.GetHandle();
}

void
FTracksCustomizationManager::Unregister( FDelegateHandle iHandle )
{
    if( iHandle == mArrangeSections.GetHandle() )
        mArrangeSections.Unbind();
}

void
FTracksCustomizationManager::ExecuteArrangeSections()
{
    mArrangeSections.ExecuteIfBound();
}
