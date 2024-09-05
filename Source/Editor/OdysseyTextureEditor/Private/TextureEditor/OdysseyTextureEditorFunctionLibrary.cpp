// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTextureEditorFunctionLibrary.h"
#include "OdysseyLayerStack.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "OdysseyTextureFactory.h"

UOdysseyTextureFactory*
UOdysseyTextureEditorFunctionLibrary::GetTextureFactory()
{
	static UOdysseyTextureFactory* textureFactory = nullptr;
	if (!textureFactory)
	{
		textureFactory = NewObject<UOdysseyTextureFactory>();
		textureFactory->AddToRoot();
	}
	return textureFactory;
}

UTexture2D*
UOdysseyTextureEditorFunctionLibrary::CreateTextureAsset(FString AssetName, FString PackagePath, int Width, int Height, EOdysseyTextureSourceFormat Format)
{
	if (AssetName.IsEmpty())
		return nullptr;

	if (PackagePath.IsEmpty())
		return nullptr;

	if (Width <= 0 || Height <= 0)
		return nullptr;

	FOdysseyTextureConfiguration configuration;
	configuration.Name = FName(*AssetName);
	configuration.Width = Width;
	configuration.Height = Height;
	configuration.Format = Format;
	configuration.LayerType = EOdysseyTextureDefaultLayerType::kNone;

	GetTextureFactory()->SetConfiguration(configuration);

	IAssetTools& assetTools = FAssetToolsModule::GetModule().Get();
	UTexture2D* Texture = Cast<UTexture2D>(
		assetTools.CreateAsset(
			AssetName,
			PackagePath,
			UTexture2D::StaticClass(),
			GetTextureFactory()
		)
	);

	return Texture;
}