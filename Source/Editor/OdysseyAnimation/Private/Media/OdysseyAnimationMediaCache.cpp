// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Media/OdysseyAnimationMediaCache.h"

#include "Media/OdysseyAnimationMediaPlayer.h"
#include "OdysseyAnimation.h"

FOdysseyAnimationMediaCache::FOdysseyAnimationMediaCache()
    : mAnimation(nullptr)
{
}

void
FOdysseyAnimationMediaCache::Init()
{
}

void
FOdysseyAnimationMediaCache::OnOpen(UOdysseyAnimation* iAnimation)
{
    mAnimation = iAnimation;

}

void
FOdysseyAnimationMediaCache::OnClose()
{
    mAnimation = nullptr;
}

bool
FOdysseyAnimationMediaCache::QueryCacheState(EMediaCacheState iState, TRangeSet<FTimespan>& oTimeRanges) const
{
    if (!mAnimation)
        return false;

    //TODO:
    if (iState == EMediaCacheState::Loaded)
    {
        uint32 frameCount = mAnimation->GetFrameCount();
        for (uint32 i = 0; i < frameCount; i++)
        {
            oTimeRanges.Add(mAnimation->GetFrameTimeRange(i));
        }
        return true;
    }

    return false;
}

int32
FOdysseyAnimationMediaCache::GetSampleCount(EMediaCacheState State) const
{
    if (!mAnimation)
        return 0;
    //TODO:
    return 0;
}
