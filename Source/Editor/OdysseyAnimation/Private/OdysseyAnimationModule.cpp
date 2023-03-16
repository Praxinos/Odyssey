// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationModule.h"

#include "IMediaModule.h"

#define LOCTEXT_NAMESPACE "OdysseyAnimationModule"

void FOdysseyAnimationModule::StartupModule()
{
	RegisterAssetTypeActions();
	RegisterMedia();
}

void FOdysseyAnimationModule::ShutdownModule()
{
	UnregisterAssetTypeActions();
	UnregisterMedia();
}

void
FOdysseyAnimationModule::RegisterAssetTypeActions()
{
	IAssetTools& assetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	// Create Asset Categories
	EAssetTypeCategories::Type category = assetTools.RegisterAdvancedAssetCategory(FName(TEXT("ILIAD")), LOCTEXT("IliadPainterAssetCategory", "ILIAD"));

	//Create Asset Types Actions
	mIliadTypeActions = MakeShareable(new FOdysseyAnimationAssetTypeActions(category));

	//Register created Asset Type Actions
	assetTools.RegisterAssetTypeActions(mIliadTypeActions.ToSharedRef());
}

void
FOdysseyAnimationModule::RegisterMedia()
{
	// register player factory
	auto MediaModule = FModuleManager::LoadModulePtr<IMediaModule>("Media");

	if (MediaModule != nullptr)
		MediaModule->RegisterPlayerFactory(mAnimationMediaPlayerFactory);
}

void
FOdysseyAnimationModule::UnregisterAssetTypeActions()
{
	if (!FModuleManager::Get().IsModuleLoaded("AssetTools"))
		return;
	
	IAssetTools& assetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
    assetTools.UnregisterAssetTypeActions(mIliadTypeActions.ToSharedRef());
}

void
FOdysseyAnimationModule::UnregisterMedia()
{
	// unregister player factory
	auto MediaModule = FModuleManager::GetModulePtr<IMediaModule>("Media");

	if (MediaModule != nullptr)
		MediaModule->UnregisterPlayerFactory(mAnimationMediaPlayerFactory);
}

IMPLEMENT_MODULE(FOdysseyAnimationModule, OdysseyAnimation);

#undef LOCTEXT_NAMESPACE
