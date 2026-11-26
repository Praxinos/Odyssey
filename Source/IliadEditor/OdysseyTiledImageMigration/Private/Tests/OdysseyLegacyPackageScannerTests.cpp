// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "AssetRegistry/AssetData.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "Misc/AutomationTest.h"

#include "OdysseyLegacyAssetGroup.h"
#include "OdysseyLegacyPackageScanner.h"
#include "OdysseyRasterStorageTag.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace UE::OdysseyTiledImageMigration::Tests
{
/** The folder the test scans synchronously. */
constexpr FStringView StampsPath = TEXTVIEW("/Odyssey/Brushes/_Common/Stamps/RGBA16/Penbrush");
/** The legacy brush stamps shipped with the plugin, used as fixtures. */
const FName LegacyStampA(TEXT("/Odyssey/Brushes/_Common/Stamps/RGBA16/Penbrush/T_AAcircle_1024"));
const FName LegacyStampB(TEXT("/Odyssey/Brushes/_Common/Stamps/RGBA16/Penbrush/T_noAACircle_1024"));
/** A plain texture of the same folder, without raster layers. */
const FName PlainTexture(TEXT("/Odyssey/Brushes/_Common/Stamps/RGBA16/Penbrush/T_AAcircle_0001"));

/** Returns package data importing the given classes. */
[[nodiscard]] FAssetPackageData CreatePackageData(TConstArrayView<const TCHAR*> ImportedClasses)
{
    FAssetPackageData PackageData;
    PackageData.ImportedClasses.Reserve(ImportedClasses.Num());
    for (const TCHAR* ImportedClass : ImportedClasses)
    {
        PackageData.ImportedClasses.Add(FName(ImportedClass));
    }
    return PackageData;
}

/** Returns the data of a texture asset, with the given tags. */
[[nodiscard]] FAssetData CreateAssetData(FAssetDataTagMap Tags)
{
    const FTopLevelAssetPath TextureClass(TEXT("/Script/Engine"), TEXT("Texture2D"));
    const FName PackageName(TEXT("/Game/Test/T_Asset"));
    const FName PackagePath(TEXT("/Game/Test"));
    const FName AssetName(TEXT("T_Asset"));
    return FAssetData(PackageName, PackagePath, AssetName, TextureClass, MoveTemp(Tags));
}

/** Returns true when the packages hold the given package name. */
bool ContainsPackage(TConstArrayView<FOdysseyLegacyPackage> Packages, FName PackageName)
{
    return Packages.ContainsByPredicate([PackageName](const FOdysseyLegacyPackage& Package) -> bool
    {
        return Package.PackageName == PackageName;
    });
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyHoldsRasterOwnersTest, "Odyssey.TiledImageMigration.HoldsRasterOwners", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** A package holds raster owners when it imports a raster layer or cell class, under its current or its redirected name. */
bool FOdysseyHoldsRasterOwnersTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TiledImageMigration;
    using namespace UE::OdysseyTiledImageMigration::Tests;
    const TArray<const UClass*> OwnerClasses = GetRasterOwnerClasses();

    const FAssetPackageData Layer = CreatePackageData({ TEXT("/Script/Engine.Texture2D"), TEXT("/Script/OdysseyTexture.OdysseyTextureLayerImageRaster") });
    TestTrue(TEXT("A package with a raster layer holds raster owners"), HoldsRasterOwners(Layer, OwnerClasses));

    // The cells moved from OdysseyAnimation to OdysseyAnimationEditor; BaseOdyssey.ini redirects the former name.
    const FAssetPackageData OldCell = CreatePackageData({ TEXT("/Script/OdysseyAnimation.OdysseyAnimationCellImageRaster") });
    TestTrue(TEXT("A raster cell saved under its former class name is a raster owner"), HoldsRasterOwners(OldCell, OwnerClasses));

    const FAssetPackageData Plain = CreatePackageData({ TEXT("/Script/Engine.Texture2D"), TEXT("/Script/Engine.AssetImportData") });
    TestFalse(TEXT("A package without raster layer or cell holds no raster owner"), HoldsRasterOwners(Plain, OwnerClasses));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyHasRasterStorageTagTest, "Odyssey.TiledImageMigration.HasRasterStorageTag", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** The raster storage tag of one asset marks its whole package. */
bool FOdysseyHasRasterStorageTagTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TiledImageMigration;
    using namespace UE::OdysseyTiledImageMigration::Tests;
    const FAssetData Untagged = CreateAssetData(FAssetDataTagMap());
    FAssetDataTagMap Tags;
    Tags.Add(RasterStorageTag, FString(RasterStorageTiledImage));
    const FAssetData Tagged = CreateAssetData(MoveTemp(Tags));

    const FAssetData UntaggedAssets[] = { Untagged };
    TestFalse(TEXT("A package whose assets lack the tag is not marked"), HasRasterStorageTag(UntaggedAssets));
    const FAssetData MixedAssets[] = { Untagged, Tagged };
    TestTrue(TEXT("A package with one tagged asset is marked"), HasRasterStorageTag(MixedAssets));
    TestFalse(TEXT("A package without asset is not marked"), HasRasterStorageTag({}));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyFindLegacyPackagesTest, "Odyssey.TiledImageMigration.FindLegacyPackages", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** The legacy brush stamps of the plugin are found in the asset registry and grouped under the plugin. */
bool FOdysseyFindLegacyPackagesTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TiledImageMigration;
    using namespace UE::OdysseyTiledImageMigration::Tests;
    IAssetRegistry& Registry = IAssetRegistry::GetChecked();
    const TArray<FString> Paths = { FString(StampsPath) };
    Registry.ScanPathsSynchronous(Paths, /*bForceRescan*/ false);

    const TArray<const UClass*> OwnerClasses = GetRasterOwnerClasses();
    const TOptional<FAssetPackageData> PlainData = Registry.GetAssetPackageDataCopy(PlainTexture);
    const bool bStampALegacy = IsLegacyPackage(LegacyStampA, OwnerClasses);
    const bool bStampBLegacy = IsLegacyPackage(LegacyStampB, OwnerClasses);
    const bool bFixturesLegacy = bStampALegacy && bStampBLegacy;
    if (!bFixturesLegacy || !PlainData)
    {
        AddWarning(TEXT("Skipped: the fixture stamps of the Odyssey plugin are no longer legacy."));
        return true;
    }

    TestFalse(TEXT("A plain texture is not legacy"), IsLegacyPackage(PlainTexture, OwnerClasses));
    const TArray<FOdysseyLegacyPackage> Packages = FindLegacyPackages(OwnerClasses);
    TestTrue(TEXT("The first legacy stamp is found"), ContainsPackage(Packages, LegacyStampA));
    TestTrue(TEXT("The second legacy stamp is found"), ContainsPackage(Packages, LegacyStampB));
    TestFalse(TEXT("A plain texture is not found"), ContainsPackage(Packages, PlainTexture));

    const TArray<FOdysseyLegacyAssetGroup> Groups = CreateLegacyAssetGroups(Packages);
    const FOdysseyLegacyAssetGroup* PluginGroup = Groups.FindByPredicate([](const FOdysseyLegacyAssetGroup& Group) -> bool
    {
        return Group.MountPoint == FName(TEXT("Odyssey"));
    });
    if (!TestNotNull(TEXT("The stamps are grouped under the Odyssey plugin"), PluginGroup))
    {
        return true;
    }

    TestFalse(TEXT("The plugin group is not the project"), PluginGroup->IsProject());
    TestNotEqual(TEXT("The plugin of the project is not an engine plugin"), PluginGroup->State, EOdysseyLegacyAssetGroupState::EnginePlugin);
    TestTrue(TEXT("The plugin group holds the first stamp"), ContainsPackage(PluginGroup->Packages, LegacyStampA));
    TestTrue(TEXT("The group size adds the package sizes"), PluginGroup->DiskSize > 0);
    if (Groups.Num() > 1)
    {
        TestTrue(TEXT("Only the project group may precede the plugin groups"), Groups[0].IsProject() || !Groups[1].IsProject());
    }
    return true;
}

#endif
