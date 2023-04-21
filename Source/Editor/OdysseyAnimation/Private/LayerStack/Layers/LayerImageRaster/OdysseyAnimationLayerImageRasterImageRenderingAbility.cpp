// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRasterImageRenderingAbility.h"

FOdysseyAnimationLayerImageRasterImageRenderingAbility::FOdysseyAnimationLayerImageRasterImageRenderingAbility(UOdysseyAnimationLayerImageRaster* iLayerImageRaster)
    : mLayerImageRaster(iLayerImageRaster)
{
}

TArray<::ULIS::FRectI>
FOdysseyAnimationLayerImageRasterImageRenderingAbility::GetRects(int iFrame) const
{
    if (!mLayerImageRaster)
        return {};

    int cellIndex = INDEX_NONE;
    int cellFrameIndex = INDEX_NONE;
    if (!mLayerImageRaster->GetCellIndexAtFrame(iFrame, cellIndex, cellFrameIndex))
        return {};

    TSharedPtr<FOdysseyAnimationCell> cell = mLayerImageRaster->GetCell(cellIndex);
    if (!cell)
        return {};

    TSharedPtr<IOdysseyAnimationImageRenderingAbility> cellAbility = cell->GetAbility<IOdysseyAnimationImageRenderingAbility>();
    if (!cellAbility)
        return {};

    return cellAbility->GetRects(cellFrameIndex);
}

TArray<::ULIS::FEvent>
FOdysseyAnimationLayerImageRasterImageRenderingAbility::RenderOverBlock(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mLayerImageRaster)
        return iWaitList;

    if (!ioBlock)
        return iWaitList;

    ::ULIS::eFormat format = ioBlock->Format();

    //Generate the folder block, which is all children layers blended together
    TArray<::ULIS::FEvent> rasterBlockEvent;
    TSharedPtr<::ULIS::FBlock> rasterBlock = RenderInNewBlock(iFrame, format, iRect, rasterBlockEvent);
    if(!rasterBlock)
        return iWaitList;

    rasterBlockEvent.Append(iWaitList);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
    TArray<::ULIS::FEvent> eventConvertAndExecute = ULISUtils::ConvertAndExecute(ioBlock, rasterBlock->Format(), rasterBlock->Rect(), iPos, rasterBlockEvent,
        [this, &rasterBlock, &ctx](TSharedPtr<::ULIS::FBlock> ioDest, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) -> TArray<::ULIS::FEvent>
        {
            ::ULIS::FEvent eventBlend = FULISEventBuilder()
                .RetainBlock(rasterBlock)
                .Build();

            ctx.Blend(
                *rasterBlock,
                *ioDest,
                iRect,
                iPos,
                ::ULIS::eBlendMode(mLayerImageRaster->BlendMode),
                ::ULIS::Alpha_Normal,
                mLayerImageRaster->Opacity,
                ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                iWaitList.Num(),
                iWaitList.GetData(),
                &eventBlend
            );
            return { eventBlend };
        }
    );

    ctx.Flush();

    return eventConvertAndExecute;
}

TArray<::ULIS::FEvent>
FOdysseyAnimationLayerImageRasterImageRenderingAbility::RenderInBlock(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mLayerImageRaster)
        return iWaitList;

    if (!ioBlock)
        return iWaitList;

    int cellIndex = INDEX_NONE;
    int cellFrameIndex = INDEX_NONE;
    if (!mLayerImageRaster->GetCellIndexAtFrame(iFrame, cellIndex, cellFrameIndex))
        return iWaitList;

    TSharedPtr<FOdysseyAnimationCell> cell = mLayerImageRaster->GetCell(cellIndex);
    if (!cell)
        return iWaitList;

    TSharedPtr<IOdysseyAnimationImageRenderingAbility> cellAbility = cell->GetAbility<IOdysseyAnimationImageRenderingAbility>();
    if (!cellAbility)
        return iWaitList;

    return cellAbility->RenderInBlock(ioBlock, iFrame, iRect, iPos, iWaitList);
}

TArray<FGuid>
FOdysseyAnimationLayerImageRasterImageRenderingAbility::GetComposition(int iFrameIndex) const
{
    TArray<FGuid> idComposition = { GetId() };
    if (!mLayerImageRaster )
        return idComposition;

    int celFrameIndex = INDEX_NONE;
    TSharedPtr<FOdysseyAnimationCell> cell = mLayerImageRaster->GetCellAtFrame(iFrameIndex, celFrameIndex);
    if (cell)
    {
        TSharedPtr<IOdysseyAnimationImageRenderingAbility> cellAbility = cell->GetAbility<IOdysseyAnimationImageRenderingAbility>();
        if (cellAbility)
        {
            idComposition.Append(cellAbility->GetComposition(celFrameIndex));
        }
    }

    return idComposition;
}

TSharedPtr<IOdysseyHandle>
FOdysseyAnimationLayerImageRasterImageRenderingAbility::Preload(int iFrameIndex)
{
    if (!mLayerImageRaster )
        return nullptr;

    int celFrameIndex = INDEX_NONE;
    TSharedPtr<FOdysseyAnimationCell> cell = mLayerImageRaster->GetCellAtFrame(iFrameIndex, celFrameIndex);
    TArray<TSharedPtr<IOdysseyHandle>> handles;
    if (cell)
    {
        TSharedPtr<IOdysseyAnimationImageRenderingAbility> cellAbility = cell->GetAbility<IOdysseyAnimationImageRenderingAbility>();
        if (cellAbility)
        {
            handles.Add(cellAbility->Preload(celFrameIndex));
        }
    }

    return MakeShared<FOdysseyHandleContainer>(handles);
}