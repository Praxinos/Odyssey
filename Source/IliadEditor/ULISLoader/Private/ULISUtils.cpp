// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ULISUtils.h"

#include "ULISEventBuilder.h"
#include "ULISLoaderModule.h"

namespace ULISUtils
{

TArray<::ULIS::FEvent>
ConvertAndExecute(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioDest, ::ULIS::eFormat iFormat, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList, tConvertAndExecuteFunction iFunction)
{
    if (iFormat == ioDest->Format())
        return iFunction(ioDest, iRect, iPos, iWaitList);

    //Convert source block to layer format
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> destBlock = MakeShared<::ULIS::FBlock>( iRect.w, iRect.h, iFormat );
    ::ULIS::FRectI destRect = ::ULIS::FRectI::FromXYWH(iPos.x, iPos.y, iRect.w, iRect.h);
    ::ULIS::FEvent eventConvert = FULISEventBuilder().RetainBlock(ioDest).RetainBlock(destBlock).Build();
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iFormat);
    ctx.ConvertFormat(
        *ioDest,
        *destBlock,
        destRect,
        ::ULIS::FVec2I( 0 ),
        ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
        iWaitList.Num(),
        iWaitList.GetData(),
        &eventConvert
    );

    TArray<::ULIS::FEvent> eventFunction = iFunction(destBlock, iRect, ::ULIS::FVec2I( 0 ), {eventConvert});

    //Manage sourceBlock destruction on last conversion event completed
    ::ULIS::FEvent eventConvertToDestinationFormat = FULISEventBuilder().RetainBlock(destBlock).RetainBlock(ioDest).Build();

    //Convert back to source format, and copy at the right blace in ioBlock
    ctx.ConvertFormat(
        *destBlock,
        *ioDest,
        ::ULIS::FRectI::Auto,
        iPos,
        ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
        eventFunction.Num(),
        eventFunction.GetData(),
        &eventConvertToDestinationFormat
    );

    return {eventConvertToDestinationFormat};
}

::ULIS::FRectI
ToULISRectI(const FIntRect& iRect)
{
    return ::ULIS::FRectI::FromXYWH(iRect.Min.X, iRect.Min.Y, iRect.Width(), iRect.Height());
}

TArray<::ULIS::FRectI>
ToULISRectIs(const TArray<FIntRect>& iRects)
{
    TArray<::ULIS::FRectI> rects;
    rects.Reserve(iRects.Num());
    for (const FIntRect& rect : iRects )
    {
        rects.Add(ToULISRectI(rect));
    }

    return rects;
}

FIntRect
ToIntRect(const ::ULIS::FRectI& iRect)
{
    return FIntRect(iRect.x, iRect.y, iRect.x + iRect.w, iRect.y + iRect.h);
}

TArray<FIntRect>
ToIntRects(const TArray<::ULIS::FRectI>& iRects)
{
    TArray<FIntRect> intRects;
    intRects.Reserve(iRects.Num());
    for (const ::ULIS::FRectI& rect : iRects )
    {
        intRects.Add(ToIntRect(rect));
    }

    return intRects;
}

void
GetULISBlendingModeFromBlendMode(EOdysseyColorBlendMode InColorBlendMode, EOdysseyAlphaBlendMode InAlphaBlendMode, ::ULIS::eBlendMode& OutBlendMode, ::ULIS::eAlphaMode& OutAlphaMode)
{
    OutBlendMode = ::ULIS::Blend_Normal;
    OutAlphaMode = ::ULIS::Alpha_Normal;

    switch(InColorBlendMode)
    {
        case EOdysseyColorBlendMode::Normal:
            OutBlendMode = ::ULIS::Blend_Normal;
        break;

        case EOdysseyColorBlendMode::Behind:
            OutBlendMode = ::ULIS::Blend_Behind;
        break;

        case EOdysseyColorBlendMode::Top:
            OutBlendMode = ::ULIS::Blend_Top;
        break;

        case EOdysseyColorBlendMode::Back:
            OutBlendMode = ::ULIS::Blend_Back;
        break;

        case EOdysseyColorBlendMode::Darken:
            OutBlendMode = ::ULIS::Blend_Darken;
        break;

        case EOdysseyColorBlendMode::Multiply:
            OutBlendMode = ::ULIS::Blend_Multiply;
        break;

        case EOdysseyColorBlendMode::ColorBurn:
            OutBlendMode = ::ULIS::Blend_ColorBurn;
        break;

        case EOdysseyColorBlendMode::LinearBurn:
            OutBlendMode = ::ULIS::Blend_LinearBurn;
        break;

        case EOdysseyColorBlendMode::DarkerColor:
            OutBlendMode = ::ULIS::Blend_DarkerColor;
        break;

        case EOdysseyColorBlendMode::Lighten:
            OutBlendMode = ::ULIS::Blend_Lighten;
        break;

        case EOdysseyColorBlendMode::Screen:
            OutBlendMode = ::ULIS::Blend_Screen;
        break;

        case EOdysseyColorBlendMode::ColorDodge:
            OutBlendMode = ::ULIS::Blend_ColorDodge;
        break;

        case EOdysseyColorBlendMode::LinearDodge:
            OutBlendMode = ::ULIS::Blend_LinearDodge;
        break;

        case EOdysseyColorBlendMode::LighterColor:
            OutBlendMode = ::ULIS::Blend_LighterColor;
        break;

        case EOdysseyColorBlendMode::Overlay:
            OutBlendMode = ::ULIS::Blend_Overlay;
        break;

        case EOdysseyColorBlendMode::SoftLight:
            OutBlendMode = ::ULIS::Blend_SoftLight;
        break;

        case EOdysseyColorBlendMode::HardLight:
            OutBlendMode = ::ULIS::Blend_HardLight;
        break;

        case EOdysseyColorBlendMode::VividLight:
            OutBlendMode = ::ULIS::Blend_VividLight;
        break;

        case EOdysseyColorBlendMode::LinearLight:
            OutBlendMode = ::ULIS::Blend_LinearLight;
        break;

        case EOdysseyColorBlendMode::PinLight:
            OutBlendMode = ::ULIS::Blend_PinLight;
        break;

        case EOdysseyColorBlendMode::HardMix:
            OutBlendMode = ::ULIS::Blend_HardMix;
        break;

        case EOdysseyColorBlendMode::Phoenix:
            OutBlendMode = ::ULIS::Blend_Phoenix;
        break;

        case EOdysseyColorBlendMode::Reflect:
            OutBlendMode = ::ULIS::Blend_Reflect;
        break;

        case EOdysseyColorBlendMode::Glow:
            OutBlendMode = ::ULIS::Blend_Glow;
        break;

        case EOdysseyColorBlendMode::Difference:
            OutBlendMode = ::ULIS::Blend_Difference;
        break;

        case EOdysseyColorBlendMode::Exclusion:
            OutBlendMode = ::ULIS::Blend_Exclusion;
        break;

        case EOdysseyColorBlendMode::Add:
            OutBlendMode = ::ULIS::Blend_Add;
        break;

        case EOdysseyColorBlendMode::Substract:
            OutBlendMode = ::ULIS::Blend_Substract;
        break;

        case EOdysseyColorBlendMode::Divide:
            OutBlendMode = ::ULIS::Blend_Divide;
        break;

        case EOdysseyColorBlendMode::Average:
            OutBlendMode = ::ULIS::Blend_Average;
        break;

        case EOdysseyColorBlendMode::Negation:
            OutBlendMode = ::ULIS::Blend_Negation;
        break;

        case EOdysseyColorBlendMode::Hue:
            OutBlendMode = ::ULIS::Blend_Hue;
        break;

        case EOdysseyColorBlendMode::Saturation:
            OutBlendMode = ::ULIS::Blend_Saturation;
        break;

        case EOdysseyColorBlendMode::Color:
            OutBlendMode = ::ULIS::Blend_Color;
        break;

        case EOdysseyColorBlendMode::Luminosity:
            OutBlendMode = ::ULIS::Blend_Luminosity;
        break;

        case EOdysseyColorBlendMode::PartialDerivative:
            OutBlendMode = ::ULIS::Blend_PartialDerivative;
        break;

        case EOdysseyColorBlendMode::WhiteOut:
            OutBlendMode = ::ULIS::Blend_Whiteout;
        break;

        case EOdysseyColorBlendMode::AngleCorrected:
            OutBlendMode = ::ULIS::Blend_AngleCorrected;
        break;

        case EOdysseyColorBlendMode::Dissolve:
            OutBlendMode = ::ULIS::Blend_Dissolve;
        break;

        case EOdysseyColorBlendMode::BayerDither8x8:
            OutBlendMode = ::ULIS::Blend_BayerDither8x8;
        break;

    }

    switch(InAlphaBlendMode)
    {
        case EOdysseyAlphaBlendMode::Normal:
            OutAlphaMode = ::ULIS::Alpha_Normal;
        break;

        case EOdysseyAlphaBlendMode::Mask:
            OutAlphaMode = ::ULIS::Alpha_Erase;
        break;

        case EOdysseyAlphaBlendMode::Top:
            OutAlphaMode = ::ULIS::Alpha_Top;
        break;

        case EOdysseyAlphaBlendMode::Back:
            OutAlphaMode = ::ULIS::Alpha_Back;
        break;

        case EOdysseyAlphaBlendMode::Sub:
            OutAlphaMode = ::ULIS::Alpha_Sub;
        break;

        case EOdysseyAlphaBlendMode::Add:
            OutAlphaMode = ::ULIS::Alpha_Add;
        break;

        case EOdysseyAlphaBlendMode::Mul:
            OutAlphaMode = ::ULIS::Alpha_Mul;
        break;

        case EOdysseyAlphaBlendMode::Min:
            OutAlphaMode = ::ULIS::Alpha_Min;
        break;

        case EOdysseyAlphaBlendMode::Max:
            OutAlphaMode = ::ULIS::Alpha_Max;
        break;

        case EOdysseyAlphaBlendMode::Dissolve:
            OutBlendMode = ::ULIS::Blend_Dissolve;
        break;

        case EOdysseyAlphaBlendMode::BayerDither8x8:
            OutBlendMode = ::ULIS::Blend_BayerDither8x8;
        break;
    }
}

}
