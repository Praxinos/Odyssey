// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"
#include "OdysseyTileId.generated.h"

/** Opaque tile identity. Generation distinguishes empty slots that have been reused. */
USTRUCT()
struct ODYSSEYIMAGING_API FOdysseyTileId
{
    GENERATED_BODY()

    bool IsValid() const { return Index != INDEX_NONE && Generation != INDEX_NONE; }

    UPROPERTY()
    uint64 Index = INDEX_NONE;

    UPROPERTY()
    uint64 Generation = INDEX_NONE;

    bool operator==(const FOdysseyTileId&) const = default;
};
