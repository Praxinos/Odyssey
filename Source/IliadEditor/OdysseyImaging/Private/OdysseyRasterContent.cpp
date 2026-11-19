// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyRasterContent.h"

#include "Engine/Texture2D.h"
#include "Misc/App.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "TextureCompiler.h"
#include "TextureResource.h"

#include "OdysseyBlendShader.h"
#include "OdysseyPixelFormat.h"
#include "OdysseyRasterBlockMutator.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "OdysseyTiledImage.h"
#include "OdysseyTiledImageBlock.h"
#include "ULISUtils.h"

namespace
{
constexpr int32 TileSize = 64;
}

FOdysseyRasterContent::~FOdysseyRasterContent()
{
    ReleaseRasterBlock();
    if (UOdysseyTiledImage* Bound = BoundImage.Get())
    {
        Bound->OnUndone().RemoveAll(this);
    }
}

FOdysseyRasterContent::FOdysseyRasterContent(UObject& InOwner, TObjectPtr<UOdysseyTiledImage>& InImage, bool bInSRGB,
    FGetDescription InGetDescription, FOnChanged InOnChanged, FOdysseyRasterBlock::FPostProcess InPostProcess)
    : Owner(InOwner)
    , Image(InImage)
    , bSRGB(bInSRGB)
    , GetDescription(MoveTemp(InGetDescription))
    , OnChanged(MoveTemp(InOnChanged))
    , PostProcess(MoveTemp(InPostProcess))
{
}

UOdysseyTiledImage*
FOdysseyRasterContent::GetImage() const
{
    return Image;
}

bool
FOdysseyRasterContent::EnsureImage()
{
    check(IsInGameThread());
    const FDescription Description = GetDescription();
    if (!Description.IsValid())
    {
        return false;
    }

    const EOdysseyTiledImageFormat Format = Odyssey::TiledImage::TiledImageFormatForULISFormat(Description.Format, bSRGB);
    if (!Image)
    {
        // The image identity never changes, so creating it is not part of any transaction.
        UOdysseyTiledImage* NewImage = NewObject<UOdysseyTiledImage>(&Owner, NAME_None, RF_Transactional);
        NewImage->Initialize(TileSize, Format);
        Image = NewImage;
    }
    else if (Image->GetFormat() != Format && FApp::CanEverRender())
    {
        // e.g. a cell duplicated into an animation of another format.
        const FIntRect Bounds(FIntPoint::ZeroValue, Description.Size);
        TSharedPtr<::ULIS::FBlock> Pixels = Odyssey::TiledImage::CopyToBlock(*Image, Bounds, Description.Format, bSRGB);
        Image->Initialize(TileSize, Format);
        if (Pixels)
        {
            Odyssey::TiledImage::CopyFromBlock(*Image, *Pixels, Bounds, FIntPoint::ZeroValue, bSRGB);
        }
        ReleaseRasterBlock();
    }

    if (BoundImage.Get() != Image)
    {
        if (UOdysseyTiledImage* Bound = BoundImage.Get())
        {
            Bound->OnUndone().RemoveAll(this);
        }
        Image->OnUndone().AddRaw(this, &FOdysseyRasterContent::OnImageUndone);
        BoundImage = Image.Get();
    }
    return true;
}

bool
FOdysseyRasterContent::SetContent(const ::ULIS::FBlock& Block)
{
    if (!FApp::CanEverRender() || !EnsureImage())
    {
        return false;
    }

    Image->Initialize(TileSize, Image->GetFormat());
    Odyssey::TiledImage::CopyFromBlock(*Image, Block, FIntRect(0, 0, Block.Width(), Block.Height()), FIntPoint::ZeroValue, bSRGB);
    // The block and the preview are derived from the previous content.
    ReleaseRasterBlock();
    OnChanged({}, false);
    return true;
}

TSharedPtr<FOdysseyRasterBlock>
FOdysseyRasterContent::GetRasterBlock()
{
    if (!EnsureImage())
    {
        return nullptr;
    }

    const FDescription Description = GetDescription();
    if (RasterBlock && (RasterBlock->GetWidth() != Description.Size.X || RasterBlock->GetHeight() != Description.Size.Y
        || RasterBlock->GetFormat() != Description.Format))
    {
        ReleaseRasterBlock();
    }

    if (!RasterBlock)
    {
        RasterBlock = MakeShared<FOdysseyRasterBlock>(&Owner, Description.Size.X, Description.Size.Y, Description.Format);

        // Fill the block before binding, so the image is not written back and no post process applies.
        TSharedPtr<::ULIS::FBlock> Pixels = FApp::CanEverRender()
            ? Odyssey::TiledImage::CopyToBlock(*Image, FIntRect(FIntPoint::ZeroValue, Description.Size), Description.Format, bSRGB)
            : nullptr;
        if (Pixels)
        {
            FOdysseyRasterBlockMutator Mutator(RasterBlock, false);
            Mutator.Copy(Pixels, { Pixels->Rect() });
            Mutator.Commit();
        }

        RasterBlock->OnBlockChanged().AddRaw(this, &FOdysseyRasterContent::OnBlockChanged);
        RasterBlock->OnBlockCommited().AddRaw(this, &FOdysseyRasterContent::OnBlockCommited);
        RasterBlock->PostProcess() = PostProcess;
    }
    return RasterBlock;
}

bool
FOdysseyRasterContent::BuildRenderFunction(IOdysseyTextureRenderingAbility::FRenderFunction& OutRenderFunction, const FMatrix& ContentTransform)
{
    if (!EnsureImage())
    {
        return false;
    }

    if (bHasUncommittedEdits && PreviewTexture)
    {
        return BuildPreviewRenderFunction(OutRenderFunction, ContentTransform);
    }
    return BuildImageRenderFunction(OutRenderFunction, ContentTransform, GetDescription().Size);
}

bool
FOdysseyRasterContent::HasUncommittedEdits() const
{
    return bHasUncommittedEdits;
}

void
FOdysseyRasterContent::ReleaseRasterBlock()
{
    if (RasterBlock)
    {
        // Tools may keep the block alive after it is released.
        RasterBlock->OnBlockChanged().RemoveAll(this);
        RasterBlock->OnBlockCommited().RemoveAll(this);
        RasterBlock->PostProcess().Unbind();
        RasterBlock.Reset();
    }
    PreviewTexture.Reset();
    bHasUncommittedEdits = false;
}

void
FOdysseyRasterContent::OnBlockChanged(const TArray<::ULIS::FRectI>& Rects)
{
    bHasUncommittedEdits = true;
    UpdatePreview(Rects, false);
    OnChanged(::ULISUtils::ToIntRects(Rects), true);
}

void
FOdysseyRasterContent::OnBlockCommited(const TArray<::ULIS::FRectI>& Rects)
{
    TArray<FIntRect> IntRects = ::ULISUtils::ToIntRects(Rects);
    // While undoing, the image is restored by the transaction itself.
    if (!GIsTransacting && Image && FApp::CanEverRender())
    {
        TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> Block = RasterBlock->GetBlock();
        Odyssey::TiledImage::CopyFromBlock(*Image, *Block, IntRects, bSRGB);
    }
    bHasUncommittedEdits = false;
    UpdatePreview(Rects, true);
    OnChanged(IntRects, false);
}

void
FOdysseyRasterContent::OnImageUndone()
{
    if (!RasterBlock || !FApp::CanEverRender())
    {
        bHasUncommittedEdits = false;
        OnChanged({}, false);
        return;
    }

    // Keep the derived block in sync with the restored tiles. GIsTransacting prevents writing them back.
    const FDescription Description = GetDescription();
    TSharedPtr<::ULIS::FBlock> Pixels = Odyssey::TiledImage::CopyToBlock(*Image, FIntRect(FIntPoint::ZeroValue, Description.Size), RasterBlock->GetFormat(), bSRGB);
    if (!Pixels)
    {
        return;
    }
    // A post process such as the alpha lock must not alter restored pixels.
    RasterBlock->PostProcess().Unbind();
    {
        FOdysseyRasterBlockMutator Mutator(RasterBlock, false);
        Mutator.Copy(Pixels, { Pixels->Rect() });
        Mutator.Commit();
    }
    RasterBlock->PostProcess() = PostProcess;
}

void
FOdysseyRasterContent::UpdatePreview(const TArray<::ULIS::FRectI>& Rects, bool bCommitted)
{
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> Block = RasterBlock->GetBlock();
    const ETextureSourceFormat SourceFormat = TextureSourceFormatForULISFormat(Block->Format());
    UTexture2D* Texture = PreviewTexture.Get();
    if (Texture && Texture->Source.GetSizeX() == Block->Width() && Texture->Source.GetSizeY() == Block->Height()
        && Texture->Source.GetFormat() == SourceFormat)
    {
        FOdysseySurfaceTexture2DEditable Surface(Texture, Block);
        Surface.Invalidate(Rects);
        if (bCommitted)
        {
            // Keeps the source valid in case the resource is recreated.
            CopyBlockDataToTextureSource(Block.Get(), Texture, ::ULISUtils::ToIntRects(Rects));
        }
        return;
    }

    // A preview is only needed to display uncommitted edits.
    if (bCommitted)
    {
        PreviewTexture.Reset();
        return;
    }
    Texture = NewObject<UTexture2D>(GetTransientPackage(), NAME_None, RF_Transient);
    Texture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
    // 8-bit blocks keep their encoding; other blocks keep their precision.
    const bool b8Bit = !ULIS_R_FLOATING(Block->Format()) && ULIS_R_DEPTH(Block->Format()) == 1;
    Texture->CompressionSettings = b8Bit ? TextureCompressionSettings::TC_VectorDisplacementmap : TextureCompressionSettings::TC_HDR_F32;
    Texture->Filter = TextureFilter::TF_Nearest;
    Texture->SRGB = bSRGB;
    InitTextureWithBlockData(Block.Get(), Texture, SourceFormat);
    Texture->UpdateResource();
    FTextureCompilingManager::Get().FinishCompilation({ Texture });
    PreviewTexture.Reset(Texture);
}

bool
FOdysseyRasterContent::BuildPreviewRenderFunction(IOdysseyTextureRenderingAbility::FRenderFunction& OutRenderFunction, const FMatrix& ContentTransform) const
{
    UTexture2D* Texture = PreviewTexture.Get();
    FTextureCompilingManager::Get().FinishCompilation({ Texture });
    Texture->BlockOnAnyAsyncBuild();
    Texture->WaitForPendingInitOrStreaming();

    // The RHI texture is read on the render thread, where it is valid once the resource is initialized.
    const FTextureResource* Resource = Texture->GetResource();
    if (!Resource)
    {
        return false;
    }

    OutRenderFunction = [Resource, ContentTransform](FRDGBuilder& GraphBuilder, ERHIFeatureLevel::Type FeatureLevel,
        FRDGTextureRef Destination, const FIntRect& SourceRect, const FIntRect& DestinationRect, const FMatrix& Transform)
    {
        RDG_EVENT_SCOPE(GraphBuilder, "Odyssey.RasterContent.Preview");
        AddClearRenderTargetPass(GraphBuilder, Destination, FLinearColor::Transparent, DestinationRect);
        const FTextureRHIRef SourceRHI = Resource->TextureRHI;
        if (!SourceRHI)
        {
            return;
        }
        FRDGTextureRef Source = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(SourceRHI, TEXT("Odyssey.RasterContent.Preview")));
        FOdysseyBlendShader::BlendRect(GraphBuilder, FeatureLevel, Destination, Source, Destination,
            SourceRect, DestinationRect, ContentTransform * Transform, EOdysseyColorBlendMode::Normal,
            EOdysseyAlphaBlendMode::Normal, 1.0f, EOdysseyAntiAliasing::Anisotropic);
    };
    return true;
}

bool
FOdysseyRasterContent::BuildImageRenderFunction(IOdysseyTextureRenderingAbility::FRenderFunction& OutRenderFunction, const FMatrix& ContentTransform, FIntPoint Size) const
{
    // The tiles hold linear colors, sampled as they are.
    const EPixelFormat Format = Image->GetPixelFormat();
    UOdysseyTiledImage::FRenderFunction DrawTiles = Image->BuildRenderFunction(FIntRect(FIntPoint::ZeroValue, Size));
    if (!DrawTiles)
    {
        return false;
    }

    OutRenderFunction = [DrawTiles = MoveTemp(DrawTiles), Size, Format, ContentTransform](FRDGBuilder& GraphBuilder,
        ERHIFeatureLevel::Type FeatureLevel, FRDGTextureRef Destination,
        const FIntRect& SourceRect, const FIntRect& DestinationRect, const FMatrix& Transform)
    {
        RDG_EVENT_SCOPE(GraphBuilder, "Odyssey.RasterContent.Tiles");
        // Assemble a continuous source before filtering: atlas borders must not create seams
        // under zoom, rotation or anisotropic sampling. No persistent full-size texture is added.
        FRDGTextureRef Source = GraphBuilder.CreateTexture(FRDGTextureDesc::Create2D(Size, Format,
            FClearValueBinding::Transparent, TexCreate_ShaderResource | TexCreate_RenderTargetable),
            TEXT("Odyssey.RasterContent.Source"));
        DrawTiles(GraphBuilder, Source, FIntPoint::ZeroValue);
        AddClearRenderTargetPass(GraphBuilder, Destination, FLinearColor::Transparent, DestinationRect);
        FOdysseyBlendShader::BlendRect(GraphBuilder, FeatureLevel, Destination, Source, Destination,
            SourceRect, DestinationRect, ContentTransform * Transform, EOdysseyColorBlendMode::Normal,
            EOdysseyAlphaBlendMode::Normal, 1.0f, EOdysseyAntiAliasing::Anisotropic);
    };
    return true;
}
