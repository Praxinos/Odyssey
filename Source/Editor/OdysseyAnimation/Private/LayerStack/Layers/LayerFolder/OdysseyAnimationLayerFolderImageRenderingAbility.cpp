// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Layers/LayerFolder/OdysseyAnimationLayerFolderImageRenderingAbility.h"

FOdysseyAnimationLayerFolderImageRenderingAbility::FOdysseyAnimationLayerFolderImageRenderingAbility(UOdysseyAnimationLayerFolder* iLayerFolder)
    : FOdysseyAnimationLayerImageRenderingAbility(iLayerFolder)
    , mLayerFolder(iLayerFolder)
{
}

TArray<::ULIS::FEvent>
FOdysseyAnimationLayerFolderImageRenderingAbility::RenderOverBlock(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mLayerFolder)
        return iWaitList;

    if (!ioBlock)
        return iWaitList;

    ::ULIS::eFormat format = ioBlock->Format();

    //Generate the folder block, which is all children layers blended together
    TArray<::ULIS::FEvent> folderBlockEvent;
    TSharedPtr<::ULIS::FBlock> folderBlock = RenderInNewBlock(iFrame, format, iRect, folderBlockEvent);
    if(!folderBlock)
        return iWaitList;

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
    //Convert the destination if needed and Blend the folderBlock
    TArray<::ULIS::FEvent> eventConvertAndExecute = ULISUtils::ConvertAndExecute(ioBlock, folderBlock->Format(), folderBlock->Rect(), iPos, folderBlockEvent,
        [this, &folderBlock, &ctx](TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioDest, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) -> TArray<::ULIS::FEvent>
        {
            //if ioBlock and mBlock use same format, Blend directly in ioBlock
            ::ULIS::FEvent eventBlend = FULISEventBuilder().RetainBlock(folderBlock).Build();

            ctx.Blend(
                *folderBlock,
                *ioDest,
                iRect,
                iPos,
                ::ULIS::eBlendMode(mLayerFolder->BlendMode),
                ::ULIS::Alpha_Normal,
                mLayerFolder->Opacity,
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
