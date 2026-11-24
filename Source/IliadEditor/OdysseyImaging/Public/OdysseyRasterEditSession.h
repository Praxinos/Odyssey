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
 * An edit of the pixels of a content, shown by the views before it is committed into its tiled image: the content of a
 * raster layer or cell, or the interactive render of a vector one (never committed).
 *
 * The session holds a render target in the tile format, initialized from the tiles. Editors write into it and
 * invalidate the changed areas, then commit them into the tiles or end the session to discard them.
 * While a session is active, renders whose context is filled by AddOverrides() show its target in place of the content,
 * in content space: the content transform of the node, e.g. the out of pegs transform of a cell, still applies.
 * Other renders, e.g. thumbnails or exports, show the tiles.
 *
 * A session preserving alpha, e.g. for a layer whose alpha is locked, shows and commits the colors of its target with the alpha
 * of the tiles at its beginning: the views compose both over the area they sample, and the commit composes them once into the target.
 *
 * At most one session is active per content (rendering id); beginning another one ends it. The registry of active sessions
 * and the delegates are game thread only; Begin() and Commit() wait for the render thread.
 * Memory: one canvas-sized render target in the tile format, plus a copy of the original pixels when alpha is preserved.
 */
class ODYSSEYIMAGING_API FOdysseyRasterEditSession
    : public TSharedFromThis<FOdysseyRasterEditSession>
{
public:
    /** Broadcast when a session began or ended, e.g. so that a preview rebuilds its render context. */
    DECLARE_MULTICAST_DELEGATE(FOnActiveSessionsChanged);

    /** Broadcast when the target of a session changed, or when a session ended: the areas must be rendered again. */
    DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInvalidated, const FGuid& /*RenderingId*/, const TArray<FIntRect>& /*Rects*/);

public:
    /** Ends the session, if not ended yet. */
    ~FOdysseyRasterEditSession();

    /**
     * Use Begin().
     * @param InOriginal When preserving alpha, the tiles at the beginning of the session; null otherwise.
     */
    FOdysseyRasterEditSession(const FGuid& InRenderingId, UOdysseyTiledImage& InImage, UTextureRenderTarget2D& InTarget,
        UTextureRenderTarget2D* InOriginal = nullptr);

public:
    /**
     * Begins an edit of a content; ends the active session of the same content, if any.
     *
     * @param RenderingId The rendering id of the edited node.
     * @param Image The tiled image holding the content.
     * @param Size The size of the edited area, from the image origin.
     * @param bPreserveAlpha True to keep the alpha of the tiles, e.g. for a layer whose alpha is locked: only colors are edited.
     * @return The session, or null when Size is empty.
     */
    static TSharedPtr<FOdysseyRasterEditSession> Begin(const FGuid& RenderingId, UOdysseyTiledImage& Image, FIntPoint Size, bool bPreserveAlpha = false);

    /** Returns the active session of a content, if any. */
    static TSharedPtr<FOdysseyRasterEditSession> Find(const FGuid& RenderingId);

    /** Adds to the context the content replacements showing every active session. */
    static void AddOverrides(FOdysseyRenderContext& Context);

    static FOnActiveSessionsChanged& OnActiveSessionsChanged();
    static FOnInvalidated& OnInvalidated();

public:
    const FGuid& GetRenderingId() const;

    /** Returns the pixels editors write into, in the tile format. Null once the session ended. */
    UTextureRenderTarget2D* GetTarget() const;

    /** Returns true when the session keeps the alpha of the tiles. */
    bool PreservesAlpha() const;

    bool IsActive() const;

    /** Tells the views that areas of the target changed. */
    void Invalidate(const TArray<FIntRect>& Rects);

    /**
     * Writes areas of the target into the tiled image, undoable within a transaction, then ends the session.
     * When preserving alpha, the areas of the target are first given the alpha of the original.
     * @return False when the session already ended or its image was destroyed.
     */
    bool Commit(const TArray<FIntRect>& Rects);

    /** Ends the session: its target is discarded and the views show the tiles again. */
    void End();

    /** Called after a commit, with the committed areas, once the session ended. */
    using FOnCommitted = TFunction<void(const TArray<FIntRect>& Rects)>;

    /** Sets the function called after a commit, e.g. by the content that began the session to notify its owner. */
    void SetOnCommitted(FOnCommitted InOnCommitted);

private:
    /** When preserving alpha, gives areas of the target the alpha of the original, keeping their colors. */
    void ApplyOriginalAlpha(const TArray<FIntRect>& Rects) const;

    IOdysseyTextureRenderingAbility::FRenderFunction BuildRenderFunction() const;

private:
    FGuid RenderingId;
    TWeakObjectPtr<UOdysseyTiledImage> Image;
    TStrongObjectPtr<UTextureRenderTarget2D> Target;
    /** When preserving alpha: the tiles at the beginning of the session. */
    TStrongObjectPtr<UTextureRenderTarget2D> Original;
    /** The bounds of the areas invalidated since the beginning, rendered again when the session ends; unset when none. */
    TOptional<FIntRect> InvalidBounds;
    FOnCommitted OnCommitted;
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
