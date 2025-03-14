// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyAnimationAssetTypeActions.h"

#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "OdysseyPainterEditorModule.h"
#include "OdysseyAnimation.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

//////////////////////////////////////////////////////////////////////////
// FOdysseyAnimationAssetTypeActions
FOdysseyAnimationAssetTypeActions::FOdysseyAnimationAssetTypeActions( EAssetTypeCategories::Type iAssetCategory )
    : mMyAssetCategory( iAssetCategory )
{
}

FText
FOdysseyAnimationAssetTypeActions::GetName() const
{
    return LOCTEXT( "asset-type-actions.name", "Animation" );
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
    return mMyAssetCategory;
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
        auto animation = Cast<UOdysseyAnimation>(*ObjIt);
        if (!animation)
            continue;

        FOdysseyPainterEditorModule* painterEditorModule = &FModuleManager::GetModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
        painterEditorModule->OpenStandaloneEditorForAsset(animation);
    }
}

#undef LOCTEXT_NAMESPACE
