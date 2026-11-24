// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"

#include "OdysseyTiledImage.h"

#include <ULIS>

class UTextureRenderTarget2D;

/**
 * Transitional adapters between ::ULIS::FBlock and UOdysseyTiledImage.
 *
 * They exist while raster editing still works on CPU blocks. Both go through the GPU path
 * (CopyFromTexture() and Render()), so they can be removed once every caller uses render targets.
 */
namespace Odyssey::TiledImage
{
/**
 * Returns the tile format able to store a block of the given format without precision loss.
 * 8-bit formats map to RGBA8, or to RGBA16F when they hold sRGB encoded colors, as the image holds linear colors;
 * 16-bit integer and floating formats map to RGBA32F.
 */
ODYSSEYIMAGING_API EOdysseyTiledImageFormat TiledImageFormatForULISFormat(::ULIS::eFormat Format, bool bSRGB = false);

/**
 * Returns the RGBA ULIS format whose channel order and type match the given tile format.
 * RGBA16F maps to 32-bit float RGBA, as ULIS has no half float type.
 */
ODYSSEYIMAGING_API ::ULIS::eFormat ULISFormatForTiledImageFormat(EOdysseyTiledImageFormat Format);

/**
 * Copies a part of a block into the image, as CopyFromTexture() does for a texture.
 * The block is converted to the tile format. Without bSRGB, values are copied as they are, so only channel order and type change.
 * Calls Modify() on the image, so the copy is undoable within a transaction.
 *
 * @param Image The destination image.
 * @param Block The source block.
 * @param SourceRect The part of the block to copy; clipped to the block bounds.
 * @param Position The position, in image pixels, of the top-left corner of SourceRect.
 * @param bSRGB True when the 8-bit block holds sRGB encoded colors: they are decoded, so that the image holds linear colors.
 *              The image must then have a floating tile format.
 * @return False when nothing was copied.
 */
ODYSSEYIMAGING_API bool CopyFromBlock(UOdysseyTiledImage& Image, const ::ULIS::FBlock& Block, FIntRect SourceRect, FIntPoint Position, bool bSRGB = false);

/**
 * Copies several parts of a block into the image, each one at the same position in the image.
 * The bounds of the parts are uploaded once; only the tiles touched by a part are replaced.
 * Calls Modify() on the image, so the copy is undoable within a transaction.
 *
 * @param Image The destination image.
 * @param Block The source block.
 * @param Rects The parts of the block to copy; clipped to the block bounds.
 * @param bSRGB True when the 8-bit block holds sRGB encoded colors, decoded as in the single part overload.
 * @return False when nothing was copied.
 */
ODYSSEYIMAGING_API bool CopyFromBlock(UOdysseyTiledImage& Image, const ::ULIS::FBlock& Block, const TArray<FIntRect>& Rects, bool bSRGB = false);

/**
 * Copies parts of a block into a render target holding pixels in the given tile format, e.g. the target of an edit session.
 * Values are converted as CopyFromBlock() does; each part lands at its position in the block, offset by Position, in the target.
 *
 * @param Target The destination render target; its pixel format must be the one of Format.
 * @param Format The tile format of the target pixels.
 * @param Block The source block.
 * @param Rects The parts of the block to copy, in block pixels; clipped to the block and target bounds.
 * @param bSRGB True when the 8-bit block holds sRGB encoded colors, decoded as in CopyFromBlock().
 * @param Position The position of the block origin in the target.
 * @return False when nothing was copied.
 */
ODYSSEYIMAGING_API bool CopyBlockToRenderTarget(UTextureRenderTarget2D& Target, EOdysseyTiledImageFormat Format, const ::ULIS::FBlock& Block, const TArray<FIntRect>& Rects, bool bSRGB = false, FIntPoint Position = FIntPoint::ZeroValue);

/**
 * Reads a part of the image into a new block. Pixels without a tile are transparent.
 * Waits for the GPU.
 *
 * @param Image The source image.
 * @param Rect The part of the image to read, in image pixels.
 * @param Format The format of the returned block.
 * @param bSRGB True when the returned 8-bit block holds sRGB encoded colors: the linear colors of the image are encoded.
 * @return The block, or null when Rect is empty, larger than a block can be, or the GPU readback failed.
 */
ODYSSEYIMAGING_API TSharedPtr<::ULIS::FBlock> CopyToBlock(const UOdysseyTiledImage& Image, FIntRect Rect, ::ULIS::eFormat Format, bool bSRGB = false);
}
