// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2025

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BlueprintFileUtilsBPLibrary2.generated.h"

UCLASS()
class UBlueprintFileUtilsBPLibrary2
    : public UBlueprintFunctionLibrary
{
    GENERATED_UCLASS_BODY()

    /**
     * Create a file from the content.
     */
    UFUNCTION(BlueprintCallable, Category = "FileUtils")
    static bool ToFile( const FString& Content, const FFilePath& PathFile );
};
