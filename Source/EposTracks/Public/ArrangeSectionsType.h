// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

UENUM()
enum class EArrangeSections : uint8
{
    Manually            UMETA( DisplayName = "Manually" ),
    OnOneRow            UMETA( DisplayName = "On a single row" ),
    OnTwoRowsShifted    UMETA( DisplayName = "On 2 rows shifted" ),
};
