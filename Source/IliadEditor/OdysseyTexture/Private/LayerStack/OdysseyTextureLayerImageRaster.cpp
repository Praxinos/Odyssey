// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTextureLayerImageRaster.h"

#include "Engine/TextureRenderTarget2D.h"
#include "ImageUtils.h"
#include "RenderGraphUtils.h"
#include "ScreenPass.h"
#include "TextureResource.h"
#include "UObject/ObjectSaveContext.h"

#include "OdysseyBlendShader.h"
#include "OdysseyMediaRaster.h"
#include "OdysseyPixelFormat.h"
#include "OdysseyRasterBlock.h"
#include "OdysseyRenderContext.h"
#include "OdysseyStyle.h"
#include "OdysseyTextureLayerImageRasterImport.h"
#include "OdysseyTextureLayerStack.h"
#include "OdysseyTiledImage.h"
#include "UObject/OdysseyObjectEditorUtils.h"

#include "ULISLoaderModule.h"
#include "ULISUtils.h"

#define LOCTEXT_NAMESPACE "Texture"

UOdysseyTextureLayerImageRaster::~UOdysseyTextureLayerImageRaster()
{
}

UOdysseyTextureLayerImageRaster::UOdysseyTextureLayerImageRaster()
{
    LayerTypeName = LOCTEXT("layer-image-raster.type", "Raster Layer");
    Icon = FSlateIcon("OdysseyStyle", "OdysseyLayerStack.LayerBitmap16");
}

void
UOdysseyTextureLayerImageRaster::PostLoad()
{
    Super::PostLoad();
    FOdysseyRasterContent& Content = GetRasterContent();
    Content.MigrateLegacy(Texture, LegacyPixels);
    Content.EnsureImage();
}

void
UOdysseyTextureLayerImageRaster::PostDuplicate(EDuplicateMode::Type DuplicateMode)
{
    Super::PostDuplicate(DuplicateMode);
    GetRasterContent().EnsureImage();
}

void
UOdysseyTextureLayerImageRaster::PreSave(FObjectPreSaveContext SaveContext)
{
    Super::PreSave(SaveContext);
    // Content still held by a legacy render texture must reach the tiled image before saving.
    GetRasterContent().MigrateLegacy(Texture, LegacyPixels);
}

void
UOdysseyTextureLayerImageRaster::BeginDestroy()
{
    RasterContent.Reset();
    Super::BeginDestroy();
}

FOdysseyRasterContent&
UOdysseyTextureLayerImageRaster::GetRasterContent() const
{
    if (!RasterContent)
    {
        UOdysseyTextureLayerImageRaster* MutableThis = const_cast<UOdysseyTextureLayerImageRaster*>(this);
        RasterContent = MakeShared<FOdysseyRasterContent>(*MutableThis, MutableThis->GetRenderingId(), MutableThis->TiledImage,
            [MutableThis]()
            {
                FOdysseyRasterContent::FDescription Description;
                UOdysseyTextureLayerStack* LayerStack = Cast<UOdysseyTextureLayerStack>(MutableThis->GetLayerStack());
                if (MutableThis->GetTexture() && LayerStack)
                {
                    Description.Size = MutableThis->GetDefaultRenderRect().Size();
                    Description.Format = MutableThis->GetRasterFormat();
                    Description.bSRGB = LayerStack->IsSRGB();
                    Description.bKeepBytesOnSRGBChange = LayerStack->KeepsBytesOnSRGBChange();
                }
                return Description;
            },
            [MutableThis](const TArray<FIntRect>& Rects)
            {
                MutableThis->RenderingChanged(Rects, false);
            },
            [MutableThis]()
            {
                return MutableThis->IsAlphaLocked();
            });
    }
    return *RasterContent;
}

::ULIS::eFormat
UOdysseyTextureLayerImageRaster::GetRasterFormat() const
{
    UTexture2D* OwnerTexture = GetTexture();
    if (!OwnerTexture)
    {
        return ::ULIS::Format_BGRA8;
    }
    ::ULIS::eFormat Format = ULISFormatForTextureSourceFormat(GetLayerTextureFormat(OwnerTexture->Source.GetFormat()));
    // Raster layers always hold an alpha channel.
    return static_cast<::ULIS::eFormat>(Format | ULIS_W_ALPHA(1));
}

const UOdysseyTiledImage*
UOdysseyTextureLayerImageRaster::GetTiledImage() const
{
    return TiledImage;
}

void
UOdysseyTextureLayerImageRaster::ColorSettingsChanged()
{
    // Converts the tiles to the interpretation of the texture bytes.
    GetRasterContent().EnsureImage();
    RenderingChanged();
}

bool
UOdysseyTextureLayerImageRaster::BuildRenderPipelineInternal(FRenderFunction& OutRenderFunction,
    const TArray<const IOdysseyTextureRenderingAbility*>& Parents, const FOdysseyRenderContext* RenderContext) const
{
    return GetRasterContent().BuildRenderFunction(OutRenderFunction, FMatrix::Identity);
}

void
UOdysseyTextureLayerImageRaster::Merge(const TArray<UOdysseyLayer*>& iLayers)
{
    if( !IsEditable() )
        return;

    Modify();

    FIntRect rect = GetDefaultRenderRect();
    TStrongObjectPtr<UTextureRenderTarget2D> layerRenderTarget(CreateRenderingRenderTarget());
    TStrongObjectPtr<UTextureRenderTarget2D> destinationRenderTarget(CreateRenderingRenderTarget());

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
        layer->Render_GameThread(layerRenderTarget.Get(), EOdysseyRenderingType::Render);

        const ERHIFeatureLevel::Type featureLevel = GMaxRHIFeatureLevel;

        ENQUEUE_RENDER_COMMAND(IOdysseyTextureRenderingAbility_RenderRectAtRect)(
            [layerRenderTarget, destinationRenderTarget, featureLevel, rect, layer](FRHICommandListImmediate& RHICmdList)
            {
                FRDGBuilder graphBuilder(RHICmdList);

                FRDGTextureRef layerTexture = layerRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );
                FRDGTextureRef destinationTexture = destinationRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );

                EOdysseyColorBlendMode ColorBlendMode = GetColorBlendModeFromBlendMode(layer->GetBlendMode());
                EOdysseyAlphaBlendMode AlphaBlendMode = GetAlphaBlendModeFromBlendMode(layer->GetBlendMode());

                FOdysseyBlendShader::BlendRect(
                    graphBuilder,
                    featureLevel,
                    destinationTexture,
                    layerTexture,
                    destinationTexture,
                    rect,
                    rect,
                    FMatrix::Identity,
                    ColorBlendMode,
                    layer->GetInheritsAlpha() ? EOdysseyAlphaBlendMode::Back : AlphaBlendMode,
                    layer->GetOpacity(),
                    EOdysseyAntiAliasing::NearestNeighbor
                );

                graphBuilder.Execute();
            }
        );
    }

    // The layers were blended in compositing space: their colors are converted back into the linear colors the layer holds.
    GetRasterContent().Write(*destinationRenderTarget, { rect }, InverseColorConversion(GetContentColorConversion(nullptr)));
}

FOdysseyMediaProvider
UOdysseyTextureLayerImageRaster::GetMediaProvider(uint32 iFrameIndex) const
{
    bool isActive = IsActivatedRecursively();
    bool isLocked = IsLockedRecursively();

    FOdysseyMediaProvider mediaProvider;
    mediaProvider.IsLocked(!isActive || isLocked);

    TSharedPtr<FOdysseyMediaRaster> mediaRaster = MakeShared<FOdysseyMediaRaster>(GetRenderingId(), GetRasterContent().AsWeak());
    mediaProvider.Add(mediaRaster);
    return mediaProvider;
}

void
UOdysseyTextureLayerImageRaster::Serialize(FArchive& Ar)
{
    Super::Serialize(Ar);

    if (GetFlags() & RF_ClassDefaultObject)
        return;

    if( Ar.IsLoading() )
    {
        if (!FOdysseyTextureLayerImageRasterImport::Read( this, Ar ))
        {
            //PATCH: allows us to load most of the old textures without implying new textures crash
            //This is needed because we do not write RasterBlock in save anymore
            //And when FOdysseyTextureLayerImageRasterImport::Read() returns false, it can mean 2 things :
            // - There is nothing to read (happens with recent textures)
            // - There is something to read in an old fashioned way (happens with really old textures)
            //This code is not perfect, but should do the trick, and we will be able to remove it in the future
            //when we consider that really old projects should not be openable anymore (Example: the Roller_board_long storyboard project made by Antoine Antin)
            uint64 start = Ar.Tell();
            uint64 end = Ar.TotalSize();
            uint64 neededSize = sizeof(uint32) * 7; //Id(4*uint32) + width(1*uint32) + height(1*uint32) + format(1*uint32)
            if (end - start >= neededSize) //check if we can read chunkID and chunkLen
            {
                FGuid id;
                int width;
                int height;

                Ar << id; //uint32 * 4
                Ar << width; //int32
                Ar << height; //int32

                Ar.Seek(start);

                bool canRead = id.IsValid() && width > 0 && width <= 8192 && height > 0 && height <= 8192;
                if (canRead)
                {
                    LegacyPixels = Odyssey::ReadLegacyRasterBlock(Ar, *this);
                }
            }
        }
    }
}

void
UOdysseyTextureLayerImageRaster::SetIsAlphaLocked(bool Value)
{
    if( !IsEditable() )
        return;

    Modify();

    bIsAlphaLocked = Value;
}

bool
UOdysseyTextureLayerImageRaster::IsAlphaLocked() const
{
    return bIsAlphaLocked;
}

#undef LOCTEXT_NAMESPACE
