// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"

class FAssetPackageData;

/** A package whose raster content still uses the legacy storage. */
struct FOdysseyLegacyPackage
{
    /** The long package name, e.g. /Game/Folder/Asset. */
    FName PackageName;

    /** The size of the package file, in bytes. */
    int64 DiskSize = 0;
};

namespace Odyssey::TiledImageMigration
{
/** Returns the classes whose objects own raster content that may still use the legacy storage. */
TArray<const UClass*> GetRasterOwnerClasses();

/**
 * Returns true when the package holds an object of one of the owner classes, under its current class name or a redirected one.
 * @param OwnerClasses The classes owning raster content, see GetRasterOwnerClasses().
 */
bool HoldsRasterOwners(const FAssetPackageData& PackageData, TConstArrayView<const UClass*> OwnerClasses);

/**
 * Returns true when the package needs a conversion: it holds raster owners, see HoldsRasterOwners(),
 * and none of its assets has RasterStorageTag. Returns false when the asset registry does not know the package.
 * Reads the asset registry. Game thread.
 */
bool IsLegacyPackage(FName PackageName, TConstArrayView<const UClass*> OwnerClasses);

/**
 * Returns the packages of the asset registry that need a conversion, without loading them, as IsLegacyPackage() defines it.
 * Sorted by package name. Only reflects what the asset registry has gathered: call it once its initial scan is complete.
 * Game thread.
 */
TArray<FOdysseyLegacyPackage> FindLegacyPackages(TConstArrayView<const UClass*> OwnerClasses);
}
