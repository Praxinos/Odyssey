// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyMedia.h"

class FOdysseyRasterContent;

/**
 * The raster content of a layer or a cell, as tools reach it.
 * The content is reached through functions given by its owner, called only when needed.
 */
class ODYSSEYMEDIA_API FOdysseyMediaRaster
    : public IOdysseyMedia
{
public:
    DECLARE_ODYSSEY_MEDIA()

    using FGetContent = TFunction<TSharedPtr<FOdysseyRasterContent>()>;

public:
    //Destructor
    virtual ~FOdysseyMediaRaster();
    FOdysseyMediaRaster();

    /**
     * @param iContentId Identifies the content: the rendering id of its owner, which its edit sessions replace in the views.
     * @param iGetContent Returns the content, to read, write or edit it.
     */
    FOdysseyMediaRaster(const FGuid& iContentId, FGetContent iGetContent);

public:
    //IOdysseyMedia API

    /** Returns the id of the content, the same for every media of the same layer or cell. */
    const FGuid& GetContentId() const;

    /** Returns the content, to read, write or edit it; see FOdysseyRasterContent. Null when the owner was destroyed. */
    TSharedPtr<FOdysseyRasterContent> GetContent() const;

private:
    FGuid mContentId;
    FGetContent mGetContent;
};
