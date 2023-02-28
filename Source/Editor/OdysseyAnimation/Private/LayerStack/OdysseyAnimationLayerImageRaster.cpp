// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/OdysseyAnimationLayerImageRaster.h"

#include "OdysseyPixelFormat.h"
#include "ULISEventBuilder.h"
#include "ULISLoaderModule.h"
#include "OdysseyStyleSet.h"
#include "OdysseyRasterBlock.h"

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

UOdysseyAnimationLayerImageRaster::FOnCellsChanged&
UOdysseyAnimationLayerImageRaster::OnCellsChanged()
{
    static FOnCellsChanged onCellsChanged;
    return onCellsChanged;
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
    AddFrame();
}

TRange<int>
UOdysseyAnimationLayerImageRaster::GetFrameRange() const
{
    return TRange<int>::Inclusive(0, mRasterBlocks.Num() - 1);
}

FString
UOdysseyAnimationLayerImageRaster::GetFrameId(int iFrameIndex) const
{
    if (iFrameIndex < 0 || iFrameIndex >= mRasterBlocks.Num())
        return "";

	return mRasterBlocks[iFrameIndex]->GetId().ToString();
}

TSharedPtr<FOdysseyRasterBlock>
UOdysseyAnimationLayerImageRaster::GetRasterBlock(int iFrame) const
{
    if (iFrame < 0 || iFrame >= mRasterBlocks.Num())
        return nullptr;

	return mRasterBlocks[iFrame];
}

const TArray<TSharedPtr<FOdysseyRasterBlock>>&
UOdysseyAnimationLayerImageRaster::GetRasterBlocks() const
{
    return mRasterBlocks;
}

void
UOdysseyAnimationLayerImageRaster::AddFrame(/* uint32 iLength */)
{
    UOdysseyAnimation* animation = GetAnimation();
    if ( !animation )
        return;

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = MakeShared<::ULIS::FBlock>(animation->Width(), animation->Height(), animation->Format());

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(animation->Format());
    ctx.Clear(*block.Get());
    ctx.Finish();

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = MakeShared<FOdysseyRasterBlock>(this);
    rasterBlock->SetBlock(block);
    rasterBlock->OnBlockChanged().AddUObject(this, &::UOdysseyAnimationLayerImageRaster::OnBlockChanged, rasterBlock);
    rasterBlock->OnBlockPtrChanged().AddUObject(this, &::UOdysseyAnimationLayerImageRaster::OnBlockPtrChanged, rasterBlock);

    mRasterBlocks.Add(rasterBlock);
    OnCellsChanged().Broadcast(this);
    RenderImageChanged(TRange<int>::Inclusive(mRasterBlocks.Num() - 1, mRasterBlocks.Num() - 1), false);
}

void
UOdysseyAnimationLayerImageRaster::InsertFrame(int iIndex /*, uint32 iLength */)
{
    UOdysseyAnimation* animation = GetAnimation();
    if ( !animation )
        return;

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = MakeShared<::ULIS::FBlock>(animation->Width(), animation->Height(), animation->Format());

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(animation->Format());
    ctx.Clear(*block.Get());
    ctx.Finish();

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = MakeShared<FOdysseyRasterBlock>(this);
    rasterBlock->SetBlock(block);
    rasterBlock->OnBlockChanged().AddUObject(this, &::UOdysseyAnimationLayerImageRaster::OnBlockChanged, rasterBlock);
    rasterBlock->OnBlockPtrChanged().AddUObject(this, &::UOdysseyAnimationLayerImageRaster::OnBlockPtrChanged, rasterBlock);

    mRasterBlocks.Insert(rasterBlock, iIndex);
    OnCellsChanged().Broadcast(this);
    RenderImageChanged(TRange<int>::Inclusive(iIndex, iIndex), false);
}

void
UOdysseyAnimationLayerImageRaster::OnBlockChanged(const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive, TSharedPtr<FOdysseyRasterBlock> iBlock)
{
    int frameIndex = mRasterBlocks.Find(iBlock);
    if (frameIndex == INDEX_NONE)
        return;

    RenderImageChanged(TRange<int>::Inclusive(frameIndex, frameIndex), iRects, iIsInteractive);
}

void
UOdysseyAnimationLayerImageRaster::OnBlockPtrChanged(TSharedPtr<FOdysseyRasterBlock> iBlock)
{
    int frameIndex = mRasterBlocks.Find(iBlock);
    if (frameIndex == INDEX_NONE)
        return;

    RenderImageChanged(TRange<int>::Inclusive(frameIndex, frameIndex), false);
}

TArray<::ULIS::FEvent>
UOdysseyAnimationLayerImageRaster::RenderImage(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!IsActivated)
        return iWaitList;

    if (!ioBlock)
        return iWaitList;

    if (iFrame < 0 || iFrame >= mRasterBlocks.Num())
        return iWaitList;

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = mRasterBlocks[iFrame];

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ULISRasterBlock = rasterBlock->IsBeingEdited() ? rasterBlock->GetUndoableBlock() : rasterBlock->GetBlock();
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

    return eventConvertAndExecute;
}

TArray<::ULIS::FEvent>
UOdysseyAnimationLayerImageRaster::CopyImage(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!IsActivated)
        return iWaitList;

    if (!ioBlock)
        return iWaitList;

    if (iFrame < 0 || iFrame >= mRasterBlocks.Num())
        return iWaitList;

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = mRasterBlocks[iFrame];

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ULISRasterBlock = rasterBlock->GetBlock();
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(ULISRasterBlock->Format());

    TArray<::ULIS::FEvent> eventConvertAndExecute = ULISUtils::ConvertAndExecute(ioBlock, ULISRasterBlock->Format(), iRect, iPos, iWaitList,
        [this, &ULISRasterBlock, &ctx](TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioDest, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) -> TArray<::ULIS::FEvent>
        {
            ::ULIS::FEvent eventCopy = FULISEventBuilder().RetainBlock(ULISRasterBlock).Build();
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

    return eventConvertAndExecute;
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
    RenderImageChanged(false);
}

void
UOdysseyAnimationLayerImageRaster::BlendModeChanged()
{
    OnBlendModeChanged().Broadcast(this);
    RenderImageChanged(false);
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

    for ( TSharedPtr<FOdysseyRasterBlock> rasterBlock : mRasterBlocks )
    {
        rasterBlock->OnBlockChanged().RemoveAll(this);
        rasterBlock->OnBlockPtrChanged().RemoveAll(this);
        rasterBlock->OnBlockChanged().AddUObject(this, &::UOdysseyAnimationLayerImageRaster::OnBlockChanged, rasterBlock);
        rasterBlock->OnBlockPtrChanged().AddUObject(this, &::UOdysseyAnimationLayerImageRaster::OnBlockPtrChanged, rasterBlock);
    }
}

void
UOdysseyAnimationLayerImageRaster::PostDuplicate(bool bDuplicateForPIE)
{
    for (TSharedPtr<FOdysseyRasterBlock> rasterBlock : mRasterBlocks)
    {
        rasterBlock->PostDuplicate();
    }

    Super::PostDuplicate(bDuplicateForPIE);
}

TSharedPtr<IOdysseyHandle>
UOdysseyAnimationLayerImageRaster::Preload(int iFrame)
{
    if (iFrame < 0 || iFrame >= mRasterBlocks.Num() )
        return nullptr;

    return mRasterBlocks[iFrame]->Preload();
}

void
UOdysseyAnimationLayerImageRaster::Serialize(FArchive& Ar)
{
    Super::Serialize(Ar);

    if ( Ar.IsLoading() )
    {
        mRasterBlocks.Empty();
    }

    int32 numBlocks = mRasterBlocks.Num();
    Ar << numBlocks;
    for ( int i = 0; i < numBlocks; i++ )
    {
        if ( Ar.IsLoading() )
        {
            mRasterBlocks.Add(MakeShared<FOdysseyRasterBlock>(this));
        }
        Ar << *mRasterBlocks[i];
    }
}


#undef LOCTEXT_NAMESPACE