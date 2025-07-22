// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "IMediaTextureSampleConverter.h"

class FOdysseyAnimationMediaTextureSample;

class ODYSSEYANIMATION_API FOdysseyAnimationMediaTextureSampleConverter
    : public IMediaTextureSampleConverter
{
public:
    FOdysseyAnimationMediaTextureSampleConverter(FOdysseyAnimationMediaTextureSample* iSample);
    virtual ~FOdysseyAnimationMediaTextureSampleConverter() {}

    virtual uint32 GetConverterInfoFlags() const override;
    virtual bool Convert(FRHICommandListImmediate& RHICmdList, FTextureRHIRef& InDstTexture, const FConversionHints& Hints) override;

private:
    FOdysseyAnimationMediaTextureSample* mSample;
};
