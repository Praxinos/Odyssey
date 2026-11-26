// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyLegacyAssetGroup.h"

#include "Interfaces/IPluginManager.h"
#include "ISourceControlModule.h"
#include "Misc/PackageName.h"

#include "OdysseyTiledImageMigrationLog.h"

#define LOCTEXT_NAMESPACE "OdysseyTiledImageMigration"

namespace UE::OdysseyTiledImageMigration::Private
{
/** The mount point of the project content. */
const FName ProjectMountPoint(TEXT("Game"));

/** Returns the state of the groups the project may modify: they need a checkout under source control. */
EOdysseyLegacyAssetGroupState FindWritableState()
{
    ISourceControlModule& SourceControl = ISourceControlModule::Get();
    if (SourceControl.IsEnabled())
    {
        return EOdysseyLegacyAssetGroupState::NeedsCheckout;
    }
    return EOdysseyLegacyAssetGroupState::Convertible;
}

/**
 * Returns the empty group of a mount point, or nothing when the mount point is neither the project nor a plugin.
 * @param WritableState The state of a group the project may modify, see FindWritableState().
 */
[[nodiscard]] TOptional<FOdysseyLegacyAssetGroup> CreateLegacyAssetGroup(FName MountPoint, EOdysseyLegacyAssetGroupState WritableState)
{
    FOdysseyLegacyAssetGroup Group;
    Group.MountPoint = MountPoint;
    Group.State = WritableState;
    if (MountPoint == ProjectMountPoint)
    {
        Group.DisplayName = LOCTEXT("group.project", "Project");
        return Group;
    }

    const FString PluginName = MountPoint.ToString();
    IPluginManager& PluginManager = IPluginManager::Get();
    const TSharedPtr<IPlugin> Plugin = PluginManager.FindPlugin(PluginName);
    if (!Plugin)
    {
        return {};
    }

    const FStringView FriendlyName = Plugin->GetFriendlyName();
    Group.DisplayName = FText::FromStringView(FriendlyName);
    const EPluginLoadedFrom LoadedFrom = Plugin->GetLoadedFrom();
    if (LoadedFrom == EPluginLoadedFrom::Engine)
    {
        Group.State = EOdysseyLegacyAssetGroupState::EnginePlugin;
    }
    return Group;
}
}

bool
FOdysseyLegacyAssetGroup::IsProject() const
{
    return MountPoint == UE::OdysseyTiledImageMigration::Private::ProjectMountPoint;
}

namespace Odyssey::TiledImageMigration
{
TArray<FOdysseyLegacyAssetGroup> CreateLegacyAssetGroups(TConstArrayView<FOdysseyLegacyPackage> Packages)
{
    using namespace UE::OdysseyTiledImageMigration::Private;
    check(IsInGameThread());
    const EOdysseyLegacyAssetGroupState WritableState = FindWritableState();

    TMap<FName, FOdysseyLegacyAssetGroup> GroupsByMountPoint;
    TSet<FName> UnknownMountPoints;
    FString PackageName;
    for (const FOdysseyLegacyPackage& Package : Packages)
    {
        Package.PackageName.ToString(PackageName);
        const FName MountPoint = FPackageName::GetPackageMountPoint(PackageName);
        FOdysseyLegacyAssetGroup* Group = GroupsByMountPoint.Find(MountPoint);
        if (!Group && !UnknownMountPoints.Contains(MountPoint))
        {
            TOptional<FOdysseyLegacyAssetGroup> NewGroup = CreateLegacyAssetGroup(MountPoint, WritableState);
            if (NewGroup)
            {
                Group = &GroupsByMountPoint.Add(MountPoint, MoveTemp(*NewGroup));
            }
            else
            {
                UE_LOGF(LogOdysseyTiledImageMigration, Display, "Legacy packages under /%ls are not offered for conversion: the mount point is neither the project nor a plugin.", *MountPoint.ToString());
                UnknownMountPoints.Add(MountPoint);
            }
        }

        if (Group)
        {
            Group->Packages.Add(Package);
            Group->DiskSize += Package.DiskSize;
        }
    }

    TArray<FOdysseyLegacyAssetGroup> Groups;
    Groups.Reserve(GroupsByMountPoint.Num());
    for (TPair<FName, FOdysseyLegacyAssetGroup>& Pair : GroupsByMountPoint)
    {
        Groups.Add(MoveTemp(Pair.Value));
    }
    Groups.Sort([](const FOdysseyLegacyAssetGroup& Left, const FOdysseyLegacyAssetGroup& Right) -> bool
    {
        if (Left.IsProject() != Right.IsProject())
        {
            return Left.IsProject();
        }
        return Left.DisplayName.CompareTo(Right.DisplayName) < 0;
    });
    return Groups;
}
}

#undef LOCTEXT_NAMESPACE
