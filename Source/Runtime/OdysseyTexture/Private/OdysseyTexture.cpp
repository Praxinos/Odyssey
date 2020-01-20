// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "EngineUtils.h"

#include "OdysseyTexture.h"


UOdysseyTexture::UOdysseyTexture(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UOdysseyTexture::Serialize(FArchive& Ar)
{
    LLM_SCOPE(ELLMTag::TextureMetaData);

	Super::Serialize(Ar);
    
	FStripDataFlags StripDataFlags(Ar);
    
	bool bCooked = Ar.IsCooking();
	Ar << bCooked;

	if (Ar.IsCooking() || bCooked)
	{
		SerializeCookedPlatformData(Ar);
	}

#if WITH_EDITOR
	if (Ar.IsLoading() && !Ar.IsTransacting() && !bCooked && !GetOutermost()->HasAnyPackageFlags(PKG_ReloadingForCooker))
	{
		// The composite texture may not have been loaded yet. We have to defer caching platform
		// data until post load.
		if (CompositeTexture == NULL || CompositeTextureMode == CTM_Disabled)
		{
			BeginCachePlatformData();
		}
	}
#endif // #if WITH_EDITOR
}

#if WITH_EDITOR
void UOdysseyTexture::PostLinkerChange()
{
	// Changing the linker requires re-creating the resource to make sure streaming behavior is right.
	if( !HasAnyFlags( RF_BeginDestroyed | RF_NeedLoad | RF_NeedPostLoad ) && !IsUnreachable() )
	{
		// Update the resource.
		UpdateResource();
	}
}

void UOdysseyTexture::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
#if WITH_EDITORONLY_DATA
	if (!Source.IsPowerOfTwo() && (PowerOfTwoMode == ETexturePowerOfTwoSetting::None))
	{
		// Force NPT textures to have no mipmaps.
		MipGenSettings = TMGS_NoMipmaps;
		NeverStream = true;
		if (VirtualTextureStreaming)
		{
			UE_LOG(LogTexture, Warning, TEXT("VirtualTextureStreaming not supported for \"%s\", texture size is not a power-of-2"), *GetName());
			VirtualTextureStreaming = false;
		}
	}

	// Make sure settings are correct for LUT textures.
	if(LODGroup == TEXTUREGROUP_ColorLookupTable)
	{
		MipGenSettings = TMGS_NoMipmaps;
		SRGB = false;
	}
#endif // #if WITH_EDITORONLY_DATA

	if (VirtualTextureStreaming &&
		(PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UTexture2D, AddressX)
		|| PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UTexture2D, AddressY) )
		)
	{
		//When using VT, changing the address mode should trigger a recompile
		NotifyMaterials();
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
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
#if WITH_EDITOR
	if (FApp::CanEverRender())
	{
		FinishCachePlatformData();
	}
#endif // #if WITH_EDITOR

	// Route postload, which will update bIsStreamable as UTexture::PostLoad calls UpdateResource.
	Super::PostLoad();
}

void UOdysseyTexture::PreSave(const class ITargetPlatform* TargetPlatform)
{
    Super::PreSave(TargetPlatform);
}
	 
void UOdysseyTexture::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
{
	FIntPoint SourceSize(0, 0);
#if WITH_EDITOR
	SourceSize = Source.GetLogicalSize();
#endif

	const FString DimensionsStr = FString::Printf(TEXT("%dx%d"), SourceSize.X, SourceSize.Y);
	OutTags.Add( FAssetRegistryTag("Dimensions", DimensionsStr, FAssetRegistryTag::TT_Dimensional) );
	//OutTags.Add( FAssetRegistryTag("HasAlphaChannel", HasAlphaChannel() ? TEXT("True") : TEXT("False"), FAssetRegistryTag::TT_Alphabetical) );
	//OutTags.Add( FAssetRegistryTag("Format", GPixelFormats[GetPixelFormat()].Name, FAssetRegistryTag::TT_Alphabetical) );

	Super::GetAssetRegistryTags(OutTags);
}

FString UOdysseyTexture::GetDesc()
{
    return FString();
}

FTextureResource* UOdysseyTexture::CreateResource()
{
    //the real resource of the texture, to do later correctly
    return NULL;
}

EMaterialValueType UOdysseyTexture::GetMaterialType() const
{
	if (VirtualTextureStreaming)
	{
		return MCT_TextureVirtual;
	}
	return MCT_Texture2D;
}
	 
void UOdysseyTexture::UpdateResource()
{
    //To do correctly
}
	
float UOdysseyTexture::GetAverageBrightness(bool bIgnoreTrueBlack, bool bUseGrayscale)
{
    //To do correctly
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

