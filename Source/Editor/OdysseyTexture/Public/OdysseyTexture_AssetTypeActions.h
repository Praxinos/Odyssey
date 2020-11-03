// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "AssetTypeActions_Base.h"
#include "CoreMinimal.h"
#include "Toolkits/IToolkitHost.h"

class ODYSSEYTEXTURE_API FOdysseyTextureAssetTypeActions
    : public FAssetTypeActions_Base
{
public:
    FOdysseyTextureAssetTypeActions( EAssetTypeCategories::Type iAssetCategory );

    // IAssetTypeActions interface
    virtual FText GetName() const override;
    virtual FColor GetTypeColor() const override;
    virtual UClass* GetSupportedClass() const override;
    virtual uint32 GetCategories() override;
    virtual void BuildBackendFilter( FARFilter & InFilter ) override;
	//virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
    // End of IAssetTypeActions interface

private:
    EAssetTypeCategories::Type mMyAssetCategory;
};
