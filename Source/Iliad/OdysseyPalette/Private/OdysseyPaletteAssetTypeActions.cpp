// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyPaletteAssetTypeActions.h"

#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "OdysseyPalette.h"

#define LOCTEXT_NAMESPACE "Palette"

//////////////////////////////////////////////////////////////////////////
// FOdysseyPaletteAssetTypeActions
FOdysseyPaletteAssetTypeActions::FOdysseyPaletteAssetTypeActions( EAssetTypeCategories::Type iAssetCategory )
    : mMyAssetCategory( iAssetCategory )
{
}

FText
FOdysseyPaletteAssetTypeActions::GetName() const
{
    return LOCTEXT( "asset-type-actions.name", "Palette" );
}

FColor
FOdysseyPaletteAssetTypeActions::GetTypeColor() const
{
    return FColor( 235, 130, 20 );
}

UClass*
FOdysseyPaletteAssetTypeActions::GetSupportedClass() const
{
    return UOdysseyPalette::StaticClass();
}

uint32
FOdysseyPaletteAssetTypeActions::GetCategories()
{
    return mMyAssetCategory;
}

void
FOdysseyPaletteAssetTypeActions::BuildBackendFilter( FARFilter & InFilter )
{
    InFilter.ClassPaths.Add( UOdysseyPalette::StaticClass()->GetClassPathName() );
}

#undef LOCTEXT_NAMESPACE
