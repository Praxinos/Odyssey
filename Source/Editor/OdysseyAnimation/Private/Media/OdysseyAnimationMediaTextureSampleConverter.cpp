// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
FOdysseyAnimationMediaTextureSampleConverter::Convert(FTexture2DRHIRef& InDstTexture, const FConversionHints& Hints)
{
    InDstTexture = mSample->GetTexture();
    return true;
}