// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyStaticImageRenderingAbility.h"

TSharedPtr<IOdysseyImageRenderer>
FOdysseyStaticImageRenderingAbility::BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType) const
{
    check(false); //If you need it, override it in your class
    return nullptr;
}

TArray<FGuid>
FOdysseyStaticImageRenderingAbility::GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType) const
{
    check(false); //If you need it, override it in your class
    return {};
}

TSharedPtr<IOdysseyHandle>
FOdysseyStaticImageRenderingAbility::PreloadImageRendering(IOdysseyImageRenderer::eRenderType iRenderType) const
{
    return nullptr;
}

::ULIS::eBlendMode
FOdysseyStaticImageRenderingAbility::GetImageRenderingBlendMode() const
{
    return ::ULIS::Blend_Normal;
}

float
FOdysseyStaticImageRenderingAbility::GetImageRenderingOpacity() const
{
    return 1.f;
}