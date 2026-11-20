// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include <ULIS>

namespace Odyssey
{
/**
 * Reads the pixels of a raster layer or cell saved before the tiled image storage, then removes their former disk cache entry.
 * Assets are no longer saved with these pixels: their owner moves them into its tiled image after loading.
 *
 * @param Ar The archive of the owner, positioned at the raster block. Only a persistent loading archive is read.
 * @param Owner The object whose package holds the bulk data of the pixels.
 * @return The pixels, or null when the archive holds none or they cannot be read.
 */
ODYSSEYIMAGING_API TSharedPtr<::ULIS::FBlock> ReadLegacyRasterBlock(FArchive& Ar, UObject& Owner);
}
