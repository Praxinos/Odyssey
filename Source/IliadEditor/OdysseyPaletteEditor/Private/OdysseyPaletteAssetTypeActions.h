// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "AssetTypeActions_Base.h"
#include "CoreMinimal.h"
#include "Toolkits/IToolkitHost.h"


class FOdysseyPaletteAssetTypeActions
    : public FAssetTypeActions_Base
{
public:
    FOdysseyPaletteAssetTypeActions( EAssetTypeCategories::Type iAssetCategory );

    // IAssetTypeActions interface
    virtual FText GetName() const override;
    virtual FColor GetTypeColor() const override;
    virtual UClass* GetSupportedClass() const override;
    virtual uint32 GetCategories() override;
    virtual void BuildBackendFilter( FARFilter & InFilter ) override;
    virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
    // End of IAssetTypeActions interface

private:
    EAssetTypeCategories::Type mMyAssetCategory;
};
