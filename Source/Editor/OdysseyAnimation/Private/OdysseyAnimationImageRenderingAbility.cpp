// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationImageRenderingAbility.h"

FOdysseyAnimationImageRenderingAbility::FOdysseyAnimationImageRenderingAbility(UOdysseyAnimation* iAnimation)
    : mAnimation(iAnimation)
{
}

TArray<::ULIS::FRectI>
FOdysseyAnimationImageRenderingAbility::GetRects(int iFrame) const
{
    if (!mAnimation)
        return {};

    return { ::ULIS::FRectI::FromXYWH(0, 0, mAnimation->Width(), mAnimation->Height()) };
}

TArray<::ULIS::FEvent>
FOdysseyAnimationImageRenderingAbility::RenderInBlock(TSharedPtr<::ULIS::FBlock> ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mAnimation)
        return iWaitList;

    //if animation has a block in its proxy, copy the block in ioBlock
    /* TSharedPtr<::ULIS::FBlock> block = mAnimation->GetBlockAtIndex(iFrame);
    if (block)
    {
        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(block->Format());

        TArray<::ULIS::FEvent> eventConvertAndExecute = ULISUtils::ConvertAndExecute(ioBlock, block->Format(), iRect, iPos, iWaitList,
            [this, &block, &ctx](TSharedPtr<::ULIS::FBlock> ioDest, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) -> TArray<::ULIS::FEvent>
            {
                ::ULIS::FEvent eventCopy = FULISEventBuilder().RetainBlock(block).Build();
                ctx.Copy(
                    *block,
                    *ioDest,
                    iRect,
                    iPos,
                    ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                    iWaitList.Num(),
                    iWaitList.GetData(),
                    &eventCopy
                );
                return { eventCopy };
            }
        );

        ctx.Flush();

        return eventConvertAndExecute;
    } */

    UOdysseyAnimationLayerStack* layerStack = mAnimation->GetLayerStack();
    if (!layerStack)
        return iWaitList;

    TSharedPtr<IOdysseyAnimationImageRenderingAbility> layerStackAbility = layerStack->GetAbility<IOdysseyAnimationImageRenderingAbility>();
    if (!layerStackAbility)
        return iWaitList;

    return layerStackAbility->RenderInBlock(ioBlock, iFrame, iRect, iPos, iWaitList);
}

TArray<FGuid>
FOdysseyAnimationImageRenderingAbility::GetComposition(int iFrameIndex) const
{
    TArray<FGuid> idComposition = { GetId() };
    if (!mAnimation)
        return idComposition;

    UOdysseyAnimationLayerStack* layerStack = mAnimation->GetLayerStack();
    if (!layerStack)
        return idComposition;

    TSharedPtr<IOdysseyAnimationImageRenderingAbility> layerStackAbility = layerStack->GetAbility<IOdysseyAnimationImageRenderingAbility>();
    if (!layerStackAbility)
        return idComposition;

    idComposition.Append(layerStackAbility->GetComposition(iFrameIndex));

    return idComposition;
}

TSharedPtr<IOdysseyHandle>
FOdysseyAnimationImageRenderingAbility::Preload(int iFrame)
{
    if (!mAnimation)
        return nullptr;

    UOdysseyAnimationLayerStack* layerStack = mAnimation->GetLayerStack();
    if (!layerStack)
        return nullptr;

    TSharedPtr<IOdysseyAnimationImageRenderingAbility> layerStackAbility = layerStack->GetAbility<IOdysseyAnimationImageRenderingAbility>();
    if (!layerStackAbility)
        return nullptr;

    return layerStackAbility->Preload(iFrame);
}