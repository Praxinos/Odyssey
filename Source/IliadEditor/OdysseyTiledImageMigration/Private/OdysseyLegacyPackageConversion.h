// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"

#include "OdysseyLegacyPackageScanner.h"

/** What the conversion did with a package. */
enum class EOdysseyLegacyConversionOutcome : uint8
{
    /** The package was converted and saved. */
    Saved,
    /** The package is open in an asset editor: the user saves it from there, or closes it and converts again. */
    OpenInEditor,
    /** The package was loaded with unsaved changes before the conversion, which would have saved them: the user saves it. */
    HasUnsavedChanges,
    /** The package file could not be checked out or made writable. */
    CheckoutFailed,
    /** The package could not be loaded, or holds no asset once loaded. */
    LoadFailed,
    /** The package could not be saved. */
    SaveFailed,
    /** The package was saved, but the asset registry still sees it as legacy, e.g. a raster owner without content. */
    StillLegacy,
    /** The conversion was canceled before the package. */
    NotProcessed,
};

/** What the conversion did with one package. */
struct FOdysseyLegacyPackageOutcome
{
    /** The long package name. */
    FName PackageName;

    /** NotProcessed until the conversion reaches the package. */
    EOdysseyLegacyConversionOutcome Outcome = EOdysseyLegacyConversionOutcome::NotProcessed;
};

/** What a conversion did, package by package. */
struct FOdysseyLegacyConversionResult
{
    /** One outcome per requested package. */
    TArray<FOdysseyLegacyPackageOutcome> Outcomes;

    /** True when the user canceled the conversion, from the progress dialog or the dialog of a failed save. */
    bool bCanceled = false;

    /** Returns the number of packages with the given outcome. */
    int32 Count(EOdysseyLegacyConversionOutcome Outcome) const;
};

namespace Odyssey::TiledImageMigration
{
/**
 * Resaves legacy packages in the current storage, checking their files out under source control, behind a progress
 * dialog the user can cancel between packages; a failed save shows the dialog of the editor.
 * A package open in an asset editor, or loaded with unsaved changes, is skipped. A package loaded before the call stays
 * loaded; the others are unloaded, without resetting the undo history.
 * Game thread.
 *
 * @return What the conversion did with each package, in the order of Packages.
 */
FOdysseyLegacyConversionResult ConvertLegacyPackages(TConstArrayView<FOdysseyLegacyPackage> Packages);
}
