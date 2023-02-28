// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationLayerImageRaster.h"

#include "OdysseyPixelFormat.h"
#include "ULISEventBuilder.h"
#include "ULISLoaderModule.h"
#include "OdysseyStyleSet.h"
#include "OdysseyRasterBlock.h"
#include "OdysseyAnimationLayerImageRaster.h"

#define LOCTEXT_NAMESPACE "UOdysseyAnimationLayerImageRaster"

UOdysseyAnimationLayerImageRaster::FOnIsAlphaLockedChanged&
UOdysseyAnimationLayerImageRaster::OnIsAlphaLockedChanged()
{
    static FOnIsAlphaLockedChanged onIsAlphaLockedChanged;
    return onIsAlphaLockedChanged;
}

UOdysseyAnimationLayerImageRaster::FOnBlendModeChanged&
UOdysseyAnimationLayerImageRaster::OnBlendModeChanged()
{
    static FOnBlendModeChanged onBlendModeChanged;
    return onBlendModeChanged;
}

UOdysseyAnimationLayerImageRaster::FOnOpacityChanged&
UOdysseyAnimationLayerImageRaster::OnOpacityChanged()
{
    static FOnOpacityChanged onOpacityChanged;
    return onOpacityChanged;
}

UOdysseyAnimationLayerImageRaster::~UOdysseyAnimationLayerImageRaster()
{
}

UOdysseyAnimationLayerImageRaster::UOdysseyAnimationLayerImageRaster()
{
	LayerTypeName = LOCTEXT("LayerTypeName", "Raster Image Layer");
    Icon = *FOdysseyStyle::GetBrush( "OdysseyLayerStack.ImageLayer16");
}

void
UOdysseyAnimationLayerImageRaster::OnCreated_Implementation()
{
    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetLayerStack());
    if(!layerStack)
        return;

    UOdysseyAnimation* animation = layerStack->GetAnimation();

    //TODO: Create a first cell with empty rasterblock

    /* TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = MakeShared<::ULIS::FBlock>( animation->Width, animation->Height, format);

	::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
    ctx.Clear(*block.Get());
    ctx.Finish();

    //Caches the tiles on disk, we do this
    RasterBlock = NewObject<UOdysseyRasterBlock>(this, "RasterBlock", RF_Public | RF_Transactional);
    RasterBlock->SetBlock(block);

    RasterBlock->OnBlockChanged().AddUObject(this, &::UOdysseyAnimationLayerImageRaster::OnBlockChanged);
    RasterBlock->OnBlockPtrChanged().AddUObject(this, &::UOdysseyAnimationLayerImageRaster::OnBlockPtrChanged); */
}

UOdysseyRasterBlock*
UOdysseyAnimationLayerImageRaster::GetRasterBlock(int iFrame) const
{
	return RasterBlock;
}

void
UOdysseyAnimationLayerImageRaster::OnBlockChanged(const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive, UOdysseyRasterBlock* iBlock)
{
    //TODO: find frameIndex (Or should we send ranges of frames)
    //RenderImageChanged(frameIndex, iRects, iIsInteractive);
}

void
UOdysseyAnimationLayerImageRaster::OnBlockPtrChanged()
{
    //TODO: find frameIndex (Or should we send ranges of frames)
    //RenderImageChanged(frameIndex, { ::ULIS::FRectI::FromXYWH(0, 0, RasterBlock->GetWidth(), RasterBlock->GetHeight()) }, false);
}

TArray<::ULIS::FEvent>
UOdysseyAnimationLayerImageRaster::RenderImage(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!IsActivated)
        return iWaitList;

    if (!ioBlock)
        return iWaitList;

    //TODO: Find RasterBlock from FrameIndex

    /* TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ULISRasterBlock = RasterBlock->IsBeingEdited() ? RasterBlock->GetEditableBlock() : RasterBlock->GetBlock();
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(ULISRasterBlock->Format());

    TArray<::ULIS::FEvent> eventConvertAndExecute = ULISUtils::ConvertAndExecute(ioBlock, ULISRasterBlock->Format(), iRect, iPos, iWaitList,
        [this, &ULISRasterBlock, &ctx](TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioDest, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) -> TArray<::ULIS::FEvent>
        {
            ::ULIS::FEvent eventBlend = FULISEventBuilder().RetainBlock(ULISRasterBlock).Build();
            ctx.Blend(
                *ULISRasterBlock,
                *ioDest,
                iRect,
                iPos,
                ::ULIS::eBlendMode(BlendMode),
                ::ULIS::Alpha_Normal,
                Opacity,
                ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                iWaitList.Num(),
                iWaitList.GetData(),
                &eventBlend
            );
            return { eventBlend };
        }
    );

    ctx.Flush();

    return eventConvertAndExecute; */
}

TArray<::ULIS::FEvent>
UOdysseyAnimationLayerImageRaster::CopyImage(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!IsActivated)
        return iWaitList;

    if (!ioBlock)
        return iWaitList;

    //TODO: Find RasterBlock from FrameIndex

    /* TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ULISRasterBlock = RasterBlock->GetBlock();
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(ULISRasterBlock->Format());

    TArray<::ULIS::FEvent> eventConvertAndExecute = ULISUtils::ConvertAndExecute(ioBlock, ULISRasterBlock->Format(), iRect, iPos, iWaitList,
        [this, &ULISRasterBlock, &ctx](TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioDest, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) -> TArray<::ULIS::FEvent>
        {
            ::ULIS::FEvent eventCopy;
            ctx.Copy(
                *ULISRasterBlock,
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

    return eventConvertAndExecute; */
}

void
UOdysseyAnimationLayerImageRaster::Merge(const TArray<UOdysseyLayer*>& iLayers)
{
    //TODO: Merge Cells and Raster Blocks

    /*
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("Layer Image Raster", "Merge Layers"));
#endif

    RasterBlock->Modify();
    
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ULISRasterBlock = RasterBlock->GetBlock();

    //Make tmpblock to merge the layers into
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(ULISRasterBlock->Format());

    TArray<::ULIS::FEvent> lastEvent = {};
    for (UOdysseyLayer* layer : iLayers)
    {
        UOdysseyAnimationLayer* animationLayer = Cast<UOdysseyAnimationLayer>(layer);
        if (!animationLayer)
            continue;

        lastEvent = animationLayer->RenderImage(ULISRasterBlock, ULISRasterBlock->Rect(), ::ULIS::FVec2I(0), lastEvent);
    }
    ctx.Finish();

    RasterBlock->Invalidate({ ULISRasterBlock->Rect() }, false);
    */
}

void
UOdysseyAnimationLayerImageRaster::IsAlphaLockedChanged()
{
    OnIsAlphaLockedChanged().Broadcast(this);
}

void
UOdysseyAnimationLayerImageRaster::OpacityChanged()
{
    OnOpacityChanged().Broadcast(this);
    RenderImageChanged({ ::ULIS::FRectI::FromXYWH(0, 0, RasterBlock->GetWidth(), RasterBlock->GetHeight()) }, false);
}

void
UOdysseyAnimationLayerImageRaster::BlendModeChanged()
{
    OnBlendModeChanged().Broadcast(this);
    RenderImageChanged({ ::ULIS::FRectI::FromXYWH(0, 0, RasterBlock->GetWidth(), RasterBlock->GetHeight()) }, false);
}

void
UOdysseyAnimationLayerImageRaster::PropertyChanged(const FName& iPropertyName)
{
    Super::PropertyChanged(iPropertyName);

    if (iPropertyName == "BlendMode")
        BlendModeChanged();
    if (iPropertyName == "Opacity")
        OpacityChanged();
    if (iPropertyName == "IsAlphaLocked")
        IsAlphaLockedChanged();
}

void
UOdysseyAnimationLayerImageRaster::PostLoad()
{
    Super::PostLoad();

    //TODO: bind callbacks for all cells

    /* if ( RasterBlock )
    {
        RasterBlock->OnBlockChanged().RemoveAll(this);
        RasterBlock->OnBlockPtrChanged().RemoveAll(this);
        RasterBlock->OnBlockChanged().AddUObject(this, &::UOdysseyAnimationLayerImageRaster::OnBlockChanged);
        RasterBlock->OnBlockPtrChanged().AddUObject(this, &::UOdysseyAnimationLayerImageRaster::OnBlockPtrChanged);
    } */
}

void
UOdysseyAnimationLayerImageRaster::PostDuplicate(bool bDuplicateForPIE)
{
    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetLayerStack());
    if (!layerStack)
        return;

    UOdysseyAnimation* animation = layerStack->GetAnimation();
    if (!animation)
        return;

    //TODO: Duplicate RasterBlocks and adjust Size for each cell

    /* TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> originalBlock = RasterBlock->GetBlock();
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> duplicatedBlock = MakeShared<::ULIS::FBlock>( animation->Width, animation->Height, format);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
    ctx.ConvertFormat(
        *originalBlock.Get(),
        *duplicatedBlock.Get(),
        ::ULIS::FRectI::Auto,
        ::ULIS::FVec2I( 0 ),
        ::ULIS::FSchedulePolicy::AsyncCacheEfficient
    );
    ctx.Finish();

    //ensure we remove any callbacks registered on the wrong Rasterblock
    RasterBlock->OnBlockChanged().RemoveAll(this);
    RasterBlock->OnBlockPtrChanged().RemoveAll(this);

    //Replace old rasterblock with an owned one
    RasterBlock = NewObject<UOdysseyRasterBlock>(this, "RasterBlock", RF_Public | RF_Transactional);
    RasterBlock->SetBlock(duplicatedBlock);

    //Set the right callbacks
    RasterBlock->OnBlockChanged().AddUObject(this, &::UOdysseyAnimationLayerImageRaster::OnBlockChanged);
    RasterBlock->OnBlockPtrChanged().AddUObject(this, &::UOdysseyAnimationLayerImageRaster::OnBlockPtrChanged);*/
}

void
UOdysseyAnimationLayerImageRaster::Preload(int iFrame, TArray<TSharedPtr<IOdysseyHandle>>& oHandles)
{
    //TODO: Find Raster Block for frameIndex
    /* if ( RasterBlock )
        oHandles.Add(RasterBlock->Preload()); */
}


#undef LOCTEXT_NAMESPACE