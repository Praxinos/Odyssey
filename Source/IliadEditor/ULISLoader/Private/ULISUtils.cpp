// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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




}
