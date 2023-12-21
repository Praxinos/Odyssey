// IDDN.FR.001.220036.002.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

//---

/**
 * Panel Thumbnail, which keeps a Texture to be displayed in the export panel list.
 */
class FPanelThumbnail
    : public ISlateViewport
    , public TSharedFromThis<FPanelThumbnail>
{
public:

    /** Create and initialize a new instance. */
    FPanelThumbnail();

    /** Virtual destructor. */
    virtual ~FPanelThumbnail();

public:
    /**
     * Ensure that this thumbnail has a render target of the specified size
     */
    void ResizeRenderTarget( const FIntPoint& InSize );

    /**
     * Access the (potentially null) render target to be used for rendering onto this thumbnail
     */
    FSlateTextureRenderTarget2DResource* GetRenderTarget() const;

    /**
     * Get the desired size for this thumbnail on the UI
     */
    FIntPoint GetDesiredSize() const;

public:
    // ISlateViewport interface
    virtual FIntPoint GetSize() const override;
    virtual FSlateShaderResource* GetViewportRenderTargetTexture() const override;
    virtual bool RequiresVsync() const override;

    /** Destroy the texture on the next tick */
    void DestroyTexture();

private:
    /** The desired size of the thumbnail on the actual UI (Not necessarily the same size as the texture) */
    FIntPoint mDesiredSize;

    /** The Texture RHI that holds the thumbnail. */
    FSlateTexture2DRHIRef* mThumbnailTexture { nullptr };
    /** The texture render target used for 3D rendering on to the texture. May be null. */
    FSlateTextureRenderTarget2DResource* mThumbnailRenderTarget { nullptr };
};
