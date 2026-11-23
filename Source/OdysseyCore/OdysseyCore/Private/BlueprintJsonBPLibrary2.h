// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2025

#pragma once

#include "CoreMinimal.h"

#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "BlueprintJSonBPLibrary2.generated.h"

struct FJsonObjectWrapper;

USTRUCT( BlueprintType )
struct FJsonFloatArray
{
    GENERATED_BODY()

    UPROPERTY( BlueprintReadWrite, EditAnywhere, Category = "Odyssey|Json" )
    TArray<float> Values;
};

UCLASS()
class UBlueprintJsonBPLibrary2
    : public UBlueprintFunctionLibrary
{
    GENERATED_UCLASS_BODY()

    /**
     * Convert a json field to an array of array of float.
     *
     * Info: this function with those parameters is the easiest way to manage this array of array of float.
     * An (little) improvement would be to have multiple functions to manage each basic type: int, bool, string, ...
     * A more robust improvement would be to manage array and return a generic JsonValue, but JsonValue is not exposed at all to BP,
     * only JsonObject (aka "name": value" pairs) are exposed to BP via JsonObjectWrapper
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Json")
    static bool GetFieldAsArrayArrayFloat( const FJsonObjectWrapper& JsonObject, const FString& FieldName, UPARAM( DisplayName = "Value" ) TArray<FJsonFloatArray>& OutValue );
};
