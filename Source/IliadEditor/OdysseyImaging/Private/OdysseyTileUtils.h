// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

namespace Odyssey::TileUtils {

    inline constexpr uint32 MaxAtlasDimensionInTiles = 64;

    /** Every supported tile size must fit a full atlas within the RHI texture dimension limit. */
    bool IsValidTileSize(int64 InTileSize);

    TArray<FIntPoint> GetTilePositionsFromRect(int32 InTileSize, const FIntRect& InRect);

}
