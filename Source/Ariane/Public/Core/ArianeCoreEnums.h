// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "ArianeCoreEnums.generated.h"

UENUM()
enum class EArianeAllocationModel : uint8
{
    InstancedStruct = 0, // allocated via FInstancedStruct
    OperatingSystem = 1 // allocated via new
};
