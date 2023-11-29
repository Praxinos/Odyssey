// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPaletteAssetTypeActions.h"

#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "OdysseyPalette.h"

#define LOCTEXT_NAMESPACE "OdysseyPaletteAssetTypeActions"

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

/*
void FOdysseyPaletteAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor )
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
}*/

#undef LOCTEXT_NAMESPACE
