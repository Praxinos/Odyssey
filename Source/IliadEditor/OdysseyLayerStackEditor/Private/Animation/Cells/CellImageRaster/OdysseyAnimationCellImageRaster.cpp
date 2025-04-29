// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationCellImageRaster.h"

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
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "OdysseyBlendShader.h"
#include "OdysseyPixelFormat.h"
#include "TextureCompiler.h"
#include "ScreenPass.h"

UOdysseyAnimationCellImageRaster::UOdysseyAnimationCellImageRaster()
{
    Texture = CreateDefaultSubobject<UTexture2D>(TEXT("Texture"));
    Texture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
    Texture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
    Texture->Filter = TextureFilter::TF_Nearest;
}

TSharedPtr<FOdysseyRasterBlock>
UOdysseyAnimationCellImageRaster::GetRasterBlock() const
{
    if (!mRasterBlock)
    {
        int width = GetAnimation()->GetWidth();
        int height = GetAnimation()->GetHeight();

        ::ULIS::eFormat format = ::ULIS::Format_BGRA8;
        switch(GetAnimation()->GetFormat())
        {
            case EOdysseyAnimationFormat::BGRA8: format = ::ULIS::Format_BGRA8;
            case EOdysseyAnimationFormat::RGBAF: format = ::ULIS::Format_RGBAF;
        }

        mRasterBlock = MakeShared<FOdysseyRasterBlock>(const_cast<UOdysseyAnimationCellImageRaster*>(this), width, height, format);

        mRasterBlock->OnBlockChanged().AddUObject(const_cast<UOdysseyAnimationCellImageRaster*>(this), &UOdysseyAnimationCellImageRaster::OnBlockChanged);
        mRasterBlock->OnBlockCommited().AddUObject(const_cast<UOdysseyAnimationCellImageRaster*>(this), &UOdysseyAnimationCellImageRaster::OnBlockCommited);
        mRasterBlock->PostProcess().BindUObject(const_cast<UOdysseyAnimationCellImageRaster*>(this), &UOdysseyAnimationCellImageRaster::RasterBlockPostProcess);

        InitTextureWithBlockData( mRasterBlock->GetBlock().Get(), Texture, TextureSourceFormatForULISFormat(mRasterBlock->GetFormat()));
        Texture->UpdateResource();
        FTextureCompilingManager::Get().FinishCompilation({ Texture });
    }
    return mRasterBlock;
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

        mRasterBlock->OnBlockChanged().AddUObject(const_cast<UOdysseyAnimationCellImageRaster*>(this), &UOdysseyAnimationCellImageRaster::OnBlockChanged);
        mRasterBlock->OnBlockCommited().AddUObject(const_cast<UOdysseyAnimationCellImageRaster*>(this), &UOdysseyAnimationCellImageRaster::OnBlockCommited);
        mRasterBlock->PostProcess().BindUObject(const_cast<UOdysseyAnimationCellImageRaster*>(this), &UOdysseyAnimationCellImageRaster::RasterBlockPostProcess);

        //Init Texture
        InitTextureWithBlockData( mRasterBlock->GetBlock().Get(), Texture, TextureSourceFormatForULISFormat(mRasterBlock->GetFormat()));
        Texture->UpdateResource();
        FTextureCompilingManager::Get().FinishCompilation({ Texture });
    }
}

void
UOdysseyAnimationCellImageRaster::PostLoad()
{
    Super::PostLoad();

    if (mRasterBlock)
    {
        mRasterBlock->OnBlockChanged().AddUObject(const_cast<UOdysseyAnimationCellImageRaster*>(this), &UOdysseyAnimationCellImageRaster::OnBlockChanged);
        mRasterBlock->OnBlockCommited().AddUObject(const_cast<UOdysseyAnimationCellImageRaster*>(this), &UOdysseyAnimationCellImageRaster::OnBlockCommited);
        mRasterBlock->PostProcess().BindUObject(const_cast<UOdysseyAnimationCellImageRaster*>(this), &UOdysseyAnimationCellImageRaster::RasterBlockPostProcess);

        //Init Texture
        InitTextureWithBlockData( mRasterBlock->GetBlock().Get(), Texture, TextureSourceFormatForULISFormat(mRasterBlock->GetFormat()));
        Texture->UpdateResource();
        FTextureCompilingManager::Get().FinishCompilation({ Texture });
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

bool
UOdysseyAnimationCellImageRaster::IsImageRenderingGameThreadOnly() const
{
    TSharedPtr<FOdysseyMediaRaster> mediaRaster = mMediaRaster.Pin();
    return !!mediaRaster;
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

void
UOdysseyAnimationCellImageRaster::OnBlockChanged(const TArray<::ULIS::FRectI>& iRects)
{
    FOdysseySurfaceTexture2DEditable surface(Texture, GetRasterBlock()->GetBlock());
    surface.Invalidate(iRects);
    RenderingChanged(::ULISUtils::ToIntRects(iRects), true);
}

void
UOdysseyAnimationCellImageRaster::OnBlockCommited(const TArray<::ULIS::FRectI>& iRects)
{
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
            mRasterBlock = MakeShared<FOdysseyRasterBlock>(this);
            Ar << *mRasterBlock;
        }
    }
}

TSharedPtr<FOdysseyTextureRenderer>
UOdysseyAnimationCellImageRaster::BuildTextureRenderer(FFrameNumber iFrame, TMap<const IOdysseyTextureRenderingAbility*, FGuid>* iIds) const
{
    TSharedPtr<FOdysseyTextureRenderer> renderer = MakeShared<FOdysseyTextureRenderer>();

    FGuid id = renderer->AddChild(
        renderer->GetRootPassId(),
        EOdysseyBlendingMode::kNormal,
        1.0f,
        FMatrix::Identity,
        FOdysseyTextureRenderer::FOnExecuteRenderPass::CreateLambda(
            [this](FRDGBuilder& iGraphBuilder, const FOdysseyTextureRenderer::FRenderPassParameters& iParams)
            {
                FRDGTextureRef sourceTexture = iGraphBuilder.RegisterExternalTexture(CreateRenderTarget(Texture->GetResource()->TextureRHI, TEXT("UOdysseyAnimation::sourceTexture")));

                AddDrawTexturePass(
                    iGraphBuilder,
                    FScreenPassViewInfo(),
                    sourceTexture,
                    iParams.DestinationTexture,
                    iParams.SrcRect.Min,
                    iParams.SrcRect.Size(),
                    iParams.DstRect.Min,
                    iParams.DstRect.Size()
                );
            }
        )
    );

    if (iIds)
        iIds->Add(this, id);

    return renderer;
}
