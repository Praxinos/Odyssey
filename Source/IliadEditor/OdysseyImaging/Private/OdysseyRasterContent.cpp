// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyRasterContent.h"

#include "Misc/App.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"

#include "OdysseyBlendShader.h"
#include "OdysseyRasterBlockEdit.h"
#include "OdysseyRasterBlockMutator.h"
#include "OdysseyRasterEditSession.h"
#include "OdysseyTiledImage.h"
#include "OdysseyTiledImageBlock.h"
#include "ULISLoaderModule.h"
#include "ULISUtils.h"

namespace
{
constexpr int32 TileSize = 64;
}

FOdysseyRasterContent::~FOdysseyRasterContent()
{
    ReleaseRasterBlock();
    EndEdit();
    if (UOdysseyTiledImage* Bound = BoundImage.Get())
    {
        Bound->OnUndone().RemoveAll(this);
    }
}

FOdysseyRasterContent::FOdysseyRasterContent(UObject& InOwner, const FGuid& InRenderingId, TObjectPtr<UOdysseyTiledImage>& InImage,
    FGetDescription InGetDescription, FOnChanged InOnChanged, FIsAlphaLocked InIsAlphaLocked)
    : Owner(InOwner)
    , RenderingId(InRenderingId)
    , Image(InImage)
    , GetDescription(MoveTemp(InGetDescription))
    , OnChanged(MoveTemp(InOnChanged))
    , IsAlphaLocked(MoveTemp(InIsAlphaLocked))
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

    const EOdysseyTiledImageFormat Format = Odyssey::TiledImage::TiledImageFormatForULISFormat(Description.Format, Description.bSRGB);
    if (!Image)
    {
        // The image identity never changes, so creating it is not part of any transaction.
        UOdysseyTiledImage* NewImage = NewObject<UOdysseyTiledImage>(&Owner, NAME_None, RF_Transactional);
        NewImage->Initialize(TileSize, Format);
        Image = NewImage;
    }
    else if (Image->GetFormat() != Format && FApp::CanEverRender())
    {
        // e.g. a cell duplicated into an animation of another format, or the sRGB setting of a texture changed.
        const FIntRect Bounds(FIntPoint::ZeroValue, Description.Size);
        const EOdysseyTiledImageFormat PreviousFormat = Image->GetFormat();
        // For 8-bit content, linear tiles are RGBA8 and decoded sRGB tiles are RGBA16F.
        const bool bEncodingChanged = !ULIS_R_FLOATING(Description.Format) && ULIS_R_DEPTH(Description.Format) == 1
            && (PreviousFormat == EOdysseyTiledImageFormat::RGBA8) != (Format == EOdysseyTiledImageFormat::RGBA8)
            && (PreviousFormat == EOdysseyTiledImageFormat::RGBA16F || Format == EOdysseyTiledImageFormat::RGBA16F);
        TSharedPtr<::ULIS::FBlock> Pixels;
        if (bEncodingChanged && Description.bKeepBytesOnEncodingChange)
        {
            // Read the bytes as they were interpreted, write them as they are now interpreted.
            Pixels = Odyssey::TiledImage::CopyToBlock(*Image, Bounds, Description.Format, !Description.bSRGB);
            Image->Initialize(TileSize, Format);
            if (Pixels)
            {
                Odyssey::TiledImage::CopyFromBlock(*Image, *Pixels, Bounds, FIntPoint::ZeroValue, Description.bSRGB);
            }
        }
        else
        {
            // Keep the linear colors, in the precision of the new format.
            Pixels = Odyssey::TiledImage::CopyToBlock(*Image, Bounds, ::ULIS::Format_RGBAF);
            Image->Initialize(TileSize, Format);
            if (Pixels)
            {
                Odyssey::TiledImage::CopyFromBlock(*Image, *Pixels, Bounds, FIntPoint::ZeroValue);
            }
        }
        ReleaseRasterBlock();
        EndEdit();
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
    Odyssey::TiledImage::CopyFromBlock(*Image, Block, FIntRect(0, 0, Block.Width(), Block.Height()), FIntPoint::ZeroValue, GetDescription().bSRGB);
    // The block and the edit are derived from the previous content.
    ReleaseRasterBlock();
    EndEdit();
    OnChanged({}, false);
    return true;
}

TSharedPtr<FOdysseyRasterBlockEdit>
FOdysseyRasterContent::BeginEdit()
{
    check(IsInGameThread());
    if (!FApp::CanEverRender() || !EnsureImage())
    {
        return nullptr;
    }

    EndEdit();
    const FDescription Description = GetDescription();
    TSharedPtr<::ULIS::FBlock> Pixels = Odyssey::TiledImage::CopyToBlock(*Image, FIntRect(FIntPoint::ZeroValue, Description.Size), Description.Format, Description.bSRGB);
    if (!Pixels)
    {
        return nullptr;
    }
    const bool bPreserveAlpha = IsAlphaLocked && IsAlphaLocked();
    TSharedPtr<FOdysseyRasterEditSession> Session = FOdysseyRasterEditSession::Begin(RenderingId, *Image, Description.Size, bPreserveAlpha);
    if (!Session)
    {
        return nullptr;
    }

    TWeakPtr<FOdysseyRasterContent> WeakThis = AsShared();
    TSharedPtr<FOdysseyRasterBlockEdit> Edit = MakeShared<FOdysseyRasterBlockEdit>(Session.ToSharedRef(), Pixels.ToSharedRef(),
        Image->GetFormat(), Description.bSRGB,
        [WeakThis](const TArray<FIntRect>& Rects)
        {
            if (TSharedPtr<FOdysseyRasterContent> Content = WeakThis.Pin())
            {
                Content->OnEditCommitted(Rects);
            }
        });
    ActiveEdit = Edit;
    return Edit;
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
            ? Odyssey::TiledImage::CopyToBlock(*Image, FIntRect(FIntPoint::ZeroValue, Description.Size), Description.Format, Description.bSRGB)
            : nullptr;
        if (Pixels)
        {
            FOdysseyRasterBlockMutator Mutator(RasterBlock, false);
            Mutator.Copy(Pixels, { Pixels->Rect() });
            Mutator.Commit();
        }

        RasterBlock->OnBlockCommited().AddRaw(this, &FOdysseyRasterContent::OnBlockCommited);
        RasterBlock->PostProcess().BindRaw(this, &FOdysseyRasterContent::PreserveBlockAlpha);
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
    // Uncommitted edits are shown by the views, through the content replacement of the edit session.
    return BuildImageRenderFunction(OutRenderFunction, ContentTransform, GetDescription().Size);
}

bool
FOdysseyRasterContent::HasUncommittedEdits() const
{
    const TSharedPtr<FOdysseyRasterBlockEdit> Edit = ActiveEdit.Pin();
    return Edit.IsValid() && Edit->IsActive();
}

void
FOdysseyRasterContent::ReleaseRasterBlock()
{
    if (RasterBlock)
    {
        // Users may keep the block alive after it is released.
        RasterBlock->OnBlockCommited().RemoveAll(this);
        RasterBlock->PostProcess().Unbind();
        RasterBlock.Reset();
    }
}

void
FOdysseyRasterContent::EndEdit()
{
    if (TSharedPtr<FOdysseyRasterBlockEdit> Edit = ActiveEdit.Pin())
    {
        Edit->End();
    }
    ActiveEdit.Reset();
}

void
FOdysseyRasterContent::OnEditCommitted(const TArray<FIntRect>& Rects)
{
    ActiveEdit.Reset();
    // The raster block is derived from the previous tiles: it is created again when needed.
    ReleaseRasterBlock();
    OnChanged(Rects, false);
}

void
FOdysseyRasterContent::OnBlockCommited(const TArray<::ULIS::FRectI>& Rects)
{
    // The edit started from the previous pixels: committing it would discard this change.
    EndEdit();
    const TArray<FIntRect> IntRects = ::ULISUtils::ToIntRects(Rects);
    // While undoing, the image is restored by the transaction itself.
    if (!GIsTransacting && Image && FApp::CanEverRender())
    {
        TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> Block = RasterBlock->GetBlock();
        Odyssey::TiledImage::CopyFromBlock(*Image, *Block, IntRects, GetDescription().bSRGB);
    }
    OnChanged(IntRects, false);
}

TArray<::ULIS::FEvent>
FOdysseyRasterContent::PreserveBlockAlpha(const TMap<FIntPoint, TSharedPtr<::ULIS::FBlock>>& OriginalBlocks,
    const FOdysseyInvalidTileMap& InvalidMap, const TArray<::ULIS::FEvent>& WaitList)
{
    if (!RasterBlock || !IsAlphaLocked || !IsAlphaLocked())
    {
        return WaitList;
    }

    TArray<::ULIS::FEvent> Events;
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> Block = RasterBlock->GetBlock();
    ::ULIS::FContext& Context = IULISLoaderModule::StaticFindOrAddContext(RasterBlock->GetFormat());
    for (const FIntPoint& Tile : InvalidMap.InvalidTiles())
    {
        // The colors of the edited pixels with the alpha of the original ones.
        const TSharedPtr<::ULIS::FBlock>& OriginalBlock = OriginalBlocks[Tile];
        const ::ULIS::FRectI Rect = ::ULISUtils::ToULISRectI(InvalidMap.GetTileRect(Tile));
        ::ULIS::FEvent Event;
        Context.Blend(*OriginalBlock, *Block, ::ULIS::FRectI::Auto, Rect.Position(), ::ULIS::Blend_Back, ::ULIS::Alpha_Top, 1.f,
            ::ULIS::FSchedulePolicy::AsyncCacheEfficient, WaitList.Num(), WaitList.GetData(), &Event);
        Events.Add(Event);
    }
    return Events;
}

void
FOdysseyRasterContent::OnImageUndone()
{
    // The edit started from pixels the undo replaced.
    EndEdit();
    if (!RasterBlock || !FApp::CanEverRender())
    {
        OnChanged({}, false);
        return;
    }

    // Keep the derived block in sync with the restored tiles. GIsTransacting prevents writing them back.
    const FDescription Description = GetDescription();
    TSharedPtr<::ULIS::FBlock> Pixels = Odyssey::TiledImage::CopyToBlock(*Image, FIntRect(FIntPoint::ZeroValue, Description.Size), RasterBlock->GetFormat(), Description.bSRGB);
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
    RasterBlock->PostProcess().BindRaw(this, &FOdysseyRasterContent::PreserveBlockAlpha);
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
