// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"

#include "OdysseyLegacyPackageScanner.h"

/** Whether the packages of a group can be converted. */
enum class EOdysseyLegacyAssetGroupState : uint8
{
    /** The packages can be saved. */
    Convertible,
    /** Source control is enabled: the packages are checked out before the conversion. */
    NeedsCheckout,
    /** The packages belong to a plugin installed in the engine, which the project must not modify. */
    EnginePlugin,
};

/** The legacy packages of the project, or of one plugin. */
struct FOdysseyLegacyAssetGroup
{
    /** The name shown to the user: the project, or the friendly name of the plugin. */
    FText DisplayName;

    /** The mount point of the packages, e.g. Game or the name of the plugin. */
    FName MountPoint;

    /** The conversion state of the packages of the group, set by CreateLegacyAssetGroups(). */
    EOdysseyLegacyAssetGroupState State = EOdysseyLegacyAssetGroupState::Convertible;

    /** The packages of the group, ordered as CreateLegacyAssetGroups() documents. */
    TArray<FOdysseyLegacyPackage> Packages;

    /** The total size of the package files, in bytes. */
    int64 DiskSize = 0;

    /** Returns true when the project content, rather than a plugin, holds the packages. */
    bool IsProject() const;
};

namespace Odyssey::TiledImageMigration
{
/**
 * Groups legacy packages by mount point: the project content first, then one group per plugin, sorted by name.
 * Each group keeps the packages in the order of Packages. Packages under other mount points, e.g. the engine content,
 * belong to no group.
 * Game thread.
 */
[[nodiscard]] TArray<FOdysseyLegacyAssetGroup> CreateLegacyAssetGroups(TConstArrayView<FOdysseyLegacyPackage> Packages);
}
