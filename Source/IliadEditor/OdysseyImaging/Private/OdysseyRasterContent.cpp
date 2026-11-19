// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyRasterContent.h"

#include "Engine/TextureRenderTarget2D.h"
#include "Math/TranslationMatrix.h"
#include "Misc/App.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "RenderingThread.h"
#include "ScreenPass.h"
#include "TextureResource.h"

#include "OdysseyBlendShader.h"
#include "OdysseyColorSpaceShader.h"
#include "OdysseyRasterBlockEdit.h"
#include "OdysseyRasterEditSession.h"
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
    if (!FApp::CanEverRender() || !EnsureImage())
    {
        return false;
    }

    Image->Initialize(TileSize, Image->GetFormat());
    Odyssey::TiledImage::CopyFromBlock(*Image, Block, FIntRect(0, 0, Block.Width(), Block.Height()), FIntPoint::ZeroValue, GetDescription().bSRGB);
    // The edit is derived from the previous content.
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

UTextureRenderTarget2D*
FOdysseyRasterContent::CreateRenderTarget()
{
    if (!FApp::CanEverRender() || !EnsureImage())
    {
        return nullptr;
    }
    return Image->CreateRenderTarget(GetDescription().Size);
}

bool
FOdysseyRasterContent::Read(UTextureRenderTarget2D& Destination, const FIntRect& Rect, FIntPoint Position)
{
    if (!FApp::CanEverRender() || !EnsureImage())
    {
        return false;
    }
    Image->Render(&Destination, Rect, Position);
    return true;
}

bool
FOdysseyRasterContent::Write(UTextureRenderTarget2D& Source, const TArray<FIntRect>& Rects, EOdysseyColorConversion Conversion)
{
    if (!FApp::CanEverRender())
    {
        return false;
    }

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

TSharedPtr<::ULIS::FBlock>
FOdysseyRasterContent::ReadBlock()
{
    if (!FApp::CanEverRender() || !EnsureImage())
    {
        return nullptr;
    }
    const FDescription Description = GetDescription();
    return Odyssey::TiledImage::CopyToBlock(*Image, FIntRect(FIntPoint::ZeroValue, Description.Size), Description.Format, Description.bSRGB);
}

bool
FOdysseyRasterContent::WriteBlock(const ::ULIS::FBlock& Block, const TArray<FIntRect>& Rects)
{
    if (!FApp::CanEverRender() || !EnsureImage())
    {
        return false;
    }

    // As the former raster block, convert the block to the expected format first: its 8-bit colors are then sRGB encoded when bSRGB is set.
    const FDescription Description = GetDescription();
    const ::ULIS::FBlock* Pixels = &Block;
    TSharedPtr<::ULIS::FBlock> Converted;
    if (Block.Format() != Description.Format)
    {
        Converted = MakeShared<::ULIS::FBlock>(Block.Width(), Block.Height(), Description.Format);
        ::ULIS::FContext& Context = IULISLoaderModule::StaticFindOrAddContext(Description.Format);
        Context.ConvertFormat(Block, *Converted, ::ULIS::FRectI::Auto, ::ULIS::FVec2I(0), ::ULIS::FSchedulePolicy::MultiScanlines);
        Context.Finish();
        Pixels = Converted.Get();
    }

    const FIntRect BlockBounds(0, 0, Pixels->Width(), Pixels->Height());
    TArray<FIntRect> InBlock;
    InBlock.Reserve(Rects.Num());
    for (FIntRect Rect : Rects)
    {
        Rect.Clip(BlockBounds);
        if (Rect.Width() > 0 && Rect.Height() > 0)
        {
            InBlock.Add(Rect);
        }
    }

    const EOdysseyTiledImageFormat TileFormat = Image->GetFormat();
    return WriteThroughSession(InBlock, [Pixels, TileFormat, &Description](FOdysseyRasterEditSession& Session, const TArray<FIntRect>& Clipped)
    {
        Odyssey::TiledImage::CopyBlockToRenderTarget(*Session.GetTarget(), TileFormat, *Pixels, Clipped, Description.bSRGB);
    });
}

bool
FOdysseyRasterContent::EditBlock(TFunctionRef<void(::ULIS::FBlock& Block)> Edit)
{
    TSharedPtr<::ULIS::FBlock> Block = ReadBlock();
    if (!Block)
    {
        return false;
    }
    Edit(*Block);
    return WriteBlock(*Block, { FIntRect(0, 0, Block->Width(), Block->Height()) });
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
    OnChanged(Rects, false);
}

void
FOdysseyRasterContent::OnImageUndone()
{
    // The edit started from pixels the undo replaced.
    EndEdit();
    OnChanged({}, false);
}

bool
FOdysseyRasterContent::WriteThroughSession(const TArray<FIntRect>& Rects,
    TFunctionRef<void(FOdysseyRasterEditSession& Session, const TArray<FIntRect>& Rects)> Fill)
{
    check(IsInGameThread());
    if (!FApp::CanEverRender() || !EnsureImage())
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

    // The edit started from the previous pixels: committing it would discard this change.
    EndEdit();
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
    OnChanged(Clipped, false);
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

        // The area of the content the blend samples: the source rect, brought back into content space, within the canvas.
        const FMatrix ToSource = ContentTransform * Transform;
        const FMatrix ToContent = ToSource.Inverse();
        FBox2D Sampled(ForceInit);
        for (const FIntPoint& Corner : { SourceRect.Min, FIntPoint(SourceRect.Max.X, SourceRect.Min.Y), SourceRect.Max, FIntPoint(SourceRect.Min.X, SourceRect.Max.Y) })
        {
            const FVector Position = ToContent.TransformPosition(FVector(Corner.X, Corner.Y, 0.0));
            Sampled += FVector2D(Position.X, Position.Y);
        }
        // A margin keeps the filtering of the border pixels: anisotropic filtering samples along the footprint
        // of a destination pixel, up to its maximum anisotropy, so the margin grows with the texels per pixel.
        constexpr float MaxAnisotropy = 16.0f;
        const float TexelsPerPixel = FMath::Max(Sampled.GetSize().X / FMath::Max(DestinationRect.Width(), 1),
            Sampled.GetSize().Y / FMath::Max(DestinationRect.Height(), 1));
        const int32 FilterMargin = FMath::CeilToInt(TexelsPerPixel * MaxAnisotropy) + 2;
        FIntRect Area(FMath::FloorToInt(Sampled.Min.X) - FilterMargin, FMath::FloorToInt(Sampled.Min.Y) - FilterMargin,
            FMath::CeilToInt(Sampled.Max.X) + FilterMargin, FMath::CeilToInt(Sampled.Max.Y) + FilterMargin);
        Area.Clip(FIntRect(FIntPoint::ZeroValue, Size));
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
