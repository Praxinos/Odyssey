// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationImageRenderingAbility.h"

TSharedPtr<IOdysseyImageRenderer>
FOdysseyAnimationImageRenderingAbility::BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame) const
{
    check(false); //If you need it, override it in your class
    return nullptr;
}

TArray<FGuid>
FOdysseyAnimationImageRenderingAbility::GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame) const
{
    check(false); //If you need it, override it in your class
    return {};
}

TSharedPtr<IOdysseyHandle>
FOdysseyAnimationImageRenderingAbility::PreloadImageRendering(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame) const
{
    return nullptr;
}

::ULIS::eBlendMode
FOdysseyAnimationImageRenderingAbility::GetImageRenderingBlendMode() const
{
    return ::ULIS::Blend_Normal;
}

float
FOdysseyAnimationImageRenderingAbility::GetImageRenderingOpacity() const
{
    return 1.f;
}