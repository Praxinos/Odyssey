// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyMediaProvider.h"

FOdysseyMediaProvider::FOdysseyMediaProvider()
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
