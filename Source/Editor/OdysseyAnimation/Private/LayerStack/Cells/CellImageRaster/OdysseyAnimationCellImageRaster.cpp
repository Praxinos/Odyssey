// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"

#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRasterImageRenderer.h"
#include "ULISLoaderModule.h"
#include "OdysseyMediaRaster.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRasterExport.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRasterImport.h"
#include "Misc/OdysseyDuplicate.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "OdysseyRasterBlock.h"
#include "OdysseyAnimation.h"

TSharedPtr<FOdysseyRasterBlock>
UOdysseyAnimationCellImageRaster::GetRasterBlock() const
{
	if (!mRasterBlock)
	{
		int width = GetAnimation()->GetWidth();
		int height = GetAnimation()->GetHeight();
		::ULIS::eFormat format = GetAnimation()->GetFormat();

		mRasterBlock = MakeShared<FOdysseyRasterBlock>(const_cast<UOdysseyAnimationCellImageRaster*>(this), width, height, format);
	
		mRasterBlock->OnBlockChanged().AddUObject(const_cast<UOdysseyAnimationCellImageRaster*>(this), &UOdysseyAnimationCellImageRaster::OnBlockChanged);
		mRasterBlock->OnBlockCommited().AddUObject(const_cast<UOdysseyAnimationCellImageRaster*>(this), &UOdysseyAnimationCellImageRaster::OnBlockCommited);
		mRasterBlock->PostProcess().BindUObject(const_cast<UOdysseyAnimationCellImageRaster*>(this), &UOdysseyAnimationCellImageRaster::RasterBlockPostProcess);
	}
    return mRasterBlock;
}

void
UOdysseyAnimationCellImageRaster::PostDuplicate(EDuplicateMode::Type iDuplicateMode)
{
    Super::PostDuplicate(iDuplicateMode);

    //The cell could be duplicated in a different animation with different parameters
    //Ensure the block uses those parameters
	if (mRasterBlock)
	{
		::ULIS::eFormat format = GetAnimation()->GetFormat();
		int width = GetAnimation()->GetWidth();
		int height = GetAnimation()->GetHeight();
		mRasterBlock->PostDuplicate();
		mRasterBlock->ConvertTo(width, height, format);
	
		mRasterBlock->OnBlockChanged().AddUObject(this, &UOdysseyAnimationCellImageRaster::OnBlockChanged);
		mRasterBlock->OnBlockCommited().AddUObject(this, &UOdysseyAnimationCellImageRaster::OnBlockCommited);
		mRasterBlock->PostProcess().BindUObject(this, &UOdysseyAnimationCellImageRaster::RasterBlockPostProcess);
	}
}

TArray<::ULIS::FEvent>
UOdysseyAnimationCellImageRaster::RasterBlockPostProcess(const TMap<FIntPoint, TSharedPtr<::ULIS::FBlock>>& iOriginalBlocks, const FULISInvalidTileMap& iInvalidMap, const TArray<::ULIS::FEvent>& iWaitList)
{
	UOdysseyAnimationLayerImageRaster* layer = Cast<UOdysseyAnimationLayerImageRaster>(GetLayer());
    if (!layer || !layer->IsAlphaLocked)
        return iWaitList;

    //Apply AlphaLock
	TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlock();

    TArray<::ULIS::FEvent> events;
    TSharedPtr<::ULIS::FBlock> block = rasterBlock->GetBlock();
    TArray<FIntPoint> invalidTiles = iInvalidMap.InvalidTiles();    
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(rasterBlock->GetFormat());
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

bool
UOdysseyAnimationCellImageRaster::IsImageRenderingGameThreadOnly() const
{
    TSharedPtr<FOdysseyMediaRaster> mediaRaster = mMediaRaster.Pin();
    return !!mediaRaster;
}

TSharedPtr<IOdysseyImageRenderer>
UOdysseyAnimationCellImageRaster::BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame, FImageRendererFilter iFilter) const
{
    if (iFilter.IsBound() && !iFilter.Execute(this))
        return nullptr;
        
    return MakeShared<FOdysseyAnimationCellImageRasterImageRenderer>(this, iFrame, iRenderType, GetImageRenderingRects(), iFilter);
}

TArray<FGuid>
UOdysseyAnimationCellImageRaster::GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex) const
{
    return { GetImageRenderingId() };
}

TArray<::ULIS::FRectI>
UOdysseyAnimationCellImageRaster::GetImageRenderingRects() const
{
    if (!mRasterBlock)
        return {};

    return { ::ULIS::FRectI::FromXYWH(0, 0, mRasterBlock->GetWidth(), mRasterBlock->GetHeight()) };
}

void
UOdysseyAnimationCellImageRaster::OnBlockChanged(const TArray<::ULIS::FRectI>& iRects)
{
    ImageRenderingChanged(iRects, true);
}

void
UOdysseyAnimationCellImageRaster::OnBlockCommited(const TArray<::ULIS::FRectI>& iRects)
{
	DirtyThumbnail();
    ImageRenderingChanged(iRects);
}

FOdysseyMediaProvider
UOdysseyAnimationCellImageRaster::GetMediaProvider(uint32 iFrameIndex) const
{
    //Don't create a mediaRaster if there is an image render in use
    FScopeLock lock(&mImageRenderingMutex);

    TSharedPtr<FOdysseyMediaRaster> mediaRaster = MakeShared<FOdysseyMediaRaster>(GetRasterBlock());
    mMediaRaster = mediaRaster;
    FOdysseyMediaProvider mediaProvider;
    mediaProvider.Add(mediaRaster);
    return mediaProvider;
}

FCriticalSection*
UOdysseyAnimationCellImageRaster::GetImageRenderingMutex() const
{
    return &mImageRenderingMutex;
}


void
UOdysseyAnimationCellImageRaster::Serialize(FArchive& Ar)
{
    Super::Serialize(Ar);

    if( Ar.IsSaving() )
    {
        FOdysseyAnimationCellImageRasterExport::Write( this, Ar );
    }

    if( Ar.IsLoading() )
    {
        if (!FOdysseyAnimationCellImageRasterImport::Read( this, Ar ))
        {
        }
    }
}

void
UOdysseyAnimationCellImageRaster::OldSerialize(FArchive& Ar)
{
    Super::OldSerialize(Ar);

    if( Ar.IsSaving() )
    {
        FOdysseyAnimationCellImageRasterExport::Write( this, Ar );
    }

    if( Ar.IsLoading() )
    {
        if (!FOdysseyAnimationCellImageRasterImport::Read( this, Ar ))
        {
            //Old Style No Chunk Loading
            Ar << *mRasterBlock;
        }
    }
}
