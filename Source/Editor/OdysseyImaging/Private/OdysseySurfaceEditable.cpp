// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
#include "OdysseySurfaceEditable.h"
#include "OdysseyBlock.h"
#include "ULISLoaderModule.h"
#include "ImageCore.h"
#include "TextureCompressorModule.h"
#include "Engine/TextureLODSettings.h"
#include "Engine/TextureCube.h"
#include "Engine/Texture2DArray.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/VolumeTexture.h"
#include "VT/VirtualTextureBuildSettings.h"
#include "Interfaces/ITargetPlatformManagerModule.h"
#include "Interfaces/ITargetPlatform.h"
#include "Interfaces/ITextureFormat.h"
#include "DeviceProfiles/DeviceProfileManager.h"
#include "RHI.h"
#include <ULIS3>

/////////////////////////////////////////////////////
// Utlity
void
CopyUTextureSourceDataIntoBlock(FOdysseyBlock* iBlock,UTexture2D* iTexture)
{
    checkf(iBlock->Width() == iTexture->Source.GetSizeX() &&
           iBlock->Height() == iTexture->Source.GetSizeY()
           ,TEXT("Sizes do not match"));

	if (UE4TextureSourceFormatNeedsConversionToULISFormat(iTexture->Source.GetFormat()))
	{
		TArray64<uint8> src;
		src.SetNumUninitialized(iTexture->Source.CalcMipSize(0));
		iTexture->Source.GetMipData(src,0);
		ConvertUE4TextureSourceFormatToULISFormat(src.GetData(), iBlock->GetArray().GetData(), iBlock->Width(), iBlock->Height(), iTexture->Source.GetFormat());
	}
	else
	{
		iTexture->Source.GetMipData(iBlock->GetArray(),0);
	}
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
    if( Data )
    {
        FMemory::Memcpy(iBlock->GetArray().GetData(),
            Data,
            iBlock->GetArray().Num()
        );
    }
    Mip.BulkData.Unlock();
    iBlock->ResyncData();
}

void
CopyURenderTargetPixelDataIntoBlock(FOdysseyBlock* iBlock, UTextureRenderTarget2D* iRenderTarget)
{
    checkf(iBlock->Width() == iRenderTarget->GetSurfaceWidth()  &&
           iBlock->Height() == iRenderTarget->GetSurfaceHeight()
           ,TEXT("Sizes do not match"));

    FTextureRenderTargetResource* renderTargetResource = iRenderTarget->GameThread_GetRenderTargetResource();

}

void
InitTextureWithBlockData(const FOdysseyBlock* iBlock, UTexture2D* iTexture, ETextureSourceFormat iFormat)
{
	::ul3::tFormat targetFormat = ULISFormatForUE4TextureSourceFormat(iFormat);
	const ::ul3::FBlock* block = iBlock->GetBlock();
	if (iBlock->Format() != targetFormat)
	{
		::ul3::FBlock* convblock = new ::ul3::FBlock(iBlock->Width(), iBlock->Height(), targetFormat);

		IULISLoaderModule& hULIS = IULISLoaderModule::Get();
		::ul3::uint32 MT_bit = block->Height() > 256 ? ULIS3_PERF_MT : 0;
		::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
		::ul3::Conv(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, iBlock->GetBlock(), convblock);
		block = convblock;
	}

	if (UE4TextureSourceFormatNeedsConversionToULISFormat(iFormat))
	{
		TArray64<uint8> dst;
		dst.SetNumUninitialized(iBlock->Width() * iBlock->Height() * UE4TextureSourceFormatBytesPerPixel(iFormat));
		ConvertULISFormatToUE4TextureSourceFormat(block->DataPtr(), dst.GetData(), iBlock->Width(), iBlock->Height(), iFormat);
		iTexture->Source.Init(iBlock->Width(), iBlock->Height(), 1, 1, iFormat, dst.GetData());
	}
	else
	{
		iTexture->Source.Init(block->Width(), block->Height(), 1, 1, iFormat, block->DataPtr());
	}

	if (block != iBlock->GetBlock())
	{
		delete block;
	}
}

void
CopyBlockDataIntoUTexture(const FOdysseyBlock* iBlock,UTexture2D* iTexture)
{
    checkf(iBlock->Width() == iTexture->Source.GetSizeX() &&
           iBlock->Height() == iTexture->Source.GetSizeY()
           ,TEXT("Sizes do not match"));
           
	InitTextureWithBlockData(iBlock, iTexture, iTexture->Source.GetFormat());
}

FOdysseyBlock*
NewOdysseyBlockFromUTextureData(UTexture2D* iTexture, ::ul3::tFormat iFormat)
{
    FOdysseyBlock* block = new FOdysseyBlock(iTexture->Source.GetSizeX(),iTexture->Source.GetSizeY(),iFormat);
	FillOdysseyBlockFromUTextureData(block,iTexture, iFormat);
    return block;
}

void
FillOdysseyBlockFromUTextureData(FOdysseyBlock* ioBlock, UTexture2D* iTexture, ::ul3::tFormat iFormat)
{
	if (!ioBlock)
		return;

	if (ioBlock->Width() != iTexture->Source.GetSizeX() || ioBlock->Height() != iTexture->Source.GetSizeY() || ioBlock->Format() != iFormat)
	{
		ioBlock->Reallocate(iTexture->Source.GetSizeX(), iTexture->Source.GetSizeY(), iFormat);
	}

	//::ul3::FBlock* dst = ioBlock->Block();

	::ul3::tFormat sourceFormat = ULISFormatForUE4TextureSourceFormat(iTexture->Source.GetFormat());
	if (iFormat == sourceFormat)
	{
		CopyUTextureSourceDataIntoBlock(ioBlock,iTexture);
	}
	else
	{
		FOdysseyBlock* block = new FOdysseyBlock(iTexture->Source.GetSizeX(),iTexture->Source.GetSizeY(),sourceFormat);
		CopyUTextureSourceDataIntoBlock(block,iTexture);

		IULISLoaderModule& hULIS = IULISLoaderModule::Get();
		::ul3::uint32 MT_bit = block->Height() > 256 ? ULIS3_PERF_MT : 0;
		::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
		::ul3::Conv( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, block->GetBlock(), ioBlock->GetBlock() );
		delete block;
	}
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
	static ITargetPlatformManagerModule* TPM = GetTargetPlatformManager();
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
InvalidateTextureFromSourceData_Old(const ::ul3::FBlock* iData, UTexture2D* iTexture, const ::ul3::FRect& iRect)
{
	static ITargetPlatformManagerModule* TPM = GetTargetPlatformManager();
	const ITextureFormat* TextureFormat = NULL;
	if (TPM)
	{
		TArray<FTextureBuildSettings> buildSettings;
		GetBuildSettingsForRunningPlatform(*iTexture, buildSettings);

		FTextureFormatSettings FormatSettings;
		iTexture->GetLayerFormatSettings(0, FormatSettings);

		TextureFormat = TPM->FindTextureFormat(buildSettings[0].TextureFormatName);

		FImage* sourceRawImage = new FImage();
		sourceRawImage->SizeX = iRect.w;
		sourceRawImage->SizeY = iRect.h;
		sourceRawImage->NumSlices = iTexture->Source.GetNumSlices();
		sourceRawImage->Format = GetRawImageFormatFromTextureSourceFormat(iTexture->Source.GetFormat());//TODO: check format and convert if needed
		sourceRawImage->GammaSpace = FormatSettings.SRGB ? (iTexture->bUseLegacyGamma ? EGammaSpace::Pow22 : EGammaSpace::sRGB) : EGammaSpace::Linear;

		if (UE4TextureSourceFormatNeedsConversionToULISFormat(iTexture->Source.GetFormat()))
		{
			//TArray64<uint8> dst;
			//dst.SetNumUninitialized((iTexture->Source.CalcMipSize(0) * w) / (iTexture->Source.GetSizeX() * iTexture->Source.GetSizeY()) );
			int rowSize = (iTexture->Source.CalcMipSize(0) * iRect.w) / (iTexture->Source.GetSizeX() * iTexture->Source.GetSizeY());
			sourceRawImage->RawData.SetNumUninitialized(rowSize * iRect.h);
			for(int i = 0; i < iRect.h; i++)
			{
				ConvertULISFormatToUE4TextureSourceFormat(iData->PixelPtr(iRect.x, iRect.y + i), sourceRawImage->RawData.GetData() + rowSize * i, iRect.w, 1, iTexture->Source.GetFormat());
				//sourceRawImage->RawData.Append(dst.GetData(), dst.Num());
			}
		}
		else
		{
			for(int i = 0; i < iRect.h; i++)
			{
				sourceRawImage->RawData.Append(iData->PixelPtr(iRect.x, iRect.y + i), iData->BytesPerPixel() * iRect.w);
			}
		}

		FCompressedImage2D* dstRawImage = new FCompressedImage2D();
		TextureFormat->CompressImage(*sourceRawImage, buildSettings[0], true, *dstRawImage);

		// Update Region
		FUpdateTextureRegion2D* region = new FUpdateTextureRegion2D(iRect.x, iRect.y, 0, 0, iRect.w, iRect.h);
		TFunction<void(uint8* SrcData, const FUpdateTextureRegion2D* Regions)> dataCleanupFunc = [dstRawImage](uint8*, const FUpdateTextureRegion2D* Regions) {
			delete Regions;
			delete dstRawImage;
		};

		int blockBytes = GPixelFormats[dstRawImage->PixelFormat].BlockBytes;
		iTexture->UpdateTextureRegions(0, 1, region, dstRawImage->SizeX * blockBytes, blockBytes, dstRawImage->RawData.GetData(), dataCleanupFunc);
		FRenderCommandFence fence;
		fence.BeginFence();
		fence.Wait();

		delete sourceRawImage;
	}
}

void
InvalidateTextureFromSourceDataNew(const ::ul3::FBlock* iData,UTexture2D* iTexture,const ::ul3::FRect& iRect)
{
	/*
	 * Okay here is the idea
	 * We need to send our data the best way possible to the graphics card and send it according to all the texture parameters
	 * For that we use the Compressor->BuildTexture() Method to apply the texture parameters to our input data
	 * 
	 * The problem is, Compressor->BuildTexture() is made assuming we are giving it the whole texture image.
	 * But we want to send it only a small part of that image and BuildTexture can lead to image resizing according to the MaxTextureResolution of the BuildSettings parameter.
	 * 
	 * What we do here is the following :
	 * - If MaxTextureResolution is 0 or equals the Texture Full Size, then we don't need to worry about the part we are giving to the BuildTexture
	 * - If MaxTextureResolution > 0 and < Texture Full Size, then we need to ajust MaxTextureResolution to make it resize our image part as intended
	 * - If the resulting MaxTextureResolution < 1, then we need to take a bigger part of the image than expected in order to have a MaxTextureResolution == 1
	 * 
	 * Also this is possible because MaxTextureSize is always a power of two and is only applied if iTexture also has PowerOfTwo sizes
	 * 
	 * There will be adjustments to make to iRect x y w h values
	 * This is clearly a big Patch and should be provided by Epic Games in the first place.
	 */

	//Get Compressor Module
	static ITextureCompressorModule* Compressor = &FModuleManager::LoadModuleChecked<ITextureCompressorModule>("TextureCompressor");
	if (!Compressor)
		return;

	//Get Texture BuildSettings
	TArray<FTextureBuildSettings> buildSettings;
	GetBuildSettingsForRunningPlatform(*iTexture, buildSettings);

	//Compute Texture Full Size
	uint32 textureFullWidth = iTexture->Source.GetSizeX();
	uint32 textureFullHeight = iTexture->Source.GetSizeY();
	switch (static_cast<const ETexturePowerOfTwoSetting::Type>(buildSettings[0].PowerOfTwoMode))
	{
		case ETexturePowerOfTwoSetting::None:
			break;

		case ETexturePowerOfTwoSetting::PadToPowerOfTwo:
			textureFullWidth = FMath::RoundUpToPowerOfTwo(textureFullWidth);
			textureFullHeight = FMath::RoundUpToPowerOfTwo(textureFullHeight);
			break;

		case ETexturePowerOfTwoSetting::PadToSquarePowerOfTwo:
			textureFullWidth = textureFullHeight = FMath::Max(FMath::RoundUpToPowerOfTwo(textureFullWidth), FMath::RoundUpToPowerOfTwo(textureFullHeight));
			break;

		default:
			checkf(false, TEXT("Unknown entry in ETexturePowerOfTwoSetting::Type"));
			break;
	}

	//Adjust the build settings if needed
	::ul3::FRect srcRect = iRect;
	::ul3::FRect dstRect = iRect;
	if (buildSettings[0].MaxTextureResolution > 0 && (buildSettings[0].MaxTextureResolution < textureFullWidth || buildSettings[0].MaxTextureResolution < textureFullHeight))
	{
		uint32 ratio = FMath::Max(textureFullWidth, textureFullHeight) / buildSettings[0].MaxTextureResolution;

		//Be sure to pad our Rect to a power of two, as we know that we are in a power of two context this lead to no errors
		//We assume that iRect x and y are already rounded to a power of to as usually tiles à 64 pixels wide so every tile position is in power of two

		srcRect.x = (srcRect.x / ratio) * ratio; //round down x to a multiple of ratio, works because we are using integers
		srcRect.y = (srcRect.y / ratio) * ratio; //round down y to a multiple of ratio, works because we are using integers
		srcRect.w = FMath::Max(ratio, FMath::RoundUpToPowerOfTwo(iRect.x + srcRect.w - srcRect.x));
		srcRect.h = FMath::Max(ratio, FMath::RoundUpToPowerOfTwo(iRect.y + srcRect.h - srcRect.y));

		//the following is ok as ration and iTexture->GetSizeX() are always powers of two
		if ((uint32)srcRect.x + srcRect.w > textureFullWidth)
		{
			srcRect.x = textureFullWidth - srcRect.w;
		}

		if ((uint32)srcRect.y + srcRect.h > textureFullHeight)
		{
			srcRect.y = textureFullHeight - srcRect.w;
		}
		
		//Finally set the temporary MaxTextureResolution
		buildSettings[0].MaxTextureResolution = FMath::Max(srcRect.w, srcRect.h) / ratio;

		dstRect.x = srcRect.x / ratio;
		dstRect.y = srcRect.y / ratio;
		dstRect.w = srcRect.w / ratio;
		dstRect.h = srcRect.h / ratio;

		//Once all of this is done we have to clamp the srcRect Width and Height to keep them inside our input data boundaries
		if (srcRect.x + srcRect.w > iTexture->Source.GetSizeX())
		{
			srcRect.w = iTexture->Source.GetSizeX() - srcRect.x;
		}

		if (srcRect.y + srcRect.h > iTexture->Source.GetSizeY())
		{
			srcRect.h = iTexture->Source.GetSizeY() - srcRect.y;
		}
	}

	//Create the source Image containing the Texture Source Data we need to Refresh
	TArray<FImage> TileImages;
	FImage* sourceRawImage = new(TileImages) FImage(); //TileImages destructor will destroy that correctly ?
	sourceRawImage->SizeX = srcRect.w;
	sourceRawImage->SizeY = srcRect.h;
	sourceRawImage->NumSlices = iTexture->Source.GetNumSlices() > 0 ? iTexture->Source.GetNumSlices() : 1;
	sourceRawImage->Format = GetRawImageFormatFromTextureSourceFormat(iTexture->Source.GetFormat());
	sourceRawImage->GammaSpace = buildSettings[0].GetGammaSpace();

	//Fill the source Image with our Data and convert it from our format to the Texture Source format if needed
	//TODO: Find a way to avoid
	if (UE4TextureSourceFormatNeedsConversionToULISFormat(iTexture->Source.GetFormat()))
	{
		int w = iTexture->Source.IsValid() ? iTexture->Source.GetSizeX() : iTexture->GetSizeX();
		int h = iTexture->Source.IsValid() ? iTexture->Source.GetSizeY() : iTexture->GetSizeY();
		int rowSize = (iTexture->Source.CalcMipSize(0) * srcRect.w) / (w * h);
		sourceRawImage->RawData.SetNumUninitialized(rowSize * srcRect.h);
		for(int i = 0; i < srcRect.h; i++)
		{
			ConvertULISFormatToUE4TextureSourceFormat(iData->PixelPtr(srcRect.x, srcRect.y + i), sourceRawImage->RawData.GetData() + rowSize * i, srcRect.w, 1, iTexture->Source.GetFormat());
		}
	}
	else
	{
		int rowSize = iData->BytesPerPixel() * srcRect.w; 
		sourceRawImage->RawData.SetNumUninitialized( rowSize * srcRect.h);
		for(int i = 0; i < srcRect.h; i++)
		{
			FMemory::Memcpy(sourceRawImage->RawData.GetData() + rowSize * i, iData->PixelPtr(srcRect.x, srcRect.y + i), rowSize);
		}
	}

	//Apply the texture parameters to retrieve the data to send to the graphics card
	TArray<FCompressedImage2D> CompressedMip;
	TArray<FImage> EmptyList;
	uint32 NumMipsInTail, ExtData;
	if (!ensure(Compressor->BuildTexture(TileImages, EmptyList, buildSettings[0], CompressedMip, NumMipsInTail, ExtData)))
		return;

	//Send the image data to the graphics card and wait for it to finish
	//FUpdateTextureRegion2D* region = new FUpdateTextureRegion2D(iRect.x * CompressedMip[0].SizeX / iRect.w, iRect.y * CompressedMip[0].SizeY / iRect.h, 0, 0, CompressedMip[0].SizeX, CompressedMip[0].SizeY);
	FUpdateTextureRegion2D* region = new FUpdateTextureRegion2D(dstRect.x, dstRect.y, 0, 0, dstRect.w, dstRect.h);
	int blockBytes = GPixelFormats[CompressedMip[0].PixelFormat].BlockBytes;
	iTexture->TemporarilyDisableStreaming(); //Verify that we are not streaming to avoid regions to be wrong. This can do an extra call to UpdateResources which is heavy, but called only if TemporarilyDisableStreaming() hasn't been called before on this texture or if someone from outside disabled it.
	iTexture->UpdateTextureRegions(0, 1, region, CompressedMip[0].SizeX * blockBytes, blockBytes, CompressedMip[0].RawData.GetData(), [](uint8*, const FUpdateTextureRegion2D* Regions) {});
	FRenderCommandFence fence;
	fence.BeginFence();
	fence.Wait();

	//cleanup
	delete region;
}


void
InvalidateTextureFromSourceData(const ::ul3::FBlock* iData,UTexture2D* iTexture,const ::ul3::FRect& iRect)
{
	static ITextureCompressorModule* Compressor = /* InCompressor ? InCompressor : */ &FModuleManager::LoadModuleChecked<ITextureCompressorModule>("TextureCompressor");
	if (!Compressor)
		return;
	
	TArray<FTextureBuildSettings> buildSettings;
	GetBuildSettingsForRunningPlatform(*iTexture, buildSettings);

	FTextureFormatSettings FormatSettings;
	iTexture->GetLayerFormatSettings(0, FormatSettings);

	TArray<FImage> TileImages;
	FImage* sourceRawImage = new(TileImages) FImage(); //TileImages destructor will destroy that correctly ?
	sourceRawImage->SizeX = iRect.w;
	sourceRawImage->SizeY = iRect.h;
	sourceRawImage->NumSlices = iTexture->Source.GetNumSlices() > 0 ? iTexture->Source.GetNumSlices() : 1;
	sourceRawImage->Format = GetRawImageFormatFromTextureSourceFormat(iTexture->Source.GetFormat());//TODO: check format and convert if needed
	sourceRawImage->GammaSpace = FormatSettings.SRGB ? (iTexture->bUseLegacyGamma ? EGammaSpace::Pow22 : EGammaSpace::sRGB) : EGammaSpace::Linear;

	if (UE4TextureSourceFormatNeedsConversionToULISFormat(iTexture->Source.GetFormat()))
	{
		//TArray64<uint8> dst;
		//dst.SetNumUninitialized((iTexture->Source.CalcMipSize(0) * w) / (iTexture->Source.GetSizeX() * iTexture->Source.GetSizeY()) );
		int w = iTexture->Source.IsValid() ? iTexture->Source.GetSizeX() : iTexture->GetSizeX();
		int h = iTexture->Source.IsValid() ? iTexture->Source.GetSizeY() : iTexture->GetSizeY();
		int rowSize = (iTexture->Source.CalcMipSize(0) * iRect.w) / (w * h);
		sourceRawImage->RawData.SetNumUninitialized(rowSize * iRect.h);
		for(int i = 0; i < iRect.h; i++)
		{
			ConvertULISFormatToUE4TextureSourceFormat(iData->PixelPtr(iRect.x, iRect.y + i), sourceRawImage->RawData.GetData() + rowSize * i, iRect.w, 1, iTexture->Source.GetFormat());
			//sourceRawImage->RawData.Append(dst.GetData(), dst.Num());
		}
	}
	else
	{
		int rowSize = iData->BytesPerPixel() * iRect.w; 
		sourceRawImage->RawData.SetNumUninitialized( rowSize * iRect.h);
		for(int i = 0; i < iRect.h; i++)
		{
			FMemory::Memcpy(sourceRawImage->RawData.GetData() + rowSize * i, iData->PixelPtr(iRect.x, iRect.y + i), rowSize);
			// sourceRawImage->RawData.Append(iData->PixelPtr(iRect.x, iRect.y + i), iData->BytesPerPixel() * iRect.w);
		}
	}

	//FCompressedImage2D* dstRawImage = new FCompressedImage2D();
	TArray<FCompressedImage2D> CompressedMip;
	TArray<FImage> EmptyList;
	uint32 NumMipsInTail, ExtData;
	if (!ensure(Compressor->BuildTexture(TileImages, EmptyList, buildSettings[0], CompressedMip, NumMipsInTail, ExtData)))
		return;

	// Update Region

	//TODO: Take CompressedMip size to use MaximumTextureSize
	//FUpdateTextureRegion2D* region = new FUpdateTextureRegion2D(iRect.x, iRect.y, 0, 0, CompressedMip[0].SizeX, CompressedMip[0].SizeY);

	// float xRatio = ;
	// float yRatio = ;
	//FUpdateTextureRegion2D* region = new FUpdateTextureRegion2D(iRect.x * CompressedMip[0].SizeX / iRect.w, iRect.y * CompressedMip[0].SizeY / iRect.h, 0, 0, CompressedMip[0].SizeX, CompressedMip[0].SizeY);

	FUpdateTextureRegion2D* region = new FUpdateTextureRegion2D(iRect.x, iRect.y, 0, 0, iRect.w, iRect.h);
	TFunction<void(uint8* SrcData, const FUpdateTextureRegion2D* Regions)> dataCleanupFunc = [](uint8*, const FUpdateTextureRegion2D* Regions) {
		delete Regions;
	};

	int blockBytes = GPixelFormats[CompressedMip[0].PixelFormat].BlockBytes;
	iTexture->UpdateTextureRegions(0, 1, region, CompressedMip[0].SizeX * blockBytes, blockBytes, CompressedMip[0].RawData.GetData(), dataCleanupFunc);
	FRenderCommandFence fence;
	fence.BeginFence();
	fence.Wait();
}

void
InvalidateTextureFromData(const ::ul3::FBlock* iData,UTexture2D* iTexture,const ::ul3::FRect& iRect)
{
	// std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    checkf(iData,TEXT("Error"));
    checkf(iTexture,TEXT("Error"));

    /* checkf(iData->Width() == iTexture->Source.GetSizeX() &&
           iData->Height() == iTexture->Source.GetSizeY()
           ,TEXT("Sizes do not match")); */

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
	if (platformFormat == 0)
	{
		//InvalidateTextureFromSourceData_Old(iData, iTexture, iRect);
		//InvalidateTextureFromSourceData(iData, iTexture, iRect);
		InvalidateTextureFromSourceDataNew(iData, iTexture, iRect);
		// std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		// UE_LOG(LogTemp, Warning, TEXT("Time difference 0 = %ld �s"), std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());
		return;
	}
		
	// default params if platforForamt == iData->Format()
	const ::ul3::FBlock* srcBlock = iData;
	FUpdateTextureRegion2D* region = new FUpdateTextureRegion2D(x, y, x, y, w, h);
	TFunction<void(uint8* SrcData, const FUpdateTextureRegion2D* Regions)> dataCleanupFunc = [srcBlock, iData](uint8*, const FUpdateTextureRegion2D* Regions) {
		delete Regions;
		if (srcBlock != iData)
			delete srcBlock;
	};
	uint32 bpp = iData->BytesPerPixel();
	uint32 pitch = iData->BytesPerScanLine();

	//TODO:
	//sourceRawImage->GammaSpace = FormatSettings.SRGB ? (iTexture->bUseLegacyGamma ? EGammaSpace::Pow22 : EGammaSpace::sRGB) : EGammaSpace::Linear;

    if (srcBlock->Format() != platformFormat)
    {
        IULISLoaderModule& hULIS = IULISLoaderModule::Get();
        ::ul3::uint32 MT_bit = srcBlock->Height() > 256 ? ULIS3_PERF_MT : 0;
        ::ul3::uint32 perfIntent = MT_bit | 0;

        ::ul3::FBlock* block = new ::ul3::FBlock(w, h, srcBlock->Format());
        ::ul3::FVec2I pos(0, 0);
        ::ul3::Copy(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, srcBlock, block, iRect, pos);

		::ul3::FBlock* conv = new ::ul3::FBlock(w, h, platformFormat);
		srcBlock = conv;
        ::ul3::Conv(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, block, conv);

        delete block;

		bpp = srcBlock->BytesPerPixel();
		pitch = srcBlock->BytesPerScanLine();
		
		delete region;
		
		region = new FUpdateTextureRegion2D(x, y, 0, 0, w, h);
        // srcBlock destruction is handled in dataCleanupFunc
    }
	
    iTexture->UpdateTextureRegions(0,1,region,pitch,bpp,const_cast<uint8*>(srcBlock->DataPtr()),dataCleanupFunc);
	FRenderCommandFence fence;
	fence.BeginFence();
	fence.Wait();
	// std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	// UE_LOG(LogTemp, Warning, TEXT("Time difference 0 = %ld �s"), std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count());
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
    mBlock = new FOdysseyBlock(mTexture->Source.GetSizeX(),mTexture->Source.GetSizeY(), platformFormat != 0 ? platformFormat : sourceFormat, &InvalidateSurfaceCallback,static_cast<void*>(this));
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
