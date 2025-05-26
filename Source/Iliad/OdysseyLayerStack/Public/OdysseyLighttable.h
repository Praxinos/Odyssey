// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OdysseyRenderingAbility.h"

#include "OdysseyLighttable.generated.h"

USTRUCT(BlueprintType)
struct FOdysseyLighttableKey
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Lighttable")
    bool bIsActivated = false;

    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Lighttable"
                , meta = ( ClampMin = "0"
                , UIMin    = "0"
                , ClampMax = "100"
                , UIMax    = "100"
                , Delta = "1"
                , Units = "Percent"))
    float Opacity = 0.f;
};

UENUM(BlueprintType)
enum class EOdysseyLighttableDisplayPosition : uint8
{
    AboveLayer,
    UnderLayer
};

USTRUCT(BlueprintType)
struct FOdysseyLighttable
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Lighttable")
    bool bIsActivated = false;

    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Lighttable")
    EOdysseyLighttableDisplayPosition DisplayPosition = EOdysseyLighttableDisplayPosition::UnderLayer;

    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Lighttable")
    FLinearColor PreviousKeysColor = FColor::Orange;

    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Lighttable")
    FLinearColor NextKeysColor = FColor(0, 128, 255);

    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Lighttable"
                , meta = ( ClampMin = "0"
                , UIMin    = "0"
                , ClampMax = "100"
                , UIMax    = "100"
                , Delta = "1"
                , Units = "Percent"))
    float PreviousKeysContrast = 50.f;

    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Lighttable"
                , meta = ( ClampMin = "0"
                , UIMin    = "0"
                , ClampMax = "100"
                , UIMax    = "100"
                , Delta = "1"
                , Units = "Percent"))
    float NextKeysContrast = 50.f;

    UPROPERTY() //Static array cannot be exposed to blueprint Class (Use UOdysseyLighttableFunctionLibrary::GetPreviousKey())
    FOdysseyLighttableKey PreviousKeys[10];

    UPROPERTY() //Static array cannot be exposed to blueprint Class (Use UOdysseyLighttableFunctionLibrary::GetPreviousKey())
    FOdysseyLighttableKey NextKeys[10];
};

UCLASS()
class UOdysseyLighttableFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category="Odyssey|Lighttable")
    static FOdysseyLighttableKey GetPreviousKey(FOdysseyLighttable Lighttable, int Index = 0);

    UFUNCTION(BlueprintPure, Category="Odyssey|Lighttable")
    static FOdysseyLighttableKey GetNextKey(FOdysseyLighttable Lighttable, int Index = 0);

    UFUNCTION(BlueprintPure, Category="Odyssey|Lighttable")
    static FOdysseyLighttable SetPreviousKey(FOdysseyLighttable Lighttable, FOdysseyLighttableKey Key, int Index = 0);

    UFUNCTION(BlueprintPure, Category="Odyssey|Lighttable")
    static FOdysseyLighttable SetNextKey(FOdysseyLighttable Lighttable, FOdysseyLighttableKey Key, int Index = 0);
};
