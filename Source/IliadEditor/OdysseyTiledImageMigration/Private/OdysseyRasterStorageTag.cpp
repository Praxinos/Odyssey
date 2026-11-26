// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyRasterStorageTag.h"

#include "AssetRegistry/AssetData.h"

namespace Odyssey::TiledImageMigration
{
const FName RasterStorageTag(TEXT("OdysseyRasterStorage"));
const FStringView RasterStorageTiledImage = TEXTVIEW("TiledImage");

bool HasRasterStorageTag(TConstArrayView<FAssetData> PackageAssets)
{
    for (const FAssetData& Asset : PackageAssets)
    {
        if (Asset.FindTag(RasterStorageTag))
        {
            return true;
        }
    }
    return false;
}
}
