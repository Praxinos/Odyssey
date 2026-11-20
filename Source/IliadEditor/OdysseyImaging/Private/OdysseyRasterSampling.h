// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"

namespace Odyssey::RasterSampling
{
/**
 * Returns the area of a content a render samples: the source rect, brought back into content space, with a margin
 * for the filtering of its border pixels, within the content.
 * @param SourceRect The source rect given to the render function.
 * @param DestinationRect The destination rect given to the render function.
 * @param ToSource The transform from content space to the source space of the render.
 * @param Size The size of the content.
 * @return The sampled area, in content pixels; without area when the render samples nothing of the content.
 */
FIntRect GetSampledArea(const FIntRect& SourceRect, const FIntRect& DestinationRect, const FMatrix& ToSource, FIntPoint Size);
}
