// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

//---

DECLARE_DELEGATE( FOnNotifySequencer );

class EPOSTRACKS_API FTracksCustomizationManager
{
public:
    FDelegateHandle Register( FOnNotifySequencer iOnNotifySequencer );
    void Unregister( FDelegateHandle iHandle );

    void PatchNotifySequencer();

private:
    FOnNotifySequencer mPatchNotifySequencer;
};
