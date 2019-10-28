// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#include "OdysseyTextureDummy/OdysseyTextureDummy_AssetTypeActions.h"
#include "OdysseyTextureDummy/OdysseyTextureDummy.h"

#include "OdysseyStyle.h"
#include "IContentBrowserSingleton.h"
#include "ContentBrowserModule.h"


#define LOCTEXT_NAMESPACE "OdysseyTextureDummy_AssetTypeActions"


//////////////////////////////////////////////////////////////////////////
// FOdysseyTextureAssetTypeActions
FOdysseyTextureAssetTypeActions::FOdysseyTextureAssetTypeActions(EAssetTypeCategories::Type InAssetCategory)
    : MyAssetCategory(InAssetCategory)
{
}


FText
FOdysseyTextureAssetTypeActions::GetName() const
{
    return LOCTEXT("FOdysseyTextureTypeActionsName", "Texture");
}


FColor
FOdysseyTextureAssetTypeActions::GetTypeColor() const
{
    return FColor(39, 52, 106);
}


UClass*
FOdysseyTextureAssetTypeActions::GetSupportedClass() const
{
    return UOdysseyTextureDummy::StaticClass();
}


uint32
FOdysseyTextureAssetTypeActions::GetCategories()
{
    return EAssetTypeCategories::MaterialsAndTextures | MyAssetCategory;
}


#undef LOCTEXT_NAMESPACE

