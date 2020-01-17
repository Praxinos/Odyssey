// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyTexture.h"


UOdysseyTexture::UOdysseyTexture(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UOdysseyTexture::Serialize(FArchive& Ar)
{
}

#if WITH_EDITOR
void UOdysseyTexture::PostLinkerChange()
{
    
}

void UOdysseyTexture::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    
}
#endif // WITH_EDITOR


void UOdysseyTexture::BeginDestroy()
{
	Super::BeginDestroy();
}

bool UOdysseyTexture::IsReadyForAsyncPostLoad() const
{
    return false;
}
	
void UOdysseyTexture::PostLoad()
{
    
}

void UOdysseyTexture::PreSave(const class ITargetPlatform* TargetPlatform)
{
    
}
	 
void UOdysseyTexture::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
{
}

FString UOdysseyTexture::GetDesc()
{
    return FString();
}

FTextureResource* UOdysseyTexture::CreateResource()
{
    return NULL;
}

EMaterialValueType UOdysseyTexture::GetMaterialType() const
{
    return MCT_Float1;
}
	 
void UOdysseyTexture::UpdateResource()
{
    
}
	
float UOdysseyTexture::GetAverageBrightness(bool bIgnoreTrueBlack, bool bUseGrayscale)
{
    return 0.f;
}


	
uint32 UOdysseyTexture::CalcTextureMemorySizeEnum( ETextureMipCount Enum ) const
{
    return 0;
}
    
void UOdysseyTexture::WaitForStreaming()
{
    
}

void UOdysseyTexture::GetResourceSizeEx(FResourceSizeEx& CumulativeResourceSize)
{
    
}

void UOdysseyTexture::PostEditUndo()
{
    
}
bool UOdysseyTexture::IsCurrentlyVirtualTextured() const
{
    return false;
}

