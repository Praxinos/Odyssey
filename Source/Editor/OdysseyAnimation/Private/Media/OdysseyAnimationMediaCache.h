// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "IMediaCache.h"
#include "IMediaSamples.h"

class FOdysseyAnimationMediaCache
	: public IMediaCache
{
public:
    FOdysseyAnimationMediaCache();

public:
	void Init();
	void OnOpen(UOdysseyAnimation* iAnimation);
	void OnClose();

public:
    //~ IMediaCache interface
    
	/**
	 * Query the time ranges of cached media samples for the specified caching state.
	 *
	 * This method can be used to probe a media player's decoder for which samples are
	 * scheduled for loading, being loaded, or finished loading. This is generally only
	 * supported by those players that expose some kind of internal sample caching or
	 * load/decode scheduling mechanism, and most players may simply ignore this call.
	 *
	 * @param State The sample state we're interested in.
	 * @param OutTimeRanges Will contain the set of matching sample time ranges.
	 * @return true on success, false if not supported.
	 */
	virtual bool QueryCacheState(EMediaCacheState State, TRangeSet<FTimespan>& OutTimeRanges) const override;
	
	/**
	 * Query the number of samples for the specified caching state.
	 *
	 * @param State The sample state we're interested in.
	 * @return Sample count.
	 */
	virtual int32 GetSampleCount(EMediaCacheState State) const override;

private:
	UOdysseyAnimation* mAnimation;
};
