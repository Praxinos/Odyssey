// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "TracksCustomizationManager.h"

//---

FDelegateHandle
FTracksCustomizationManager::Register( FOnNotifySequencer iOnNotifySequencer )
{
    mPatchNotifySequencer = iOnNotifySequencer;

    return mPatchNotifySequencer.GetHandle();
}

void
FTracksCustomizationManager::Unregister( FDelegateHandle iHandle )
{
    if( iHandle == mPatchNotifySequencer.GetHandle() )
        mPatchNotifySequencer.Unbind();
}

void
FTracksCustomizationManager::PatchNotifySequencer()
{
    mPatchNotifySequencer.ExecuteIfBound();
}
