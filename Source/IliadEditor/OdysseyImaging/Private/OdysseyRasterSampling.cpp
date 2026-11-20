// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyRasterSampling.h"

namespace Odyssey::RasterSampling
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
}
