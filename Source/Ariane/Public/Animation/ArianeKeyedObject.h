// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal headers
#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
// Ariane Headers

#include "ArianeKeyedObject.generated.h"

struct FArianeObject;

USTRUCT(BlueprintType)
struct ARIANE_API FArianeKeyedObject
{
GENERATED_BODY()

public:
    virtual ~FArianeKeyedObject();
    FArianeKeyedObject();
    FArianeKeyedObject( FArianeObject* Object );
    virtual void PostLoad();
    virtual void PostEditUndo();
    const FGuid& GetGuid() const;

protected:
    UPROPERTY()
    FGuid Guid;
};
