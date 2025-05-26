// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyMediaProvider.h"

FOdysseyMediaProvider::FOdysseyMediaProvider()
    : mIsLocked(false)
{

}

void
FOdysseyMediaProvider::Add(const FGuid& iMediaId, const FCreateMediaDelegate& iCreateMediaDelegate)
{
    FGuid id = iMediaId;
    if (!mMediaFactories.Contains(id))
    {
        mMediaFactories.Add(id, {});
    }
    FMediaFactory mediaFactory;
    mediaFactory.mCreateMediaDelegate = iCreateMediaDelegate;
    mMediaFactories[id].Add(mediaFactory);
}

void
FOdysseyMediaProvider::Add(TSharedPtr<IOdysseyMedia> iMedia)
{
    FGuid id = iMedia->Id();
    if (!mMediaFactories.Contains(id))
    {
        mMediaFactories.Add(id, {});
    }
    FMediaFactory mediaFactory;
    mediaFactory.mMedia = iMedia;
    mMediaFactories[id].Add(mediaFactory);
}

bool
FOdysseyMediaProvider::IsLocked() const
{
    return mIsLocked;
}

void
FOdysseyMediaProvider::IsLocked(bool iIsLocked)
{
    mIsLocked = iIsLocked;
}
