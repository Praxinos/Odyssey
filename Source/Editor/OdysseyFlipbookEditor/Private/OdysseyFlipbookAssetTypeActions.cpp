// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyFlipbookAssetTypeActions.h"

#include "PaperFlipbook.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "OdysseyFlipbookEditorModule.h"
#include "OdysseyFlipbook.h"
#include "OdysseyFlipbookWrapper.h"
#include "OdysseyTextureAssetUserData.h"

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
    return EAssetTypeCategories::Animation | mMyAssetCategory;
}

void 
FOdysseyFlipbookAssetTypeActions::BuildBackendFilter( FARFilter & InFilter )
{
    InFilter.ClassNames.Add( UPaperFlipbook::StaticClass()->GetFName());
}

//Works, but suppress the normal editor of Paper2DFlipbook, need to find a better solution to keep both editors

void FOdysseyFlipbookAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor )
{
    for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
    {
       auto odysseyFlipbook = Cast<UPaperFlipbook>(*ObjIt);
        if (odysseyFlipbook != NULL)
        {
            FOdysseyFlipbookEditorModule* odysseyFlipbookModule = &FModuleManager::LoadModuleChecked<FOdysseyFlipbookEditorModule>("OdysseyFlipbookEditor");
            odysseyFlipbookModule->CreateOdysseyFlipbookEditor(odysseyFlipbook);
        }
    }
    /*
    TArray<UObject*> noUserDataObjects;
    
    for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
    {
        auto odysseyFlipbook = Cast<UPaperFlipbook>(*ObjIt);

        if (odysseyFlipbook != NULL)
        {
            FOdysseyFlipbookWrapper odysseyFlipbookWrapper( odysseyFlipbook );
            UTexture2D* texture2D = odysseyFlipbookWrapper.GetKeyframeTexture(0);  //  BUG Keyframe 0 = NULL ??
            UOdysseyTextureAssetUserData* userData = Cast<UOdysseyTextureAssetUserData>(texture2D->GetAssetUserDataOfClass(UOdysseyTextureAssetUserData::StaticClass()));
            if( userData )
            {
                FOdysseyFlipbookEditorModule* odysseyFlipbookModule = &FModuleManager::LoadModuleChecked<FOdysseyFlipbookEditorModule>("OdysseyFlipbookEditor");
                odysseyFlipbookModule->CreateOdysseyFlipbookEditor(odysseyFlipbook);
            }
            else
            {
                noUserDataObjects.Add(odysseyFlipbook);
            }
        }
    }
    
    if( noUserDataObjects.Num() != 0 )
        FAssetTypeActions_Base::OpenAssetEditor( noUserDataObjects, EditWithinLevelEditor );
*/
}

#undef LOCTEXT_NAMESPACE
