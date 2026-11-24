// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"

#include <ULIS>

class FOdysseyRasterBlockEdit;
class FOdysseyRasterContent;

/**
 * Transitional: access to a raster content through CPU blocks, for users that do not work on render targets yet.
 *
 * These functions only rely on the public API of FOdysseyRasterContent. Their blocks are in the format the content owner
 * expects, see FOdysseyRasterContent::FDescription. Remove each use once its user works on render targets,
 * and this file with the last one. TODO(GPU-tools).
 * Threading: game thread only; every function waits for the GPU.
 */
namespace Odyssey::RasterContentBlocks
{
/**
 * Reads the whole content into a new block. Waits for the GPU.
 * @return Null when the owner cannot hold content or the rendering is unavailable.
 */
ODYSSEYIMAGING_API TSharedPtr<::ULIS::FBlock> ReadBlock(FOdysseyRasterContent& Content);

/**
 * Reads an area of the content into a new block, as large as the area. Waits for the GPU.
 * @param Rect The area to read, in content pixels; clipped to the canvas.
 * @param OutRect The area read: the position of the block in the content.
 * @return Null when the clipped area is empty, the owner cannot hold content or the rendering is unavailable.
 */
ODYSSEYIMAGING_API TSharedPtr<::ULIS::FBlock> ReadBlock(FOdysseyRasterContent& Content, const FIntRect& Rect, FIntRect& OutRect);

/**
 * Writes areas of a block into the content, as FOdysseyRasterContent::Write() does.
 * The block is first converted to the format the owner expects, its 8-bit colors being sRGB encoded when bSRGB is set.
 * @param Block The pixels to write.
 * @param Rects The areas to write, in content pixels; clipped to the content and block bounds.
 * @param Position The position of the block origin in the content.
 * @return False when nothing was written.
 */
ODYSSEYIMAGING_API bool WriteBlock(FOdysseyRasterContent& Content, const ::ULIS::FBlock& Block, const TArray<FIntRect>& Rects, FIntPoint Position = FIntPoint::ZeroValue);

/**
 * Reads the whole content into a block, lets the caller change it, then writes the whole block back as WriteBlock() does.
 * @param Edit Changes the block, in the format the owner expects. Pending ULIS operations must be finished when it returns.
 * @return False when nothing was written.
 */
ODYSSEYIMAGING_API bool EditBlock(FOdysseyRasterContent& Content, TFunctionRef<void(::ULIS::FBlock& Block)> Edit);

/**
 * Reads an area of the content into a block as large as the area, lets the caller change it, then writes it back as WriteBlock() does.
 * Only this area is read and written.
 * @param Rect The area to edit, in content pixels; clipped to the canvas.
 * @param Edit Changes the block, in the format the owner expects; Area is the position of the block in the content.
 *             Pending ULIS operations must be finished when it returns.
 * @return False when nothing was written, e.g. the clipped area is empty.
 */
ODYSSEYIMAGING_API bool EditBlock(FOdysseyRasterContent& Content, const FIntRect& Rect, TFunctionRef<void(::ULIS::FBlock& Block, const FIntRect& Area)> Edit);

/**
 * Returns the smallest area of a block holding its visible pixels, e.g. to read and write only the area a pasted block changes.
 * @return The area, in block pixels; empty when every pixel is transparent.
 */
ODYSSEYIMAGING_API FIntRect FindVisibleRect(const ::ULIS::FBlock& Block);

/**
 * Begins an edit of the content through blocks, on top of FOdysseyRasterContent::BeginEditSession(): the working block of the edit
 * starts with the pixels of the content, and its updates are uploaded into the edit session.
 * @return Null when the owner cannot hold content or the rendering is unavailable.
 */
ODYSSEYIMAGING_API TSharedPtr<FOdysseyRasterBlockEdit> BeginBlockEdit(FOdysseyRasterContent& Content);
}
