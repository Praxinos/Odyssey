// IDDN.FR.001.060015.014.S.X.2019.000.00000
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
 * and this file with the last one.
 */
namespace Odyssey::RasterContentBlocks
{
/**
 * Reads the whole content into a new block. Waits for the GPU.
 * @return Null when the owner cannot hold content or the rendering is unavailable.
 */
ODYSSEYIMAGING_API TSharedPtr<::ULIS::FBlock> ReadBlock(FOdysseyRasterContent& Content);

/**
 * Writes areas of a block into the content, as FOdysseyRasterContent::Write() does.
 * The block is first converted to the format the owner expects, its 8-bit colors being sRGB encoded when bSRGB is set.
 * @param Block The pixels to write, at the same positions as in the content.
 * @param Rects The areas to write, in content pixels; clipped to the content and block bounds.
 * @return False when nothing was written.
 */
ODYSSEYIMAGING_API bool WriteBlock(FOdysseyRasterContent& Content, const ::ULIS::FBlock& Block, const TArray<FIntRect>& Rects);

/**
 * Reads the whole content into a block, lets the caller change it, then writes the whole block back as WriteBlock() does.
 * @param Edit Changes the block, in the format the owner expects. Pending ULIS operations must be finished when it returns.
 * @return False when nothing was written.
 */
ODYSSEYIMAGING_API bool EditBlock(FOdysseyRasterContent& Content, TFunctionRef<void(::ULIS::FBlock& Block)> Edit);

/**
 * Begins an edit of the content through blocks, on top of FOdysseyRasterContent::BeginEdit(): the working block of the edit
 * starts with the pixels of the content, and its updates are uploaded into the edit session.
 * @return Null when the owner cannot hold content or the rendering is unavailable.
 */
ODYSSEYIMAGING_API TSharedPtr<FOdysseyRasterBlockEdit> BeginBlockEdit(FOdysseyRasterContent& Content);
}
