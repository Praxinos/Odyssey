// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTexture2DAssetTypeActions.h"

#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "OdysseyTextureAssetUserData.h"
#include "OdysseyTexture2DEditorModule.h"

#define LOCTEXT_NAMESPACE "OdysseyTexture2DAssetTypeActions"

//////////////////////////////////////////////////////////////////////////
// FOdysseyTexture2DAssetTypeActions
FOdysseyTexture2DAssetTypeActions::FOdysseyTexture2DAssetTypeActions( EAssetTypeCategories::Type iAssetCategory )
    : mMyAssetCategory( iAssetCategory )
{
}

FText
FOdysseyTexture2DAssetTypeActions::GetName() const
{
    return LOCTEXT( "FOdysseyTextureTypeActionsName", "Texture" );
}

FColor
FOdysseyTexture2DAssetTypeActions::GetTypeColor() const
{
    return FColor( 200, 50, 50 );
}

UClass*
FOdysseyTexture2DAssetTypeActions::GetSupportedClass() const
{
    return UTexture2D::StaticClass();
}

uint32
FOdysseyTexture2DAssetTypeActions::GetCategories()
{
    return EAssetTypeCategories::MaterialsAndTextures | mMyAssetCategory;
}

void 
FOdysseyTexture2DAssetTypeActions::BuildBackendFilter( FARFilter & InFilter )
{
    InFilter.ClassNames.Add( UTexture2D::StaticClass()->GetFName());
}

//Works, but suppress the normal editor of UTexture2D, need to find a better solution to keep both editors

void FOdysseyTexture2DAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor )
{
    TArray<UObject*> noUserDataObjects;
    
	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto odysseyTexture = Cast<UTexture2D>(*ObjIt);
		if (odysseyTexture != NULL)
		{
            UOdysseyTextureAssetUserData* userData = Cast<UOdysseyTextureAssetUserData>(odysseyTexture->GetAssetUserDataOfClass(UOdysseyTextureAssetUserData::StaticClass()));
            
            if( userData )
            {
                FOdysseyTexture2DEditorModule* odysseyTextureModule = &FModuleManager::LoadModuleChecked<FOdysseyTexture2DEditorModule>("OdysseyTexture2DEditor");
                odysseyTextureModule->CreateOdysseyTexture2DEditor(odysseyTexture);
            }
            else
            {
                noUserDataObjects.Add(odysseyTexture);
            }
		}
	}
    
    if( noUserDataObjects.Num() != 0 )
        FAssetTypeActions_Base::OpenAssetEditor( noUserDataObjects, EditWithinLevelEditor );
}

#undef LOCTEXT_NAMESPACE
