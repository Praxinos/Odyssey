// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyFlipbookAssetTypeActions.h"

#include "PaperFlipbook.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "OdysseyFlipbook.h"
#include "OdysseyFlipbookEditorModule.h"
#include "OdysseyFlipbookWrapper.h"
#include "OdysseyTextureLayerStackUserData.h"
#include "OdysseyPainterEditorSettings.h"

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
    return UPaperFlipbook::StaticClass();
}

uint32
FOdysseyFlipbookAssetTypeActions::GetCategories()
{
    return EAssetTypeCategories::Animation | mMyAssetCategory;
}

void 
FOdysseyFlipbookAssetTypeActions::BuildBackendFilter( FARFilter & InFilter )
{
    InFilter.ClassPaths.Add( UPaperFlipbook::StaticClass()->GetClassPathName() );
}


void FOdysseyFlipbookAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor )
{
    TArray<UObject*> objects;

    for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
    {
        auto odysseyFlipbook = Cast<UPaperFlipbook>(*ObjIt);

        if (odysseyFlipbook != NULL)
        {
            FOdysseyFlipbookEditorModule* odysseyFlipbookModule = &FModuleManager::LoadModuleChecked<FOdysseyFlipbookEditorModule>("OdysseyFlipbookEditor");
            odysseyFlipbookModule->CreateOdysseyFlipbookEditor(odysseyFlipbook);
        }
    }
    if( objects.Num() != 0 )
        FAssetTypeActions_Base::OpenAssetEditor( objects, EditWithinLevelEditor );
}

#undef LOCTEXT_NAMESPACE
