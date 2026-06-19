// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Media/OdysseyAnimationMediaTextureSampleConverter.h"

#include "Media/OdysseyAnimationMediaTextureSample.h"

FOdysseyAnimationMediaTextureSampleConverter::FOdysseyAnimationMediaTextureSampleConverter(FOdysseyAnimationMediaTextureSample* iSample)
    : mSample(iSample)
{
}

uint32
FOdysseyAnimationMediaTextureSampleConverter::GetConverterInfoFlags() const
{
    return ConverterInfoFlags_WillCreateOutputTexture;
}

bool
FOdysseyAnimationMediaTextureSampleConverter::Convert(FRHICommandListImmediate& RHICmdList, FTextureRHIRef& InDstTexture, const FConversionHints& Hints)
{
    InDstTexture = mSample->GetTexture();
    return true;
}
