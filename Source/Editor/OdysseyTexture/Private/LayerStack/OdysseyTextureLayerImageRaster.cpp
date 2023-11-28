// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTextureLayerImageRaster.h"

#include "OdysseyPixelFormat.h"
#include "ULISEventBuilder.h"
#include "ULISLoaderModule.h"
#include "OdysseyStyleSet.h"
#include "OdysseyMediaRaster.h"
#include "OdysseyRasterBlock.h"
#include "OdysseyRasterBlockMutator.h"
#include "OdysseyRasterBlockUndo.h"
#include "OdysseyTextureLayerImageRaster.h"
#include "OdysseyLayerFunctionLibrary.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "LayerStack/OdysseyTextureLayerImageRasterImageRenderer.h"
#include "OdysseyTextureLayerImageRasterImport.h"
#include "OdysseyTextureLayerImageRasterExport.h"

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
    ImageRenderingChanged(iRects, true);
}

void
UOdysseyTextureLayerImageRaster::OnBlockCommited(const TArray<::ULIS::FRectI>& iRects)
{
    ImageRenderingChanged(iRects);
}

void
UOdysseyTextureLayerImageRaster::OnBlockPtrChanged()
{
    ImageRenderingChanged();
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
            [&](TSharedPtr<::ULIS::FBlock> iBlock, const FULISInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
            {
                ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(RasterBlock->GetFormat());
                TArray<::ULIS::FEvent> lastEvent = {};
                for ( UOdysseyLayer* layer : iLayers )
                {
                    UOdysseyTextureLayer* textureLayer = Cast<UOdysseyTextureLayer>(layer);
                    if ( !textureLayer )
                        continue;

                    TSharedPtr<IOdysseyImageRenderer> renderer = textureLayer->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render);
		            renderer->Init();
                    lastEvent = renderer->Blend(iBlock, textureLayer->GetImageRenderingBlendMode(), textureLayer->GetImageRenderingOpacity(), iBlock->Rect(), lastEvent);
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
    OnOpacityChanged().Broadcast(this);

    ImageRenderingChanged();
}

void
UOdysseyTextureLayerImageRaster::BlendModeChanged()
{
    OnBlendModeChanged().Broadcast(this);
    
    ImageRenderingChanged();
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
UOdysseyTextureLayerImageRaster::PostInitProperties()
{
    Super::PostInitProperties();

    RasterBlock->PostProcess().BindUObject(this, &UOdysseyTextureLayerImageRaster::RasterBlockPostProcess);
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
        RasterBlock->PostProcess().Unbind();
        RasterBlock->OnBlockChanged().AddUObject(this, &::UOdysseyTextureLayerImageRaster::OnBlockChanged);
        RasterBlock->OnBlockCommited().AddUObject(this, &::UOdysseyTextureLayerImageRaster::OnBlockCommited);
        RasterBlock->OnBlockPtrChanged().AddUObject(this, &::UOdysseyTextureLayerImageRaster::OnBlockPtrChanged);
        RasterBlock->PostProcess().BindUObject(this, &UOdysseyTextureLayerImageRaster::RasterBlockPostProcess);   
    }
}

void
UOdysseyTextureLayerImageRaster::PostDuplicate(bool bDuplicateForPIE)
{
    Super::PostDuplicate(bDuplicateForPIE);

    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetLayerStack());
    if (!layerStack)
        return;

    UTexture2D* texture = layerStack->GetTexture();
    if (!texture)
        return;

    //The layer a different texture with different parameters
    //Ensure the block uses those parameters
    ::ULIS::eFormat format = ULISFormatForTextureSourceFormat(texture->Source.GetFormat());
    //let's ensure the format has alpha, so add alpha channel of needed
    format = static_cast< ::ULIS::eFormat >(format | ULIS_W_ALPHA( 1 ) );
    int width = texture->Source.GetSizeX();
    int height = texture->Source.GetSizeY();
    RasterBlock->PostDuplicate(width, height, format);
    
    /* TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> originalBlock = RasterBlock->GetBlock();
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
    RasterBlock->SetBlock(duplicatedBlock); */
}

FOdysseyMediaProvider
UOdysseyTextureLayerImageRaster::GetMediaProvider(uint32 iFrameIndex) const
{
    FOdysseyMediaProvider mediaProvider;
    TSharedPtr<FOdysseyMediaRaster> mediaRaster = MakeShared<FOdysseyMediaRaster>(RasterBlock);
    bool isActive = UOdysseyLayerFunctionLibrary::IsLayerActivatedInStack(this);
    bool isLocked = UOdysseyLayerFunctionLibrary::IsLayerLockedInStack(this);
    mediaRaster->IsLocked(!isActive || isLocked);
    mediaProvider.Add(mediaRaster);
    return mediaProvider;
}

void
UOdysseyTextureLayerImageRaster::Serialize(FArchive& Ar)
{
    Super::Serialize(Ar);
    
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

TSharedPtr<IOdysseyImageRenderer>
UOdysseyTextureLayerImageRaster::BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, FImageRendererFilter iFilter) const
{
    if (iFilter.IsBound() && !iFilter.Execute(this))
        return nullptr;
    
    return MakeShared<FOdysseyTextureLayerImageRasterImageRenderer>(this, iRenderType, GetImageRenderingRects(), iFilter);
}

TArray<FGuid>
UOdysseyTextureLayerImageRaster::GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType) const
{
    return { GetImageRenderingId() };
}

::ULIS::eBlendMode
UOdysseyTextureLayerImageRaster::GetImageRenderingBlendMode() const
{
    return (::ULIS::eBlendMode)BlendMode;
}

float
UOdysseyTextureLayerImageRaster::GetImageRenderingOpacity() const
{
    return Opacity;
}

#undef LOCTEXT_NAMESPACE