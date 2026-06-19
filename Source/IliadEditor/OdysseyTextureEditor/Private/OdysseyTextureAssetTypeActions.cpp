// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTextureAssetTypeActions.h"

#include "ContentBrowserModule.h"
#include "Engine/texture2D.h"
#include "IContentBrowserSingleton.h"
#include "Interfaces/ITextureEditorModule.h"

#include "OdysseyPainterEditorModule.h"
#include "OdysseyStyle.h"
#include "OdysseyTextureEditorSettings.h"

#define LOCTEXT_NAMESPACE "TextureEditor"

//////////////////////////////////////////////////////////////////////////
// FOdysseyTextureAssetTypeActions
FOdysseyTextureAssetTypeActions::FOdysseyTextureAssetTypeActions( EAssetTypeCategories::Type iAssetCategory )
    : mMyAssetCategory( iAssetCategory )
{
    // will store default type actions for UTexture2D so that we can call the default actions for the type.
    // it has to be done when the module is loaded first.
    // see https://forums.unrealengine.com/t/extending-asset-type-actions/56902
    static TSharedPtr<IAssetTypeActions> defaultTypeActions = nullptr;

    // must be inited only once, hence the static status;
    if ( defaultTypeActions == nullptr )
    {
        IAssetTools& assetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

        defaultTypeActions = assetTools.GetAssetTypeActionsForClass( GetSupportedClass() ).Pin();
    }

    mDefaultTypeActions = defaultTypeActions;
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
    UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
    for (UObject* object : InObjects)
    {
        UTexture2D* texture = Cast<UTexture2D>(object);
        if (!texture)
            continue;

        if (AssetEditorSubsystem->FindEditorForAsset(texture, true))
            continue;

        if( UOdysseyTextureEditorSettings::Get()->DefaultTextureEditor == EOdysseyDefaultTextureEditor::OdysseyPainterEditor)
        {
            FOdysseyPainterEditorModule* painterEditorModule = &FModuleManager::GetModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
            painterEditorModule->OpenStandaloneEditorForAsset(texture);
        }
        else if (UOdysseyTextureEditorSettings::Get()->DefaultTextureEditor == EOdysseyDefaultTextureEditor::UnrealDefaultEditor)
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

bool
FOdysseyTextureAssetTypeActions::IsImportedAsset() const
{
    return mDefaultTypeActions.IsValid() ? mDefaultTypeActions->IsImportedAsset() : false;
}

#undef LOCTEXT_NAMESPACE
