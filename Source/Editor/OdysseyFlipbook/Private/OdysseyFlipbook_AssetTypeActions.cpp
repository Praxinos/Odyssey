// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyFlipbook_AssetTypeActions.h"

#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
// #include "OdysseyFlipbookEditorModule.h"


#include "OdysseyFlipbook.h"

#define LOCTEXT_NAMESPACE "OdysseyFlipbook_AssetTypeActions"

//////////////////////////////////////////////////////////////////////////
// FOdysseyFlipbookAssetTypeActions
FOdysseyFlipbookAssetTypeActions::FOdysseyFlipbookAssetTypeActions( EAssetTypeCategories::Type iAssetCategory )
    : mMyAssetCategory( iAssetCategory )
{
}

FText
FOdysseyFlipbookAssetTypeActions::GetName() const
{
    return LOCTEXT( "FOdysseyFlipbookTypeActionsName", "Flipbook" );
}

FColor
FOdysseyFlipbookAssetTypeActions::GetTypeColor() const
{
    return FColor( 255, 0, 0 );
}

UClass*
FOdysseyFlipbookAssetTypeActions::GetSupportedClass() const
{
    return UOdysseyFlipbook::StaticClass();
}

uint32
FOdysseyFlipbookAssetTypeActions::GetCategories()
{
    return /* EAssetTypeCategories::Animation | */ mMyAssetCategory;
}

//Works, but suppress the normal editor of UFlipbook2D, need to find a better solution to keep both editors
/*
void FOdysseyFlipbookAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor )
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;
    TArray<UObject*> noUserDataObjects;
    
	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto odysseyFlipbook = Cast<UFlipbook2D>(*ObjIt);
		if (odysseyFlipbook != NULL)
		{
            UOdysseyFlipbookAssetUserData* userData = Cast<UOdysseyFlipbookAssetUserData>(odysseyFlipbook->GetAssetUserDataOfClass(UOdysseyFlipbookAssetUserData::StaticClass()));
            
            if( userData )
            {
                FOdysseyFlipbookEditorModule* odysseyFlipbookModule = &FModuleManager::LoadModuleChecked<FOdysseyFlipbookEditorModule>("OdysseyFlipbookEditor");
                odysseyFlipbookModule->CreateOdysseyFlipbookEditor(Mode, EditWithinLevelEditor, odysseyFlipbook);
            }
            else
            {
                noUserDataObjects.Add(odysseyFlipbook);
            }
		}
	}
    
    if( noUserDataObjects.Num() != 0 )
        FAssetTypeActions_Base::OpenAssetEditor( noUserDataObjects, EditWithinLevelEditor );
}*/

#undef LOCTEXT_NAMESPACE
