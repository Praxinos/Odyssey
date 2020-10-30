// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#pragma once

#include "CoreMinimal.h"

UENUM()
enum class EArrangeSections : uint8
{
    OnOneRow            UMETA( DisplayName = "On a single row" ),
    OnTwoRowsShifted    UMETA( DisplayName = "On 2 rows shifted" ),
};
