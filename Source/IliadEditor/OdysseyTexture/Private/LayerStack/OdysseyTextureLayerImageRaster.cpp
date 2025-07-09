// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTextureLayerImageRaster.h"

#include "OdysseyPixelFormat.h"
#include "ULISEventBuilder.h"
#include "ULISLoaderModule.h"
#include "ULISUtils.h"
#include "OdysseyStyle.h"
#include "OdysseyMediaRaster.h"
#include "OdysseyRasterBlock.h"
#include "OdysseyRasterBlockMutator.h"
#include "OdysseyRasterBlockUndo.h"
#include "OdysseyTextureLayerImageRaster.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "OdysseyTextureLayerImageRasterImport.h"
#include "OdysseyTextureLayerImageRasterExport.h"
#include "OdysseyTextureLayerStack.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "TextureCompiler.h"
#include "Engine/TextureRenderTarget2D.h"
#include "ImageUtils.h"
#include "OdysseyBlendShader.h"
#include "ScreenPass.h"
#include "UObject/ObjectSaveContext.h"

#define LOCTEXT_NAMESPACE "Texture"

UOdysseyTextureLayerImageRaster::~UOdysseyTextureLayerImageRaster()
{
}

UOdysseyTextureLayerImageRaster::UOdysseyTextureLayerImageRaster()
{
    LayerTypeName = LOCTEXT("layer-image-raster.type", "Raster Image Layer");
    Icon = FSlateIcon("OdysseyStyle", "OdysseyLayerStack.LayerBitmap16");
}

void
UOdysseyTextureLayerImageRaster::PostLoad()
{
    Super::PostLoad();
    InitRasterBlock();
}

void
UOdysseyTextureLayerImageRaster::InitRasterBlock() const
{
    if ( !RasterBlock )
    {
        UTexture2D* texture = GetTexture();
        if (!texture)
            return;

        //The layer a different texture with different parameters
        //Ensure the block uses those parameters
        ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(texture->Source.GetFormat());
        //let's ensure the format has alpha, so add alpha channel of needed
        format = static_cast< ::ULIS::eFormat >(format | ULIS_W_ALPHA( 1 ) );
        int width = texture->Source.GetSizeX();
        int height = texture->Source.GetSizeY();

        RasterBlock = MakeShared<FOdysseyRasterBlock>(const_cast<UOdysseyTextureLayerImageRaster*>(this), width, height, format);
    }

    RasterBlock->OnBlockChanged().RemoveAll(this);
    RasterBlock->OnBlockCommited().RemoveAll(this);

    RasterBlock->OnBlockChanged().AddUObject(const_cast<UOdysseyTextureLayerImageRaster*>(this), &UOdysseyTextureLayerImageRaster::OnBlockChanged);
    RasterBlock->OnBlockCommited().AddUObject(const_cast<UOdysseyTextureLayerImageRaster*>(this), &UOdysseyTextureLayerImageRaster::OnBlockCommited);
    RasterBlock->PostProcess().BindUObject(const_cast<UOdysseyTextureLayerImageRaster*>(this), &UOdysseyTextureLayerImageRaster::RasterBlockPostProcess);
}

void
UOdysseyTextureLayerImageRaster::InitTexture()
{
    Super::InitTexture();

    UTexture2D* texture = GetRenderTexture();
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlock(); //ensures mRasterBlock exists
    InitTextureWithBlockData(rasterBlock->GetBlock().Get(), texture, TextureSourceFormatForULISFormat(rasterBlock->GetFormat()));
    texture->UpdateResource();
    FTextureCompilingManager::Get().FinishCompilation({ texture });
}

TSharedPtr<FOdysseyRasterBlock>
UOdysseyTextureLayerImageRaster::GetRasterBlock() const
{
    if (!RasterBlock)
        InitRasterBlock();

    return RasterBlock;
}

void
UOdysseyTextureLayerImageRaster::OnBlockChanged(const TArray<::ULIS::FRectI>& iRects)
{
    FOdysseySurfaceTexture2DEditable surface(GetRenderTexture(), GetRasterBlock()->GetBlock());
    surface.Invalidate(iRects);
    RenderingChanged(::ULISUtils::ToIntRects(iRects), true);
}

void
UOdysseyTextureLayerImageRaster::OnBlockCommited(const TArray<::ULIS::FRectI>& iRects)
{
    TSharedPtr<::ULIS::FBlock> block =  GetRasterBlock()->GetBlock();
    FOdysseySurfaceTexture2DEditable surface(GetRenderTexture(), block);
    surface.Invalidate(iRects);

    TArray<FIntRect> intRects = ::ULISUtils::ToIntRects(iRects);
    for (const FIntRect& rect : intRects)
    {
        CopyBlockDataToTextureSource(block.Get(), GetRenderTexture(), rect, rect.Min);
    }
    RenderingChanged();
}

void
UOdysseyTextureLayerImageRaster::Merge(const TArray<UOdysseyLayer*>& iLayers)
{
    FIntRect rect = GetDefaultRenderRect();
    TStrongObjectPtr<UTextureRenderTarget2D> layerRenderTarget(NewObject<UTextureRenderTarget2D>());
    TStrongObjectPtr<UTextureRenderTarget2D> destinationRenderTarget(NewObject<UTextureRenderTarget2D>());
    layerRenderTarget->InitAutoFormat(rect.Width(), rect.Height());
    destinationRenderTarget->InitAutoFormat(rect.Width(), rect.Height());

    //Clear the destination rendertarget before blending on it
    ENQUEUE_RENDER_COMMAND(IOdysseyTextureRenderingAbility_RenderRectAtRect)(
        [destinationRenderTarget, rect](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder graphBuilder(RHICmdList);
            FRDGTextureRef destinationTexture = destinationRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );
            AddClearRenderTargetPass(graphBuilder, destinationTexture, FLinearColor::Transparent, rect);
            graphBuilder.Execute();
        }
    );

    for ( UOdysseyLayer* layer : iLayers )
    {
        layer->Render_GameThread(
            layerRenderTarget.Get(),
            FFrameNumber(0),
            EOdysseyRenderingType::Render
        );

        const ERHIFeatureLevel::Type featureLevel = GMaxRHIFeatureLevel;

        ENQUEUE_RENDER_COMMAND(IOdysseyTextureRenderingAbility_RenderRectAtRect)(
            [layerRenderTarget, destinationRenderTarget, featureLevel, rect, layer](FRHICommandListImmediate& RHICmdList)
            {
                FRDGBuilder graphBuilder(RHICmdList);

                FRDGTextureRef layerTexture = layerRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );
                FRDGTextureRef destinationTexture = destinationRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );

                FOdysseyBlendShader::BlendRect(
                    graphBuilder,
                    featureLevel,
                    destinationTexture,
                    layerTexture,
                    destinationTexture,
                    rect,
                    rect,
                    FMatrix::Identity,
                    layer->GetBlendMode(),
                    EOdysseyAlphaMode::kNormal,
                    layer->GetOpacity(),
                    EOdysseyAntiAliasing::NearestNeighbor
                );

                graphBuilder.Execute();
            }
        );
    }

    FImage OutImage;
    if (!FImageUtils::GetRenderTargetImage(destinationRenderTarget.Get(), OutImage))
        return;

    ::ULIS::eFormat format = ULISFormatForRawImageFormat(OutImage.Format);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
    TSharedPtr<::ULIS::FBlock> block = MakeShareable(new ::ULIS::FBlock( rect.Width(), rect.Height(), format ));
    CopyImageToBlock(OutImage, block.Get());

    FOdysseyRasterBlockMutator mutator(RasterBlock);
    mutator.Copy(block, {::ULISUtils::ToULISRectI(rect)});
    mutator.Commit();
}

void
UOdysseyTextureLayerImageRaster::PostInitProperties()
{
    Super::PostInitProperties();

    if (GetFlags() & RF_ClassDefaultObject)
        return;

    UTexture2D* texture = GetTexture();
    if (texture->Source.GetFormat() != TSF_Invalid)
    {
        ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(texture->Source.GetFormat());
        //let's ensure the format has alpha, so add alpha channel of needed
        format = static_cast< ::ULIS::eFormat >(format | ULIS_W_ALPHA( 1 ) );

        //Caches the tiles on disk
        RasterBlock = MakeShared<FOdysseyRasterBlock>(this, texture->Source.GetSizeX(), texture->Source.GetSizeY(), format);
        RasterBlock->OnBlockChanged().AddUObject(this, &::UOdysseyTextureLayerImageRaster::OnBlockChanged);
        RasterBlock->OnBlockCommited().AddUObject(this, &::UOdysseyTextureLayerImageRaster::OnBlockCommited);
        RasterBlock->PostProcess().BindUObject(this, &UOdysseyTextureLayerImageRaster::RasterBlockPostProcess);
    }
    else
    {
        RasterBlock = MakeShared<FOdysseyRasterBlock>(this);
        RasterBlock->OnBlockChanged().AddUObject(this, &::UOdysseyTextureLayerImageRaster::OnBlockChanged);
        RasterBlock->OnBlockCommited().AddUObject(this, &::UOdysseyTextureLayerImageRaster::OnBlockCommited);
        RasterBlock->PostProcess().BindUObject(this, &UOdysseyTextureLayerImageRaster::RasterBlockPostProcess);
    }
}



void
UOdysseyTextureLayerImageRaster::PostDuplicate(EDuplicateMode::Type iDuplicateMode)
{
    Super::PostDuplicate(iDuplicateMode);

    UTexture2D* texture = GetTexture();
    if (!texture)
        return;

    //The layer a different texture with different parameters
    //Ensure the block uses those parameters
    ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(texture->Source.GetFormat());
    //let's ensure the format has alpha, so add alpha channel of needed
    format = static_cast< ::ULIS::eFormat >(format | ULIS_W_ALPHA( 1 ) );
    int width = texture->Source.GetSizeX();
    int height = texture->Source.GetSizeY();

    if (RasterBlock)
    {
        RasterBlock->PostDuplicate();
        RasterBlock->ConvertTo(width, height, format);

        InitRasterBlock();
        InitTexture();
    }
}

FOdysseyMediaProvider
UOdysseyTextureLayerImageRaster::GetMediaProvider(uint32 iFrameIndex) const
{
    bool isActive = IsActivatedRecursively();
    bool isLocked = IsLockedRecursively();

    FOdysseyMediaProvider mediaProvider;
    mediaProvider.IsLocked(!isActive || isLocked);

    TSharedPtr<FOdysseyMediaRaster> mediaRaster = MakeShared<FOdysseyMediaRaster>(RasterBlock);
    mediaProvider.Add(mediaRaster);
    return mediaProvider;
}

void
UOdysseyTextureLayerImageRaster::Serialize(FArchive& Ar)
{
    Super::Serialize(Ar);

    if (GetFlags() & RF_ClassDefaultObject)
        return;

    if( Ar.IsSaving() )
    {
        FOdysseyTextureLayerImageRasterExport::Write( this, Ar );
    }

    if( Ar.IsLoading() )
    {
        if (!FOdysseyTextureLayerImageRasterImport::Read( this, Ar ))
        {
            //Old Style No Chunk Loading
            Ar << *RasterBlock;
        }
    }
}

TArray<::ULIS::FEvent>
UOdysseyTextureLayerImageRaster::RasterBlockPostProcess(const TMap<FIntPoint, TSharedPtr<::ULIS::FBlock>>& iOriginalBlocks, const FOdysseyInvalidTileMap& iInvalidMap, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!IsAlphaLocked())
        return iWaitList;

    //Apply AlphaLock
    TArray<::ULIS::FEvent> events;
    TSharedPtr<::ULIS::FBlock> block = RasterBlock->GetBlock();
    TArray<FIntPoint> invalidTiles = iInvalidMap.InvalidTiles();
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(RasterBlock->GetFormat());
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

TArray<FGuid>
UOdysseyTextureLayerImageRaster::GetRenderingComposition(uint64 iRenderType, int iFrame) const
{
    return { GetRenderingId() };
}

void
UOdysseyTextureLayerImageRaster::SetIsAlphaLocked(bool Value)
{
    bIsAlphaLocked = Value;
}

bool
UOdysseyTextureLayerImageRaster::IsAlphaLocked() const
{
    return bIsAlphaLocked;
}

void
UOdysseyTextureLayerImageRaster::PreSave(FObjectPreSaveContext SaveContext)
{
    Super::PreSave(SaveContext);
    //InitTexture();
}

#undef LOCTEXT_NAMESPACE
