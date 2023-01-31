// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTextureAssetTypeActions.h"

#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "Interfaces/ITextureEditorModule.h"
#include "OdysseyTextureLayerStackUserData.h"
#include "OdysseyTextureEditorModule.h"
#include "OdysseyTextureEditorSettings.h"
#include "OdysseyTexture.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureAssetTypeActions"

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
    return FColor( 200, 50, 50 );
}

UClass*
FOdysseyTextureAssetTypeActions::GetSupportedClass() const
{
    return UTexture2D::StaticClass();
}

uint32
FOdysseyTextureAssetTypeActions::GetCategories()
{
    return EAssetTypeCategories::Textures | mMyAssetCategory;
}

void 
FOdysseyTextureAssetTypeActions::BuildBackendFilter( FARFilter & InFilter )
{
    InFilter.ClassPaths.Add( UTexture2D::StaticClass()->GetClassPathName() );
}


void FOdysseyTextureAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor )
{    
    EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto odysseyTexture = Cast<UTexture2D>(*ObjIt);
		if (odysseyTexture != NULL)
		{
            if( UOdysseyTextureEditorSettings::Get()->IliadDefaultEditorEnabled )
            {
                FOdysseyTextureEditorModule* odysseyTextureModule = &FModuleManager::LoadModuleChecked<FOdysseyTextureEditorModule>("OdysseyTextureEditor");
                odysseyTextureModule->CreateOdysseyTextureEditor(odysseyTexture);
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
