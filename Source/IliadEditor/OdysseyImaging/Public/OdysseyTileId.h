// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"
#include "OdysseyTileId.generated.h"

/**
 * Opaque identity of a tile in FOdysseyTileManager: a slot index and the generation of that slot.
 * Generation distinguishes empty slots that have been reused. Only valid during an editor session:
 * UOdysseyTiledImage saves the tile data, not the ids.
 */
USTRUCT()
struct ODYSSEYIMAGING_API FOdysseyTileId
{
    GENERATED_BODY()

    /** The value of the fields of an invalid id. */
    static constexpr uint64 InvalidValue = MAX_uint64;

    bool IsValid() const { return Index != InvalidValue && Generation != InvalidValue; }

    UPROPERTY()
    uint64 Index = InvalidValue;

    UPROPERTY()
    uint64 Generation = InvalidValue;

    bool operator==(const FOdysseyTileId&) const = default;
};
