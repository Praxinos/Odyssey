// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "OdysseyTileManagerBlueprintLibrary.generated.h"

UCLASS(BlueprintType)
class UOdysseyTileManagerBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

    UFUNCTION(BlueprintPure, Category = "Odyssey | Stats")
    static int64 GetTileManagerNumTilesUncompressed();

    UFUNCTION(BlueprintPure, Category = "Odyssey | Stats")
    static int64 GetTileManagerSizeUncompressed();

    UFUNCTION(BlueprintPure, Category = "Odyssey | Stats")
    static int64 GetTileManagerNumTilesCompressed();

    UFUNCTION(BlueprintPure, Category = "Odyssey | Stats")
    static int64 GetTileManagerSizeCompressed();

    UFUNCTION(BlueprintPure, Category = "Odyssey | Stats")
    static int64 GetTileManagerNumTilesOnDisk();

    UFUNCTION(BlueprintPure, Category = "Odyssey | Stats")
    static int64 GetTileManagerSizeOnDisk();

    UFUNCTION(BlueprintCallable, Category = "Odyssey | Stats")
    static void EvictAllTiles();
};
