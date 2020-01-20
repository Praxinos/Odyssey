// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyTexture_AssetTypeActions.h"

#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"

#include "OdysseyTexture.h"

#define LOCTEXT_NAMESPACE "OdysseyTexture_AssetTypeActions"

//////////////////////////////////////////////////////////////////////////
// FOdysseyTextureAssetTypeActions
FOdysseyTextureAssetTypeActions::FOdysseyTextureAssetTypeActions( EAssetTypeCategories::Type iAssetCategory )
    : mMyAssetCategory( iAssetCategory )
{
}

FText
FOdysseyTextureAssetTypeActions::GetName() const
{
    return LOCTEXT( "FOdysseyTextureTypeActionsName", "Texture" );
}

FColor
FOdysseyTextureAssetTypeActions::GetTypeColor() const
{
    return FColor( 255, 0, 0 );
}

UClass*
FOdysseyTextureAssetTypeActions::GetSupportedClass() const
{
    return UOdysseyTexture::StaticClass();
}

uint32
FOdysseyTextureAssetTypeActions::GetCategories()
{
    return EAssetTypeCategories::MaterialsAndTextures | mMyAssetCategory;
}

#undef LOCTEXT_NAMESPACE
