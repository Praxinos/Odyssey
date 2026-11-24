// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

namespace Odyssey::TileUtils {

    inline constexpr uint32 MaxAtlasDimensionInTiles = 64;

    /**
     * The size of the atlases the GPU cache repacks its tiles into, in bytes: large enough to batch many tiles,
     * small enough to release memory page by page when the budget shrinks.
     */
    inline constexpr uint64 GPUCachePageBytes = 4 * 1024 * 1024;

    /** Every supported tile size must fit a full atlas within the RHI texture dimension limit. */
    bool IsValidTileSize(int64 InTileSize);

    TArray<FIntPoint> GetTileCoordsInRect(int32 InTileSize, const FIntRect& InRect);

    /** Returns the rects clipped to Bounds, without those left empty. */
    TArray<FIntRect> ClipRects(const TArray<FIntRect>& InRects, const FIntRect& InBounds);

    /** Returns the union of the rects; an empty rect when there are none. */
    FIntRect UnionRects(const TArray<FIntRect>& InRects);

}
