// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "IMediaSamples.h"

class FOdysseyAnimationMediaSamples
	: public IMediaSamples
{
public:
    FOdysseyAnimationMediaSamples();

public:
	void Init(TSharedPtr<FOdysseyAnimationMediaPlayer> iPlayer, TSharedPtr<FOdysseyAnimationMediaControls> iControls);
	void OnOpen(UOdysseyAnimation* iAnimation);
	void OnClose();
	
protected:
	//~ IMediaSamples interface
	virtual void FlushSamples() override;

	virtual EFetchBestSampleResult FetchBestVideoSampleForTimeRange(const TRange<FMediaTimeStamp> & TimeRange, TSharedPtr<IMediaTextureSample, ESPMode::ThreadSafe>& OutSample, bool bReverse) override;
	virtual bool PeekVideoSampleTime(FMediaTimeStamp & TimeStamp) override;

private:
	FTimespan FindMaxOverlapingFrame(FTimespan iStartTime, FTimespan iEndTime, uint32* oIndex);
	bool SanitizeTimeRange(TRange<FMediaTimeStamp>* oTimeRange);

private:
	UOdysseyAnimation* mAnimation;
	TWeakPtr<FOdysseyAnimationMediaPlayer> mPlayer;
	TWeakPtr<FOdysseyAnimationMediaControls> mControls;
	TSharedPtr<class FOdysseyAnimationMediaTextureSample> mSample;
};
