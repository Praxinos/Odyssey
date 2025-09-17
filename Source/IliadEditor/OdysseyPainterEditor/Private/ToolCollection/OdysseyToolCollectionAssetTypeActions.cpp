// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyToolCollectionAssetTypeActions.h"

#include "ContentBrowserModule.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "IContentBrowserSingleton.h"
#include "BlueprintEditorModule.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/MessageDialog.h"
#include "Misc/PackageName.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditor"


/////////////////////////////////////////////////////
// FAssetTypeActions_OdysseyToolCollection


FOdysseyToolCollectionAssetTypeActions::FOdysseyToolCollectionAssetTypeActions( EAssetTypeCategories::Type InAssetCategory )
    : OdysseyToolCollectionAssetCategory( InAssetCategory )
{
}


FText
FOdysseyToolCollectionAssetTypeActions::GetName()  const
{
    return LOCTEXT( "asset-type-actions.name", "2D Paint Tool Collection" );
}


FColor
FOdysseyToolCollectionAssetTypeActions::GetTypeColor()  const
{
    return FColor(200, 200, 0);
}


UClass*
FOdysseyToolCollectionAssetTypeActions::GetSupportedClass()  const
{
    return UOdysseyToolCollection::StaticClass();
}


bool
FOdysseyToolCollectionAssetTypeActions::HasActions( const  TArray< UObject* >&  InObjects)  const
{
    return true;
}


void
FOdysseyToolCollectionAssetTypeActions::GetActions( const  TArray< UObject* >& InObjects, FMenuBuilder& MenuBuilder )
{
}


uint32
FOdysseyToolCollectionAssetTypeActions::GetCategories()
{
    return  OdysseyToolCollectionAssetCategory;
}


#undef LOCTEXT_NAMESPACE
