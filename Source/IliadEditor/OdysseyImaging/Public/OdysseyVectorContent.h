// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"
#include "Templates/Function.h"
#include "UObject/GCObject.h"
#include "UObject/ObjectPtr.h"

#include "OdysseyTextureRenderingAbility.h"

#include <ULIS>

class FOdysseyRasterEditSession;
class FOdysseyVectorCell;
class FOdysseyVectorGroupPaint;
class UOdysseyTiledImage;
enum class EOdysseyTiledImageFormat : uint8;
struct FOdysseyVectorContentRasterizer;

/**
 * Vector content shared by vector texture layers and vector animation cells: the render of their vector scene.
 *
 * The scene is the persisted data; its render is a cache, held as linear colors by a tiled image that is never saved.
 * This class creates the image, keeps it up to date with the scene and keeps it from being garbage collected.
 * The image is in the transient package: as its outer, the owner would be kept alive by this reference.
 *
 * - Interactive redraws of the scene are rendered into an edit session, which the views show in place of the tiles.
 * - Other redraws only mark the area to render: the tiles are rendered when the content is displayed, see EnsureRendered().
 * - The scene is rendered by blend2d in 8-bit, by a rasterizer shared by every content of the same size.
 */
class ODYSSEYIMAGING_API FOdysseyVectorContent
    : public FGCObject
{
public:
    /** The content the owner expects. */
    struct FDescription
    {
        /** Returns true when the owner can hold content, e.g. it is attached to a texture or an animation. */
        bool IsValid() const { return Size.X > 0 && Size.Y > 0; }

        bool operator==(const FDescription& Other) const { return Size == Other.Size && bSRGB == Other.bSRGB; }

        /** The canvas: the scene is rendered within it. */
        FIntPoint Size = FIntPoint::ZeroValue;

        /** True when the 8-bit render holds sRGB encoded colors: they are decoded, as the tiles hold linear colors. */
        bool bSRGB = false;
    };

    using FGetDescription = TFunction<FDescription()>;

    /** Returns the flags the scene is drawn with, e.g. FOdysseyVectorEngine::DRAWING_WIREFRAME. */
    using FGetDrawingFlags = TFunction<uint64()>;

    /**
     * Called when the scene changed outside an interactive redraw; interactive redraws are reported by
     * FOdysseyRasterEditSession::OnInvalidated().
     * @param Rects The changed areas, in content pixels.
     */
    using FOnChanged = TFunction<void(const TArray<FIntRect>& Rects)>;

public:
    virtual ~FOdysseyVectorContent() override;

    /**
     * @param InRenderingId The rendering id of the owner, which the edit sessions replace in the views.
     * @param InCell The vector cell holding the scene; its redraw requests are followed.
     * @param InGetDescription Returns the content the owner expects.
     * @param InGetDrawingFlags Returns the flags the scene is drawn with.
     * @param InOnChanged Called when the render changed.
     */
    FOdysseyVectorContent(const FGuid& InRenderingId, TSharedPtr<FOdysseyVectorCell> InCell, FGetDescription InGetDescription, FGetDrawingFlags InGetDrawingFlags, FOnChanged InOnChanged);

    FOdysseyVectorContent(const FOdysseyVectorContent&) = delete;
    FOdysseyVectorContent& operator=(const FOdysseyVectorContent&) = delete;

public:
    /** Returns the vector cell whose scene is rendered. */
    const TSharedPtr<FOdysseyVectorCell>& GetCell() const;

    /** Returns the existing image without creating it. */
    UOdysseyTiledImage* GetTiledImage() const;

    /**
     * Builds the function rendering the tiles, once brought up to date. Uncommitted edits are shown by the views through the edit session.
     * @param OutRenderFunction The render function.
     * @param ContentTransform Applied to the content before the transform given to the render function.
     * @return False when there is nothing to render.
     */
    bool BuildRenderFunction(IOdysseyTextureRenderingAbility::FRenderFunction& OutRenderFunction, const FMatrix& ContentTransform);

public:
    // FGCObject
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
    virtual FString GetReferencerName() const override;

private:
    /**
     * Brings the tiles up to date with the scene: creates the image when missing and renders the areas changed since the last render,
     * or the whole canvas when the description or the drawing flags changed. Never notifies the owner.
     * @return False when the owner cannot hold content or the rendering is unavailable.
     */
    bool EnsureRendered();

    void OnRequestRedraw(FOdysseyVectorGroupPaint* Scene, uint64 RedrawFlags);

    /** Renders an area of the scene into the edit session, begun when needed. */
    void RenderInteractive(const FIntRect& Rect, const FDescription& Description);

    /** Ends the edit session, if any: the areas it showed are rendered into the tiles later. Returns their bounds, if any. */
    TOptional<FIntRect> EndSession();

    /**
     * Renders an area of the scene and passes the render, as an unpremultiplied 8-bit block of the canvas size, to Consume.
     * Only the area of the block is meaningful.
     * @return False when nothing was rendered.
     */
    bool RenderScene(const FIntRect& Rect, FIntPoint Size, uint64 DrawingFlags, TFunctionRef<void(const ::ULIS::FBlock&)> Consume);

    /** Returns the area of the canvas the cell asks to redraw; the whole canvas when it does not tell. */
    FIntRect GetRedrawRect(FIntPoint Size) const;

    void AddPending(const FIntRect& Rect);

private:
    FGuid RenderingId;
    /** The render of the scene; created when first rendered. */
    TObjectPtr<UOdysseyTiledImage> Image;
    TSharedPtr<FOdysseyVectorCell> Cell;
    FGetDescription GetDescription;
    FGetDrawingFlags GetDrawingFlags;
    FOnChanged OnChanged;

    /** The description of the last render; unset when the tiles were never rendered. */
    TOptional<FDescription> RenderedDescription;
    /** The drawing flags of the last render. */
    uint64 RenderedDrawingFlags = 0;
    /** The bounds of the areas to render into the tiles; unset when the tiles are up to date. */
    TOptional<FIntRect> PendingRect;

    /** The edit session showing interactive redraws, while they are not rendered into the tiles. */
    TSharedPtr<FOdysseyRasterEditSession> Session;
    /** The bounds of the areas rendered into the session. */
    TOptional<FIntRect> SessionBounds;

    /** Renders the scene into its image; shared with the contents of the same size. */
    TSharedPtr<FOdysseyVectorContentRasterizer> Rasterizer;
};
