// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTextureFunctionLibrary.h"
#include "Engine/Texture2D.h"

bool
UOdysseyTextureFunctionLibrary::HasLayerStack(UTexture2D* iTexture)
{
	return !!GetLayerStack(iTexture);
}

UOdysseyTextureLayerStack*
UOdysseyTextureFunctionLibrary::GetLayerStack(UTexture2D* iTexture)
{
	if ( !iTexture )
		return nullptr;

	UOdysseyTextureLayerStackUserData* userData = Cast<UOdysseyTextureLayerStackUserData>(iTexture->GetAssetUserDataOfClass(UOdysseyTextureLayerStackUserData::StaticClass()));
	if (!userData)
		return nullptr;
	
	return userData->GetLayerStack();
}