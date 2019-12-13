// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreTypes.h"
#include "UObject/ObjectMacros.h"
#include "OdysseyBrushCache.generated.h"

/////////////////////////////////////////////////////
// Cache Enum
UENUM( BlueprintType )
enum  class  ECacheLevel : uint8
{
    kStep       UMETA( DisplayName="Step" ),
    kSubstroke  UMETA( DisplayName="Substroke" ),
    kStroke     UMETA( DisplayName="Stroke" ),
    kState      UMETA( DisplayName="State" ),
    kSuper      UMETA( DisplayName="Super" ),
};

#define  ODYSSEY_BRUSH_CACHE_OPERATION_START( iLevel, iName )   \
    if( brush->KeyExistsInPool( iLevel, iName ) )               \
    {                                                           \
        return  brush->RetrieveInPool( iLevel, iName );         \
    }                                                           \
    else                                                        \
    {

#define  ODYSSEY_BRUSH_CACHE_OPERATION_END }
