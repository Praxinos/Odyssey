// IDDN.FR.001.060015.015.S.X.2019.000.00000
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
    FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
    mPaper2DTypeAction = AssetToolsModule.Get().GetAssetTypeActionsForClass(UPaperFlipbook::StaticClass() ).Pin();
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


void
FOdysseyFlipbookAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor )
{
    if( UOdysseyFlipbookEditorSettings::Get()->DefaultFlipbookEditor == EOdysseyDefaultFlipbookEditor::OdysseyPainterEditor)
    {
        OpenOdysseyAssetEditor(InObjects);
    }
    else if (UOdysseyFlipbookEditorSettings::Get()->DefaultFlipbookEditor == EOdysseyDefaultFlipbookEditor::UnrealDefaultEditor)
    {
        OpenPaper2DAssetEditor(InObjects);
    }
}

void
FOdysseyFlipbookAssetTypeActions::OpenOdysseyAssetEditor(const TArray<UObject*>& InObjects)
{
    for (UObject* object : InObjects)
    {
        auto flipbook = Cast<UPaperFlipbook>(object);
        if (!flipbook)
            continue;

        FOdysseyPainterEditorModule* painterEditorModule = &FModuleManager::GetModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
        painterEditorModule->OpenStandaloneEditorForAsset(flipbook);
    }
}

void
FOdysseyFlipbookAssetTypeActions::OpenPaper2DAssetEditor(const TArray<UObject*>& InObjects)
{
    mPaper2DTypeAction->OpenAssetEditor(InObjects, nullptr);
}

#undef LOCTEXT_NAMESPACE
