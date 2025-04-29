// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationCellImageRaster.h"

#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "ScreenPass.h"

#if WITH_EDITOR
#include "ULISLoaderModule.h"
#include "OdysseyMediaRaster.h"
#include "OdysseyAnimationCellImageRasterExport.h"
#include "OdysseyAnimationCellImageRasterImport.h"
#include "OdysseyAnimationLayerImageRaster.h"
#include "OdysseyRasterBlock.h"
#include "OdysseyAnimation.h"
#include "ULISUtils.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "CanvasTypes.h"
#include "CanvasItem.h"
#include "OdysseyPixelFormat.h"
#include "TextureCompiler.h"
#include "UObject/ObjectSaveContext.h"
#endif

UOdysseyAnimationCellImageRaster::UOdysseyAnimationCellImageRaster()
{
}

TArray<FGuid>
UOdysseyAnimationCellImageRaster::GetRenderingComposition(EOdysseyRenderingType iRenderType, int iFrameIndex) const
{
    return { GetRenderingId() };
}

FIntRect
UOdysseyAnimationCellImageRaster::GetDefaultRenderRect() const
{
    return FIntRect(0, 0, GetAnimation()->GetWidth(), GetAnimation()->GetHeight());
}

FOdysseyTextureRenderFunction
UOdysseyAnimationCellImageRaster::BuildRenderPipeline(
    FFrameNumber iFrame,
    EOdysseyRenderingType iType
) const
{

#if WITH_EDITOR
    if ( !Texture )
        InitTexture();
#endif

    return Super::BuildRenderPipeline(iFrame, iType);
}

#if WITH_EDITOR
void
UOdysseyAnimationCellImageRaster::InitRasterBlock() const
{
    if ( !mRasterBlock )
    {
        int width = GetAnimation()->GetWidth();
        int height = GetAnimation()->GetHeight();

        ::ULIS::eFormat format = ::ULIS::Format_BGRA8;
        switch ( GetAnimation()->GetFormat() )
        {
            case EOdysseyAnimationFormat::BGRA8: format = ::ULIS::Format_BGRA8;
            case EOdysseyAnimationFormat::RGBAF: format = ::ULIS::Format_RGBAF;
        }

        mRasterBlock = MakeShared<FOdysseyRasterBlock>(const_cast<UOdysseyAnimationCellImageRaster*>(this), width, height, format);
    }

    mRasterBlock->OnBlockChanged().RemoveAll(this);
    mRasterBlock->OnBlockCommited().RemoveAll(this);

    mRasterBlock->OnBlockChanged().AddUObject(const_cast<UOdysseyAnimationCellImageRaster*>(this), &UOdysseyAnimationCellImageRaster::OnBlockChanged);
    mRasterBlock->OnBlockCommited().AddUObject(const_cast<UOdysseyAnimationCellImageRaster*>(this), &UOdysseyAnimationCellImageRaster::OnBlockCommited);
    mRasterBlock->PostProcess().BindUObject(const_cast<UOdysseyAnimationCellImageRaster*>(this), &UOdysseyAnimationCellImageRaster::RasterBlockPostProcess);
}

void
UOdysseyAnimationCellImageRaster::InitTexture() const
{
    if ( !Texture )
    {
        Texture = NewObject<UTexture2D>(const_cast<UOdysseyAnimationCellImageRaster*>(this), TEXT("Texture"));
        Texture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
        Texture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
        Texture->Filter = TextureFilter::TF_Nearest;
    }

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlock(); //ensures mRasterBlock exists
    InitTextureWithBlockData(rasterBlock->GetBlock().Get(), Texture, TextureSourceFormatForULISFormat(rasterBlock->GetFormat()));
    Texture->UpdateResource();
    FTextureCompilingManager::Get().FinishCompilation({ Texture });
}

TSharedPtr<FOdysseyRasterBlock>
UOdysseyAnimationCellImageRaster::GetRasterBlock() const
{
    if (!mRasterBlock)
        InitRasterBlock();

    return mRasterBlock;
}

UTexture2D*
UOdysseyAnimationCellImageRaster::GetTexture() const
{
    if ( !Texture )
        InitTexture();

    return Texture;
}

void
UOdysseyAnimationCellImageRaster::PostDuplicate(EDuplicateMode::Type iDuplicateMode)
{
    Super::PostDuplicate(iDuplicateMode);

    //The cell could be duplicated in a different animation with different parameters
    //Ensure the block uses those parameters
    ::ULIS::eFormat format = ::ULIS::Format_BGRA8;
    switch(GetAnimation()->GetFormat())
    {
        case EOdysseyAnimationFormat::BGRA8: format = ::ULIS::Format_BGRA8;
        case EOdysseyAnimationFormat::RGBAF: format = ::ULIS::Format_RGBAF;
    }

    int width = GetAnimation()->GetWidth();
    int height = GetAnimation()->GetHeight();

    if (mRasterBlock)
    {
        mRasterBlock->PostDuplicate();
        mRasterBlock->ConvertTo(width, height, format);

        InitRasterBlock();
        InitTexture();
    }
}

void
UOdysseyAnimationCellImageRaster::PostLoad()
{
    Super::PostLoad();

    if (mRasterBlock)
    {
        InitRasterBlock();
        InitTexture();
    }
}

TArray<::ULIS::FEvent>
UOdysseyAnimationCellImageRaster::RasterBlockPostProcess(const TMap<FIntPoint, TSharedPtr<::ULIS::FBlock>>& iOriginalBlocks, const FOdysseyInvalidTileMap& iInvalidMap, const TArray<::ULIS::FEvent>& iWaitList)
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
        ::ULIS::FRectI rect = ::ULISUtils::ToULISRectI(iInvalidMap.GetTileRect(invalidTile));
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


void
UOdysseyAnimationCellImageRaster::OnBlockChanged(const TArray<::ULIS::FRectI>& iRects)
{
    //CopyBlockDataIntoUTexture(GetRasterBlock()->GetBlock().Get(), GetTexture());
    //Texture->UpdateResource();
    FOdysseySurfaceTexture2DEditable surface(Texture, GetRasterBlock()->GetBlock());
    surface.Invalidate(iRects);
    RenderingChanged(::ULISUtils::ToIntRects(iRects), true);
}

void
UOdysseyAnimationCellImageRaster::OnBlockCommited(const TArray<::ULIS::FRectI>& iRects)
{
    //CopyBlockDataIntoUTexture(GetRasterBlock()->GetBlock().Get(), GetTexture());
    //Texture->UpdateResource();
    FOdysseySurfaceTexture2DEditable surface(Texture, GetRasterBlock()->GetBlock());
    surface.Invalidate(iRects);
    DirtyThumbnail();
    RenderingChanged(::ULISUtils::ToIntRects(iRects));
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

    if( Ar.IsSaving() && !Ar.IsTransacting() && !Ar.IsCooking())
    {
        FOdysseyAnimationCellImageRasterExport::Write( this, Ar );
    }

    if( Ar.IsLoading() && !Ar.IsTransacting() )
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
            mRasterBlock = MakeShared<FOdysseyRasterBlock>(this);
            Ar << *mRasterBlock;
        }
    }
}

void
UOdysseyAnimationCellImageRaster::PreSave(FObjectPreSaveContext SaveContext)
{
    Super::PreSave(SaveContext);

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlock(); //ensures mRasterBlock exists
    InitTextureWithBlockData(rasterBlock->GetBlock().Get(), Texture, TextureSourceFormatForULISFormat(rasterBlock->GetFormat()));
    Texture->UpdateResource();
    FTextureCompilingManager::Get().FinishCompilation({ Texture });
}
#endif
