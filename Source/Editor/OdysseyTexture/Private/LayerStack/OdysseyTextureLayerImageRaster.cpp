// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTextureLayerImageRaster.h"

#include "OdysseyPixelFormat.h"
#include "ULISEventBuilder.h"
#include "ULISLoaderModule.h"
#include "OdysseyStyleSet.h"
#include "OdysseyRasterBlock.h"
#include "OdysseyRasterBlockMutator.h"
#include "OdysseyRasterBlockUndo.h"
#include "OdysseyTextureLayerImageRaster.h"
#include "OdysseySurfaceTexture2DEditable.h"

#define LOCTEXT_NAMESPACE "UOdysseyTextureLayerImageRaster"

UOdysseyTextureLayerImageRaster::FOnBlendModeChanged&
UOdysseyTextureLayerImageRaster::OnBlendModeChanged()
{
    static FOnBlendModeChanged onBlendModeChanged;
    return onBlendModeChanged;
}

UOdysseyTextureLayerImageRaster::FOnOpacityChanged&
UOdysseyTextureLayerImageRaster::OnOpacityChanged()
{
    static FOnOpacityChanged onOpacityChanged;
    return onOpacityChanged;
}

UOdysseyTextureLayerImageRaster::~UOdysseyTextureLayerImageRaster()
{
}

UOdysseyTextureLayerImageRaster::UOdysseyTextureLayerImageRaster()
    : RasterBlock(MakeShared<FOdysseyRasterBlock>(this))
{
	LayerTypeName = LOCTEXT("LayerTypeName", "Raster Image Layer");
    Icon = *FOdysseyStyle::GetBrush( "OdysseyLayerStack.LayerBitmap16");
}

void
UOdysseyTextureLayerImageRaster::OnCreated_Implementation()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetLayerStack());
    if(!layerStack)
        return;

    UTexture2D* texture = layerStack->GetTexture();
    ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(texture->Source.GetFormat());
    //let's ensure the format has alpha, so add alpha channel of needed
    format = static_cast< ::ULIS::eFormat >(format | ULIS_W_ALPHA( 1 ) );

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = MakeShared<::ULIS::FBlock>( texture->Source.GetSizeX(), texture->Source.GetSizeY(), format);

	::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
    ctx.Clear(*block.Get());
    ctx.Finish();

    //Caches the tiles on disk, we do this
    RasterBlock->SetBlock(block);
    RasterBlock->OnBlockChanged().AddUObject(this, &::UOdysseyTextureLayerImageRaster::OnBlockChanged);
    RasterBlock->OnBlockCommited().AddUObject(this, &::UOdysseyTextureLayerImageRaster::OnBlockCommited);
    RasterBlock->OnBlockPtrChanged().AddUObject(this, &::UOdysseyTextureLayerImageRaster::OnBlockPtrChanged);
}

TSharedPtr<FOdysseyRasterBlock>
UOdysseyTextureLayerImageRaster::GetRasterBlock() const
{
	return RasterBlock;
}

void
UOdysseyTextureLayerImageRaster::OnBlockChanged(const TArray<::ULIS::FRectI>& iRects)
{
    RenderImageChanged(iRects, true);
}

void
UOdysseyTextureLayerImageRaster::OnBlockCommited(const TArray<::ULIS::FRectI>& iRects)
{
    RenderImageChanged(iRects, false);
}

void
UOdysseyTextureLayerImageRaster::OnBlockPtrChanged()
{
    RenderImageChanged({ ::ULIS::FRectI::FromXYWH(0, 0, RasterBlock->GetWidth(), RasterBlock->GetHeight()) }, false);
}

TArray<::ULIS::FEvent>
UOdysseyTextureLayerImageRaster::RenderImage(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!IsActivated)
        return iWaitList;

    if (!ioBlock)
        return iWaitList;

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ULISRasterBlock = /* RasterBlock->IsBeingEdited() ? RasterBlock->GetUndoableBlock() : */ RasterBlock->GetBlock();
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
UOdysseyTextureLayerImageRaster::CopyImage(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!IsActivated)
        return iWaitList;

    if (!ioBlock)
        return iWaitList;

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ULISRasterBlock = RasterBlock->GetBlock();
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

    return eventConvertAndExecute;
}

void
UOdysseyTextureLayerImageRaster::Merge(const TArray<UOdysseyLayer*>& iLayers)
{
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("Layer Image Raster", "Merge Layers"));
#endif
    
    FOdysseyRasterBlockMutator mutator(RasterBlock);
    mutator.EditTilesFromRects(
        { ::ULIS::FRectI::FromXYWH(0, 0, RasterBlock->GetWidth(), RasterBlock->GetHeight()) },
        FOdysseyRasterBlockMutator::FEditDelegate::CreateLambda(
            [&](const FULISInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
            {
                TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ULISRasterBlock = RasterBlock->GetBlock();
                ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(RasterBlock->GetFormat());
                TArray<::ULIS::FEvent> lastEvent = {};
                for ( UOdysseyLayer* layer : iLayers )
                {
                    UOdysseyTextureLayer* textureLayer = Cast<UOdysseyTextureLayer>(layer);
                    if ( !textureLayer )
                        continue;
                    lastEvent = textureLayer->RenderImage(ULISRasterBlock, ULISRasterBlock->Rect(), ::ULIS::FVec2I(0), lastEvent);
                }
                return { lastEvent };
            }
        )
    );
    mutator.Commit();
}

void
UOdysseyTextureLayerImageRaster::OpacityChanged()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetLayerStack());
    if (!layerStack)
        return;

    OnOpacityChanged().Broadcast(this);
    RenderImageChanged({ ::ULIS::FRectI::FromXYWH(0, 0, RasterBlock->GetWidth(), RasterBlock->GetHeight()) }, false);
}

void
UOdysseyTextureLayerImageRaster::BlendModeChanged()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetLayerStack());
    if (!layerStack)
        return;

    OnBlendModeChanged().Broadcast(this);
    RenderImageChanged({ ::ULIS::FRectI::FromXYWH(0, 0, RasterBlock->GetWidth(), RasterBlock->GetHeight()) }, false);
}

void
UOdysseyTextureLayerImageRaster::PropertyChanged(const FName& iPropertyName)
{
    Super::PropertyChanged(iPropertyName);

    if (iPropertyName == "BlendMode")
        BlendModeChanged();
    if (iPropertyName == "Opacity")
        OpacityChanged();
}

void
UOdysseyTextureLayerImageRaster::PostLoad()
{
    Super::PostLoad();
    if ( RasterBlock )
    {
        RasterBlock->OnBlockChanged().RemoveAll(this);
        RasterBlock->OnBlockCommited().RemoveAll(this);
        RasterBlock->OnBlockPtrChanged().RemoveAll(this);
        RasterBlock->OnBlockChanged().AddUObject(this, &::UOdysseyTextureLayerImageRaster::OnBlockChanged);
        RasterBlock->OnBlockCommited().AddUObject(this, &::UOdysseyTextureLayerImageRaster::OnBlockCommited);
        RasterBlock->OnBlockPtrChanged().AddUObject(this, &::UOdysseyTextureLayerImageRaster::OnBlockPtrChanged);
    }
}

void
UOdysseyTextureLayerImageRaster::PostDuplicate(bool bDuplicateForPIE)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetLayerStack());
    if (!layerStack)
        return;

    UTexture2D* texture = layerStack->GetTexture();
    if (!texture)
        return;

    //At the start of this method, RasterBlock is a pointer to the original object
    //We have to create a new owned RasterBlock to avoid blocks to be shared between several layers

    ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(texture->Source.GetFormat());
    //let's ensure the format has alpha, so add alpha channel of needed
    format = static_cast< ::ULIS::eFormat >(format | ULIS_W_ALPHA( 1 ) );
    
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> originalBlock = RasterBlock->GetBlock();
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> duplicatedBlock = MakeShared<::ULIS::FBlock>( texture->Source.GetSizeX(), texture->Source.GetSizeY(), format);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
    ctx.ConvertFormat(
        *originalBlock.Get(),
        *duplicatedBlock.Get(),
        ::ULIS::FRectI::Auto,
        ::ULIS::FVec2I( 0 ),
        ::ULIS::FSchedulePolicy::AsyncCacheEfficient
    );
    ctx.Finish();

    //Replace old rasterblock with an owned one
    RasterBlock->SetBlock(duplicatedBlock);
}

TSharedPtr<IOdysseyHandle>
UOdysseyTextureLayerImageRaster::Preload()
{
    return RasterBlock->Preload();
}

void
UOdysseyTextureLayerImageRaster::Serialize(FArchive& Ar)
{
    Super::Serialize(Ar);
    
    Ar << *RasterBlock;
}

TArray<::ULIS::FEvent>
UOdysseyTextureLayerImageRaster::RasterBlockPostProcess(const TMap<FIntPoint, TSharedPtr<::ULIS::FBlock>>& iOriginalBlocks, const FULISInvalidTileMap& iInvalidMap, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!IsAlphaLocked)
        return iWaitList;

    //Apply AlphaLock
    TArray<::ULIS::FEvent> events;
    TSharedPtr<::ULIS::FBlock> block = RasterBlock->GetBlock();
    TArray<FIntPoint> invalidTiles = iInvalidMap.InvalidTiles();    
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(RasterBlock->GetFormat());
    for (const FIntPoint& invalidTile : invalidTiles)
    {
        TSharedPtr<::ULIS::FBlock> originalBlock = iOriginalBlocks[invalidTile];
        ::ULIS::FRectI rect = iInvalidMap.GetTileRect(invalidTile);
        ::ULIS::FEvent eventBlend;
        ctx.Blend(
            *originalBlock
            , *block
            , ::ULIS::FRectI::Auto
            , rect.Position()
            , ::ULIS::Blend_Back
            , ::ULIS::Alpha_Top
            , 1.f
            , ::ULIS::FSchedulePolicy::AsyncCacheEfficient
            , iWaitList.Num()
            , iWaitList.GetData()
            , &eventBlend
        );
        events.Add(eventBlend);
    }

    return events;
}

#undef LOCTEXT_NAMESPACE