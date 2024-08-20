// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyImageRenderingAbility.h"

#include "OdysseyAnimationLightTable.generated.h"

USTRUCT(BlueprintType)
struct FOdysseyAnimationLightTableKey
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Odyssey|LightTable")
    bool bIsActivated = false;

	UPROPERTY(BlueprintReadWrite, Category="Odyssey|LightTable")
    float Opacity = 100.f; //TODO: UIMin = 0, UIMax = 100, Linear
};

UENUM(BlueprintType)
enum class EOdysseyLightTableDisplayPosition : uint8
{
    AboveLayer,
    UnderLayer
};

USTRUCT(BlueprintType)
struct FOdysseyAnimationLightTable
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Odyssey|LightTable")
    bool bIsActivated = false;

	UPROPERTY(BlueprintReadWrite, Category="Odyssey|LightTable")
    EOdysseyLightTableDisplayPosition DisplayPosition = EOdysseyLightTableDisplayPosition::UnderLayer;

	UPROPERTY(BlueprintReadWrite, Category="Odyssey|LightTable")
    FLinearColor PreviousKeysColor = FColor::Orange;

	UPROPERTY(BlueprintReadWrite, Category="Odyssey|LightTable")
    FLinearColor NextKeysColor = FColor(0, 128, 255);

	UPROPERTY(BlueprintReadWrite, Category="Odyssey|LightTable")
    float PreviousKeysContrast = 0.f; //TODO: UIMin = 0 UIMax = 100 and Linear

	UPROPERTY(BlueprintReadWrite, Category="Odyssey|LightTable")
    float NextKeysContrast = 0.f;  //TODO: UIMin = 0 UIMax = 100 and Linear

	UPROPERTY() //Static array cannot be exposed to blueprint Class (Use UOdysseyAnimationLighttableFunctionLibrary::GetPreviousKey())
    FOdysseyAnimationLightTableKey PreviousKeys[10];

	UPROPERTY() //Static array cannot be exposed to blueprint Class (Use UOdysseyAnimationLighttableFunctionLibrary::GetPreviousKey())
	FOdysseyAnimationLightTableKey NextKeys[10];
};

UCLASS()
class UOdysseyAnimationLighttableFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="Odyssey|LightTable")
	static FOdysseyAnimationLightTableKey GetPreviousKey(FOdysseyAnimationLightTable Lighttable, int Index = 0);

	UFUNCTION(BlueprintPure, Category="Odyssey|LightTable")
	static FOdysseyAnimationLightTableKey GetNextKey(FOdysseyAnimationLightTable Lighttable, int Index = 0);

	UFUNCTION(BlueprintPure, Category="Odyssey|LightTable")
	static FOdysseyAnimationLightTable SetPreviousKey(FOdysseyAnimationLightTable Lighttable, FOdysseyAnimationLightTableKey Key, int Index = 0);

	UFUNCTION(BlueprintPure, Category="Odyssey|LightTable")
	static FOdysseyAnimationLightTable SetNextKey(FOdysseyAnimationLightTable Lighttable, FOdysseyAnimationLightTableKey Key, int Index = 0);
};
