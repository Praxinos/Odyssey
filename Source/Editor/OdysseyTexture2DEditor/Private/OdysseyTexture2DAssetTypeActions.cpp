// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTexture2DAssetTypeActions.h"

#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "Interfaces/ITextureEditorModule.h"
#include "OdysseyTextureAssetUserData.h"
#include "OdysseyTexture2DEditorModule.h"
#include "OdysseyTexture2DEditorSettings.h"
#include "OdysseyTexture.h"

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


void FOdysseyTexture2DAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor )
{    
    EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto odysseyTexture = Cast<UTexture2D>(*ObjIt);
		if (odysseyTexture != NULL)
		{
            if( UOdysseyTexture2DEditorSettings::Get()->IliadDefaultEditorEnabled )
            {
                FOdysseyTexture2DEditorModule* odysseyTextureModule = &FModuleManager::LoadModuleChecked<FOdysseyTexture2DEditorModule>("OdysseyTexture2DEditor");
                odysseyTextureModule->CreateOdysseyTexture2DEditor(odysseyTexture);
            }
            else
            {
                ITextureEditorModule* TextureEditorModule = &FModuleManager::LoadModuleChecked<ITextureEditorModule>("TextureEditor");
                TextureEditorModule->CreateTextureEditor(Mode, EditWithinLevelEditor, odysseyTexture);
            }
		}
	}
}

#undef LOCTEXT_NAMESPACE
