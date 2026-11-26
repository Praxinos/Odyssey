// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"

struct FAssetData;

namespace Odyssey::TiledImageMigration
{
/**
 * The asset registry tag of the assets whose raster layers or cells store their content in tiled images.
 * An asset holding raster layers or cells without it was saved before the tiled image storage.
 */
extern const FName RasterStorageTag;

/** The value of RasterStorageTag: the storage of the raster content. */
extern const FStringView RasterStorageTiledImage;

/** Returns true when one of the assets of a package has RasterStorageTag. */
bool HasRasterStorageTag(TConstArrayView<FAssetData> PackageAssets);
}
