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

FOdysseyMediaRaster::FOdysseyMediaRaster(const FGuid& iContentId, TWeakPtr<FOdysseyRasterContent> iContent)
    : mContentId(iContentId)
    , mContent(MoveTemp(iContent))
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
    return mContent.Pin();
}
