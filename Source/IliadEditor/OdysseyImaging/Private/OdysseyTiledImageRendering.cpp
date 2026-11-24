// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyTiledImageRendering.h"

#include "Math/TranslationMatrix.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"

#include "OdysseyBlendShader.h"
#include "OdysseyTiledImage.h"

namespace Odyssey::TiledImageRendering
{
FIntRect GetSampledArea(const FIntRect& SourceRect, const FIntRect& DestinationRect, const FMatrix& ToSource, FIntPoint Size)
{
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
    return Area;
}

bool BuildTiledImageRenderFunction(const UOdysseyTiledImage& Image, FIntPoint Size, const FMatrix& ContentTransform,
    IOdysseyTextureRenderingAbility::FRenderFunction& OutRenderFunction)
{
    // The tiles hold linear colors, sampled as they are.
    const EPixelFormat Format = Image.GetPixelFormat();
    UOdysseyTiledImage::FRenderFunction DrawTiles = Image.BuildRenderFunction(FIntRect(FIntPoint::ZeroValue, Size));
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
        const FIntRect Area = GetSampledArea(SourceRect, DestinationRect, ToSource, Size);
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
}
