// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "AssetRegistry/AssetData.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "HAL/FileManager.h"
#include "Interfaces/IPluginManager.h"
#include "ISourceControlModule.h"
#include "Misc/AutomationTest.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "UObject/Package.h"

#include "OdysseyLegacyPackageConversion.h"
#include "OdysseyLegacyPackageScanner.h"
#include "OdysseyRasterStorageTag.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace UE::OdysseyTiledImageMigration::Tests
{
/** The legacy brush stamp of the plugin copied into the project for the test, relative to the plugin content directory. */
constexpr FStringView FixtureFile = TEXTVIEW("Brushes/_Common/Stamps/RGBA16/Penbrush/T_AAcircle_1024.uasset");

/** The package of the copy. */
constexpr FStringView CopyPackageName = TEXTVIEW("/Game/OdysseyTiledImageMigrationTest/T_LegacyStamp");

/** Returns the file of the copy. */
[[nodiscard]] FString CreateCopyFilename()
{
    const FString PackageName(CopyPackageName);
    const FString Extension = FPackageName::GetAssetPackageExtension();
    return FPackageName::LongPackageNameToFilename(PackageName, Extension);
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyConvertLegacyPackagesTest, "Odyssey.TiledImageMigration.ConvertLegacyPackages", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** A copy of a legacy stamp is found, converted and saved, after which the asset registry no longer sees it as legacy. */
bool FOdysseyConvertLegacyPackagesTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TiledImageMigration;
    using namespace UE::OdysseyTiledImageMigration::Tests;
    // The conversion would check the copy out or mark it for add, and leave it in a changelist once deleted.
    ISourceControlModule& SourceControl = ISourceControlModule::Get();
    if (SourceControl.IsEnabled())
    {
        AddWarning(TEXT("Skipped: source control is enabled."));
        return true;
    }

    IPluginManager& PluginManager = IPluginManager::Get();
    const TSharedPtr<IPlugin> Plugin = PluginManager.FindPlugin(TEXT("Odyssey"));
    if (!TestNotNull(TEXT("The Odyssey plugin is found"), Plugin.Get()))
    {
        return true;
    }

    const FString ContentDir = Plugin->GetContentDir();
    const FString SourceFilename = FPaths::Combine(ContentDir, FString(FixtureFile));
    const FString CopyFilename = UE::OdysseyTiledImageMigration::Tests::CreateCopyFilename();
    IFileManager& FileManager = IFileManager::Get();
    const uint32 CopyResult = FileManager.Copy(*CopyFilename, *SourceFilename);
    if (!TestEqual(TEXT("The legacy stamp is copied into the project"), CopyResult, uint32(COPY_OK)))
    {
        return true;
    }

    IAssetRegistry& Registry = IAssetRegistry::GetChecked();
    const TArray<FString> Filenames = { CopyFilename };
    Registry.ScanFilesSynchronous(Filenames, /*bForceRescan*/ true);
    const FName PackageName(CopyPackageName);
    const TArray<const UClass*> OwnerClasses = GetRasterOwnerClasses();
    const TArray<FOdysseyLegacyPackage> Candidates = FindLegacyPackages(OwnerClasses);
    const FOdysseyLegacyPackage* Candidate = Candidates.FindByPredicate([PackageName](const FOdysseyLegacyPackage& Package) -> bool
    {
        return Package.PackageName == PackageName;
    });

    if (!Candidate)
    {
        AddWarning(TEXT("Skipped: the fixture stamp of the Odyssey plugin is no longer legacy."));
    }
    else
    {
        const FOdysseyLegacyPackage Packages[] = { *Candidate };
        const FOdysseyLegacyConversionResult Result = ConvertLegacyPackages(Packages);
        TestFalse(TEXT("The conversion is not canceled"), Result.bCanceled);
        TestEqual(TEXT("The copy is saved"), Result.Count(EOdysseyLegacyConversionOutcome::Saved), 1);

        TestFalse(TEXT("The saved copy is no longer legacy"), IsLegacyPackage(PackageName, OwnerClasses));
        TArray<FAssetData> SavedAssets;
        Registry.GetAssetsByPackageName(PackageName, SavedAssets, /*bIncludeOnlyOnDiskAssets*/ true);
        TestTrue(TEXT("The saved copy has the raster storage tag"), HasRasterStorageTag(SavedAssets));
        const TOptional<FAssetPackageData> PackageData = Registry.GetAssetPackageDataCopy(PackageName);
        if (TestTrue(TEXT("The registry knows the saved copy"), PackageData.IsSet()))
        {
            const FName TiledImageClass(TEXT("/Script/OdysseyImaging.OdysseyTiledImage"));
            TestTrue(TEXT("The saved copy holds tiled images"), PackageData->ImportedClasses.Contains(TiledImageClass));
        }

        const FString LongPackageName(CopyPackageName);
        TestNull(TEXT("The conversion unloads the package it loaded"), FindPackage(nullptr, *LongPackageName));
    }

    const bool bFileDeleted = FileManager.Delete(*CopyFilename, /*RequireExists*/ false, /*EvenReadOnly*/ true);
    TestTrue(TEXT("The copy is deleted"), bFileDeleted);
    const FString CopyDirectory = FPaths::GetPath(CopyFilename);
    const bool bDirectoryDeleted = FileManager.DeleteDirectory(*CopyDirectory, /*RequireExists*/ false, /*Tree*/ false);
    TestTrue(TEXT("The copy folder is deleted"), bDirectoryDeleted);
    Registry.ScanModifiedAssetFiles(Filenames);
    const FString CopyPath = FPackageName::GetLongPackagePath(FString(CopyPackageName));
    Registry.RemovePath(CopyPath);
    return true;
}

#endif
