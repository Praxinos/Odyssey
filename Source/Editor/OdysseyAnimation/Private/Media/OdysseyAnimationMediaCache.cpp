// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Media/OdysseyAnimationMediaCache.h"

#include "Media/OdysseyAnimationMediaPlayer.h"

#define LOCTEXT_NAMESPACE "OdysseyAnimationMediaCache"

FOdysseyAnimationMediaCache::FOdysseyAnimationMediaCache()
{
}

void
FOdysseyAnimationMediaCache::Init(TWeakPtr<FOdysseyAnimationMediaPlayer> iPlayer)
{
    mPlayer = iPlayer;
}

void
FOdysseyAnimationMediaCache::OnOpen()
{

}

void
FOdysseyAnimationMediaCache::OnClose()
{

}

bool
FOdysseyAnimationMediaCache::QueryCacheState(EMediaCacheState iState, TRangeSet<FTimespan>& oTimeRanges) const
{
    //TODO:
	TSharedPtr<FOdysseyAnimationMediaPlayer> player = mPlayer.Pin();
	if ( !player )
		return false;

	UOdysseyAnimation* animation = player->GetAnimation();
	if ( !animation )
		return false;

    if (iState == EMediaCacheState::Loaded)
    {
        uint32 frameCount = animation->GetFrameCount();
        for (uint32 i = 0; i < frameCount; i++)
        {
			oTimeRanges.Add(animation->GetFrameTimeRange(i));
        }
        return true;
    }

    return false;
}

int32
FOdysseyAnimationMediaCache::GetSampleCount(EMediaCacheState State) const
{
    //TODO:
    return 0;
}

#undef LOCTEXT_NAMESPACE