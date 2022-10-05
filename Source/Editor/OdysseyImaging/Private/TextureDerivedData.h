// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022
#pragma once
#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
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
#include "DeviceProfiles/DeviceProfileManager.h"

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
    OutBuildSettings.AlphaCoverageThresholds = (FVector4f)Texture.AlphaCoverageThresholds;
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