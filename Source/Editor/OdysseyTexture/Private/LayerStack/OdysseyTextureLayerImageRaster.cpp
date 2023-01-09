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

#ifdef WITH_EDITOR
//#include "Editor/TransBuffer.h"
#endif

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

    RasterBlock->OnPixelsChanged().AddUObject(this, &::UOdysseyTextureLayerImageRaster::OnPixelsChanged);
    RasterBlock->OnBlockChanged().AddUObject(this, &::UOdysseyTextureLayerImageRaster::OnBlockChanged);
}

UOdysseyRasterBlock*
UOdysseyTextureLayerImageRaster::GetRasterBlock() const
{
	return RasterBlock;
}

void
UOdysseyTextureLayerImageRaster::OnPixelsChanged(const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive)
{
    RenderImageChanged(iRects, iIsInteractive);
}

void
UOdysseyTextureLayerImageRaster::OnBlockChanged()
{
    RenderImageChanged({ ::ULIS::FRectI::FromXYWH(0, 0, RasterBlock->GetWidth(), RasterBlock->GetHeight()) }, false);
}

/* void
UOdysseyTextureLayerImageRaster::UpdateBlock(const ::ULIS::FBlock& iSourceBlock, const TArray<::ULIS::FRectI>& iRects, const ::ULIS::FVec2I& iSourceOffset, const TArray<::ULIS::FEvent>& iWaitList, bool iTransaction)
{

    //Set dirtyRects to save only the changed rects into the undo system
    mNextDirtyRects = iRects; //Append in case UpdateBlock is called several times in the same root transaction

    //Mark package dirty and inform the undo system that something will change
    //It will serialize() this layer into the undo buffer
    Modify(); 

    mPrevDirtyRects = mNextDirtyRects;

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(mBlock->Format());

    for ( const ::ULIS::FRectI& rect : iRects )
    {
        ctx.ConvertFormat(
            iSourceBlock,
            *mBlock,
            rect,
            rect.Position() + iSourceOffset,
            ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
            iWaitList.Num(),
            iWaitList.GetData()
        );
    }

    ctx.Finish();

    mBlock->Dirty(iRects.GetData(), iRects.Num());

#ifdef WITH_EDITOR
    if (iTransaction)
        GEditor->EndTransaction();
#endif
}

#ifdef WITH_EDITOR
void
UOdysseyTextureLayerImageRaster::OnTransactionStateChanged(const FTransactionContext& TransactionContext, ETransactionStateEventType TransactionState)
{
    //if ( TransactionState == ETransactionStateEventType::TransactionFinalized )
    //{
        //Undo saving has been done, we can empty dirtyrects now
        //UTransBuffer* TransBuffer = CastChecked<UTransBuffer>(GEditor->Trans);
        //TransBuffer->OnTransactionStateChanged().RemoveAll(this);
    //}
}
#endif

void
UOdysseyTextureLayerImageRaster::SetRenderBlockOverride(::ULIS::FBlock* iBlock)
{
    check( !iBlock || (iBlock->Width() == mBlock->Width() && iBlock->Height() == mBlock->Height() && iBlock->Format() == mBlock->Format()) );
    mRenderBlockOverride = iBlock;
} */

TArray<::ULIS::FEvent>
UOdysseyTextureLayerImageRaster::RenderImage(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
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
            ::ULIS::FEvent eventBlend = FULISEventBuilder().RetainBlock(ULISRasterBlock).Build();
            ctx.Blend(
                /* mRenderBlockOverride ? *mRenderBlockOverride : */*ULISRasterBlock,
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

    RasterBlock->Update(ULISRasterBlock, { ULISRasterBlock->Rect() }, false);
}
/*
void
UOdysseyTextureLayerImageRaster::Serialize(FArchive& Ar)
{
	//TODO: Move this to ULIS, once ULIS has been moved to ILIAD.
	//TODO: Or create a intermediate URasterBlock class to manage this and all operations on blocks.
	Super::Serialize(Ar);

    if ( Ar.IsTransacting() ) //don't save the whole block when transacting, let the transaction annotation do the job
        return;

	uint32 serializeVersion = 0;
	Ar << serializeVersion;

	//Load/Save Size & Format
    int width = mBlock ? mBlock->Width() : 0;
    int height = mBlock ? mBlock->Height() : 0;
    ::ULIS::eFormat format = ::ULIS::Format_BGRA8;

	if ( Ar.IsSaving() )
	{
		if ( mBlock )
		{
            format = mBlock->Format();
		}
	}
			
    uint32 formatInt = static_cast<uint32>(format);
    Ar << width;
    Ar << height;
	Ar << formatInt;
    format = static_cast<ULIS::eFormat>(formatInt);

    //Create mBlock if we are loading
    if (Ar.IsLoading())
    {
		if ( !mBlock && width > 0 && height > 0 )
		{
			mBlock = new ::ULIS::FBlock(width, height, format);
            mBlock->OnInvalid( ::ULIS::FOnInvalidBlock( &OnBlockInvalidated, static_cast< void* >( this ) ) );
		}
    }

	//Serialize mBlock using Zlib
    if ( mBlock )
    {
        Ar.SerializeCompressed(mBlock->Bits(), mBlock->BytesTotal(), NAME_Zlib);
        //mBlock->Dirty();
    }
} */
/*
#if WITH_EDITOR
UOdysseyTextureLayerImageRaster::FBlockTransactionAnnotation::FBlockTransactionAnnotation()
    : mPrevBlocks()
    , mNextBlocks()
{
}

UOdysseyTextureLayerImageRaster::FBlockTransactionAnnotation::FBlockTransactionAnnotation(const UOdysseyTextureLayerImageRaster* iLayer, const TArray<::ULIS::FRectI>& iPrevRects, const TArray<::ULIS::FRectI>& iNextRects)
    : mLayer(const_cast<UOdysseyTextureLayerImageRaster*>(iLayer))
    , mPrevBlocks()
    , mNextBlocks()
    , mPrevRects(iPrevRects)
    , mNextRects(iNextRects)
{
    if ( !mLayer->mBlock )
        return;

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(mLayer->mBlock->Format());
    for ( const ::ULIS::FRectI& rect : mPrevRects )
    {
        ::ULIS::FBlock& block = mPrevBlocks.Emplace_GetRef(rect.w, rect.h, mLayer->mBlock->Format());
        ctx.Copy(
            *mLayer->mBlock,
            block,
            rect,
            ::ULIS::FVec2F(0)
        );
        ctx.Finish();
    }

    for (const ::ULIS::FRectI& rect : mNextRects)
    {
        ::ULIS::FBlock& block = mNextBlocks.Emplace_GetRef(rect.w, rect.h, mLayer->mBlock->Format());
        ctx.Copy(
            *mLayer->mBlock,
            block,
            rect,
            ::ULIS::FVec2F(0)
        );
        ctx.Finish();
    }
}

void
UOdysseyTextureLayerImageRaster::FBlockTransactionAnnotation::CopyToBlock()
{   
    if ( !mLayer->mBlock )
        return;

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(mLayer->mBlock->Format());
    for ( int i = 0; i < mPrevRects.Num(); i++ )
    {
        //Load rect Block
        ctx.Copy(
            mPrevBlocks[i],
            *mLayer->mBlock,
            ::ULIS::FRectI::Auto,
            mPrevRects[i].Position()
        );
        ctx.Finish();
    }

    for ( int i = 0; i < mNextRects.Num(); i++ )
    {
        //Load rect Block
        ctx.Copy(
            mNextBlocks[i],
            *mLayer->mBlock,
            ::ULIS::FRectI::Auto,
            mNextRects[i].Position()
        );
        ctx.Finish();
    }

    mLayer->mPrevDirtyRects = mPrevRects;
    mLayer->mNextDirtyRects = mNextRects;

    TArray<::ULIS::FRectI> rects = mPrevRects;
    rects.Append(mNextRects);
    mLayer->mBlock->Dirty(rects.GetData(), rects.Num());
}

void
UOdysseyTextureLayerImageRaster::FBlockTransactionAnnotation::Serialize(FArchive& Ar)
{
    if ( !mLayer->mBlock )
        return;

    ::ULIS::eFormat format = Ar.IsSaving() ? mLayer->mBlock->Format() : ::ULIS::Format_BGRA8;
    uint32 formatInt = static_cast<uint32>(format);
	Ar << formatInt;
    format = static_cast<ULIS::eFormat>(formatInt);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);

    int32 numPrevRects = mPrevRects.Num();
    Ar << numPrevRects;
    int32 numNextRects = mNextRects.Num();
    Ar << numNextRects;
    if ( Ar.IsSaving() )
    {
        for ( int i = 0; i < numPrevRects; i++ )
        {
            ::ULIS::FRectI& rect = mPrevRects[i];
            //Save Rect
            Ar << rect.x;
            Ar << rect.y;
            Ar << rect.w;
            Ar << rect.h;

            //Save rect Block
            Ar.SerializeCompressed(mPrevBlocks[i].Bits(), mPrevBlocks[i].BytesTotal(), NAME_Zlib);
        }

        for ( int i = 0; i < numNextRects; i++ )
        {
            ::ULIS::FRectI& rect = mNextRects[i];

            //Save Rect
            Ar << rect.x;
            Ar << rect.y;
            Ar << rect.w;
            Ar << rect.h;

            //Save rect Block
            Ar.SerializeCompressed(mNextBlocks[i].Bits(), mNextBlocks[i].BytesTotal(), NAME_Zlib);
        }
    }
    else if ( Ar.IsLoading() )
    {
        for ( int i = 0; i < numPrevRects; i++ )
        {
            //Load Rect
            ::ULIS::FRectI rect;
            Ar << rect.x;
            Ar << rect.y;
            Ar << rect.w;
            Ar << rect.h;

            mPrevRects.Add(rect);

            //Load rect Block
            ::ULIS::FBlock& block = mPrevBlocks.Emplace_GetRef(rect.w, rect.h, format);
            Ar.SerializeCompressed(block.Bits(), block.BytesTotal(), NAME_Zlib);
        }

        for ( int i = 0; i < numNextRects; i++ )
        {
            //Load Rect
            ::ULIS::FRectI rect;
            Ar << rect.x;
            Ar << rect.y;
            Ar << rect.w;
            Ar << rect.h;

            mNextRects.Add(rect);

            //Load rect Block
            ::ULIS::FBlock& block = mNextBlocks.Emplace_GetRef(rect.w, rect.h, format);
            Ar.SerializeCompressed(block.Bits(), block.BytesTotal(), NAME_Zlib);
        }
    }
}

TSharedPtr<ITransactionObjectAnnotation>
UOdysseyTextureLayerImageRaster::FactoryTransactionAnnotation(const ETransactionAnnotationCreationMode InCreationMode) const
{
    if (InCreationMode == ETransactionAnnotationCreationMode::DefaultInstance)
	{
		return MakeShared<FBlockTransactionAnnotation>();
	}

	return MakeShared<FBlockTransactionAnnotation>(this, mPrevDirtyRects, mNextDirtyRects);
}

void
UOdysseyTextureLayerImageRaster::PostEditUndo(TSharedPtr<ITransactionObjectAnnotation> TransactionAnnotation)
{
    TSharedPtr<FBlockTransactionAnnotation> blockAnnotation = StaticCastSharedPtr<FBlockTransactionAnnotation>(TransactionAnnotation);
    if (!blockAnnotation)
        return;

    blockAnnotation->CopyToBlock();

    Super::PostEditUndo(TransactionAnnotation);    
}*/

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

    UTexture2D* texture = layerStack->GetTexture();
    if (!texture)
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

    UTexture2D* texture = layerStack->GetTexture();
    if (!texture)
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
        RasterBlock->OnPixelsChanged().RemoveAll(this);
        RasterBlock->OnBlockChanged().RemoveAll(this);
        RasterBlock->OnPixelsChanged().AddUObject(this, &::UOdysseyTextureLayerImageRaster::OnPixelsChanged);
        RasterBlock->OnBlockChanged().AddUObject(this, &::UOdysseyTextureLayerImageRaster::OnBlockChanged);
    }
}

void
UOdysseyTextureLayerImageRaster::PostDuplicate(bool bDuplicateForPIE)
{
    //TODO: call mRaster Block Init() again if needed

    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetLayerStack());
    if (!layerStack)
        return;

    UTexture2D* texture = layerStack->GetTexture();
    if (!texture)
        return;

    ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(texture->Source.GetFormat());
    //let's ensure the format has alpha, so add alpha channel of needed
    format = static_cast< ::ULIS::eFormat >(format | ULIS_W_ALPHA( 1 ) );

    if (RasterBlock->GetWidth() == texture->Source.GetSizeX() && RasterBlock->GetHeight() == texture->Source.GetSizeY() && RasterBlock->GetFormat() == format)
        return;
    
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ULISRasterBlock = RasterBlock->GetBlock();
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = MakeShared<::ULIS::FBlock>( texture->Source.GetSizeX(), texture->Source.GetSizeY(), format);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
    ctx.ConvertFormat(
        *ULISRasterBlock.Get(),
        *block.Get(),
        ::ULIS::FRectI::Auto,
        ::ULIS::FVec2I( 0 ),
        ::ULIS::FSchedulePolicy::AsyncCacheEfficient
    );
    ctx.Finish();

    RasterBlock->SetBlock(block);
}
/* 
void
UOdysseyTextureLayerImageRaster::OnBlockInvalidated(const ::ULIS::FBlock* iBlock, const ::ULIS::FRectI* iRects, const uint32 iNumRects, void* iInfo)
{
    //Indicate UObject system that we will change LayersHierarchy property
    //(actually the block has already changed, but there no where else to call it before here and being sure that PostChangePropertyValue will be called after)
    

    TArray<::ULIS::FRectI> rects(iRects, iNumRects);
    UOdysseyTextureLayerImageRaster* self = static_cast<UOdysseyTextureLayerImageRaster*>(iInfo);
    self->RenderImageChanged(rects);
} */

#undef LOCTEXT_NAMESPACE