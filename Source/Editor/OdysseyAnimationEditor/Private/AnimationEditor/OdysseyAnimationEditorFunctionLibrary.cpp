// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationEditorFunctionLibrary.h"
#include "OdysseyLayerStack.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "OdysseyAnimationFactory.h"

UOdysseyAnimation*
UOdysseyAnimationEditorFunctionLibrary::CreateAnimationAsset(FString AssetName, FString PackagePath, int Width, int Height, EOdysseyAnimationFormat Format, float FramesPerSecond)
{
	if (AssetName.IsEmpty())
		return nullptr;

	if (PackagePath.IsEmpty())
		return nullptr;

	if (Width <= 0 || Height <= 0)
		return nullptr;

	if (FramesPerSecond < 0.f)
		return nullptr;

	IAssetTools& assetTools = FAssetToolsModule::GetModule().Get();
	UOdysseyAnimation* animation = Cast<UOdysseyAnimation>(
		assetTools.CreateAsset(
			AssetName,
			PackagePath,
			UOdysseyAnimation::StaticClass(),
			UOdysseyAnimationFactory::StaticClass()->GetDefaultObject<UFactory>()
		)
	);

	FOdysseyAnimationConfiguration conf;
	conf.Width = Width;
	conf.Height = Height;
	conf.Format = Format;
	conf.FramesPerSecond = FramesPerSecond;

	animation->Init(conf);

	return animation;
}