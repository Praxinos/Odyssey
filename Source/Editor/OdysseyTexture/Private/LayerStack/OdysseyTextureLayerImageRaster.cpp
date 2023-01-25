// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTextureLayerImageRaster.h"

#include "OdysseyPixelFormat.h"
#include "ULISEventBuilder.h"
#include "ULISLoaderModule.h"
#include "OdysseyStyleSet.h"
#include "OdysseyRasterBlock.h"
#include "OdysseyTextureLayerImageRaster.h"
#include "OdysseySurfaceTexture2DEditable.h"

#define LOCTEXT_NAMESPACE "UOdysseyTextureLayerImageRaster"

UOdysseyTextureLayerImageRaster::FOnIsAlphaLockedChanged&
UOdysseyTextureLayerImageRaster::OnIsAlphaLockedChanged()
{
    static FOnIsAlphaLockedChanged onIsAlphaLockedChanged;
    return onIsAlphaLockedChanged;
}

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
{
	LayerTypeName = LOCTEXT("LayerTypeName", "Raster Image Layer");
    Icon = *FOdysseyStyle::GetBrush( "OdysseyLayerStack.ImageLayer16");
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
    RasterBlock = NewObject<UOdysseyRasterBlock>(this, "RasterBlock", RF_Public | RF_Transactional);
    RasterBlock->SetBlock(block);

    RasterBlock->OnBlockChanged().AddUObject(this, &::UOdysseyTextureLayerImageRaster::OnBlockChanged);
    RasterBlock->OnBlockPtrChanged().AddUObject(this, &::UOdysseyTextureLayerImageRaster::OnBlockPtrChanged);
}

UOdysseyRasterBlock*
UOdysseyTextureLayerImageRaster::GetRasterBlock() const
{
	return RasterBlock;
}

void
UOdysseyTextureLayerImageRaster::OnBlockChanged(const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive)
{
    RenderImageChanged(iRects, iIsInteractive);
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

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ULISRasterBlock = RasterBlock->IsBeingEdited() ? RasterBlock->GetEditableBlock() : RasterBlock->GetBlock();
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

    RasterBlock->Modify();
    
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ULISRasterBlock = RasterBlock->GetBlock();

    //Make tmpblock to merge the layers into
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(ULISRasterBlock->Format());

    TArray<::ULIS::FEvent> lastEvent = {};
    for (UOdysseyLayer* layer : iLayers)
    {
        UOdysseyTextureLayer* textureLayer = Cast<UOdysseyTextureLayer>(layer);
        if (!textureLayer)
            continue;

        lastEvent = textureLayer->RenderImage(ULISRasterBlock, ULISRasterBlock->Rect(), ::ULIS::FVec2I(0), lastEvent);
    }
    ctx.Finish();

    RasterBlock->Invalidate({ ULISRasterBlock->Rect() }, false);
}

void
UOdysseyTextureLayerImageRaster::IsAlphaLockedChanged()
{
    OnIsAlphaLockedChanged().Broadcast(this);
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
    if (iPropertyName == "IsAlphaLocked")
        IsAlphaLockedChanged();
}

void
UOdysseyTextureLayerImageRaster::PostLoad()
{
    Super::PostLoad();
    if ( RasterBlock )
    {
        RasterBlock->OnBlockChanged().RemoveAll(this);
        RasterBlock->OnBlockPtrChanged().RemoveAll(this);
        RasterBlock->OnBlockChanged().AddUObject(this, &::UOdysseyTextureLayerImageRaster::OnBlockChanged);
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

    //ensure we remove any callbacks registered on the wrong Rasterblock
    RasterBlock->OnBlockChanged().RemoveAll(this);
    RasterBlock->OnBlockPtrChanged().RemoveAll(this);

    //Replace old rasterblock with an owned one
    RasterBlock = NewObject<UOdysseyRasterBlock>(this, "RasterBlock", RF_Public | RF_Transactional);
    RasterBlock->SetBlock(duplicatedBlock);

    //Set the right callbacks
    RasterBlock->OnBlockChanged().AddUObject(this, &::UOdysseyTextureLayerImageRaster::OnBlockChanged);
    RasterBlock->OnBlockPtrChanged().AddUObject(this, &::UOdysseyTextureLayerImageRaster::OnBlockPtrChanged);
}

void
UOdysseyTextureLayerImageRaster::Preload(TArray<TSharedPtr<IOdysseyHandle>>& oHandles)
{
    if ( RasterBlock )
        oHandles.Add(RasterBlock->Preload());
}


#undef LOCTEXT_NAMESPACE