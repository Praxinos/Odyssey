// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyLayer.h"
#include "OdysseyCustomLayer.generated.h"

/**
 * @brief Should be the base class for all Blueprint based Layers
 * 
 */

UCLASS(Abstract, BlueprintType)
class ODYSSEYLAYERSTACK_API UOdysseyCustomLayer
    : public UOdysseyLayer
{
    GENERATED_BODY()

public:
    /**
     * @brief Will return every custom layer classes made in blueprint, in FAssetData format
     * 
     * @return TArray<FAssetData> 
     */
    static TArray<FAssetData> FindAllCustomLayerClassesAssetData();

    /**
     * @brief Will return the actual UClass contained in an FAssetData, wether it's a C++ or Blueprint UClass 
     * 
     * @param AssetData 
     * @return UClass* 
     */
    static UClass* LoadClassFromAssetData(const FAssetData& AssetData);

public:
#if WITH_EDITOR
	virtual void GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const override;
#endif

public:
    //Default properties
    UPROPERTY(EditDefaultsOnly, Category="Layer")
    FText Category = FText::FromString(TEXT(""));

    UPROPERTY(EditDefaultsOnly, Category="Layer")
    int DisplayOrder = 0;

    UPROPERTY(EditDefaultsOnly, Category="Layer")
	TSet<UClass*> CompatibleLayerStacks;
};