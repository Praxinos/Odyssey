// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "AssetDefinitionDefault.h"

#include "AssetDefinition_ShotSequence.generated.h"

UCLASS()
class UAssetDefinition_ShotSequence
    : public UAssetDefinitionDefault
{
    GENERATED_BODY()

public:
    // UAssetDefinition Begin
    virtual FText GetAssetDisplayName() const override;
    virtual FLinearColor GetAssetColor() const override;
    virtual TSoftClassPtr<UObject> GetAssetClass() const override;
    virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override;
    virtual FAssetOpenSupport GetAssetOpenSupport(const FAssetOpenSupportArgs& OpenSupportArgs) const override;
    virtual EAssetCommandResult OpenAssets(const FAssetOpenArgs& OpenArgs) const override;
    // UAssetDefinition End
};
