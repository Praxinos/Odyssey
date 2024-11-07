// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
    virtual bool Convert(FTexture2DRHIRef & InDstTexture, const FConversionHints & Hints) override;

private:
    FOdysseyAnimationMediaTextureSample* mSample;
};
