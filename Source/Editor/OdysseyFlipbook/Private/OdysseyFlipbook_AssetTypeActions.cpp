// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyFlipbook_AssetTypeActions.h"

#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
// #include "OdysseyFlipbookEditorModule.h"


#include "OdysseyFlipbook.h"

#define LOCTEXT_NAMESPACE "OdysseyFlipbook_AssetTypeActions"

//////////////////////////////////////////////////////////////////////////
// FOdysseyFlipbookAssetTypeActions
FOdysseyFlipbookAssetTypeActions::FOdysseyFlipbookAssetTypeActions( EAssetTypeCategories::Type iAssetCategory )
    : mMyAssetCategory( iAssetCategory )
{
}

FText
FOdysseyFlipbookAssetTypeActions::GetName() const
{
    return LOCTEXT( "FOdysseyFlipbookTypeActionsName", "Flipbook" );
}

FColor
FOdysseyFlipbookAssetTypeActions::GetTypeColor() const
{
    return FColor( 129, 196, 115 );
}

UClass*
FOdysseyFlipbookAssetTypeActions::GetSupportedClass() const
{
    return UOdysseyFlipbook::StaticClass();
}

uint32
FOdysseyFlipbookAssetTypeActions::GetCategories()
{
    return /* EAssetTypeCategories::Animation | */ mMyAssetCategory;
}

#undef LOCTEXT_NAMESPACE
