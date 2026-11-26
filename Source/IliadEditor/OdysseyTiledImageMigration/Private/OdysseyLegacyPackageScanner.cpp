// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyLegacyPackageScanner.h"

#include "AssetRegistry/AssetData.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "UObject/CoreRedirects.h"

#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "LayerStack/OdysseyTextureLayerImageRaster.h"
#include "OdysseyRasterStorageTag.h"
#include "OdysseyTiledImageMigrationLog.h"

namespace UE::OdysseyTiledImageMigration::Private
{
/** Returns true when the imported class, as named in the package file, is one of the owner classes once redirected. */
bool IsOwnerClass(FName ImportedClass, TConstArrayView<const UClass*> OwnerClasses)
{
    // The registry keeps the class names as saved: older assets name the classes before their redirects.
    const FString ImportedClassName = ImportedClass.ToString();
    const FCoreRedirectObjectName SavedName(ImportedClassName);
    const FCoreRedirectObjectName CurrentName = FCoreRedirects::GetRedirectedName(ECoreRedirectFlags::Type_Class, SavedName);
    const FTopLevelAssetPath CurrentPath(CurrentName.PackageName, CurrentName.ObjectName);
    for (const UClass* OwnerClass : OwnerClasses)
    {
        if (OwnerClass->GetClassPathName() == CurrentPath)
        {
            return true;
        }
    }
    return false;
}

/**
 * Returns true when the package holds raster owners, as HoldsRasterOwners() defines it.
 * @param InOutOwnerClassCache Whether each imported class name already met is an owner class: most packages share the
 *                             same few classes, whose redirects are then resolved once.
 */
bool HoldsRasterOwnersCached(const FAssetPackageData& PackageData, TConstArrayView<const UClass*> OwnerClasses, TMap<FName, bool>& InOutOwnerClassCache)
{
    for (const FName ImportedClass : PackageData.ImportedClasses)
    {
        bool* const bCachedOwner = InOutOwnerClassCache.Find(ImportedClass);
        bool bOwner = false;
        if (bCachedOwner)
        {
            bOwner = *bCachedOwner;
        }
        else
        {
            bOwner = IsOwnerClass(ImportedClass, OwnerClasses);
            InOutOwnerClassCache.Add(ImportedClass, bOwner);
        }

        if (bOwner)
        {
            return true;
        }
    }
    return false;
}

/** Returns true when none of the assets of the package has RasterStorageTag. */
bool LacksRasterStorageTag(const IAssetRegistry& Registry, FName PackageName)
{
    TArray<FAssetData> Assets;
    Registry.GetAssetsByPackageName(PackageName, Assets, /*bIncludeOnlyOnDiskAssets*/ true);
    return !Odyssey::TiledImageMigration::HasRasterStorageTag(Assets);
}
}

namespace Odyssey::TiledImageMigration
{
TArray<const UClass*> GetRasterOwnerClasses()
{
    const UClass* const LayerClass = UOdysseyTextureLayerImageRaster::StaticClass();
    const UClass* const CellClass = UOdysseyAnimationCellImageRaster::StaticClass();
    return { LayerClass, CellClass };
}

bool HoldsRasterOwners(const FAssetPackageData& PackageData, TConstArrayView<const UClass*> OwnerClasses)
{
    TMap<FName, bool> OwnerClassCache;
    return UE::OdysseyTiledImageMigration::Private::HoldsRasterOwnersCached(PackageData, OwnerClasses, OwnerClassCache);
}

bool IsLegacyPackage(FName PackageName, TConstArrayView<const UClass*> OwnerClasses)
{
    check(IsInGameThread());
    const IAssetRegistry& Registry = IAssetRegistry::GetChecked();
    const TOptional<FAssetPackageData> PackageData = Registry.GetAssetPackageDataCopy(PackageName);
    if (!PackageData)
    {
        UE_LOGF(LogOdysseyTiledImageMigration, Warning, "%ls is unknown to the asset registry.", *PackageName.ToString());
        return false;
    }

    if (!HoldsRasterOwners(*PackageData, OwnerClasses))
    {
        return false;
    }
    return UE::OdysseyTiledImageMigration::Private::LacksRasterStorageTag(Registry, PackageName);
}

TArray<FOdysseyLegacyPackage> FindLegacyPackages(TConstArrayView<const UClass*> OwnerClasses)
{
    using namespace UE::OdysseyTiledImageMigration::Private;
    TRACE_CPUPROFILER_EVENT_SCOPE(Odyssey::TiledImageMigration::FindLegacyPackages);
    check(IsInGameThread());
    const IAssetRegistry& Registry = IAssetRegistry::GetChecked();
    TArray<FOdysseyLegacyPackage> OwnerPackages;
    TMap<FName, bool> OwnerClassCache;
    // Called within the registry lock, which forbids reading the tags of the assets: they are read once the enumeration ends.
    Registry.EnumerateAllPackages([&OwnerPackages, &OwnerClassCache, OwnerClasses](FName PackageName, const FAssetPackageData& PackageData) -> void
    {
        if (HoldsRasterOwnersCached(PackageData, OwnerClasses, OwnerClassCache))
        {
            OwnerPackages.Add({ PackageName, PackageData.DiskSize });
        }
    });

    TArray<FOdysseyLegacyPackage> Packages;
    Packages.Reserve(OwnerPackages.Num());
    for (const FOdysseyLegacyPackage& Package : OwnerPackages)
    {
        if (LacksRasterStorageTag(Registry, Package.PackageName))
        {
            Packages.Add(Package);
        }
    }

    Packages.Sort([](const FOdysseyLegacyPackage& Left, const FOdysseyLegacyPackage& Right) -> bool
    {
        return Left.PackageName.LexicalLess(Right.PackageName);
    });
    return Packages;
}
}
