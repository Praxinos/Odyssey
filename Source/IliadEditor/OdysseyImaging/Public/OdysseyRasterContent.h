// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"
#include "Templates/Function.h"
#include "UObject/ObjectPtr.h"

#include "OdysseyColorConversion.h"
#include "OdysseyTextureRenderingAbility.h"

#include <ULIS>

class FOdysseyRasterEditSession;
class UOdysseyTiledImage;
class UTexture2D;
class UTextureRenderTarget2D;

/**
 * Raster content shared by raster texture layers and raster animation cells.
 *
 * The tiled image is the only persisted and undoable storage of the pixels, as linear colors. The owner holds it in an instanced
 * UPROPERTY so that it is saved, duplicated and garbage collected with the owner; this class creates it in that
 * property, keeps it in the format the owner expects, renders it and writes into it.
 *
 * It is the only access to the pixels of its owner, and sets the rules of every access: the format the owner expects,
 * the locked alpha, a single active edit, undo through the tiled image, and the notification of the owner.
 *
 * - Tools edit the content through BeginEditSession(): they write into the render target of the edit session, which the views show
 *   in place of the content until the tool commits it into the tiled image or ends it.
 * - Other users read and write render targets in the tile format with Read(), Write() and Clear(). Each write is undoable
 *   within a transaction, keeps the alpha of the content when it is locked, and ends the active edit.
 * - Transitional: users still working on CPU blocks go through Odyssey::RasterContentBlocks, built on this API.
 *
 * Glossary of raster editing (see Documentation/Doxygen/manual/Architecture/RasterEditing.md):
 * - content: this class, the only access to the pixels of a raster layer or cell;
 * - tiled image: UOdysseyTiledImage, the storage of the pixels, undoable through Modify();
 * - edit session: FOdysseyRasterEditSession, a render target shown in place of the content until it is committed;
 * - block edit: FOdysseyRasterBlockEdit, the transitional CPU front of an edit session used by the current tools;
 * - preview: FOdysseyRasterEditPreview, which makes the views of a source show the active edit sessions;
 * - media: FOdysseyMediaRaster, the weak handle the layer or cell gives the tools.
 *
 * Threading: game thread only. Write(), the conversions of EnsureImage(), BeginEditSession() and Clear() of a content whose
 * alpha is locked wait for the GPU. Read() only enqueues the render: flush the rendering commands before reading the
 * destination on the CPU.
 */
class ODYSSEYIMAGING_API FOdysseyRasterContent
    : public TSharedFromThis<FOdysseyRasterContent>
{
public:
    /**
     * The content the owner expects: its size in pixels and the format of its CPU blocks, see Odyssey::RasterContentBlocks.
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
        bool bKeepBytesOnSRGBChange = true;
    };

    using FGetDescription = TFunction<FDescription()>;

    /**
     * Called when committed pixels changed; uncommitted edits are reported by FOdysseyRasterEditSession::OnInvalidated().
     * @param Rects The changed areas, in content pixels; the whole canvas when everything changed.
     */
    using FOnChanged = TFunction<void(const TArray<FIntRect>& Rects)>;

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
    UOdysseyTiledImage* GetTiledImage() const;

    /** Returns the content the owner expects. */
    FDescription Describe() const;

    /**
     * Creates the image when missing, and converts it when the owner expects another format, tiles outside the canvas included.
     * Between 8-bit linear and 8-bit sRGB encoded content, the conversion follows bKeepBytesOnSRGBChange;
     * otherwise it keeps the colors.
     * @return False when the owner cannot hold content or the rendering is unavailable.
     */
    bool EnsureImage();

    /**
     * Moves the pixels of an asset saved before the tiled image storage into the content, without undo.
     * The legacy data is reset once moved, and kept when it cannot be, e.g. while the owner is not yet attached.
     * @param LegacyTexture The former render texture of the owner, holding its pixels; read when there are no legacy pixels.
     * @param LegacyPixels The pixels of a former raster block, in the format the owner expects.
     */
    void MigrateLegacy(TObjectPtr<UTexture2D>& LegacyTexture, TSharedPtr<::ULIS::FBlock>& LegacyPixels);

    /**
     * Begins an edit of the content: the tool writes into the render target of the session, initialized with the pixels of the content,
     * which the views show until it is committed or ended. Ends the active edit of this content, if any.
     * The edit keeps the alpha of the content when it is locked. Its commit is undoable within a transaction,
     * and notifies the owner that pixels changed.
     * @return Null when the owner cannot hold content or the rendering is unavailable.
     */
    TSharedPtr<FOdysseyRasterEditSession> BeginEditSession();

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
     * Known cost: the write goes through a canvas-sized edit session, even for a small area
     * (see Documentation/Doxygen/manual/Architecture/KnownLimitations.md).
     * @param Source The pixels to write, at the same positions as in the content.
     * @param Rects The areas to write, in content pixels; clipped to the content bounds.
     * @param Conversion Converts the source values into the linear colors of the content,
     *                   e.g. Decode for a render of the owner blended with encoded colors.
     * @return False when nothing was written.
     */
    bool Write(UTextureRenderTarget2D& Source, const TArray<FIntRect>& Rects, EOdysseyColorConversion Conversion = EOdysseyColorConversion::None);

    /**
     * Makes the whole content transparent, undoable within a transaction: the tiles are removed, those outside the canvas included.
     * When the alpha is locked, the alpha is kept as Write() keeps it. The active edit ends, and the owner is notified.
     * @return False when the owner cannot hold content or the rendering is unavailable.
     */
    bool Clear();

    /**
     * Builds the function rendering the image. The uncommitted edits are shown by the views through the edit session.
     * Rendering has no side effect: the image is neither created nor converted, see EnsureImage().
     * @param OutRenderFunction The render function.
     * @param ContentTransform Applied to the content before the transform given to the render function.
     * @return False when there is nothing to render, e.g. the image does not exist yet.
     */
    bool BuildRenderFunction(IOdysseyTextureRenderingAbility::FRenderFunction& OutRenderFunction, const FMatrix& ContentTransform) const;

private:
    /**
     * Replaces the whole content with the block, without undo. Used to create or migrate content.
     * @return False when the owner cannot hold content or the rendering is unavailable.
     */
    bool SetContent(const ::ULIS::FBlock& Block);

    void EndEdit();
    void OnEditCommitted(const TArray<FIntRect>& Rects);
    void OnImageUndone();

    /** Notifies the owner that pixels changed; empty rects stand for the whole canvas. */
    void NotifyChanged(const TArray<FIntRect>& Rects);

private:
    UObject& Owner;
    FGuid RenderingId;
    TObjectPtr<UOdysseyTiledImage>& Image;
    FGetDescription GetDescription;
    FOnChanged OnChanged;
    FIsAlphaLocked IsAlphaLocked;

    /** The image whose undo notification is bound. */
    TWeakObjectPtr<UOdysseyTiledImage> BoundImage;
};
