// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyFlipbookAssetTypeActions.h"

#include "PaperFlipbook.h"
#include "OdysseyPainterEditorModule.h"
#include "OdysseyFlipbookEditorSettings.h"

#define LOCTEXT_NAMESPACE "FlipbookEditor"

//////////////////////////////////////////////////////////////////////////
// FOdysseyFlipbookAssetTypeActions
FOdysseyFlipbookAssetTypeActions::FOdysseyFlipbookAssetTypeActions( EAssetTypeCategories::Type iAssetCategory )
    : mMyAssetCategory( iAssetCategory )
{
}

FText
FOdysseyFlipbookAssetTypeActions::GetName() const
{
    return LOCTEXT( "asset-type-actions.name", "Flipbook" );
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
    for (UObject* object : InObjects)
    {
        auto flipbook = Cast<UPaperFlipbook>(object);
        if (!flipbook)
            continue;

        if( UOdysseyFlipbookEditorSettings::Get()->DefaultFlipbookEditor == EOdysseyDefaultFlipbookEditor::OdysseyPainterEditor)
        {
            FOdysseyPainterEditorModule* painterEditorModule = &FModuleManager::GetModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
            painterEditorModule->OpenStandaloneEditorForAsset(flipbook);
        }
        else if (UOdysseyFlipbookEditorSettings::Get()->DefaultFlipbookEditor == EOdysseyDefaultFlipbookEditor::UnrealDefaultEditor)
        {
            FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
            TArray<TWeakPtr<IAssetTypeActions>> assetTypeActions = AssetToolsModule.Get().GetAssetTypeActionsListForClass(UPaperFlipbook::StaticClass());
            assetTypeActions.Remove(AsShared());

            if (assetTypeActions.IsEmpty())
                continue;

            TSharedPtr<IAssetTypeActions> paper2DAssetTypeAction = assetTypeActions[0].Pin();
            if (!paper2DAssetTypeAction)
                continue;

            paper2DAssetTypeAction->OpenAssetEditor(InObjects, EditWithinLevelEditor );
        }
    }
}

#undef LOCTEXT_NAMESPACE
