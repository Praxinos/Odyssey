// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#pragma once

#include "CoreMinimal.h"

//---

DECLARE_DELEGATE( FOnArrangeSections );

class EPOSTRACKS_API FCustomizationManager
{
public:
    FDelegateHandle Register( FOnArrangeSections iOnArrangeSections );
    void Unregister( FDelegateHandle iHandle );

    void ExecuteArrangeSections();

private:
    FOnArrangeSections mArrangeSections;
};
