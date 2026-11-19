// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"
#include "Templates/Function.h"
#include "UObject/ObjectPtr.h"

#include "OdysseyColorConversion.h"
#include "OdysseyTextureRenderingAbility.h"

#include <ULIS>

class FOdysseyRasterBlockEdit;
class FOdysseyRasterEditSession;
class UOdysseyTiledImage;
class UTextureRenderTarget2D;

/**
 * Raster content shared by raster texture layers and raster animation cells.
 *
 * The tiled image is the only persisted and undoable storage of the pixels, as linear colors. The owner holds it in an instanced
 * UPROPERTY so that it is saved, duplicated and garbage collected with the owner; this class creates it in that
 * property, keeps it in the format the owner expects, renders it and writes into it.
 *
 * - Tools edit the content through BeginEdit(): the edit session shows their pixels in the views until they commit them
 *   into the tiled image or end the edit.
 * - Other users read and write render targets in the tile format with Read() and Write(). Each write is undoable within
 *   a transaction, keeps the alpha of the content when it is locked, and ends the active edit.
 * - Transitional: users still working on CPU blocks go through ReadBlock(), WriteBlock() and EditBlock(), whose blocks
 *   are in the format the owner expects. They go away once these users work on render targets.
 */
class ODYSSEYIMAGING_API FOdysseyRasterContent
    : public TSharedFromThis<FOdysseyRasterContent>
{
public:
    /**
     * The content the owner expects: its size in pixels and the format of the blocks read and written by ReadBlock() and WriteBlock().
     *
     * The size is the canvas: blocks and edit sessions cover it, and rendering assembles the tiles it samples within it.
     * The tiled image is unbounded: tiles outside the canvas are kept, e.g. when the canvas shrinks, and shown again when it grows.
     *
     * The format is queried on each use rather than given once: EnsureImage() converts the image whenever the owner
     * expects another format, e.g. after the sRGB setting of a texture changed or a cell was duplicated into an animation
     * of another format, without the owner having to report each of these changes.
     */
    struct FDescription
    {
        /** Returns true when the owner can hold content, e.g. it is attached to a texture or an animation. */
        bool IsValid() const { return Size.X > 0 && Size.Y > 0; }

        FIntPoint Size = FIntPoint::ZeroValue;
        ::ULIS::eFormat Format = ::ULIS::Format_RGBA8;

        /**
         * True when 8-bit blocks hold sRGB encoded colors. The image always holds linear colors:
         * they are decoded when written into it and encoded when blocks are read from it.
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
     * @param InOwner The object owning the image property; the outer of the image.
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

    /** Returns the content the owner expects. */
    FDescription Describe() const;

    /**
     * Creates the image when missing, and converts it when the owner expects another format, tiles outside the canvas included.
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
     * Creates a render target holding pixels in the tile format, as large as the content, to read or write the content.
     * @return Null when the owner cannot hold content or the rendering is unavailable.
     */
    UTextureRenderTarget2D* CreateRenderTarget();

    /**
     * Reads an area of the content, as linear colors, into a render target.
     * Use a target created by CreateRenderTarget() to keep every value.
     * @param Destination The render target to read into.
     * @param Rect The area to read, in content pixels.
     * @param Position The position of the top-left corner of Rect in the destination.
     * @return False when the owner cannot hold content or the rendering is unavailable.
     */
    bool Read(UTextureRenderTarget2D& Destination, const FIntRect& Rect, FIntPoint Position);

    /**
     * Writes areas of a render target into the content, undoable within a transaction.
     * The alpha of the content is kept when it is locked. The active edit ends, and the owner is notified.
     * @param Source The pixels to write, at the same positions as in the content.
     * @param Rects The areas to write, in content pixels; clipped to the content bounds.
     * @param Conversion Converts the source values into the linear colors of the content,
     *                   e.g. Decode for a render of the owner blended with encoded colors.
     * @return False when nothing was written.
     */
    bool Write(UTextureRenderTarget2D& Source, const TArray<FIntRect>& Rects, EOdysseyColorConversion Conversion = EOdysseyColorConversion::None);

    /**
     * Transitional: reads the whole content into a new block in the format the owner expects. Waits for the GPU.
     * @return Null when the owner cannot hold content or the rendering is unavailable.
     */
    TSharedPtr<::ULIS::FBlock> ReadBlock();

    /**
     * Transitional: writes areas of a block into the content, as Write() does.
     * The block is first converted to the format the owner expects, its 8-bit colors being sRGB encoded when bSRGB is set.
     * @param Block The pixels to write, at the same positions as in the content.
     * @param Rects The areas to write, in content pixels; clipped to the content and block bounds.
     * @return False when nothing was written.
     */
    bool WriteBlock(const ::ULIS::FBlock& Block, const TArray<FIntRect>& Rects);

    /**
     * Transitional: reads the whole content into a block, lets the caller change it, then writes the whole block back as WriteBlock() does.
     * @param Edit Changes the block, in the format the owner expects. Pending ULIS operations must be finished when it returns.
     * @return False when nothing was written.
     */
    bool EditBlock(TFunctionRef<void(::ULIS::FBlock& Block)> Edit);

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
    void EndEdit();
    void OnEditCommitted(const TArray<FIntRect>& Rects);
    void OnImageUndone();

    /**
     * Writes areas through an edit session, which keeps the alpha when it is locked, then notifies the owner.
     * @param Rects The areas to write; clipped to the content bounds.
     * @param Fill Writes the new pixels of the clipped areas into the session target.
     */
    bool WriteThroughSession(const TArray<FIntRect>& Rects, TFunctionRef<void(FOdysseyRasterEditSession& Session, const TArray<FIntRect>& Rects)> Fill);

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

    /** The edit begun by BeginEdit(), owned by the tool. */
    TWeakPtr<FOdysseyRasterBlockEdit> ActiveEdit;
};
