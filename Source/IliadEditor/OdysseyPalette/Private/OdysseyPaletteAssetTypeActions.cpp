// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyPaletteAssetTypeActions.h"

#include "OdysseyPalette.h"
#include "PaletteEditor/OdysseyPaletteEditorToolkit.h"

#define LOCTEXT_NAMESPACE "Palette"

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

void
FOdysseyPaletteAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
    UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();

    for( auto object : InObjects )
    {
        if (!object || !object->IsA<UOdysseyPalette>())
            continue;

        if (AssetEditorSubsystem->FindEditorForAsset(object, true) != nullptr)
            continue;

        UOdysseyPalette* palette = Cast< UOdysseyPalette >( object );
        TSharedPtr<FOdysseyPaletteEditorToolkit> toolkit = MakeShared<FOdysseyPaletteEditorToolkit>(palette);
        toolkit->Open();
    }
}

#undef LOCTEXT_NAMESPACE
