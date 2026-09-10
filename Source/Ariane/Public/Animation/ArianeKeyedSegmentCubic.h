// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal headers
#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
// Ariane Headers
#include "ArianeKeyedSegment.h"

#include "ArianeKeyedSegmentCubic.generated.h"

struct FArianeSegmentCubic;

USTRUCT(BlueprintType)
struct ARIANE_API FArianeKeyedSegmentCubic : public FArianeKeyedSegment
{
GENERATED_BODY()

public:
    virtual ~FArianeKeyedSegmentCubic();
    FArianeKeyedSegmentCubic();
    FArianeKeyedSegmentCubic( FArianeSegmentCubic* CubicSegment );
    const FVector& GetHandlePosition(uint32 Index) const;

protected:
    UPROPERTY()
    FVector HandlePositions[2];
};
