// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "CustomizationManager.h"

//---

FDelegateHandle
FCustomizationManager::Register( FOnArrangeSections iOnArrangeSections )
{
    mArrangeSections = iOnArrangeSections;

    return mArrangeSections.GetHandle();
}

void
FCustomizationManager::Unregister( FDelegateHandle iHandle )
{
    if( iHandle == mArrangeSections.GetHandle() )
        mArrangeSections.Unbind();
}

void
FCustomizationManager::ExecuteArrangeSections()
{
    mArrangeSections.ExecuteIfBound();
}
