// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyTextureDummy/OdysseyTextureDummy_AssetTypeActions.h"

#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"

#include "OdysseyTextureDummy/OdysseyTextureDummy.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureDummy_AssetTypeActions"

//////////////////////////////////////////////////////////////////////////
// FOdysseyTextureDummyAssetTypeActions
FOdysseyTextureDummyAssetTypeActions::FOdysseyTextureDummyAssetTypeActions( EAssetTypeCategories::Type iAssetCategory )
    : mMyAssetCategory( iAssetCategory )
{
}

FText
FOdysseyTextureDummyAssetTypeActions::GetName() const
{
    return LOCTEXT( "FOdysseyTextureDummyTypeActionsName", "Texture" );
}

FColor
FOdysseyTextureDummyAssetTypeActions::GetTypeColor() const
{
    return FColor( 39, 52, 106 );
}

UClass*
FOdysseyTextureDummyAssetTypeActions::GetSupportedClass() const
{
    return UOdysseyTextureDummy::StaticClass();
}

uint32
FOdysseyTextureDummyAssetTypeActions::GetCategories()
{
    return EAssetTypeCategories::MaterialsAndTextures | mMyAssetCategory;
}

#undef LOCTEXT_NAMESPACE
