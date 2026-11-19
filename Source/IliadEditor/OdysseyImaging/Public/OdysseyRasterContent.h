// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectPtr.h"

#include "OdysseyRasterBlock.h"
#include "OdysseyTextureRenderingAbility.h"

#include <ULIS>

class FOdysseyRasterBlockEdit;
class UOdysseyTiledImage;

/**
 * Raster content shared by raster texture layers and raster animation cells.
 *
 * The tiled image is the only persisted and undoable storage of the pixels, as linear colors. The owner holds it in an instanced
 * UPROPERTY so that it is saved, duplicated and garbage collected with the owner; this class creates it in that
 * property, keeps it in the format the owner expects, renders it and writes into it.
 *
 * Tools edit the content through BeginEdit(): the edit session shows their pixels in the views until they commit them
 * into the tiled image or end the edit.
 *
 * Transitional: other users still edit an FOdysseyRasterBlock derived from the tiled image; its committed edits are
 * written into the tiled image. This part goes away once they read and write render targets through this class.
 */
class ODYSSEYIMAGING_API FOdysseyRasterContent
    : public TSharedFromThis<FOdysseyRasterContent>
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

        /**
         * True when the 8-bit raster block holds sRGB encoded colors. The image always holds linear colors:
         * they are decoded when written into it and encoded when the raster block is read from it.
         */
        bool bSRGB = false;

        /**
         * When bSRGB changes, e.g. with the sRGB setting of a texture: true to keep the encoded bytes, so that the
         * displayed colors change; false to keep the colors, so that the bytes change.
         */
        bool bKeepBytesOnEncodingChange = true;
    };

    using FGetDescription = TFunction<FDescription()>;

    /**
     * Called when pixels changed.
     * @param Rects The changed areas, in content pixels; empty when everything changed.
     * @param bInteractive True for uncommitted edits.
     */
    using FOnChanged = TFunction<void(const TArray<FIntRect>& Rects, bool bInteractive)>;

    /** Returns true when edits must keep the alpha of the content, e.g. the alpha of the layer is locked. */
    using FIsAlphaLocked = TFunction<bool()>;

public:
    ~FOdysseyRasterContent();

    /**
     * @param InOwner The object owning the image property; the outer of the image and the owner of the raster block.
     * @param InRenderingId The rendering id of the owner, which the edit sessions replace in the views.
     * @param InImage The owner's instanced property holding the tiled image. It must outlive this object.
     * @param InGetDescription Returns the content the owner expects.
     * @param InOnChanged Called when pixels changed.
     * @param InIsAlphaLocked Returns true when edits must keep the alpha of the content.
     */
    FOdysseyRasterContent(UObject& InOwner, const FGuid& InRenderingId, TObjectPtr<UOdysseyTiledImage>& InImage,
        FGetDescription InGetDescription, FOnChanged InOnChanged, FIsAlphaLocked InIsAlphaLocked);

    FOdysseyRasterContent(const FOdysseyRasterContent&) = delete;
    FOdysseyRasterContent& operator=(const FOdysseyRasterContent&) = delete;

public:
    /** Returns the existing image without creating it. */
    UOdysseyTiledImage* GetImage() const;

    /**
     * Creates the image when missing, and converts it when the owner expects another format.
     * Between 8-bit linear and 8-bit sRGB encoded content, the conversion follows bKeepBytesOnEncodingChange;
     * otherwise it keeps the colors.
     * @return False when the owner cannot hold content.
     */
    bool EnsureImage();

    /**
     * Replaces the whole content with the block, without undo. Used to create or migrate content.
     * @return False when the owner cannot hold content or the rendering is unavailable.
     */
    bool SetContent(const ::ULIS::FBlock& Block);

    /**
     * Begins an edit of the content, shown by the views until it is committed or ended.
     * Ends the active edit of this content, if any. The edit keeps the alpha of the content when it is locked.
     * Its commit is undoable within a transaction, and notifies the owner that pixels changed.
     * @return Null when the owner cannot hold content or the rendering is unavailable.
     */
    TSharedPtr<FOdysseyRasterBlockEdit> BeginEdit();

    /**
     * Transitional: returns the raster block edited by other users than tools, created from the image when needed.
     * Committed edits of this block are written into the image, and end the active edit.
     * @return Null when the owner cannot hold content.
     */
    TSharedPtr<FOdysseyRasterBlock> GetRasterBlock();

    /**
     * Builds the function rendering the image. The uncommitted edits are shown by the views through the edit session.
     * @param OutRenderFunction The render function.
     * @param ContentTransform Applied to the content before the transform given to the render function.
     * @return False when there is nothing to render.
     */
    bool BuildRenderFunction(IOdysseyTextureRenderingAbility::FRenderFunction& OutRenderFunction, const FMatrix& ContentTransform);

    /** Returns true while an edit begun by BeginEdit() is neither committed nor ended. */
    bool HasUncommittedEdits() const;

private:
    void ReleaseRasterBlock();
    void EndEdit();
    void OnEditCommitted(const TArray<FIntRect>& Rects);
    void OnBlockCommited(const TArray<::ULIS::FRectI>& Rects);
    /** Keeps the alpha of the original pixels in the edited tiles of the raster block, when the alpha is locked. */
    TArray<::ULIS::FEvent> PreserveBlockAlpha(const TMap<FIntPoint, TSharedPtr<::ULIS::FBlock>>& OriginalBlocks,
        const FOdysseyInvalidTileMap& InvalidMap, const TArray<::ULIS::FEvent>& WaitList);
    void OnImageUndone();
    bool BuildImageRenderFunction(IOdysseyTextureRenderingAbility::FRenderFunction& OutRenderFunction, const FMatrix& ContentTransform, FIntPoint Size) const;

private:
    UObject& Owner;
    FGuid RenderingId;
    TObjectPtr<UOdysseyTiledImage>& Image;
    FGetDescription GetDescription;
    FOnChanged OnChanged;
    FIsAlphaLocked IsAlphaLocked;

    /** The image whose undo notification is bound. */
    TWeakObjectPtr<UOdysseyTiledImage> BoundImage;

    TSharedPtr<FOdysseyRasterBlock> RasterBlock;
    /** The edit begun by BeginEdit(), owned by the tool. */
    TWeakPtr<FOdysseyRasterBlockEdit> ActiveEdit;
};
