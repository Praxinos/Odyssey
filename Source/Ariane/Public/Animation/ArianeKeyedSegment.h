// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal headers
#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
// Ariane Headers

#include "ArianeKeyedSegment.generated.h"

struct FArianeSegment;

USTRUCT(BlueprintType)
struct ARIANE_API FArianeKeyedSegment
{
GENERATED_BODY()

public:
    virtual ~FArianeKeyedSegment();
    FArianeKeyedSegment();
    FArianeKeyedSegment( FArianeSegment* Segment );

    const FGuid& GetGuid();

protected:
    UPROPERTY()
    FGuid Guid;
};
