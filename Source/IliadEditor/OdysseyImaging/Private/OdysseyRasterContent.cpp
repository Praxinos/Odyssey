// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyRasterContent.h"

#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Math/TranslationMatrix.h"
#include "Misc/App.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "RenderingThread.h"
#include "ScreenPass.h"
#include "TextureResource.h"
#include "UObject/StrongObjectPtr.h"

#include "OdysseyBlendShader.h"
#include "OdysseyColorSpaceShader.h"
#include "OdysseyRasterEditSession.h"
#include "OdysseyRasterSampling.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "OdysseyTiledImage.h"
#include "OdysseyTiledImageBlock.h"
#include "ULISLoaderModule.h"

namespace
{
constexpr int32 TileSize = 64;
}

FOdysseyRasterContent::~FOdysseyRasterContent()
{
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
        NewImage->Initialize(TileSize, Format);
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
        if (bEncodingChanged && Description.bKeepBytesOnEncodingChange)
        {
            // Read the bytes as they were interpreted, write them as they are now interpreted.
            Pixels = Odyssey::TiledImage::CopyToBlock(*Image, Bounds, Description.Format, !Description.bSRGB);
            Image->Initialize(TileSize, Format);
            if (Pixels)
            {
                Odyssey::TiledImage::CopyFromBlock(*Image, *Pixels, PixelsRect, Bounds.Min, Description.bSRGB);
            }
        }
        else
        {
            // Keep the linear colors, in the precision of the new format.
            Pixels = Odyssey::TiledImage::CopyToBlock(*Image, Bounds, ::ULIS::Format_RGBAF);
            Image->Initialize(TileSize, Format);
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
    if (!EnsureImage())
    {
        return false;
    }

    Image->Initialize(TileSize, Image->GetFormat());
    Odyssey::TiledImage::CopyFromBlock(*Image, Block, FIntRect(0, 0, Block.Width(), Block.Height()), FIntPoint::ZeroValue, GetDescription().bSRGB);
    // The edit is derived from the previous content.
    EndEdit();
    NotifyChanged({}, false);
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
FOdysseyRasterContent::BeginEdit()
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
    // Copies the source pixels, converted into linear colors, at the same positions.
    IOdysseyTextureRenderingAbility::FRenderFunction Copy = [SourceResource = Source.GameThread_GetRenderTargetResource()](
        FRDGBuilder& GraphBuilder, ERHIFeatureLevel::Type FeatureLevel, FRDGTextureRef Destination,
        const FIntRect& SourceRect, const FIntRect& DestinationRect, const FMatrix& Transform)
    {
        FRDGTextureRef SourceTexture = GraphBuilder.RegisterExternalTexture(
            ::CreateRenderTarget(SourceResource->GetRenderTargetTexture(), TEXT("Odyssey.RasterContent.WriteSource")));
        AddDrawTexturePass(GraphBuilder, FScreenPassViewInfo(), SourceTexture, Destination,
            SourceRect.Min, SourceRect.Size(), DestinationRect.Min, DestinationRect.Size());
    };
    Copy = FOdysseyColorSpaceShader::WrapRenderFunction(MoveTemp(Copy), Conversion);

    return WriteThroughSession(Rects, [&Copy](FOdysseyRasterEditSession& Session, const TArray<FIntRect>& Clipped)
    {
        ENQUEUE_RENDER_COMMAND(OdysseyRasterContentWrite)(
            [TargetResource = Session.GetTarget()->GameThread_GetRenderTargetResource(), Copy, Clipped](FRHICommandListImmediate& RHICmdList)
            {
                FRDGBuilder GraphBuilder(RHICmdList);
                FRDGTextureRef Target = GraphBuilder.RegisterExternalTexture(
                    ::CreateRenderTarget(TargetResource->GetRenderTargetTexture(), TEXT("Odyssey.RasterContent.WriteTarget")));
                for (const FIntRect& Rect : Clipped)
                {
                    Copy(GraphBuilder, GMaxRHIFeatureLevel, Target, Rect, Rect, FMatrix::Identity);
                }
                GraphBuilder.Execute();
            });
    });
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
    NotifyChanged({}, false);
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
    return BuildImageRenderFunction(OutRenderFunction, ContentTransform, GetDescription().Size);
}

bool
FOdysseyRasterContent::HasUncommittedEdits() const
{
    return FOdysseyRasterEditSession::Find(RenderingId).IsValid();
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
    NotifyChanged(Rects, false);
}

void
FOdysseyRasterContent::OnImageUndone()
{
    // The edit started from pixels the undo replaced.
    EndEdit();
    NotifyChanged({}, false);
}

void
FOdysseyRasterContent::NotifyChanged(const TArray<FIntRect>& Rects, bool bInteractive)
{
    if (Rects.IsEmpty())
    {
        OnChanged({ FIntRect(FIntPoint::ZeroValue, GetDescription().Size) }, bInteractive);
        return;
    }
    OnChanged(Rects, bInteractive);
}

bool
FOdysseyRasterContent::WriteThroughSession(const TArray<FIntRect>& Rects,
    TFunctionRef<void(FOdysseyRasterEditSession& Session, const TArray<FIntRect>& Rects)> Fill)
{
    check(IsInGameThread());
    if (!EnsureImage())
    {
        return false;
    }

    const FDescription Description = GetDescription();
    const FIntRect Bounds(FIntPoint::ZeroValue, Description.Size);
    TArray<FIntRect> Clipped;
    Clipped.Reserve(Rects.Num());
    for (FIntRect Rect : Rects)
    {
        Rect.Clip(Bounds);
        if (Rect.Width() > 0 && Rect.Height() > 0)
        {
            Clipped.Add(Rect);
        }
    }
    if (Clipped.IsEmpty())
    {
        return false;
    }

    // Beginning a session ends the active edit, which started from the previous pixels: committing it would discard this change.
    const bool bPreserveAlpha = IsAlphaLocked && IsAlphaLocked();
    TSharedPtr<FOdysseyRasterEditSession> Session = FOdysseyRasterEditSession::Begin(RenderingId, *Image, Description.Size, bPreserveAlpha);
    if (!Session)
    {
        return false;
    }
    Fill(*Session, Clipped);
    if (!Session->Commit(Clipped))
    {
        return false;
    }
    NotifyChanged(Clipped, false);
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
        AddClearRenderTargetPass(GraphBuilder, Destination, FLinearColor::Transparent, DestinationRect);

        // The area of the content the blend samples.
        const FMatrix ToSource = ContentTransform * Transform;
        const FIntRect Area = Odyssey::RasterSampling::GetSampledArea(SourceRect, DestinationRect, ToSource, Size);
        if (Area.Width() <= 0 || Area.Height() <= 0)
        {
            return;
        }

        // Assemble a continuous source of the sampled tiles only before filtering: atlas borders must not create seams
        // under zoom, rotation or anisotropic sampling. No persistent texture is added.
        FRDGTextureRef Source = GraphBuilder.CreateTexture(FRDGTextureDesc::Create2D(Area.Size(), Format,
            FClearValueBinding::Transparent, TexCreate_ShaderResource | TexCreate_RenderTargetable),
            TEXT("Odyssey.RasterContent.Source"));
        DrawTiles(GraphBuilder, Source, FIntPoint::ZeroValue - Area.Min);
        // The source starts at the area origin in content space.
        const FMatrix FromArea = FTranslationMatrix(FVector(Area.Min.X, Area.Min.Y, 0.0)) * ToSource;
        FOdysseyBlendShader::BlendRect(GraphBuilder, FeatureLevel, Destination, Source, Destination,
            SourceRect, DestinationRect, FromArea, EOdysseyColorBlendMode::Normal,
            EOdysseyAlphaBlendMode::Normal, 1.0f, EOdysseyAntiAliasing::Anisotropic);
    };
    return true;
}
