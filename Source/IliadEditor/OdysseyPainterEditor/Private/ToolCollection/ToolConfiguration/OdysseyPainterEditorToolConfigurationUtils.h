// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "OdysseyPainterEditorToolConfigurationUtils.generated.h"

USTRUCT()
struct FToolIntValue
{
    GENERATED_BODY()

    UPROPERTY()
    int64 Value;
};

USTRUCT()
struct FToolEnumValue
{
    GENERATED_BODY()

    UPROPERTY()
    uint64 Value;
};

USTRUCT()
struct FToolFloatValue
{
    GENERATED_BODY()

    UPROPERTY()
    float Value;
};
