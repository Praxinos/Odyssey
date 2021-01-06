// Copyright © 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "SOdysseyTextureDetails.h"
#include "Engine/Texture.h"
#include "Engine/VolumeTexture.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/TextureRenderTarget2DArray.h"
#include "Engine/TextureRenderTargetVolume.h"
#include "Engine/TextureRenderTargetCube.h"
#include "Engine/TextureCube.h"
#include "Engine/Texture2DDynamic.h"
#include "Engine/Texture2DArray.h"
#include "TextureEditorSettings.h"

// defined in VolumeTexturePreview.cpp of UnrealEd Module
UNREALED_API void GetBestFitForNumberOfTiles(int32 InSize, int32& OutRatioX, int32& OutRatioY);

void SOdysseyTextureDetails::Construct(const FArguments& InArgs)
{
    mTexture = InArgs._Texture;

    FDetailsViewArgs Args;
	Args.bHideSelectionTip = true;

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	mTexturePropertiesWidget = PropertyModule.CreateDetailView(Args);
	mTexturePropertiesWidget->SetObject(mTexture);

    ChildSlot
    [
        SNew(SVerticalBox)

        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(2.0f)
        [
            SNew(SBorder)
            [
                SNew(SHorizontalBox)

                + SHorizontalBox::Slot()
                .FillWidth(0.5f)
                [
                    SNew(SVerticalBox)

                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .VAlign(VAlign_Center)
                    .Padding(4.0f)
                    [
                        //SAssignNew(mImportedText, STextBlock)
                        SNew(STextBlock)
                        .Text(this, &SOdysseyTextureDetails::GetImportedText)
                    ]

                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .VAlign(VAlign_Center)
                    .Padding(4.0f)
                    [
                        //SAssignNew(mCurrentText, STextBlock)
                        SNew(STextBlock)
                        .Text(this, &SOdysseyTextureDetails::GetCurrentText)
                    ]

                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .VAlign(VAlign_Center)
                    .Padding(4.0f)
                    [
                        // SAssignNew(mMaxInGameText, STextBlock)
                        SNew(STextBlock)
                        .Text(this, &::SOdysseyTextureDetails::GetMaxInGameText)
                    ]

                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .VAlign(VAlign_Center)
                    .Padding(4.0f)
                    [
                        // SAssignNew(mSizeText, STextBlock)
                        SNew(STextBlock)
                        .Text(this, &::SOdysseyTextureDetails::GetSizeText)
                    ]

                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .VAlign(VAlign_Center)
                    .Padding(4.0f)
                    [
                        // SAssignNew(mHasAlphaChannelText, STextBlock)
                        SNew(STextBlock)
                        .Text(this, &::SOdysseyTextureDetails::GetHasAlphaChannelText)
                    ]
                ]

                + SHorizontalBox::Slot()
                .FillWidth(0.5f)
                [
                    SNew(SVerticalBox)

                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .VAlign(VAlign_Center)
                    .Padding(4.0f)
                    [
                        // SAssignNew(mMethodText, STextBlock)
                        SNew(STextBlock)
                        .Text(this, &::SOdysseyTextureDetails::GetMethodText)
                    ]

                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .VAlign(VAlign_Center)
                    .Padding(4.0f)
                    [
                        // SAssignNew(mFormatText, STextBlock)
                        SNew(STextBlock)
                        .Text(this, &::SOdysseyTextureDetails::GetFormatText)
                    ]

                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .VAlign(VAlign_Center)
                    .Padding(4.0f)
                    [
                        // SAssignNew(mLODBiasText, STextBlock)
                        SNew(STextBlock)
                        .Text(this, &::SOdysseyTextureDetails::GetLODBiasText)
                    ]

                    + SVerticalBox::Slot()
                    .AutoHeight()
                    .VAlign(VAlign_Center)
                    .Padding(4.0f)
                    [
                        // SAssignNew(mNumMipsText, STextBlock)
                        SNew(STextBlock)
                        .Text(this, &::SOdysseyTextureDetails::GetNumMipsText)
                    ]
                ]
            ]
        ]

        + SVerticalBox::Slot()
        .FillHeight(1.0f)
        .Padding(2.0f)
        [
            SNew(SBorder)
            .Padding(4.0f)
            [
                mTexturePropertiesWidget.ToSharedRef()
            ]
        ]
    ];

    // PopulateQuickInfo();
}


void
SOdysseyTextureDetails::SetTexture(UTexture* iTexture)
{
    mTexture = iTexture;
    mTexturePropertiesWidget->SetObject(iTexture);
    // PopulateQuickInfo();
}

FText
SOdysseyTextureDetails::GetImportedText() const
{
    if (!mTexture)
        return FText();

	UVolumeTexture* VolumeTexture = Cast<UVolumeTexture>(mTexture);
	UTextureRenderTargetVolume* VolumeTextureRT = Cast<UTextureRenderTargetVolume>(mTexture);

	const uint32 SurfaceWidth = (uint32)mTexture->GetSurfaceWidth();
	const uint32 SurfaceHeight = (uint32)mTexture->GetSurfaceHeight();
	const uint32 SurfaceDepth =
		[&]() -> uint32
		{
			if (VolumeTexture)
			{
				return (uint32)VolumeTexture->GetSizeZ();
			}
			else if (VolumeTextureRT)
			{
				return (uint32)VolumeTextureRT->SizeZ;
			}
			return 1;
		}();
    
	const uint32 ImportedWidth = FMath::Max<uint32>(SurfaceWidth, mTexture->Source.GetSizeX());
	const uint32 ImportedHeight =  FMath::Max<uint32>(SurfaceHeight, mTexture->Source.GetSizeY());
	const uint32 ImportedDepth = FMath::Max<uint32>(SurfaceDepth, VolumeTexture || VolumeTextureRT ? mTexture->Source.GetNumSlices() : 1);

    FNumberFormattingOptions Options;
	Options.UseGrouping = false;
    
    if (VolumeTexture || VolumeTextureRT)
		return FText::Format( NSLOCTEXT("TextureEditor", "QuickInfo_Imported_3x", "Imported: {0}x{1}x{2}"), FText::AsNumber(ImportedWidth, &Options), FText::AsNumber(ImportedHeight, &Options), FText::AsNumber(ImportedDepth, &Options));
    
    return FText::Format( NSLOCTEXT("TextureEditor", "QuickInfo_Imported_2x", "Imported: {0}x{1}"), FText::AsNumber(ImportedWidth, &Options), FText::AsNumber(ImportedHeight, &Options));
}



FText
SOdysseyTextureDetails::GetCurrentText() const
{
    if (!mTexture)
        return FText();

	UVolumeTexture* VolumeTexture = Cast<UVolumeTexture>(mTexture);
    UTextureCube* TextureCube = Cast<UTextureCube>(mTexture);
	UTextureRenderTargetVolume* VolumeTextureRT = Cast<UTextureRenderTargetVolume>(mTexture);

	const uint32 SurfaceWidth = (uint32)mTexture->GetSurfaceWidth();
	const uint32 SurfaceHeight = (uint32)mTexture->GetSurfaceHeight();
	const uint32 SurfaceDepth =
		[&]() -> uint32
		{
			if (VolumeTexture)
			{
				return (uint32)VolumeTexture->GetSizeZ();
			}
			else if (VolumeTextureRT)
			{
				return (uint32)VolumeTextureRT->SizeZ;
			}
			return 1;
		}();
    
	const FStreamableRenderResourceState SRRState = mTexture->GetStreamableResourceState();
	const int32 ActualMipBias = SRRState.IsValid() ? (SRRState.ResidentFirstLODIdx() + SRRState.AssetLODBias) : mTexture->GetCachedLODBias();
	const uint32 ActualWidth = FMath::Max<uint32>(SurfaceWidth >> ActualMipBias, 1);
	const uint32 ActualHeight = FMath::Max<uint32>(SurfaceHeight >> ActualMipBias, 1);
	const uint32 ActualDepth =  FMath::Max<uint32>(SurfaceDepth >> ActualMipBias, 1);

	// Editor dimensions (takes user specified mip setting into account)
	// const int32 MipLevel = FMath::Max(GetMipLevel(), 0);
	const int32 MipLevel = 0; //TODO: manage this when we will be able to select the MipLevel to Preview
	uint32 PreviewEffectiveTextureWidth = FMath::Max<uint32>(ActualWidth >> MipLevel, 1);
	uint32 PreviewEffectiveTextureHeight = FMath::Max<uint32>(ActualHeight >> MipLevel, 1);;
	uint32 PreviewEffectiveTextureDepth = FMath::Max<uint32>(ActualDepth >> MipLevel, 1);

    FNumberFormattingOptions Options;
	Options.UseGrouping = false;
    
    if (VolumeTexture || VolumeTextureRT)
        return FText::Format( NSLOCTEXT("TextureEditor", "QuickInfo_Displayed_3x", "Displayed: {0}x{1}x{2}"), FText::AsNumber(PreviewEffectiveTextureWidth, &Options ), FText::AsNumber(PreviewEffectiveTextureHeight, &Options), FText::AsNumber(PreviewEffectiveTextureDepth, &Options));
    
    FText CubemapAdd;
    if(TextureCube)
    {
        CubemapAdd = NSLOCTEXT("TextureEditor", "QuickInfo_PerCubeSide", "x6 (CubeMap)");
    }

    return FText::Format( NSLOCTEXT("TextureEditor", "QuickInfo_Displayed_2x", "Displayed: {0}x{1}{2}"), FText::AsNumber(PreviewEffectiveTextureWidth, &Options ), FText::AsNumber(PreviewEffectiveTextureHeight, &Options), CubemapAdd);
}

FText
SOdysseyTextureDetails::GetMaxInGameText() const
{
    if (!mTexture)
        return FText();

    UTexture2D* Texture2D = Cast<UTexture2D>(mTexture);
    UTextureCube* TextureCube = Cast<UTextureCube>(mTexture);
	UVolumeTexture* VolumeTexture = Cast<UVolumeTexture>(mTexture);
	UTextureRenderTargetVolume* VolumeTextureRT = Cast<UTextureRenderTargetVolume>(mTexture);

	const uint32 SurfaceWidth = (uint32)mTexture->GetSurfaceWidth();
	const uint32 SurfaceHeight = (uint32)mTexture->GetSurfaceHeight();
	const uint32 SurfaceDepth =
		[&]() -> uint32
		{
			if (VolumeTexture)
			{
				return (uint32)VolumeTexture->GetSizeZ();
			}
			else if (VolumeTextureRT)
			{
				return (uint32)VolumeTextureRT->SizeZ;
			}
			return 1;
		}();
    
	// In game max bias and dimensions
	const int32 MaxResMipBias = Texture2D ? (Texture2D->GetNumMips() - Texture2D->GetNumMipsAllowed(true)) : mTexture->GetCachedLODBias();
	const uint32 MaxInGameWidth = FMath::Max<uint32>(SurfaceWidth >> MaxResMipBias, 1);
	const uint32 MaxInGameHeight = FMath::Max<uint32>(SurfaceHeight >> MaxResMipBias, 1);
	const uint32 MaxInGameDepth = FMath::Max<uint32>(SurfaceDepth >> MaxResMipBias, 1);

    FNumberFormattingOptions Options;
	Options.UseGrouping = false;
    
    if (VolumeTexture || VolumeTextureRT)
		return FText::Format( NSLOCTEXT("TextureEditor", "QuickInfo_MaxInGame_3x", "Max In-Game: {0}x{1}x{2}"), FText::AsNumber(MaxInGameWidth, &Options), FText::AsNumber(MaxInGameHeight, &Options), FText::AsNumber(MaxInGameDepth, &Options));
    

    FText CubemapAdd;
    if(TextureCube)
    {
        CubemapAdd = NSLOCTEXT("TextureEditor", "QuickInfo_PerCubeSide", "x6 (CubeMap)");
    }

    return FText::Format( NSLOCTEXT("TextureEditor", "QuickInfo_MaxInGame_2x", "Max In-Game: {0}x{1}{2}"), FText::AsNumber(MaxInGameWidth, &Options), FText::AsNumber(MaxInGameHeight, &Options), CubemapAdd);
}

FText
SOdysseyTextureDetails::GetSizeText() const
{
    if (!mTexture)
        return FText();

    // Texture asset size
	const uint32 Size = (mTexture->GetResourceSizeBytes(EResourceSizeMode::Exclusive) + 512) / 1024;

	FNumberFormattingOptions SizeOptions;
	SizeOptions.UseGrouping = false;
	SizeOptions.MaximumFractionalDigits = 0;

    return FText::Format(NSLOCTEXT("TextureEditor", "QuickInfo_ResourceSize", "Resource Size: {0} Kb"), FText::AsNumber(Size, &SizeOptions));
}

FText
SOdysseyTextureDetails::GetMethodText() const
{
    if (!mTexture)
        return FText();

    FText Method = mTexture->IsCurrentlyVirtualTextured() ? NSLOCTEXT("TextureEditor", "QuickInfo_MethodVirtualStreamed", "Virtual Streamed")
                                                : (!mTexture->IsStreamable() ? NSLOCTEXT("TextureEditor", "QuickInfo_MethodNotStreamed", "Not Streamed") 
                                                                        : NSLOCTEXT("TextureEditor", "QuickInfo_MethodStreamed", "Streamed") );

    return FText::Format(NSLOCTEXT("TextureEditor", "QuickInfo_Method", "Method: {0}"), Method);
}

FText
SOdysseyTextureDetails::GetFormatText() const
{
    if (!mTexture)
        return FText();

    
	UTexture2D* Texture2D = Cast<UTexture2D>(mTexture);
	UTextureRenderTarget2D* Texture2DRT = Cast<UTextureRenderTarget2D>(mTexture);
	UTextureCube* TextureCube = Cast<UTextureCube>(mTexture);
	UTexture2DArray* Texture2DArray = Cast<UTexture2DArray>(mTexture);
	UTextureRenderTarget2DArray* Texture2DArrayRT = Cast<UTextureRenderTarget2DArray>(mTexture);
	UTexture2DDynamic* Texture2DDynamic = Cast<UTexture2DDynamic>(mTexture);
	UVolumeTexture* VolumeTexture = Cast<UVolumeTexture>(mTexture);
	UTextureRenderTargetVolume* VolumeTextureRT = Cast<UTextureRenderTargetVolume>(mTexture);

    int32 TextureFormatIndex = PF_MAX;
	
	if (Texture2D)
	{
		//TextureFormatIndex = Texture2D->GetPixelFormat(SpecifiedLayer);
        TextureFormatIndex = Texture2D->GetPixelFormat(0);
	}
	else if (TextureCube)
	{
		TextureFormatIndex = TextureCube->GetPixelFormat();
	}
	else if (Texture2DArray) 
	{
		TextureFormatIndex = Texture2DArray->GetPixelFormat();
	}
	else if (Texture2DArrayRT)
	{
		TextureFormatIndex = Texture2DArrayRT->GetFormat();
	}
	else if (Texture2DRT)
	{
		TextureFormatIndex = Texture2DRT->GetFormat();
	}
	else if (Texture2DDynamic)
	{
		TextureFormatIndex = Texture2DDynamic->Format;
	}
	else if (VolumeTexture)
	{
		TextureFormatIndex = VolumeTexture->GetPixelFormat();
	}
	else if (VolumeTextureRT)
	{
		TextureFormatIndex = VolumeTextureRT->GetFormat();
	}

	if (TextureFormatIndex == PF_MAX)
        return FText();
	
	return FText::Format(NSLOCTEXT("TextureEditor", "QuickInfo_Format", "Format: {0}"), FText::FromString(GPixelFormats[TextureFormatIndex].Name));
}

FText
SOdysseyTextureDetails::GetLODBiasText() const
{
    if (!mTexture)
        return FText();

    return FText::Format(NSLOCTEXT("TextureEditor", "QuickInfo_LODBias", "Combined LOD Bias: {0}"), FText::AsNumber(mTexture->GetCachedLODBias()));
}

FText
SOdysseyTextureDetails::GetHasAlphaChannelText() const
{
    if (!mTexture)
        return FText();

    UTexture2D* Texture2D = Cast<UTexture2D>(mTexture);

    if (!Texture2D)
        return FText();
        
    return FText::Format(NSLOCTEXT("TextureEditor", "QuickInfo_HasAlphaChannel", "Has Alpha Channel: {0}"),
        Texture2D->HasAlphaChannel() ? NSLOCTEXT("TextureEditor", "True", "True") : NSLOCTEXT("TextureEditor", "False", "False"));
}

FText
SOdysseyTextureDetails::GetNumMipsText() const
{
    if (!mTexture)
        return FText();

	UTexture2D* Texture2D = Cast<UTexture2D>(mTexture);
	UTextureRenderTarget2D* Texture2DRT = Cast<UTextureRenderTarget2D>(mTexture);
	UTextureCube* TextureCube = Cast<UTextureCube>(mTexture);
	UTexture2DArray* Texture2DArray = Cast<UTexture2DArray>(mTexture);
	UTextureRenderTarget2DArray* Texture2DArrayRT = Cast<UTextureRenderTarget2DArray>(mTexture);
	UTexture2DDynamic* Texture2DDynamic = Cast<UTexture2DDynamic>(mTexture);
	UVolumeTexture* VolumeTexture = Cast<UVolumeTexture>(mTexture);
	UTextureRenderTargetVolume* VolumeTextureRT = Cast<UTextureRenderTargetVolume>(mTexture);

    int32 NumMips = 1;
	if (Texture2D)
	{
		NumMips = Texture2D->GetNumMips();
	}
	else if (TextureCube)
	{
		NumMips = TextureCube->GetNumMips();
	}
	else if (Texture2DArray) 
	{
		NumMips = Texture2DArray->GetNumMips();
	}
	else if (Texture2DArrayRT)
	{
		NumMips = Texture2DArrayRT->GetNumMips();
	}
	else if (Texture2DRT)
	{
		NumMips = Texture2DRT->GetNumMips();
	}
	else if (Texture2DDynamic)
	{
		NumMips = Texture2DDynamic->NumMips;
	}
	else if (VolumeTexture)
	{
		NumMips = VolumeTexture->GetNumMips();
	}
	else if (VolumeTextureRT)
	{
		NumMips = VolumeTextureRT->GetNumMips();
	}

    return FText::Format(NSLOCTEXT("TextureEditor", "QuickInfo_NumMips", "Number of Mips: {0}"), FText::AsNumber(NumMips));
}

EVisibility
SOdysseyTextureDetails::GetHasAlphaChannelVisibility() const
{
	UTexture2D* Texture2D = Cast<UTexture2D>(mTexture);
    return Texture2D ? EVisibility::Visible : EVisibility::Collapsed;
}
