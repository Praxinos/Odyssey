// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyTextureAssetTypeActions.h"

#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "Interfaces/ITextureEditorModule.h"
#include "OdysseyPainterEditorModule.h"
#include "Settings/OdysseyTextureEditorSettings.h"
#include "OdysseyTexture.h"
#include "OdysseyStyle.h"

#define LOCTEXT_NAMESPACE "TextureEditor"

//////////////////////////////////////////////////////////////////////////
// FOdysseyTextureAssetTypeActions
FOdysseyTextureAssetTypeActions::FOdysseyTextureAssetTypeActions( EAssetTypeCategories::Type iAssetCategory )
    : mMyAssetCategory( iAssetCategory )
{
}

FText
FOdysseyTextureAssetTypeActions::GetName() const
{
    return LOCTEXT( "asset-type-actions.name", "Texture" );
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

    for (UObject* object : InObjects)
    {
        UTexture2D* texture = Cast<UTexture2D>(object);
        if (!texture)
            continue;

        if( UOdysseyTextureEditorSettings::Get()->OdysseyDefaultEditorEnabled )
        {
            FOdysseyPainterEditorModule* painterEditorModule = &FModuleManager::GetModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
            painterEditorModule->OpenStandaloneEditorForAsset(texture);
        }
        else
        {
            ITextureEditorModule* TextureEditorModule = &FModuleManager::LoadModuleChecked<ITextureEditorModule>("TextureEditor");
            TextureEditorModule->CreateTextureEditor(Mode, EditWithinLevelEditor, texture);
        }
    }
}

const FSlateBrush*
FOdysseyTextureAssetTypeActions::GetThumbnailBrush(const FAssetData& InAssetData, const FName InClassName) const
{
    return FOdysseyStyle::GetBrush("ClassThumbnail.OdysseyTexture");
}

const FSlateBrush*
FOdysseyTextureAssetTypeActions::GetIconBrush(const FAssetData& InAssetData, const FName InClassName) const
{
    return FOdysseyStyle::GetBrush("ClassIcon.OdysseyTexture");
}

#undef LOCTEXT_NAMESPACE
