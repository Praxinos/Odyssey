// IDDN.FR.001.250001.005.S.P.2019.000.00000
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
#include "OdysseySurfaceTexture2DEditable.h"
#include "LayerStack/OdysseyTextureLayerImageRasterImageRenderer.h"
#include "OdysseyTextureLayerImageRasterImport.h"
#include "OdysseyTextureLayerImageRasterExport.h"
#include "OdysseyTextureLayerStack.h"

#define LOCTEXT_NAMESPACE "Texture"

UOdysseyTextureLayerImageRaster::~UOdysseyTextureLayerImageRaster()
{
}

UOdysseyTextureLayerImageRaster::UOdysseyTextureLayerImageRaster()
{
    LayerTypeName = LOCTEXT("layer-image-raster.type", "Raster Image Layer");
    Icon = FSlateIcon("OdysseyStyle", "OdysseyLayerStack.LayerBitmap16");
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
UOdysseyTextureLayerImageRaster::Merge(const TArray<UOdysseyLayer*>& iLayers)
{
    FOdysseyRasterBlockMutator mutator(RasterBlock);
    mutator.EditTilesFromRects(
        { ::ULIS::FRectI::FromXYWH(0, 0, RasterBlock->GetWidth(), RasterBlock->GetHeight()) },
        [&](TSharedPtr<::ULIS::FBlock> iBlock, const FULISInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
        {
            ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(RasterBlock->GetFormat());
            TArray<::ULIS::FEvent> lastEvent = {};
            for ( UOdysseyLayer* layer : iLayers )
            {
                UOdysseyTextureLayer* textureLayer = Cast<UOdysseyTextureLayer>(layer);
                if ( !textureLayer )
                    continue;

                TSharedPtr<IOdysseyImageRenderer> renderer = textureLayer->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, 0);
                renderer->Init();

                FOdysseyImageRendererBlendParams params(iBlock, {iBlock->Rect()});
                params.mBlendMode = (::ULIS::eBlendMode)textureLayer->BlendMode;
                params.mOpacity = textureLayer->Opacity;

                lastEvent = renderer->Blend(params, lastEvent);
            }
            return { lastEvent };
        }
    );
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
    RasterBlock->PostDuplicate();
    RasterBlock->ConvertTo(width, height, format);
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
UOdysseyTextureLayerImageRaster::BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame, FImageRendererFilter iFilter) const
{
    if (iFilter.IsBound() && !iFilter.Execute(this))
        return nullptr;

    return MakeShared<FOdysseyTextureLayerImageRasterImageRenderer>(this, iRenderType, GetImageRenderingRects(), iFilter);
}

TArray<FGuid>
UOdysseyTextureLayerImageRaster::GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame) const
{
    return { GetImageRenderingId() };
}

void
UOdysseyTextureLayerImageRaster::IsAlphaLockedBlueprintSetter(bool Value)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyTextureLayerImageRaster, IsAlphaLocked), Value);
}

#undef LOCTEXT_NAMESPACE
