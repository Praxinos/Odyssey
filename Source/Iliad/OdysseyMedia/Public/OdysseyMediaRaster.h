// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyMedia.h"

class FOdysseyRasterBlock;
class FOdysseyRasterBlockEdit;

/**
 * The raster content of a layer or a cell, as tools reach it.
 * Its pixels are reached through functions given by the content owner, called only when needed.
 */
class ODYSSEYMEDIA_API FOdysseyMediaRaster
    : public IOdysseyMedia
{
public:
    DECLARE_ODYSSEY_MEDIA()

    using FGetRasterBlock = TFunction<TSharedPtr<FOdysseyRasterBlock>()>;
    using FBeginEdit = TFunction<TSharedPtr<FOdysseyRasterBlockEdit>()>;

public:
    //Destructor
    virtual ~FOdysseyMediaRaster();
    FOdysseyMediaRaster();

    /**
     * @param iContentId Identifies the content: the rendering id of its owner, which its edit sessions replace in the views.
     * @param iGetRasterBlock Returns the raster block of the content, read by tools.
     * @param iBeginEdit Begins an edit of the content, see FOdysseyRasterContent::BeginEdit().
     */
    FOdysseyMediaRaster(const FGuid& iContentId, FGetRasterBlock iGetRasterBlock, FBeginEdit iBeginEdit);

public:
    //IOdysseyMedia API

    /** Returns the id of the content, the same for every media of the same layer or cell. */
    const FGuid& GetContentId() const;

    /** Transitional: returns the raster block of the content, to read its pixels. Edits go through BeginEdit(). */
    TSharedPtr<FOdysseyRasterBlock> GetRasterBlock() const;

    /** Begins an edit of the content, shown by the views until the tool commits or ends it. Null when the content cannot be edited. */
    TSharedPtr<FOdysseyRasterBlockEdit> BeginEdit() const;

private:
    FGuid mContentId;
    FGetRasterBlock mGetRasterBlock;
    FBeginEdit mBeginEdit;
};
