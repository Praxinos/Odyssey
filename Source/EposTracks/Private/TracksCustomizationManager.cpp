// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

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
