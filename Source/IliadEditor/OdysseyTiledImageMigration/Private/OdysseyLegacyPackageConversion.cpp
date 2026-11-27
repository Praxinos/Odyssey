// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyLegacyPackageConversion.h"

#include "AssetRegistry/AssetData.h"
#include "AssetCompilingManager.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "Editor.h"
#include "FileHelpers.h"
#include "HAL/FileManager.h"
#include "ISourceControlModule.h"
#include "ISourceControlProvider.h"
#include "Misc/PackageName.h"
#include "Misc/ScopedSlowTask.h"
#include "PackageTools.h"
#include "SourceControlOperations.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "UObject/Package.h"
#include "Interfaces/Interface_AsyncCompilation.h"
#include "UObject/UObjectHash.h"
#include "UObject/UObjectIterator.h"

#include "OdysseyTiledImageMigrationLog.h"

#define LOCTEXT_NAMESPACE "OdysseyTiledImageMigration"

namespace UE::OdysseyTiledImageMigration::Private
{
/**
 * How much the physical memory of the editor may grow since the last unload before the loaded packages are unloaded, in bytes.
 * Each unload collects garbage twice: small packages are unloaded together, a large animation alone. The size of a package
 * file does not tell: a legacy animation of 60 MB on disk takes gigabytes once loaded and migrated.
 */
constexpr uint64 UnloadThreshold = 1024ull * 1024 * 1024;

/** Returns the physical memory the editor uses, in bytes. */
uint64 FindUsedPhysicalMemory()
{
    const FPlatformMemoryStats Stats = FPlatformMemory::GetStats();
    return Stats.UsedPhysical;
}

/** How saving a package went. */
enum class ESaveResult : uint8
{
    Saved,
    Failed,
    /** The user canceled from the dialog of a failed save. */
    Canceled,
};

/** The packages loaded before the conversion, which it must neither unload nor save behind the back of the user. */
struct FLoadedPackages
{
    /** The packages whose assets are open in an asset editor. */
    TSet<FName> Open;

    /** The content packages loaded before the conversion; they stay loaded. */
    TSet<FName> Kept;

    /** The kept packages that have unsaved changes. */
    TSet<FName> Dirty;
};

/** Returns true when the package holds content saved on disk: neither script, transient, temporary nor newly created. */
bool IsContentPackage(TNotNull<const UPackage*> Package)
{
    if (Package == GetTransientPackage() || Package->HasAnyPackageFlags(PKG_CompiledIn | PKG_InMemoryOnly | PKG_PlayInEditor | PKG_NewlyCreated))
    {
        return false;
    }

    const FString PackageName = Package->GetName();
    return FPackageName::IsValidLongPackageName(PackageName) && !FPackageName::IsMemoryPackage(PackageName) && !FPackageName::IsTempPackage(PackageName);
}

/** Returns the names of the loaded content packages. */
TSet<FName> FindLoadedContentPackages()
{
    TSet<FName> PackageNames;
    for (const UPackage* Package : TObjectRange<UPackage>())
    {
        if (IsContentPackage(Package))
        {
            const FName PackageName = Package->GetFName();
            PackageNames.Add(PackageName);
        }
    }
    return PackageNames;
}

/** Returns the packages loaded before the conversion. */
[[nodiscard]] FLoadedPackages CreateLoadedPackages()
{
    FLoadedPackages LoadedPackages;
    UAssetEditorSubsystem* AssetEditors = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
    const TArray<UObject*> EditedAssets = AssetEditors->GetAllEditedAssets();
    LoadedPackages.Open.Reserve(EditedAssets.Num());
    for (const UObject* Asset : EditedAssets)
    {
        const UPackage* Package = Asset->GetPackage();
        const FName PackageName = Package->GetFName();
        LoadedPackages.Open.Add(PackageName);
    }

    LoadedPackages.Kept = FindLoadedContentPackages();
    FString LongPackageName;
    for (const FName PackageName : LoadedPackages.Kept)
    {
        PackageName.ToString(LongPackageName);
        const UPackage* Package = FindPackage(nullptr, *LongPackageName);
        if (Package && Package->IsDirty())
        {
            LoadedPackages.Dirty.Add(PackageName);
        }
    }
    return LoadedPackages;
}

/** Returns the files of the packages that exist on disk. */
TArray<FString> FindPackageFilenames(TConstArrayView<FName> PackageNames)
{
    TArray<FString> Filenames;
    Filenames.Reserve(PackageNames.Num());
    FString LongPackageName;
    for (const FName PackageName : PackageNames)
    {
        PackageName.ToString(LongPackageName);
        FString Filename;
        if (FPackageName::DoesPackageExist(LongPackageName, &Filename))
        {
            Filenames.Add(MoveTemp(Filename));
        }
    }
    return Filenames;
}

/**
 * Checks out, when source control is enabled, the package files that source control can check out.
 * @return The files checked out.
 */
TArray<FString> CheckOutPackages(TConstArrayView<FName> PackageNames)
{
    ISourceControlModule& SourceControl = ISourceControlModule::Get();
    if (!SourceControl.IsEnabled() || PackageNames.IsEmpty())
    {
        return {};
    }

    const TArray<FString> Filenames = FindPackageFilenames(PackageNames);
    ISourceControlProvider& Provider = SourceControl.GetProvider();
    const FSourceControlOperationRef UpdateStatus = ISourceControlOperation::Create<FUpdateStatus>();
    const ECommandResult::Type StatusResult = Provider.Execute(UpdateStatus, Filenames);
    if (StatusResult != ECommandResult::Succeeded)
    {
        UE_LOGF(LogOdysseyTiledImageMigration, Warning, "The source control status of %d legacy package files cannot be updated.", Filenames.Num());
    }

    TArray<FString> FilesToCheckOut;
    FilesToCheckOut.Reserve(Filenames.Num());
    for (const FString& Filename : Filenames)
    {
        const FSourceControlStatePtr State = Provider.GetState(Filename, EStateCacheUsage::Use);
        if (State && State->CanCheckout())
        {
            FilesToCheckOut.Add(Filename);
        }
    }

    if (FilesToCheckOut.IsEmpty())
    {
        return {};
    }

    const FSourceControlOperationRef CheckOut = ISourceControlOperation::Create<FCheckOut>();
    const ECommandResult::Type CheckOutResult = Provider.Execute(CheckOut, FilesToCheckOut);
    if (CheckOutResult != ECommandResult::Succeeded)
    {
        UE_LOGF(LogOdysseyTiledImageMigration, Warning, "Some of the %d legacy package files cannot be checked out.", FilesToCheckOut.Num());
    }
    return FilesToCheckOut;
}

/** Reverts the checked out files the conversion did not change, e.g. those of packages that failed to load or were not reached. */
void RevertUnchangedFiles(const TArray<FString>& CheckedOutFiles)
{
    if (CheckedOutFiles.IsEmpty())
    {
        return;
    }

    ISourceControlModule& SourceControl = ISourceControlModule::Get();
    ISourceControlProvider& Provider = SourceControl.GetProvider();
    const FSourceControlOperationRef RevertUnchanged = ISourceControlOperation::Create<FRevertUnchanged>();
    const ECommandResult::Type Result = Provider.Execute(RevertUnchanged, CheckedOutFiles);
    if (Result != ECommandResult::Succeeded)
    {
        UE_LOGF(LogOdysseyTiledImageMigration, Warning, "The unchanged legacy package files cannot be reverted: they stay checked out.");
    }
}

/** Returns the packages whose file does not exist or cannot be written. */
TSet<FName> FindUnwritablePackages(TConstArrayView<FName> PackageNames)
{
    IFileManager& FileManager = IFileManager::Get();
    TSet<FName> UnwritablePackages;
    FString LongPackageName;
    for (const FName PackageName : PackageNames)
    {
        PackageName.ToString(LongPackageName);
        FString Filename;
        if (!FPackageName::DoesPackageExist(LongPackageName, &Filename))
        {
            UE_LOGF(LogOdysseyTiledImageMigration, Warning, "%ls cannot be converted: its file does not exist.", *LongPackageName);
            UnwritablePackages.Add(PackageName);
            continue;
        }

        const bool bReadOnly = FileManager.IsReadOnly(*Filename);
        if (bReadOnly)
        {
            UE_LOGF(LogOdysseyTiledImageMigration, Warning, "%ls cannot be converted: its file %ls is read-only.", *LongPackageName, *Filename);
            UnwritablePackages.Add(PackageName);
        }
    }
    return UnwritablePackages;
}

/**
 * Returns the package, loading it when needed: its raster content migrates when it is loaded.
 * Null when it cannot be loaded or holds no asset.
 */
UPackage* FindOrLoadPackage(FName PackageName)
{
    const FString LongPackageName = PackageName.ToString();
    UPackage* Package = FindPackage(nullptr, *LongPackageName);
    if (!Package)
    {
        Package = LoadPackage(nullptr, *LongPackageName, LOAD_None);
    }

    if (!Package)
    {
        UE_LOGF(LogOdysseyTiledImageMigration, Warning, "%ls cannot be converted: it cannot be loaded.", *LongPackageName);
        return nullptr;
    }

    // The editor deletes the file of a package without asset when saving it, e.g. when its asset failed to load.
    if (UPackage::IsEmptyPackage(Package))
    {
        UE_LOGF(LogOdysseyTiledImageMigration, Warning, "%ls cannot be converted: it holds no asset once loaded.", *LongPackageName);
        return nullptr;
    }
    return Package;
}

/** Saves the package without asking the user, its file being already writable; a failed save shows the dialog of the editor. */
ESaveResult SavePackage(TNotNull<UPackage*> Package)
{
    TArray<UPackage*> FailedPackages;
    FEditorFileUtils::FPromptForCheckoutAndSaveParams SaveParams;
    SaveParams.bAlreadyCheckedOut = true;
    SaveParams.bCanBeDeclined = false;
    SaveParams.OutFailedPackages = &FailedPackages;
    UPackage* const SavedPackage = Package;
    const TArray<UPackage*> Packages = { SavedPackage };
    const FEditorFileUtils::EPromptReturnCode Result = FEditorFileUtils::PromptForCheckoutAndSave(Packages, SaveParams);
    if (Result == FEditorFileUtils::PR_Cancelled)
    {
        return ESaveResult::Canceled;
    }

    if (Result != FEditorFileUtils::PR_Success || !FailedPackages.IsEmpty())
    {
        UE_LOGF(LogOdysseyTiledImageMigration, Warning, "%ls cannot be converted: it cannot be saved (result %d).", *Package->GetName(), static_cast<int32>(Result));
        return ESaveResult::Failed;
    }
    return ESaveResult::Saved;
}

/** Returns an object of the package, e.g. a texture, that still reports an asynchronous compilation, or null. */
const UObject* FindCompilingObject(TNotNull<const UPackage*> Package)
{
    const UObject* CompilingObject = nullptr;
    ForEachObjectWithPackage(Package, [&CompilingObject](UObject* Object) -> bool
    {
        const IInterface_AsyncCompilation* Compilation = Cast<IInterface_AsyncCompilation>(Object);
        if (Compilation && Compilation->IsCompiling())
        {
            CompilingObject = Object;
            return false;
        }
        // Keeps iterating.
        return true;
    });
    return CompilingObject;
}

/** Unloads the given packages that are still loaded, clean and done compiling, keeping the undo history. */
void UnloadPackages(const TSet<FName>& PackageNames)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(Odyssey::TiledImageMigration::UnloadPackages);
    // The frames an animation rebakes when saved may still compile.
    FAssetCompilingManager& CompilingManager = FAssetCompilingManager::Get();
    CompilingManager.FinishAllCompilation();
    CompilingManager.ProcessAsyncTasks();

    TArray<UPackage*> PackagesToUnload;
    PackagesToUnload.Reserve(PackageNames.Num());
    FString LongPackageName;
    for (const FName PackageName : PackageNames)
    {
        PackageName.ToString(LongPackageName);
        UPackage* Package = FindPackage(nullptr, *LongPackageName);
        // UnloadPackages() refuses dirty packages.
        if (!Package || Package->IsDirty())
        {
            continue;
        }

        // UnloadPackages() expects every compilation of the package to be known to the compiling managers, see FlushAsyncCompilation().
        if (const UObject* CompilingObject = FindCompilingObject(Package))
        {
            const FString ObjectName = CompilingObject->GetPathName();
            UE_LOGF(LogOdysseyTiledImageMigration, Warning, "%ls stays loaded: %ls still compiles (garbage: %d).", *LongPackageName, *ObjectName, int32(!IsValid(CompilingObject)));
            continue;
        }
        PackagesToUnload.Add(Package);
    }

    if (PackagesToUnload.IsEmpty())
    {
        return;
    }

    UPackageTools::FUnloadPackageParams UnloadParams(PackagesToUnload);
    UnloadParams.bResetTransBuffer = false;
    if (!UPackageTools::UnloadPackages(UnloadParams))
    {
        UE_LOGF(LogOdysseyTiledImageMigration, Warning, "Some converted packages stay loaded: %ls", *UnloadParams.OutErrorMessage.ToString());
    }
}

/** Rescans the files of the saved packages, then marks those the asset registry still sees as legacy. */
void MarkStillLegacyPackages(TArrayView<FOdysseyLegacyPackageOutcome> InOutOutcomes)
{
    TArray<FName> SavedPackages;
    SavedPackages.Reserve(InOutOutcomes.Num());
    for (const FOdysseyLegacyPackageOutcome& Outcome : InOutOutcomes)
    {
        if (Outcome.Outcome == EOdysseyLegacyConversionOutcome::Saved)
        {
            SavedPackages.Add(Outcome.PackageName);
        }
    }

    const TArray<FString> Filenames = FindPackageFilenames(SavedPackages);
    if (Filenames.Num() != SavedPackages.Num())
    {
        UE_LOGF(LogOdysseyTiledImageMigration, Warning, "The files of %d saved packages cannot be found.", SavedPackages.Num() - Filenames.Num());
    }

    if (Filenames.IsEmpty())
    {
        return;
    }

    // The registry updates the package data of a saved file only when it rescans it.
    IAssetRegistry& Registry = IAssetRegistry::GetChecked();
    Registry.ScanFilesSynchronous(Filenames, /*bForceRescan*/ true);
    const TArray<const UClass*> OwnerClasses = Odyssey::TiledImageMigration::GetRasterOwnerClasses();
    for (FOdysseyLegacyPackageOutcome& Outcome : InOutOutcomes)
    {
        if (Outcome.Outcome != EOdysseyLegacyConversionOutcome::Saved)
        {
            continue;
        }

        if (Odyssey::TiledImageMigration::IsLegacyPackage(Outcome.PackageName, OwnerClasses))
        {
            UE_LOGF(LogOdysseyTiledImageMigration, Warning, "%ls was saved but still uses the legacy storage.", *Outcome.PackageName.ToString());
            Outcome.Outcome = EOdysseyLegacyConversionOutcome::StillLegacy;
        }
    }
}

/**
 * Converts one package, already checked out, and returns what the conversion did with it;
 * NotProcessed when the user canceled from the dialog of a failed save.
 */
EOdysseyLegacyConversionOutcome ConvertPackage(FName PackageName, const FLoadedPackages& LoadedPackages, const TSet<FName>& UnwritablePackages)
{
    if (LoadedPackages.Open.Contains(PackageName))
    {
        UE_LOGF(LogOdysseyTiledImageMigration, Display, "%ls is not converted: it is open in an asset editor.", *PackageName.ToString());
        return EOdysseyLegacyConversionOutcome::OpenInEditor;
    }

    if (LoadedPackages.Dirty.Contains(PackageName))
    {
        UE_LOGF(LogOdysseyTiledImageMigration, Display, "%ls is not converted: it has unsaved changes.", *PackageName.ToString());
        return EOdysseyLegacyConversionOutcome::HasUnsavedChanges;
    }

    if (UnwritablePackages.Contains(PackageName))
    {
        return EOdysseyLegacyConversionOutcome::CheckoutFailed;
    }

    UPackage* Package = FindOrLoadPackage(PackageName);
    if (!Package)
    {
        return EOdysseyLegacyConversionOutcome::LoadFailed;
    }

    const ESaveResult SaveResult = SavePackage(Package);
    if (SaveResult == ESaveResult::Canceled)
    {
        return EOdysseyLegacyConversionOutcome::NotProcessed;
    }

    if (SaveResult == ESaveResult::Failed)
    {
        return EOdysseyLegacyConversionOutcome::SaveFailed;
    }
    return EOdysseyLegacyConversionOutcome::Saved;
}
}

int32
FOdysseyLegacyConversionResult::Count(EOdysseyLegacyConversionOutcome Outcome) const
{
    int32 Number = 0;
    for (const FOdysseyLegacyPackageOutcome& PackageOutcome : Outcomes)
    {
        if (PackageOutcome.Outcome == Outcome)
        {
            ++Number;
        }
    }
    return Number;
}

namespace Odyssey::TiledImageMigration
{
FOdysseyLegacyConversionResult ConvertLegacyPackages(TConstArrayView<FOdysseyLegacyPackage> Packages)
{
    using namespace UE::OdysseyTiledImageMigration::Private;
    TRACE_CPUPROFILER_EVENT_SCOPE(Odyssey::TiledImageMigration::ConvertLegacyPackages);
    check(IsInGameThread());
    const FLoadedPackages LoadedPackages = CreateLoadedPackages();
    FOdysseyLegacyConversionResult Result;
    Result.Outcomes.Reserve(Packages.Num());
    TArray<FName> PackagesToSave;
    PackagesToSave.Reserve(Packages.Num());
    for (const FOdysseyLegacyPackage& Package : Packages)
    {
        Result.Outcomes.Add({ Package.PackageName, EOdysseyLegacyConversionOutcome::NotProcessed });
        if (!LoadedPackages.Open.Contains(Package.PackageName) && !LoadedPackages.Dirty.Contains(Package.PackageName))
        {
            PackagesToSave.Add(Package.PackageName);
        }
    }

    const TArray<FString> CheckedOutFiles = CheckOutPackages(PackagesToSave);
    const TSet<FName> UnwritablePackages = FindUnwritablePackages(PackagesToSave);

    FScopedSlowTask SlowTask(Packages.Num(), LOCTEXT("conversion.progress", "Converting the legacy Odyssey assets..."));
    SlowTask.MakeDialog(/*bShowCancelButton*/ true);
    const FText ProgressFormat = LOCTEXT("conversion.progress-package", "Converting {0}");
    // The packages the conversion loads, with their dependencies, to unload once the editor has grown by UnloadThreshold.
    TSet<FName> PackagesToUnload;
    uint64 PhysicalAtLastUnload = FindUsedPhysicalMemory();
    for (int32 Index = 0; Index < Packages.Num(); ++Index)
    {
        if (SlowTask.ShouldCancel())
        {
            Result.bCanceled = true;
            break;
        }

        const FOdysseyLegacyPackage& Package = Packages[Index];
        const FText PackageText = FText::FromName(Package.PackageName);
        const FText Progress = FText::Format(ProgressFormat, PackageText);
        SlowTask.EnterProgressFrame(1.0f, Progress);
        const TSet<FName> PackagesBefore = FindLoadedContentPackages();
        const EOdysseyLegacyConversionOutcome Outcome = ConvertPackage(Package.PackageName, LoadedPackages, UnwritablePackages);
        Result.Outcomes[Index].Outcome = Outcome;
        const TSet<FName> PackagesAfter = FindLoadedContentPackages();
        const TSet<FName> NewPackages = PackagesAfter.Difference(PackagesBefore);
        PackagesToUnload.Append(NewPackages);
        if (Outcome == EOdysseyLegacyConversionOutcome::NotProcessed)
        {
            Result.bCanceled = true;
            break;
        }

        const uint64 Physical = FindUsedPhysicalMemory();
        if (Physical >= PhysicalAtLastUnload + UnloadThreshold)
        {
            UnloadPackages(PackagesToUnload);
            PackagesToUnload.Reset();
            PhysicalAtLastUnload = FindUsedPhysicalMemory();
        }
    }

    UnloadPackages(PackagesToUnload);
    RevertUnchangedFiles(CheckedOutFiles);
    MarkStillLegacyPackages(Result.Outcomes);
    return Result;
}
}

#undef LOCTEXT_NAMESPACE
