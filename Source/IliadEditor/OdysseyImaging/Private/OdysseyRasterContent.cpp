// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyRasterContent.h"

#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Misc/App.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "RenderingThread.h"
#include "ScreenPass.h"
#include "TextureResource.h"
#include "UObject/StrongObjectPtr.h"

#include "OdysseyColorConversionPass.h"
#include "OdysseyRasterEditSession.h"
#include "OdysseyRasterSampling.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "OdysseyTileUtils.h"
#include "OdysseyTiledImage.h"
#include "OdysseyTiledImageBlock.h"
#include "ULISLoaderModule.h"

FOdysseyRasterContent::~FOdysseyRasterContent()
{
    EndEdit();
    if (UOdysseyTiledImage* Bound = BoundImage.Get())
    {
        Bound->OnTilesRestored().RemoveAll(this);
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
FOdysseyRasterContent::GetTiledImage() const
{
    return Image;
}

FOdysseyRasterContent::FDescription
FOdysseyRasterContent::Describe() const
{
    return GetDescription();
}

bool
FOdysseyRasterContent::EnsureImage()
{
    check(IsInGameThread());
    // The tiles are created, read and written on the GPU.
    if (!FApp::CanEverRender())
    {
        return false;
    }
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
        NewImage->Initialize(UOdysseyTiledImage::DefaultTileSize, Format);
        Image = NewImage;
    }
    else if (Image->GetFormat() != Format)
    {
        // e.g. a cell duplicated into an animation of another format, or the sRGB setting of a texture changed.
        // Tiles outside the canvas are converted too: a canvas shrunk then enlarged shows them again.
        FIntRect Bounds(FIntPoint::ZeroValue, Description.Size);
        const FIntRect TileBounds = Image->GetTileBounds();
        if (TileBounds.Area() > 0)
        {
            Bounds.Union(TileBounds);
        }
        const FIntRect PixelsRect(FIntPoint::ZeroValue, Bounds.Size());
        const EOdysseyTiledImageFormat PreviousFormat = Image->GetFormat();
        // For 8-bit content, linear tiles are RGBA8 and decoded sRGB tiles are RGBA16F.
        const bool bEncodingChanged = !ULIS_R_FLOATING(Description.Format) && ULIS_R_DEPTH(Description.Format) == 1
            && (PreviousFormat == EOdysseyTiledImageFormat::RGBA8) != (Format == EOdysseyTiledImageFormat::RGBA8)
            && (PreviousFormat == EOdysseyTiledImageFormat::RGBA16F || Format == EOdysseyTiledImageFormat::RGBA16F);
        TSharedPtr<::ULIS::FBlock> Pixels;
        // Recorded before Initialize() clears the tiles and changes the format: undoing the change gives them back.
        Image->Modify();
        if (bEncodingChanged && Description.bKeepBytesOnSRGBChange)
        {
            // Read the bytes as they were interpreted, write them as they are now interpreted.
            Pixels = Odyssey::TiledImage::CopyToBlock(*Image, Bounds, Description.Format, !Description.bSRGB);
            Image->Initialize(UOdysseyTiledImage::DefaultTileSize, Format);
            if (Pixels)
            {
                Odyssey::TiledImage::CopyFromBlock(*Image, *Pixels, PixelsRect, Bounds.Min, Description.bSRGB);
            }
        }
        else
        {
            // Keep the linear colors, in the precision of the new format.
            Pixels = Odyssey::TiledImage::CopyToBlock(*Image, Bounds, ::ULIS::Format_RGBAF);
            Image->Initialize(UOdysseyTiledImage::DefaultTileSize, Format);
            if (Pixels)
            {
                Odyssey::TiledImage::CopyFromBlock(*Image, *Pixels, PixelsRect, Bounds.Min);
            }
        }
        EndEdit();
    }

    if (BoundImage.Get() != Image)
    {
        if (UOdysseyTiledImage* Bound = BoundImage.Get())
        {
            Bound->OnTilesRestored().RemoveAll(this);
        }
        Image->OnTilesRestored().AddRaw(this, &FOdysseyRasterContent::OnImageUndone);
        BoundImage = Image.Get();
    }
    return true;
}

bool
FOdysseyRasterContent::SetContent(const ::ULIS::FBlock& Block)
{
    if (!EnsureImage())
    {
        return false;
    }

    Image->Initialize(UOdysseyTiledImage::DefaultTileSize, Image->GetFormat());
    Odyssey::TiledImage::CopyFromBlock(*Image, Block, FIntRect(0, 0, Block.Width(), Block.Height()), FIntPoint::ZeroValue, GetDescription().bSRGB);
    // The edit is derived from the previous content.
    EndEdit();
    NotifyChanged({});
    return true;
}

void
FOdysseyRasterContent::MigrateLegacy(TObjectPtr<UTexture2D>& LegacyTexture, TSharedPtr<::ULIS::FBlock>& LegacyPixels)
{
    if (!LegacyTexture && !LegacyPixels)
    {
        return;
    }

    TSharedPtr<::ULIS::FBlock> Pixels = LegacyPixels;
    if (!Pixels && LegacyTexture->Source.IsValid())
    {
        if (!EnsureImage())
        {
            return;
        }
        Pixels = MakeShareable(NewBlockFromUTextureData(LegacyTexture, GetDescription().Format));
    }
    if (Pixels && !SetContent(*Pixels))
    {
        return;
    }
    LegacyPixels.Reset();
    LegacyTexture = nullptr;
}

TSharedPtr<FOdysseyRasterEditSession>
FOdysseyRasterContent::BeginEditSession()
{
    check(IsInGameThread());
    if (!EnsureImage())
    {
        return nullptr;
    }

    // Beginning a session ends the active session of the content.
    const bool bPreserveAlpha = IsAlphaLocked && IsAlphaLocked();
    TSharedPtr<FOdysseyRasterEditSession> Session = FOdysseyRasterEditSession::Begin(RenderingId, *Image, GetDescription().Size, bPreserveAlpha);
    if (!Session)
    {
        return nullptr;
    }

    TWeakPtr<FOdysseyRasterContent> WeakThis = AsShared();
    Session->SetOnCommitted([WeakThis](const TArray<FIntRect>& Rects)
    {
        if (TSharedPtr<FOdysseyRasterContent> Content = WeakThis.Pin())
        {
            Content->OnEditCommitted(Rects);
        }
    });
    return Session;
}

UTextureRenderTarget2D*
FOdysseyRasterContent::CreateRenderTarget()
{
    if (!EnsureImage())
    {
        return nullptr;
    }
    return Image->CreateRenderTarget(GetDescription().Size);
}

bool
FOdysseyRasterContent::Read(UTextureRenderTarget2D& Destination, const FIntRect& Rect, FIntPoint Position)
{
    if (!EnsureImage())
    {
        return false;
    }
    Image->Render(&Destination, Rect, Position);
    return true;
}

bool
FOdysseyRasterContent::Write(UTextureRenderTarget2D& Source, const TArray<FIntRect>& Rects, EOdysseyColorConversion Conversion)
{
    check(IsInGameThread());
    if (!EnsureImage())
    {
        return false;
    }

    const FDescription Description = GetDescription();
    const FIntRect Bounds(FIntPoint::ZeroValue, Description.Size);
    const TArray<FIntRect> Clipped = Odyssey::TileUtils::ClipRects(Rects, Bounds);
    if (Clipped.IsEmpty())
    {
        return false;
    }

    // Beginning a session ends the active edit, which started from the previous pixels: committing it would discard this change.
    // The session keeps the alpha of the content when it is locked.
    const bool bPreserveAlpha = IsAlphaLocked && IsAlphaLocked();
    TSharedPtr<FOdysseyRasterEditSession> Session = FOdysseyRasterEditSession::Begin(RenderingId, *Image, Description.Size, bPreserveAlpha);
    if (!Session)
    {
        return false;
    }

    // Copies the source pixels, converted into linear colors, at the same positions.
    ENQUEUE_RENDER_COMMAND(OdysseyRasterContentWrite)(
        [SourceResource = Source.GameThread_GetRenderTargetResource(), TargetResource = Session->GetTarget()->GameThread_GetRenderTargetResource(),
         Pass = FOdysseyColorConversions::Prepare(Conversion), Clipped](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder GraphBuilder(RHICmdList);
            FRDGTextureRef SourceTexture = GraphBuilder.RegisterExternalTexture(
                ::CreateRenderTarget(SourceResource->GetRenderTargetTexture(), TEXT("Odyssey.RasterContent.WriteSource")));
            FRDGTextureRef Target = GraphBuilder.RegisterExternalTexture(
                ::CreateRenderTarget(TargetResource->GetRenderTargetTexture(), TEXT("Odyssey.RasterContent.WriteTarget")));
            for (const FIntRect& Rect : Clipped)
            {
                if (Pass.IsValid())
                {
                    Pass.Add(GraphBuilder, GMaxRHIFeatureLevel, SourceTexture, Rect, Target, Rect);
                }
                else
                {
                    AddDrawTexturePass(GraphBuilder, FScreenPassViewInfo(), SourceTexture, Target, Rect.Min, Rect.Size(), Rect.Min, Rect.Size());
                }
            }
            GraphBuilder.Execute();
        });

    if (!Session->Commit(Clipped))
    {
        return false;
    }
    NotifyChanged(Clipped);
    return true;
}

bool
FOdysseyRasterContent::Clear()
{
    check(IsInGameThread());
    if (!EnsureImage())
    {
        return false;
    }
    if (IsAlphaLocked && IsAlphaLocked())
    {
        // Transparent pixels keep the alpha of the content: only its colors are cleared.
        TStrongObjectPtr<UTextureRenderTarget2D> Transparent(CreateRenderTarget());
        return Transparent && Write(*Transparent, { FIntRect(FIntPoint::ZeroValue, GetDescription().Size) });
    }
    // The edit started from the previous pixels: committing it would discard this change.
    EndEdit();
    Image->RemoveAllTiles();
    NotifyChanged({});
    return true;
}

bool
FOdysseyRasterContent::BuildRenderFunction(IOdysseyTextureRenderingAbility::FRenderFunction& OutRenderFunction, const FMatrix& ContentTransform) const
{
    // The owner creates and converts the image at explicit points, e.g. after loading: rendering changes nothing.
    if (!Image || !FApp::CanEverRender())
    {
        return false;
    }
    // Uncommitted edits are shown by the views, through the content replacement of the edit session.
    return Odyssey::RasterSampling::BuildTiledImageRenderFunction(*Image, GetDescription().Size, ContentTransform, OutRenderFunction);
}

void
FOdysseyRasterContent::EndEdit()
{
    if (TSharedPtr<FOdysseyRasterEditSession> Session = FOdysseyRasterEditSession::Find(RenderingId))
    {
        Session->End();
    }
}

void
FOdysseyRasterContent::OnEditCommitted(const TArray<FIntRect>& Rects)
{
    NotifyChanged(Rects);
}

void
FOdysseyRasterContent::OnImageUndone()
{
    // The edit started from pixels the undo replaced.
    EndEdit();
    NotifyChanged({});
}

void
FOdysseyRasterContent::NotifyChanged(const TArray<FIntRect>& Rects)
{
    if (Rects.IsEmpty())
    {
        OnChanged({ FIntRect(FIntPoint::ZeroValue, GetDescription().Size) });
        return;
    }
    OnChanged(Rects);
}
