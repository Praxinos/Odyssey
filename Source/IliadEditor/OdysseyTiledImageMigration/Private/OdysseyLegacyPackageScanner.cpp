// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyLegacyPackageScanner.h"

#include "AssetRegistry/AssetData.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "UObject/CoreRedirects.h"

#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "LayerStack/OdysseyTextureLayerImageRaster.h"
#include "OdysseyTiledImageObjectVersion.h"

namespace UE::OdysseyTiledImageMigration::Private
{
/** Returns true when PackageData holds FOdysseyTiledImageObjectVersion. */
bool HasTiledImageVersion(const FAssetPackageData& PackageData)
{
    const TConstArrayView<UE::AssetRegistry::FPackageCustomVersion> Versions = PackageData.GetCustomVersions();
    for (const UE::AssetRegistry::FPackageCustomVersion& Version : Versions)
    {
        if (Version.Key == FOdysseyTiledImageObjectVersion::GUID)
        {
            return true;
        }
    }
    return false;
}

/** Returns true when the imported class, as named in the package file, is one of the owner classes once redirected. */
bool IsOwnerClass(FName ImportedClass, TConstArrayView<FTopLevelAssetPath> OwnerClasses)
{
    // The registry keeps the class names as saved: older assets name the classes before their redirects.
    const FString ImportedClassName = ImportedClass.ToString();
    const FCoreRedirectObjectName SavedName(ImportedClassName);
    const FCoreRedirectObjectName CurrentName = FCoreRedirects::GetRedirectedName(ECoreRedirectFlags::Type_Class, SavedName);
    const FTopLevelAssetPath CurrentPath(CurrentName.PackageName, CurrentName.ObjectName);
    return OwnerClasses.Contains(CurrentPath);
}

/**
 * Returns true when the package needs a conversion, as IsLegacyPackage() defines it.
 * @param InOutOwnerClassCache Whether each imported class name already met is an owner class: most packages share the
 *                             same few classes, whose redirects are then resolved once.
 */
bool IsLegacyPackageCached(const FAssetPackageData& PackageData, TConstArrayView<FTopLevelAssetPath> OwnerClasses, TMap<FName, bool>& InOutOwnerClassCache)
{
    if (HasTiledImageVersion(PackageData))
    {
        return false;
    }

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
}

namespace Odyssey::TiledImageMigration
{
TArray<FTopLevelAssetPath> GetRasterOwnerClasses()
{
    const UClass* const LayerClass = UOdysseyTextureLayerImageRaster::StaticClass();
    const UClass* const CellClass = UOdysseyAnimationCellImageRaster::StaticClass();
    const FTopLevelAssetPath LayerPath = LayerClass->GetClassPathName();
    const FTopLevelAssetPath CellPath = CellClass->GetClassPathName();
    return { LayerPath, CellPath };
}

bool IsLegacyPackage(const FAssetPackageData& PackageData, TConstArrayView<FTopLevelAssetPath> OwnerClasses)
{
    TMap<FName, bool> OwnerClassCache;
    return UE::OdysseyTiledImageMigration::Private::IsLegacyPackageCached(PackageData, OwnerClasses, OwnerClassCache);
}

TArray<FOdysseyLegacyPackage> FindLegacyPackages(TConstArrayView<FTopLevelAssetPath> OwnerClasses)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(Odyssey::TiledImageMigration::FindLegacyPackages);
    check(IsInGameThread());
    const IAssetRegistry& Registry = IAssetRegistry::GetChecked();
    TArray<FOdysseyLegacyPackage> Packages;
    TMap<FName, bool> OwnerClassCache;
    // Called within the registry lock: the test only reads the package data and the class redirects.
    Registry.EnumerateAllPackages([&Packages, &OwnerClassCache, OwnerClasses](FName PackageName, const FAssetPackageData& PackageData) -> void
    {
        if (UE::OdysseyTiledImageMigration::Private::IsLegacyPackageCached(PackageData, OwnerClasses, OwnerClassCache))
        {
            Packages.Add({ PackageName, PackageData.DiskSize });
        }
    });

    Packages.Sort([](const FOdysseyLegacyPackage& Left, const FOdysseyLegacyPackage& Right) -> bool
    {
        return Left.PackageName.LexicalLess(Right.PackageName);
    });
    return Packages;
}
}
