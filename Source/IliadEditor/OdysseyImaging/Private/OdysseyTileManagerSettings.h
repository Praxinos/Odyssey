// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "OdysseyTileManagerSettings.generated.h"

/**
 * The memory budgets of FOdysseyTileManager: per-user editor settings, in Editor Preferences > Plugins > Odyssey - Tile Manager.
 * Sizes are in MB of 1,000,000 bytes. Changing a budget applies it at once.
 */
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
    /** Maximum size of the uncompressed tile data kept in RAM, in MB; beyond it, the uncompressed buffers of the least recently used tiles are released and decompressed again when needed. */
    UPROPERTY(config, EditAnywhere, Category="Tile Manager", meta=(ClampMin = "1", UIMin = "1"))
    uint64 MaxUncompressedSize = 512;

    /** Maximum size of the compressed tile data kept in RAM, in MB; beyond it, the compressed buffers of the least recently used tiles already on disk are released and read again when needed. */
    UPROPERTY(config, EditAnywhere, Category="Tile Manager", meta=(ClampMin = "1", UIMin = "1"))
    uint64 MaxCompressedSize = 512;

    /** Maximum size of retained GPU atlases in MB. Zero disables caching; active renders and readbacks are additional. */
    UPROPERTY(config, EditAnywhere, Category="Tile Manager", meta=(ClampMin = "0", UIMin = "0"))
    uint64 MaxGPUSize = 256;
};
