// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
#include "OdysseySurfaceEditable.h"
#include "OdysseyBlock.h"
#include "ULISLoaderModule.h"
#include "ImageCore.h"
#include "TextureCompressorModule.h"
#include "Engine/TextureLODSettings.h"
#include "Engine/TextureCube.h"
#include "Engine/Texture2DArray.h"
#include "Engine/VolumeTexture.h"
#include "VT/VirtualTextureBuildSettings.h"
#include "Interfaces/ITargetPlatformManagerModule.h"
#include "Interfaces/ITargetPlatform.h"
#include "Interfaces/ITextureFormat.h"
#include "DeviceProfiles/DeviceProfileManager.h"
#include <ULIS3>

/////////////////////////////////////////////////////
// Utlity
void
CopyUTextureSourceDataIntoBlock(FOdysseyBlock* iBlock,UTexture2D* iTexture)
{
    checkf(iBlock->Width() == iTexture->GetSizeX() &&
           iBlock->Height() == iTexture->GetSizeY()
           ,TEXT("Sizes do not match"));

    iTexture->Source.GetMipData(iBlock->GetArray(),0);
    iBlock->ResyncData();
}

void
CopyUTexturePixelDataIntoBlock(FOdysseyBlock* iBlock,UTexture2D* iTexture)
{
    checkf(iBlock->Width() == iTexture->GetSizeX() &&
           iBlock->Height() == iTexture->GetSizeY()
           ,TEXT("Sizes do not match"));

	const FTexture2DMipMap& Mip = iTexture->GetPlatformMips()[0];
	const void* Data = Mip.BulkData.LockReadOnly();
	FMemory::Memcpy(iBlock->GetArray().GetData(),
		Data,
		iBlock->GetArray().Num()
	);
	Mip.BulkData.Unlock();
	iBlock->ResyncData();
}

void
CopyBlockDataIntoUTexture(const FOdysseyBlock* iBlock,UTexture2D* iTexture)
{
    checkf(iBlock->Width() == iTexture->GetSizeX() &&
           iBlock->Height() == iTexture->GetSizeY()
           ,TEXT("Sizes do not match"));
           
    ::ul3::tFormat targetFormat = ULISFormatForUE4TextureSourceFormat(iTexture->Source.GetFormat());
	if (iBlock->Format() == targetFormat)
	{
		iTexture->Source.Init(iBlock->Width(), iBlock->Height(), 1, 1, iTexture->Source.GetFormat(), iBlock->GetBlock()->DataPtr());
		return;
	}

	::ul3::FBlock* block = new ::ul3::FBlock(iBlock->Width(), iBlock->Height(), targetFormat);

	IULISLoaderModule& hULIS = IULISLoaderModule::Get();
	::ul3::uint32 MT_bit = block->Height() > 256 ? ULIS3_PERF_MT : 0;
	::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
	::ul3::Conv(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, iBlock->GetBlock(), block);
    
	iTexture->Source.Init(block->Width(), block->Height(), 1, 1, iTexture->Source.GetFormat(), block->DataPtr());
}

void
InitTextureWithBlockData(const FOdysseyBlock* iBlock, UTexture2D* iTexture, ETextureSourceFormat iFormat)
{
	::ul3::tFormat targetFormat = ULISFormatForUE4TextureSourceFormat(iFormat);
	if (iBlock->Format() == targetFormat)
	{
		iTexture->Source.Init(iBlock->Width(), iBlock->Height(), 1, 1, iFormat, iBlock->GetBlock()->DataPtr());
		return;
	}

	::ul3::FBlock* block = new ::ul3::FBlock(iBlock->Width(), iBlock->Height(), targetFormat);

	IULISLoaderModule& hULIS = IULISLoaderModule::Get();
	::ul3::uint32 MT_bit = block->Height() > 256 ? ULIS3_PERF_MT : 0;
	::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
	::ul3::Conv(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, iBlock->GetBlock(), block);
    
	iTexture->Source.Init(block->Width(), block->Height(), 1, 1, iFormat, block->DataPtr());
}

FOdysseyBlock*
NewOdysseyBlockFromUTextureData(UTexture2D* iTexture, ::ul3::tFormat iFormat)
{
    ::ul3::tFormat sourceFormat = ULISFormatForUE4TextureSourceFormat(iTexture->Source.GetFormat());

    FOdysseyBlock* block = new FOdysseyBlock(iTexture->GetSizeX(),iTexture->GetSizeY(),sourceFormat);
	CopyUTextureSourceDataIntoBlock(block,iTexture);

    if (sourceFormat == iFormat)
        return block;

    FOdysseyBlock* ret = new FOdysseyBlock(block->Width(),block->Height(),iFormat);
    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    ::ul3::uint32 MT_bit = block->Height() > 256 ? ULIS3_PERF_MT : 0;
    ::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
    ::ul3::Conv( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, block->GetBlock(), ret->GetBlock() );
    delete block;

    return ret;
}

void
InvalidateSurfaceFromData(const FOdysseyBlock* iData,FOdysseySurfaceEditable* iSurface)
{
    InvalidateTextureFromData(iData,iSurface->Texture());
}

void
InvalidateSurfaceFromData(const FOdysseyBlock* iData,FOdysseySurfaceEditable* iSurface,int x1,int y1,int x2,int y2)
{
    InvalidateTextureFromData(iData,iSurface->Texture(),x1,y1,x2,y2);
}

void
InvalidateTextureFromData(const FOdysseyBlock* iData,UTexture2D* iTexture)
{
    InvalidateTextureFromData(iData,iTexture,0,0,iData->Width(),iData->Height());
}

void
InvalidateTextureFromData(const FOdysseyBlock* iData,UTexture2D* iTexture,const ::ul3::FRect& iRect)
{
    checkf(iData,TEXT("Error"));
    checkf(iTexture,TEXT("Error"));

    InvalidateTextureFromData(iData->GetBlock(), iTexture, iRect);
}

void
InvalidateTextureFromData(const FOdysseyBlock* iData,UTexture2D* iTexture,int x1,int y1,int x2,int y2)
{
    checkf(iData,TEXT("Error"));
    checkf(iTexture,TEXT("Error"));

    ::ul3::FRect rect = ::ul3::FRect::FromMinMax(x1, y1, x2, y2);
    InvalidateTextureFromData(iData->GetBlock(), iTexture, rect);
}

ERawImageFormat::Type
GetRawImageFormatFromTextureSourceFormat(ETextureSourceFormat iFormat)
{
    switch (iFormat)
    {
        case TSF_G8:		return ERawImageFormat::G8;
        case TSF_G16:		return ERawImageFormat::G16;
        case TSF_BGRA8:		return ERawImageFormat::BGRA8;
        case TSF_BGRE8:		return ERawImageFormat::BGRE8;
        case TSF_RGBA16:	return ERawImageFormat::RGBA16;
        case TSF_RGBA16F:	return ERawImageFormat::RGBA16F;
        default:
            //UE_LOG(LogTexture, Fatal, TEXT("Texture %s has source art in an invalid format."), *InTexture.GetName());
            return ERawImageFormat::BGRA8;
    }
    return ERawImageFormat::BGRA8;
}

//START COPIED FROM TextureDerivedData.cpp

static void FinalizeBuildSettingsForLayer(const UTexture& Texture, int32 LayerIndex, FTextureBuildSettings& OutSettings)
{
	FTextureFormatSettings FormatSettings;
	Texture.GetLayerFormatSettings(LayerIndex, FormatSettings);

	OutSettings.bHDRSource = Texture.HasHDRSource(LayerIndex);
	OutSettings.bSRGB = FormatSettings.SRGB;
	OutSettings.bApplyYCoCgBlockScale = FormatSettings.CompressionYCoCg;

	if (FormatSettings.CompressionSettings == TC_Displacementmap || FormatSettings.CompressionSettings == TC_DistanceFieldFont)
	{
		OutSettings.bReplicateAlpha = true;
	}
	else if (FormatSettings.CompressionSettings == TC_Grayscale || FormatSettings.CompressionSettings == TC_Alpha)
	{
		OutSettings.bReplicateRed = true;
	}
}

/**
 * Sets texture build settings.
 * @param Texture - The texture for which to build compressor settings.
 * @param OutBuildSettings - Build settings.
 */
static void GetTextureBuildSettings(
	const UTexture& Texture,
	const UTextureLODSettings& TextureLODSettings,
	bool bPlatformSupportsTextureStreaming,
	bool bPlatformSupportsVirtualTextureStreaming,
	FTextureBuildSettings& OutBuildSettings
	)
{
	OutBuildSettings.ColorAdjustment.AdjustBrightness = Texture.AdjustBrightness;
	OutBuildSettings.ColorAdjustment.AdjustBrightnessCurve = Texture.AdjustBrightnessCurve;
	OutBuildSettings.ColorAdjustment.AdjustVibrance = Texture.AdjustVibrance;
	OutBuildSettings.ColorAdjustment.AdjustSaturation = Texture.AdjustSaturation;
	OutBuildSettings.ColorAdjustment.AdjustRGBCurve = Texture.AdjustRGBCurve;
	OutBuildSettings.ColorAdjustment.AdjustHue = Texture.AdjustHue;
	OutBuildSettings.ColorAdjustment.AdjustMinAlpha = Texture.AdjustMinAlpha;
	OutBuildSettings.ColorAdjustment.AdjustMaxAlpha = Texture.AdjustMaxAlpha;
	OutBuildSettings.bUseLegacyGamma = Texture.bUseLegacyGamma;
	OutBuildSettings.bPreserveBorder = Texture.bPreserveBorder;
	OutBuildSettings.bDitherMipMapAlpha = Texture.bDitherMipMapAlpha;
	OutBuildSettings.AlphaCoverageThresholds = Texture.AlphaCoverageThresholds;
	OutBuildSettings.bComputeBokehAlpha = (Texture.LODGroup == TEXTUREGROUP_Bokeh);
	OutBuildSettings.bReplicateAlpha = false;
	OutBuildSettings.bReplicateRed = false;
	OutBuildSettings.bVolume = false;
	OutBuildSettings.bCubemap = false;
	OutBuildSettings.bTextureArray = false;

	if (Texture.MaxTextureSize > 0)
	{
		OutBuildSettings.MaxTextureResolution = Texture.MaxTextureSize;
	}

	if (Texture.IsA(UTextureCube::StaticClass()))
	{
		OutBuildSettings.bCubemap = true;
		OutBuildSettings.DiffuseConvolveMipLevel = GDiffuseConvolveMipLevel;
		const UTextureCube* Cube = CastChecked<UTextureCube>(&Texture);
		OutBuildSettings.bLongLatSource = (Cube->Source.GetNumSlices() == 1);
		if (OutBuildSettings.bLongLatSource && Texture.MaxTextureSize <= 0)
		{
			// long/lat source use 512 as default
			OutBuildSettings.MaxTextureResolution = 512;
		}
	}
	else if (Texture.IsA(UTexture2DArray::StaticClass()))
	{
		OutBuildSettings.bTextureArray = true;
		OutBuildSettings.DiffuseConvolveMipLevel = 0;
		OutBuildSettings.bLongLatSource = false;
	}
	else if (Texture.IsA(UVolumeTexture::StaticClass()))
	{
		OutBuildSettings.bVolume = true;
		OutBuildSettings.DiffuseConvolveMipLevel = 0;
		OutBuildSettings.bLongLatSource = false;
	}
	else
	{
		OutBuildSettings.DiffuseConvolveMipLevel = 0;
		OutBuildSettings.bLongLatSource = false;
	}

	bool bDownsampleWithAverage;
	bool bSharpenWithoutColorShift;
	bool bBorderColorBlack;
	TextureMipGenSettings MipGenSettings;
	TextureLODSettings.GetMipGenSettings( 
		Texture,
		MipGenSettings,
		OutBuildSettings.MipSharpening,
		OutBuildSettings.SharpenMipKernelSize,
		bDownsampleWithAverage,
		bSharpenWithoutColorShift,
		bBorderColorBlack
		);

	static const auto CVarVirtualTexturesEnabled = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.VirtualTextures")); check(CVarVirtualTexturesEnabled);
	const bool bVirtualTextureStreaming = CVarVirtualTexturesEnabled->GetValueOnAnyThread() && bPlatformSupportsVirtualTextureStreaming && Texture.VirtualTextureStreaming;
	const FIntPoint SourceSize = Texture.Source.GetLogicalSize();

	OutBuildSettings.MipGenSettings = MipGenSettings;
	OutBuildSettings.bDownsampleWithAverage = bDownsampleWithAverage;
	OutBuildSettings.bSharpenWithoutColorShift = bSharpenWithoutColorShift;
	OutBuildSettings.bBorderColorBlack = bBorderColorBlack;
	OutBuildSettings.bFlipGreenChannel = Texture.bFlipGreenChannel;
	OutBuildSettings.CompositeTextureMode = Texture.CompositeTextureMode;
	OutBuildSettings.CompositePower = Texture.CompositePower;
	OutBuildSettings.LODBias = TextureLODSettings.CalculateLODBias(SourceSize.X, SourceSize.Y, Texture.MaxTextureSize, Texture.LODGroup, Texture.LODBias, Texture.NumCinematicMipLevels, Texture.MipGenSettings, bVirtualTextureStreaming);
	OutBuildSettings.LODBiasWithCinematicMips = TextureLODSettings.CalculateLODBias(SourceSize.X, SourceSize.Y, Texture.MaxTextureSize, Texture.LODGroup, Texture.LODBias, 0, Texture.MipGenSettings, bVirtualTextureStreaming);
	OutBuildSettings.bStreamable = bPlatformSupportsTextureStreaming && !Texture.NeverStream && (Texture.LODGroup != TEXTUREGROUP_UI) && (Cast<const UTexture2D>(&Texture) != NULL);
	OutBuildSettings.bVirtualStreamable = bVirtualTextureStreaming;
	OutBuildSettings.PowerOfTwoMode = Texture.PowerOfTwoMode;
	OutBuildSettings.PaddingColor = Texture.PaddingColor;
	OutBuildSettings.ChromaKeyColor = Texture.ChromaKeyColor;
	OutBuildSettings.bChromaKeyTexture = Texture.bChromaKeyTexture;
	OutBuildSettings.ChromaKeyThreshold = Texture.ChromaKeyThreshold;
	OutBuildSettings.CompressionQuality = Texture.CompressionQuality - 1; // translate from enum's 0 .. 5 to desired compression (-1 .. 4, where -1 is default while 0 .. 4 are actual quality setting override)
	// TODO - get default value from config/CVAR/LODGroup?
	OutBuildSettings.LossyCompressionAmount = (Texture.LossyCompressionAmount == TLCA_Default) ? TLCA_Lowest : Texture.LossyCompressionAmount.GetValue();

	// For virtual texturing we take the address mode into consideration
	if (OutBuildSettings.bVirtualStreamable)
	{
		const UTexture2D *Texture2D = Cast<UTexture2D>(&Texture);
		checkf(Texture2D, TEXT("Virtual texturing is only supported on 2D textures"));
		OutBuildSettings.VirtualAddressingModeX = Texture2D->AddressX;
		OutBuildSettings.VirtualAddressingModeY = Texture2D->AddressY;

		FVirtualTextureBuildSettings VirtualTextureBuildSettings;
		Texture.GetVirtualTextureBuildSettings(VirtualTextureBuildSettings);
		OutBuildSettings.bVirtualTextureEnableCompressZlib = VirtualTextureBuildSettings.bEnableCompressZlib;
		OutBuildSettings.bVirtualTextureEnableCompressCrunch = VirtualTextureBuildSettings.bEnableCompressCrunch;
		OutBuildSettings.VirtualTextureTileSize = FMath::RoundUpToPowerOfTwo(VirtualTextureBuildSettings.TileSize);

		// don't all max resolution to be less than VT tile size
		OutBuildSettings.MaxTextureResolution = FMath::Max<uint32>(OutBuildSettings.MaxTextureResolution, OutBuildSettings.VirtualTextureTileSize);

		// 0 is a valid value for border size
		// 1 would be OK in some cases, but breaks BC compressed formats, since it will result in physical tiles that aren't divisible by block size (4)
		// Could allow border size of 1 for non BC compressed virtual textures, but somewhat complicated to get that correct, especially with multiple layers
		// Doesn't seem worth the complexity for now, so clamp the size to be at least 2
		OutBuildSettings.VirtualTextureBorderSize = (VirtualTextureBuildSettings.TileBorderSize > 0) ? FMath::RoundUpToPowerOfTwo(FMath::Max(VirtualTextureBuildSettings.TileBorderSize, 2)) : 0;
	}
	else
	{
		OutBuildSettings.VirtualAddressingModeX = TA_Wrap;
		OutBuildSettings.VirtualAddressingModeY = TA_Wrap;
		OutBuildSettings.VirtualTextureTileSize = 0;
		OutBuildSettings.VirtualTextureBorderSize = 0;
		OutBuildSettings.bVirtualTextureEnableCompressZlib = false;
		OutBuildSettings.bVirtualTextureEnableCompressCrunch = false;
	}

	// By default, initialize settings for layer0
	FinalizeBuildSettingsForLayer(Texture, 0, OutBuildSettings);
}

/**
 * Sets build settings for a texture on the current running platform
 * @param Texture - The texture for which to build compressor settings.
 * @param OutBuildSettings - Array of desired texture settings
 */
static void GetBuildSettingsForRunningPlatform(
	const UTexture& Texture,
	TArray<FTextureBuildSettings>& OutSettingPerLayer
	)
{
	// Compress to whatever formats the active target platforms want
	ITargetPlatformManagerModule* TPM = GetTargetPlatformManager();
	if (TPM)
	{
		ITargetPlatform* CurrentPlatform = NULL;
		const TArray<ITargetPlatform*>& Platforms = TPM->GetActiveTargetPlatforms();

		check(Platforms.Num());

		CurrentPlatform = Platforms[0];

		for (int32 Index = 1; Index < Platforms.Num(); Index++)
		{
			if (Platforms[Index]->IsRunningPlatform())
			{
				CurrentPlatform = Platforms[Index];
				break;
			}
		}

		check(CurrentPlatform != NULL);

		const UTextureLODSettings* LODSettings = (UTextureLODSettings*)UDeviceProfileManager::Get().FindProfile(CurrentPlatform->PlatformName());
		const bool bPlatformSupportsTextureStreaming = CurrentPlatform->SupportsFeature(ETargetPlatformFeatures::TextureStreaming);
		const bool bPlatformSupportsVirtualTextureStreaming = CurrentPlatform->SupportsFeature(ETargetPlatformFeatures::VirtualTextureStreaming);

		FTextureBuildSettings SourceBuildSettings;
		GetTextureBuildSettings(Texture, *LODSettings, bPlatformSupportsTextureStreaming, bPlatformSupportsVirtualTextureStreaming, SourceBuildSettings);

		TArray< TArray<FName> > PlatformFormats;
		CurrentPlatform->GetTextureFormats(&Texture, PlatformFormats);
		check(PlatformFormats.Num() > 0);

		const int32 NumLayers = Texture.Source.GetNumLayers();
		check(PlatformFormats[0].Num() == NumLayers);

		OutSettingPerLayer.Reserve(NumLayers);
		for (int32 LayerIndex = 0; LayerIndex < NumLayers; ++LayerIndex)
		{
			FTextureBuildSettings& OutSettings = OutSettingPerLayer.Add_GetRef(SourceBuildSettings);
			OutSettings.TextureFormatName = PlatformFormats[0][LayerIndex];
			FinalizeBuildSettingsForLayer(Texture, LayerIndex, OutSettings);
		}
	}
}

//END COPIED FROM TextureDerivedData.cpp

void
InvalidateTextureFromData(const ::ul3::FBlock* iData,UTexture2D* iTexture,const ::ul3::FRect& iRect)
{
    checkf(iData,TEXT("Error"));
    checkf(iTexture,TEXT("Error"));

    checkf(iData->Width() == iTexture->GetSizeX() &&
           iData->Height() == iTexture->GetSizeY()
           ,TEXT("Sizes do not match"));

    int x = iRect.x;
    int y = iRect.y;
    int w = iRect.w;
    int h = iRect.h;
    checkf(x >= 0 &&
           y >= 0 &&
           w > 0  &&
           h > 0
           ,TEXT("Error"));

    // Considering only one region is an assumption that works but you have to be more carefull with several regions.
    
    ::ul3::tFormat platformFormat = ULISFormatForUE4PixelFormat(iTexture->GetPixelFormat());
    if (platformFormat != 0 && iData->Format() == platformFormat)
    {
		FUpdateTextureRegion2D* region = new FUpdateTextureRegion2D(x, y, x, y, w, h);
		TFunction<void(uint8* SrcData, const FUpdateTextureRegion2D* Regions)> dataCleanupFunc = [&](uint8*, const FUpdateTextureRegion2D* Regions) {
			delete Regions;
		};
		uint32 bpp = iData->BytesPerPixel();
		uint32 pitch = iData->BytesPerScanLine();
        iTexture->UpdateTextureRegions(0,1,region,pitch,bpp,const_cast<uint8*>(iData->DataPtr()),dataCleanupFunc);
    }
    else if (platformFormat != 0)
    {
        IULISLoaderModule& hULIS = IULISLoaderModule::Get();
        ::ul3::uint32 MT_bit = iData->Height() > 256 ? ULIS3_PERF_MT : 0;
        ::ul3::uint32 perfIntent = MT_bit | 0;

        ::ul3::FBlock* block = new ::ul3::FBlock(w, h, iData->Format());
		::ul3::FBlock* conv = new ::ul3::FBlock(w, h, platformFormat);
        ::ul3::FVec2I pos(0, 0);
		FUpdateTextureRegion2D* region = new FUpdateTextureRegion2D(x, y, 0, 0, w, h);
        ::ul3::Copy(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, iData, block, iRect, pos);
        ::ul3::Conv(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, block, conv);

		uint32 bpp = conv->BytesPerPixel();
		uint32 pitch = conv->BytesPerScanLine();

        delete block;

		TFunction<void(uint8* SrcData, const FUpdateTextureRegion2D* Regions)> dataCleanupFunc = [conv](uint8*, const FUpdateTextureRegion2D* Regions) {
			delete Regions;
			if (conv)
				delete conv;
		};
        iTexture->UpdateTextureRegions(0,1,region,pitch,bpp,const_cast<uint8*>(conv->DataPtr()),dataCleanupFunc);
        // conv destruction is handled in dataCleanupFunc
    }
	else //Convert from sourceFormat
	{
		::ul3::tFormat sourceFormat = ULISFormatForUE4TextureSourceFormat(iTexture->Source.GetFormat());
		ITargetPlatformManagerModule* TPM = GetTargetPlatformManager();
		const ITextureFormat* TextureFormat = NULL;
		if (TPM)
		{
			TArray<FTextureBuildSettings> buildSettings;
			GetBuildSettingsForRunningPlatform(*iTexture, buildSettings);

			FTextureFormatSettings FormatSettings;
			iTexture->GetLayerFormatSettings(0, FormatSettings);

			TextureFormat = TPM->FindTextureFormat(buildSettings[0].TextureFormatName);

			FImage* sourceRawImage = new FImage();
			sourceRawImage->SizeX = w;
			sourceRawImage->SizeY = h;
			sourceRawImage->NumSlices = iTexture->Source.GetNumSlices();
			sourceRawImage->Format = GetRawImageFormatFromTextureSourceFormat(iTexture->Source.GetFormat());
			sourceRawImage->GammaSpace = FormatSettings.SRGB ? (iTexture->bUseLegacyGamma ? EGammaSpace::Pow22 : EGammaSpace::sRGB) : EGammaSpace::Linear;

			for(int i = 0; i < h; i++)
			{
				sourceRawImage->RawData.Append(iData->PixelPtr(x, y + i), iData->BytesPerPixel() * w);
			}

			FCompressedImage2D* dstRawImage = new FCompressedImage2D();
			TextureFormat->CompressImage(*sourceRawImage, buildSettings[0], true, *dstRawImage);

			// Update Region
			FUpdateTextureRegion2D* region = new FUpdateTextureRegion2D(x, y, 0, 0, w, h);
			TFunction<void(uint8* SrcData, const FUpdateTextureRegion2D* Regions)> dataCleanupFunc = [dstRawImage](uint8*, const FUpdateTextureRegion2D* Regions) {
				delete Regions;
				delete dstRawImage;
			};

			int blockBytes = GPixelFormats[dstRawImage->PixelFormat].BlockBytes;
			iTexture->UpdateTextureRegions(0, 1, region, dstRawImage->SizeX * blockBytes, blockBytes, dstRawImage->RawData.GetData(), dataCleanupFunc);

			delete sourceRawImage;
		}
	}
}

void
InvalidateSurfaceFromData(const ::ul3::FBlock* iData,FOdysseySurfaceEditable* iSurface,const ::ul3::FRect& iRect)
{
    InvalidateTextureFromData(iData,iSurface->Texture(),iRect);
}

void
InvalidateSurfaceCallback(const FOdysseyBlock* iData,void* iInfo,int iX1,int iY1,int iX2,int iY2)
{
    FOdysseySurfaceEditable* surface = static_cast<FOdysseySurfaceEditable*>(iInfo);
    InvalidateSurfaceFromData(iData,surface,iX1,iY1,iX2,iY2);
}

void
InvalidateSurfaceCallback(const ::ul3::FBlock* iData,void* iInfo,const ::ul3::FRect& iRect)
{
    FOdysseySurfaceEditable* surface = static_cast<FOdysseySurfaceEditable*>(iInfo);
    InvalidateSurfaceFromData(iData,surface,iRect);
}

/////////////////////////////////////////////////////
// FOdysseySurfaceEditable
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseySurfaceEditable::~FOdysseySurfaceEditable()
{
    mTexture->RemoveFromRoot();
    if(!mIsBorrowedTexture) // If not borrowed, that means transient hence we are responsible for dealloc
    {
        checkf(mTexture,TEXT("Error: texture should be a valid pointer"));
        //texture->RemoveFromRoot(); // RM Prevent GC
        //delete texture;
        if(mTexture->IsValidLowLevel())
            mTexture->ConditionalBeginDestroy();
        mTexture = nullptr;
    }

    if(!mIsBorrowedBlock)
    {
        if(mBlock)
        {
            delete mBlock;
            mBlock = nullptr;
        }
    }
}

FOdysseySurfaceEditable::FOdysseySurfaceEditable(int iWidth,int iHeight, ::ul3::tFormat iFormat)
    : mIsBorrowedTexture(false)
    ,mIsBorrowedBlock(false)
{
    mTexture = UTexture2D::CreateTransient(iWidth, iHeight, UE4PixelFormatForULISFormat(iFormat));
    #if WITH_EDITORONLY_DATA
    mTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
    #endif
    mTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
    mTexture->SRGB = 1;
    //texture->AddToRoot(); // Prevent GC
    mTexture->Filter = TextureFilter::TF_Nearest;
    mTexture->UpdateResource();
    mTexture->AddToRoot();

    // Warning: the texture data source / bulk is allocated, then the block is allocated, then we copy the block content into bulk.
    mBlock = new FOdysseyBlock(iWidth,iHeight, iFormat, &InvalidateSurfaceCallback, static_cast<void*>(this), true);
	Invalidate();
    // load texture data from block
    //CopyBlockDataIntoUTexture(mBlock,mTexture);
}

FOdysseySurfaceEditable::FOdysseySurfaceEditable(UTexture2D* iTexture, FOdysseyBlock* iBlock)
    : mIsBorrowedTexture(true)
    , mIsBorrowedBlock(true)
{
	checkf(iTexture,TEXT("iTexture == NULL"));
	checkf(iBlock,TEXT("iBlock == NULL"));
	
	::ul3::tFormat platformFormat = ULISFormatForUE4PixelFormat(iTexture->GetPixelFormat());
	::ul3::tFormat sourceFormat = ULISFormatForUE4TextureSourceFormat(iTexture->Source.GetFormat());
	::ul3::tFormat expectedFormat = platformFormat != 0 ? platformFormat : sourceFormat;

	checkf(expectedFormat == iBlock->Format(),TEXT("iBlock format does not correspond to the expected format"));

    mTexture = iTexture;
    mTexture->AddToRoot();

    mBlock = iBlock;

	mBlock->GetBlock()->SetOnInvalid(::ul3::FOnInvalid(&InvalidateSurfaceCallback, static_cast<void*>(this)));
}

FOdysseySurfaceEditable::FOdysseySurfaceEditable(UTexture2D* iTexture)
    : mIsBorrowedTexture(true)
    ,mIsBorrowedBlock(false)
{
    checkf(iTexture,TEXT("Cannot Initialize with Null borrowed texture"));
    mTexture = iTexture;
    mTexture->AddToRoot();

    // Warning: the block is allocated, then the texture data is copied into it.
    ::ul3::tFormat platformFormat = ULISFormatForUE4PixelFormat(iTexture->GetPixelFormat());
	::ul3::tFormat sourceFormat = ULISFormatForUE4TextureSourceFormat(iTexture->Source.GetFormat());
    mBlock = new FOdysseyBlock(mTexture->GetSizeX(),mTexture->GetSizeY(), platformFormat != 0 ? platformFormat : sourceFormat, &InvalidateSurfaceCallback,static_cast<void*>(this));
	if (platformFormat == 0)
	{
		CopyUTextureSourceDataIntoBlock(mBlock, mTexture);
	}
	else
	{
		CopyUTexturePixelDataIntoBlock(mBlock,mTexture);
	}
}

FOdysseySurfaceEditable::FOdysseySurfaceEditable(FOdysseyBlock* iBlock)
    : mIsBorrowedTexture(false)
    ,mIsBorrowedBlock(true)
{
    checkf(iBlock,TEXT("Cannot Initialize with Null borrowed block"));
    mBlock = iBlock;

    mTexture = UTexture2D::CreateTransient(mBlock->Width(),mBlock->Height(), UE4PixelFormatForULISFormat(mBlock->Format()));
    #if WITH_EDITORONLY_DATA
    mTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
    #endif
    mTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
    mTexture->SRGB = 1;
    mTexture->Filter = TextureFilter::TF_Nearest;
    mTexture->UpdateResource();
    mTexture->AddToRoot();

    mBlock->GetBlock()->SetOnInvalid(::ul3::FOnInvalid(&InvalidateSurfaceCallback,static_cast<void*>(this)));

    // load texture data from block
    //CopyBlockDataIntoUTexture(mBlock,mTexture);
	Invalidate();
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API

FOdysseyBlock*
FOdysseySurfaceEditable::Block()
{
    return mBlock;
}

const FOdysseyBlock*
FOdysseySurfaceEditable::Block() const
{
    return mBlock;
}

UTexture2D*
FOdysseySurfaceEditable::Texture()
{
    return mTexture;
}

const UTexture2D*
FOdysseySurfaceEditable::Texture() const
{
    return mTexture;
}

bool
FOdysseySurfaceEditable::IsBorrowedTexture() const
{
    return mIsBorrowedTexture;
}

void
FOdysseySurfaceEditable::CommitBlockChangesIntoTextureBulk()
{
    CopyBlockDataIntoUTexture(mBlock,mTexture);
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- Public Tampon Methods
int
FOdysseySurfaceEditable::Width()
{
    return mBlock->Width();
}

int
FOdysseySurfaceEditable::Height()
{
    return mBlock->Height();
}

void
FOdysseySurfaceEditable::Invalidate()
{
    mBlock->GetBlock()->Invalidate();
}

void
FOdysseySurfaceEditable::Invalidate(int iX1,int iY1,int iX2,int iY2)
{
    mBlock->GetBlock()->Invalidate(::ul3::FRect::FromMinMax(iX1,iY1,iX2,iY2));
}

void
FOdysseySurfaceEditable::Invalidate(const ::ul3::FRect& iRect)
{
    mBlock->GetBlock()->Invalidate(iRect);
}
