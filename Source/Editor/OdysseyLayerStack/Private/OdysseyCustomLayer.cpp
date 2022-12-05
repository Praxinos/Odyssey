// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyCustomLayer.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/ScopedSlowTask.h"

#define LOCTEXT_NAMESPACE "OdysseyCustomLayer"

#if WITH_EDITOR
void
UOdysseyCustomLayer::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
{
	Super::GetAssetRegistryTags(OutTags);
	OutTags.Add(FAssetRegistryTag(TEXT("OdysseyLayer"), "true", FAssetRegistryTag::TT_Hidden));
}
#endif

TArray<FAssetData>
UOdysseyCustomLayer::FindAllCustomLayerClassesAssetData()
{
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

    TArray<FAssetData> ValidClasses;
    {
        FARFilter Filter;
        FString value = "true";
        Filter.TagsAndValues.Add(TEXT("OdysseyLayer"), value);
        AssetRegistryModule.Get().GetAssets(Filter, ValidClasses);
    }

    TSet<FName> ExistingPaths;
    for (const FAssetData& Asset : ValidClasses)
    {
        FAssetDataTagMapSharedView::FFindTagResult GeneratedClassPathTag = Asset.TagsAndValues.FindTag(FBlueprintTags::GeneratedClassPath);
        if (GeneratedClassPathTag.IsSet())
        {
            FString ObjectPath = FPackageName::ExportTextPathToObjectPath(GeneratedClassPathTag.GetValue());
            ExistingPaths.Add(*ObjectPath);
        }
    }

    // Check loaded classes
    UClass* odysseyCustomLayerClass = UOdysseyCustomLayer::StaticClass();
    for (TObjectIterator<UClass> ClassIterator; ClassIterator; ++ClassIterator)
    {
        UClass* Class = *ClassIterator;
        if (Class->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists))
            continue;

        if (Class == odysseyCustomLayerClass)
            continue;

        if (Class->GetAuthoritativeClass() != Class)
            continue;

        if (!Class->IsChildOf(odysseyCustomLayerClass))
            continue;
        
        if (ExistingPaths.Contains(*Class->GetPathName()))
            continue;            

        ValidClasses.Add(*ClassIterator);
    }

    return ValidClasses;
}

UClass*
UOdysseyCustomLayer::LoadClassFromAssetData(const FAssetData& AssetData)
{
    UObject* LoadedAsset = AssetData.FastGetAsset();
    if (!LoadedAsset)
    {
        FScopedSlowTask SlowTask(1.f, FText::Format(LOCTEXT("LoadingClass", "Loading asset {0}"), FText::FromName(AssetData.AssetName)));
        SlowTask.MakeDialogDelayed(1.f);

        LoadedAsset = AssetData.GetAsset();
    }

    if (!LoadedAsset)
    {
        return nullptr;
    }

    if (UClass* DirectClass = Cast<UClass>(LoadedAsset))
    {
        return DirectClass;
    }
    else if (UBlueprint* Blueprint = Cast<UBlueprint>(LoadedAsset))
    {
        return Blueprint->GeneratedClass;
    }
    return nullptr;
}

#undef LOCTEXT_NAMESPACE