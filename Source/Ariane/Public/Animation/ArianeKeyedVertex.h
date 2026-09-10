// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal headers
#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
// Ariane Headers

#include "ArianeKeyedVertex.generated.h"

struct FArianeVertex;

USTRUCT(BlueprintType)
struct ARIANE_API FArianeKeyedVertex
{
GENERATED_BODY()

public:
    virtual ~FArianeKeyedVertex();
    FArianeKeyedVertex();
    FArianeKeyedVertex( FArianeVertex* Vertex );

    const FGuid& GetGuid() const;
    const FVector& GetPosition() const;
    double GetRadius() const;

protected:
    UPROPERTY()
    FGuid Guid;

    UPROPERTY()
    FVector Position;

    UPROPERTY()
    double Radius;
};
