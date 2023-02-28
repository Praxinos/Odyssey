// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationAssetTypeActions.h"

#include "OdysseyAnimation.h"

#define LOCTEXT_NAMESPACE "OdysseyAnimationAssetTypeActions"

//////////////////////////////////////////////////////////////////////////
// FOdysseyAnimationAssetTypeActions
FOdysseyAnimationAssetTypeActions::FOdysseyAnimationAssetTypeActions( EAssetTypeCategories::Type iCategory )
    : mCategory( iCategory )
{
}

FText
FOdysseyAnimationAssetTypeActions::GetName() const
{
    return LOCTEXT( "FOdysseyAnimationTypeActionsName", "2D Animation" );
}

FColor
FOdysseyAnimationAssetTypeActions::GetTypeColor() const
{
    return FColor( 200, 50, 50 );
}

UClass*
FOdysseyAnimationAssetTypeActions::GetSupportedClass() const
{
    return UOdysseyAnimation::StaticClass();
}

uint32
FOdysseyAnimationAssetTypeActions::GetCategories()
{
    return mCategory;
}

void 
FOdysseyAnimationAssetTypeActions::BuildBackendFilter( FARFilter & InFilter )
{
    InFilter.ClassNames.Add( UOdysseyAnimation::StaticClass()->GetFName());
}


void FOdysseyAnimationAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor )
{    
    EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto odysseyAnimation = Cast<UOdysseyAnimation>(*ObjIt);
		if (odysseyAnimation == NULL)
            continue;
		
        // FOdysseyAnimationEditorModule* odysseyAnimationEditorModule = &FModuleManager::LoadModuleChecked<FOdysseyAnimationEditorModule>("OdysseyAnimationEditor");
        // odysseyAnimationEditorModule->CreateOdysseyAnimationEditor(odysseyAnimation);
	}
}

#undef LOCTEXT_NAMESPACE
