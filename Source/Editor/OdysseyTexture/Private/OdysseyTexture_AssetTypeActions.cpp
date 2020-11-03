// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTexture_AssetTypeActions.h"

#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
// #include "OdysseyTextureEditorModule.h"
#include "OdysseyTextureAssetUserData.h"


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

void 
FOdysseyTextureAssetTypeActions::BuildBackendFilter( FARFilter & InFilter )
{
    InFilter.ClassNames.Add( UTexture2D::StaticClass()->GetFName());
}

//Works, but suppress the normal editor of UTexture2D, need to find a better solution to keep both editors
/*
void FOdysseyTextureAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor )
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;
    TArray<UObject*> noUserDataObjects;
    
	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto odysseyTexture = Cast<UTexture2D>(*ObjIt);
		if (odysseyTexture != NULL)
		{
            UOdysseyTextureAssetUserData* userData = Cast<UOdysseyTextureAssetUserData>(odysseyTexture->GetAssetUserDataOfClass(UOdysseyTextureAssetUserData::StaticClass()));
            
            if( userData )
            {
                FOdysseyTextureEditorModule* odysseyTextureModule = &FModuleManager::LoadModuleChecked<FOdysseyTextureEditorModule>("OdysseyTextureEditor");
                odysseyTextureModule->CreateOdysseyTextureEditor(Mode, EditWithinLevelEditor, odysseyTexture);
            }
            else
            {
                noUserDataObjects.Add(odysseyTexture);
            }
		}
	}
    
    if( noUserDataObjects.Num() != 0 )
        FAssetTypeActions_Base::OpenAssetEditor( noUserDataObjects, EditWithinLevelEditor );
}*/

#undef LOCTEXT_NAMESPACE
