// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTextureEditorFunctionLibrary.h"
#include "OdysseyLayerStack.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "OdysseyTextureFactory.h"
#include "Factories/Texture2dFactoryNew.h"

UTexture2D*
UOdysseyTextureEditorFunctionLibrary::CreateTextureAsset(FString AssetName, FString PackagePath, int Width, int Height, EOdysseyTextureSourceFormat Format)
{
	if (AssetName.IsEmpty())
		return nullptr;

	if (PackagePath.IsEmpty())
		return nullptr;

	if (Width <= 0 || Height <= 0)
		return nullptr;

	IAssetTools& assetTools = FAssetToolsModule::GetModule().Get();
	UTexture2D* Texture = Cast<UTexture2D>(
		assetTools.CreateAsset(
			AssetName,
			PackagePath,
			UTexture2D::StaticClass(),
			UTexture2DFactoryNew::StaticClass()->GetDefaultObject<UFactory>()
		)
	);
	return Texture;
}