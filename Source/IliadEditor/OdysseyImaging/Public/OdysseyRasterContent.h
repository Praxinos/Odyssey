// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectPtr.h"
#include "UObject/StrongObjectPtr.h"

#include "OdysseyRasterBlock.h"
#include "OdysseyTextureRenderingAbility.h"

#include <ULIS>

class UOdysseyTiledImage;
class UTexture2D;

/**
 * Raster content shared by raster texture layers and raster animation cells.
 *
 * The tiled image is the only persisted and undoable storage of the pixels, as linear colors. The owner holds it in an instanced
 * UPROPERTY so that it is saved, duplicated and garbage collected with the owner; this class creates it in that
 * property, keeps it in the format the owner expects, renders it and writes into it.
 *
 * Transitional: while tools still edit an FOdysseyRasterBlock, this class derives that block from the tiled image,
 * writes the committed edits of the block back into the tiled image, and displays the uncommitted edits
 * through a transient preview texture. This part goes away once tools edit render targets.
 */
class ODYSSEYIMAGING_API FOdysseyRasterContent
{
public:
    /**
     * The content the owner expects: its size in pixels and the format of its raster block.
     *
     * Transitional, removed with the raster block:
     * - The size only sizes the raster block, the preview texture and the full-size source assembled for rendering.
     *   Tiled images are unbounded: rendering will assemble only the tiles covering the source rect it is given,
     *   and the canvas bounds will be the owner's concern.
     * - The format will be given when the image is created, then converted explicitly when the owner changes format
     *   (e.g. a cell duplicated into an animation of another format), rather than queried on each use.
     */
    struct FDescription
    {
        /** Returns true when the owner can hold content, e.g. it is attached to a texture or an animation. */
        bool IsValid() const { return Size.X > 0 && Size.Y > 0; }

        FIntPoint Size = FIntPoint::ZeroValue;
        ::ULIS::eFormat Format = ::ULIS::Format_RGBA8;
    };

    using FGetDescription = TFunction<FDescription()>;

    /**
     * Called when pixels changed.
     * @param Rects The changed areas, in content pixels; empty when everything changed.
     * @param bInteractive True for uncommitted edits.
     */
    using FOnChanged = TFunction<void(const TArray<FIntRect>& Rects, bool bInteractive)>;

public:
    ~FOdysseyRasterContent();

    /**
     * @param InOwner The object owning the image property; the outer of the image and the owner of the raster block.
     * @param InImage The owner's instanced property holding the tiled image. It must outlive this object.
     * @param bInSRGB True when the 8-bit raster block holds sRGB encoded colors. The image always holds linear colors:
     *                they are decoded when written into it and encoded when the raster block is read from it.
     * @param InGetDescription Returns the content the owner expects.
     * @param InOnChanged Called when pixels changed.
     * @param InPostProcess Applied by the raster block after each edit, e.g. the alpha lock.
     */
    FOdysseyRasterContent(UObject& InOwner, TObjectPtr<UOdysseyTiledImage>& InImage, bool bInSRGB,
        FGetDescription InGetDescription, FOnChanged InOnChanged, FOdysseyRasterBlock::FPostProcess InPostProcess);

    FOdysseyRasterContent(const FOdysseyRasterContent&) = delete;
    FOdysseyRasterContent& operator=(const FOdysseyRasterContent&) = delete;

public:
    /** Returns the existing image without creating it. */
    UOdysseyTiledImage* GetImage() const;

    /**
     * Creates the image when missing, and converts it when the owner expects another format.
     * @return False when the owner cannot hold content.
     */
    bool EnsureImage();

    /**
     * Replaces the whole content with the block, without undo. Used to create or migrate content.
     * @return False when the owner cannot hold content or the rendering is unavailable.
     */
    bool SetContent(const ::ULIS::FBlock& Block);

    /**
     * Transitional: returns the raster block edited by tools, created from the image when needed.
     * Committed edits of this block are written into the image, uncommitted edits are previewed.
     * @return Null when the owner cannot hold content.
     */
    TSharedPtr<FOdysseyRasterBlock> GetRasterBlock();

    /**
     * Builds the function rendering the content: the uncommitted edits when there are some, otherwise the image.
     * @param OutRenderFunction The render function.
     * @param ContentTransform Applied to the content before the transform given to the render function.
     * @return False when there is nothing to render.
     */
    bool BuildRenderFunction(IOdysseyTextureRenderingAbility::FRenderFunction& OutRenderFunction, const FMatrix& ContentTransform);

    /** Returns true while the raster block has edits not yet written into the image. */
    bool HasUncommittedEdits() const;

private:
    void ReleaseRasterBlock();
    void OnBlockChanged(const TArray<::ULIS::FRectI>& Rects);
    void OnBlockCommited(const TArray<::ULIS::FRectI>& Rects);
    void OnImageUndone();
    void UpdatePreview(const TArray<::ULIS::FRectI>& Rects, bool bCommitted);
    bool BuildPreviewRenderFunction(IOdysseyTextureRenderingAbility::FRenderFunction& OutRenderFunction, const FMatrix& ContentTransform) const;
    bool BuildImageRenderFunction(IOdysseyTextureRenderingAbility::FRenderFunction& OutRenderFunction, const FMatrix& ContentTransform, FIntPoint Size) const;

private:
    UObject& Owner;
    TObjectPtr<UOdysseyTiledImage>& Image;
    bool bSRGB;
    FGetDescription GetDescription;
    FOnChanged OnChanged;
    FOdysseyRasterBlock::FPostProcess PostProcess;

    /** The image whose undo notification is bound. */
    TWeakObjectPtr<UOdysseyTiledImage> BoundImage;

    TSharedPtr<FOdysseyRasterBlock> RasterBlock;
    TStrongObjectPtr<UTexture2D> PreviewTexture;
    bool bHasUncommittedEdits = false;
};
