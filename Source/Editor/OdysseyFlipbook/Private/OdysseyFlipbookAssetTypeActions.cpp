// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyFlipbook_AssetTypeActions.h"

#include "PaperFlipbook.h"
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

void 
FOdysseyFlipbookAssetTypeActions::BuildBackendFilter( FARFilter & InFilter )
{
    InFilter.ClassNames.Add( UPaperFlipbook::StaticClass()->GetFName());
}

#undef LOCTEXT_NAMESPACE
