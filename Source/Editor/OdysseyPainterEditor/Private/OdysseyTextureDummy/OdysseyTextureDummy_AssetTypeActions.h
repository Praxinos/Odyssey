// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#pragma once

#include "AssetTypeActions_Base.h"
#include "CoreMinimal.h"
#include "Toolkits/IToolkitHost.h"

class FOdysseyTextureAssetTypeActions
    : public FAssetTypeActions_Base
{
public:
    FOdysseyTextureAssetTypeActions( EAssetTypeCategories::Type iAssetCategory );

    // IAssetTypeActions interface
    virtual FText GetName() const override;
    virtual FColor GetTypeColor() const override;
    virtual UClass* GetSupportedClass() const override;
    virtual uint32 GetCategories() override;
    // End of IAssetTypeActions interface

private:
    EAssetTypeCategories::Type mMyAssetCategory;
};
