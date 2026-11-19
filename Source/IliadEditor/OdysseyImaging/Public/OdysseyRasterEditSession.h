// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"
#include "Engine/TextureRenderTarget2D.h"
#include "RHIFwd.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/WeakObjectPtrTemplates.h"

#include "OdysseyTextureRenderingAbility.h"

class FOdysseyRenderPreview;
class UOdysseyTiledImage;
struct FOdysseyRenderContext;

/**
 * An edit of the content of a raster layer or cell, shown by the views before it is committed into its tiled image.
 *
 * The session holds a render target in the tile format, initialized from the tiles. Editors write into it and
 * invalidate the changed areas, then commit them into the tiles or end the session to discard them.
 * While a session is active, renders whose context is filled by AddOverrides() show its target in place of the content,
 * in content space: the content transform of the node, e.g. the out of pegs transform of a cell, still applies.
 * Other renders, e.g. thumbnails or exports, show the tiles.
 */
class ODYSSEYIMAGING_API FOdysseyRasterEditSession
    : public TSharedFromThis<FOdysseyRasterEditSession>
{
public:
    DECLARE_MULTICAST_DELEGATE(FOnActiveSessionsChanged);

    /** Broadcast when the target of a session changed, or when a session ended: the areas must be rendered again. */
    DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInvalidated, const FGuid& /*RenderingId*/, const TArray<FIntRect>& /*Rects*/);

public:
    /** Ends the session, if not ended yet. */
    ~FOdysseyRasterEditSession();

    /** Use Begin(). */
    FOdysseyRasterEditSession(const FGuid& InRenderingId, UOdysseyTiledImage& InImage, UTextureRenderTarget2D& InTarget);

public:
    /**
     * Begins an edit of a content; ends the active session of the same content, if any.
     *
     * @param RenderingId The rendering id of the edited node.
     * @param Image The tiled image holding the content.
     * @param Size The size of the edited area, from the image origin.
     * @return The session, or null when Size is empty.
     */
    static TSharedPtr<FOdysseyRasterEditSession> Begin(const FGuid& RenderingId, UOdysseyTiledImage& Image, FIntPoint Size);

    /** Adds to the context the content replacements showing every active session. */
    static void AddOverrides(FOdysseyRenderContext& Context);

    static FOnActiveSessionsChanged& OnActiveSessionsChanged();
    static FOnInvalidated& OnInvalidated();

public:
    const FGuid& GetRenderingId() const;

    /** Returns the edited pixels, in the tile format. Null once the session ended. */
    UTextureRenderTarget2D* GetTarget() const;

    bool IsActive() const;

    /** Tells the views that areas of the target changed. */
    void Invalidate(const TArray<FIntRect>& Rects);

    /**
     * Writes areas of the target into the tiled image, undoable within a transaction, then ends the session.
     * @return False when the session already ended or its image was destroyed.
     */
    bool Commit(const TArray<FIntRect>& Rects);

    /** Ends the session: its target is discarded and the views show the tiles again. */
    void End();

private:
    IOdysseyTextureRenderingAbility::FRenderFunction BuildRenderFunction() const;

private:
    FGuid RenderingId;
    TWeakObjectPtr<UOdysseyTiledImage> Image;
    TStrongObjectPtr<UTextureRenderTarget2D> Target;
    /** Keeps the target texture alive for renders already built, even after the session ended. */
    FTextureRHIRef TargetRHI;
    TArray<FIntRect> InvalidRects;
};

/**
 * Keeps a view preview showing every active raster edit session, and forwards their invalidations to it.
 */
class ODYSSEYIMAGING_API FOdysseyRasterEditPreview
{
public:
    ~FOdysseyRasterEditPreview();
    explicit FOdysseyRasterEditPreview(TSharedRef<FOdysseyRenderPreview> InPreview);

    FOdysseyRasterEditPreview(const FOdysseyRasterEditPreview&) = delete;
    FOdysseyRasterEditPreview& operator=(const FOdysseyRasterEditPreview&) = delete;

public:
    const TSharedRef<FOdysseyRenderPreview>& GetPreview() const;

private:
    void UpdateContext();
    void Invalidate(const FGuid& RenderingId, const TArray<FIntRect>& Rects);

private:
    TSharedRef<FOdysseyRenderPreview> Preview;
    FDelegateHandle ActiveSessionsChangedHandle;
    FDelegateHandle InvalidatedHandle;
};
