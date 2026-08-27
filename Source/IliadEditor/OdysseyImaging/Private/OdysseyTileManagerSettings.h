// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "OdysseyTileManagerSettings.generated.h"

UCLASS(config=EditorPerProjectUserSettings)
class UOdysseyTileManagerSettings
    : public UObject
{
    GENERATED_BODY()

public:
    static UOdysseyTileManagerSettings* Get();

public:
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;

public:
    /**
     * Maximum Size of Uncompressed Tile to keep in RAM memory (in MB)
    */
    UPROPERTY(config, EditAnywhere, Category="Tile Manager", meta=(ClampMin = "1", UIMin = "1"))
    uint64 MaxUncompressedSize = 512;

    /**
     * Maximum Size of Compressed Tile to keep in RAM memory (in MB)
    */
    UPROPERTY(config, EditAnywhere, Category="Tile Manager", meta=(ClampMin = "1", UIMin = "1"))
    uint64 MaxCompressedSize = 512;
};
