// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyMedia.h"

class FOdysseyRasterContent;

/**
 * A weak handle to the raster content of a layer or a cell, created by the layer or cell for the tools (GetMediaProvider()).
 * It owns nothing: the content stays owned by the layer or cell, and GetContent() returns null once the owner is destroyed.
 * The content id is the rendering id of the owner, the id FOdysseyRasterEditSession::GetRenderingId() returns. Game thread only.
 */
class ODYSSEYMEDIA_API FOdysseyMediaRaster
    : public IOdysseyMedia
{
public:
    DECLARE_ODYSSEY_MEDIA()

public:
    //Destructor
    virtual ~FOdysseyMediaRaster();
    FOdysseyMediaRaster();

    /**
     * @param iContentId Identifies the content: the rendering id of its owner, which its edit sessions replace in the views.
     * @param iContent The content, owned by the layer or cell.
     */
    FOdysseyMediaRaster(const FGuid& iContentId, TWeakPtr<FOdysseyRasterContent> iContent);

public:
    /** Returns the id of the content, the same for every media of the same layer or cell. */
    const FGuid& GetContentId() const;

    /** Returns the content, to read, write or edit it; see FOdysseyRasterContent. Null when the owner was destroyed. */
    TSharedPtr<FOdysseyRasterContent> GetContent() const;

private:
    FGuid mContentId;
    TWeakPtr<FOdysseyRasterContent> mContent;
};
