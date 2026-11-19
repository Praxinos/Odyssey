// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyMediaRaster.h"

IMPLEMENT_ODYSSEY_MEDIA(FOdysseyMediaRaster)

FOdysseyMediaRaster::~FOdysseyMediaRaster()
{
}

FOdysseyMediaRaster::FOdysseyMediaRaster()
{
}

FOdysseyMediaRaster::FOdysseyMediaRaster(const FGuid& iContentId, FGetContent iGetContent, FBeginEdit iBeginEdit)
    : mContentId(iContentId)
    , mGetContent(MoveTemp(iGetContent))
    , mBeginEdit(MoveTemp(iBeginEdit))
{
}

const FGuid&
FOdysseyMediaRaster::GetContentId() const
{
    return mContentId;
}

TSharedPtr<FOdysseyRasterContent>
FOdysseyMediaRaster::GetContent() const
{
    return mGetContent ? mGetContent() : nullptr;
}

TSharedPtr<FOdysseyRasterBlockEdit>
FOdysseyMediaRaster::BeginEdit() const
{
    return mBeginEdit ? mBeginEdit() : nullptr;
}
