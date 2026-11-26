// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyRasterStorageTagWriter.h"

#include "UObject/AssetRegistryTagsContext.h"
#include "UObject/Package.h"
#include "UObject/UObjectHash.h"

#include "OdysseyLegacyPackageScanner.h"
#include "OdysseyRasterStorageTag.h"

namespace UE::OdysseyTiledImageMigration::Private
{
/** Returns true when the package holds an object of one of the owner classes. */
bool HoldsRasterOwnerObjects(TNotNull<const UPackage*> Package, TConstArrayView<const UClass*> OwnerClasses)
{
    bool bHoldsOwner = false;
    ForEachObjectWithPackage(Package, [&bHoldsOwner, OwnerClasses](UObject* Object) -> bool
    {
        for (const UClass* OwnerClass : OwnerClasses)
        {
            if (Object->IsA(OwnerClass))
            {
                bHoldsOwner = true;
                return false;
            }
        }
        // Keeps iterating.
        return true;
    });
    return bHoldsOwner;
}
}

void
FOdysseyRasterStorageTagWriter::Start()
{
    const UObject::FAssetRegistryTag::FOnGetObjectAssetRegistryTagsWithContext::FDelegate Delegate =
        UObject::FAssetRegistryTag::FOnGetObjectAssetRegistryTagsWithContext::FDelegate::CreateStatic(&FOdysseyRasterStorageTagWriter::AddTag);
    ExtraTagsHandle = UObject::FAssetRegistryTag::OnGetExtraObjectTagsWithContext.Add(Delegate);
}

void
FOdysseyRasterStorageTagWriter::Stop()
{
    UObject::FAssetRegistryTag::OnGetExtraObjectTagsWithContext.Remove(ExtraTagsHandle);
    ExtraTagsHandle.Reset();
}

//static
// Runs on any thread: the engine may gather the tags on a background thread during a concurrent save.
void
FOdysseyRasterStorageTagWriter::AddTag(FAssetRegistryTagsContext Context)
{
    using namespace UE::OdysseyTiledImageMigration::Private;
    // The tag only matters in the saved package, which the asset registry reads without loading it.
    if (!Context.IsSaving())
    {
        return;
    }

    const UObject* Asset = Context.GetObject();
    const UPackage* Package = Asset->GetPackage();
    const TArray<const UClass*> OwnerClasses = Odyssey::TiledImageMigration::GetRasterOwnerClasses();
    if (!HoldsRasterOwnerObjects(Package, OwnerClasses))
    {
        return;
    }

    FString Value(Odyssey::TiledImageMigration::RasterStorageTiledImage);
    UObject::FAssetRegistryTag Tag(Odyssey::TiledImageMigration::RasterStorageTag, MoveTemp(Value), UObject::FAssetRegistryTag::TT_Hidden);
    Context.AddTag(MoveTemp(Tag));
}
