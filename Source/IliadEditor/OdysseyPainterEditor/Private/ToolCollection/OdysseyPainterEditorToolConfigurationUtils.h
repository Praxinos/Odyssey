// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "OdysseyPainterEditorToolConfigurationUtils.generated.h"

USTRUCT()
struct FToolEnumValue
{
    GENERATED_BODY()

    UPROPERTY()
    uint64 Value = 0; // Generally, it's stored as a uint8, but just to be sure we use a bigger size
};

USTRUCT()
struct FToolBoolValue
{
    GENERATED_BODY()

    UPROPERTY()
    bool Value = false;
};

USTRUCT()
struct FToolIntValue
{
    GENERATED_BODY()

    UPROPERTY()
    int64 Value = 0;
};

USTRUCT()
struct FToolFloatValue
{
    GENERATED_BODY()

    UPROPERTY()
    float Value = 0.f;
};

USTRUCT()
struct FToolDoubleValue
{
    GENERATED_BODY()

    UPROPERTY()
    double Value = 0.f;
};

USTRUCT()
struct FToolObjectValue
{
    GENERATED_BODY()

    UPROPERTY()
    TSoftObjectPtr<UObject> Value;
};
