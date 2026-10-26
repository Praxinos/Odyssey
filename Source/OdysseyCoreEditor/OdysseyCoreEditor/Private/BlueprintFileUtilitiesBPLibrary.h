// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2025

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "BlueprintFileUtilitiesBPLibrary.generated.h"

UCLASS()
class UBlueprintFileUtilitiesBPLibrary
    : public UBlueprintFunctionLibrary
{
    GENERATED_UCLASS_BODY()

    /**
     * Unzip peg (or compressed) file.
     */
    UFUNCTION( BlueprintCallable, Category = "FileUtils" )
    static bool Unzip( const FFilePath& CompressedPathFile, FDirectoryPath& UncompressedPath );
};
