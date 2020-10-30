// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#pragma once

#include "CoreMinimal.h"

UENUM()
enum class EArrangeSections : uint8
{
                        // Check if/when this DisplayName is really necessary as the corresponding command is used in the popup/settings/shortcuts/... maybe in BP ?
    Manually            /*UMETA( DisplayName = "Manual" )*/,
    OnOneRow            /*UMETA( DisplayName = "On a single row" )*/,
    OnTwoRowsShifted    /*UMETA( DisplayName = "On 2 rows shifted" )*/,
};
