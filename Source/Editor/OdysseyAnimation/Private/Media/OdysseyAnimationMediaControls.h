// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "IMediaControls.h"

class FOdysseyAnimationMediaControls
	: public IMediaControls
{
public:
    FOdysseyAnimationMediaControls();

public:
	void Init(TWeakPtr<class FOdysseyAnimationMediaPlayer> iPlayer);
	void OnOpen();
	void OnClose();

public:

	//~ IMediaControls interface

	virtual bool CanControl(EMediaControl Control) const override;
	virtual FTimespan GetDuration() const override;
	virtual float GetRate() const override;
	virtual EMediaState GetState() const override;
	virtual EMediaStatus GetStatus() const override;
	virtual TRangeSet<float> GetSupportedRates(EMediaRateThinning Thinning) const override;
	virtual bool IsLooping() const override;
	virtual bool Seek(const FTimespan& Time) override;
	virtual bool SetLooping(bool Looping) override;
	virtual bool SetRate(float Rate) override;
	virtual void SetBlockingPlaybackHint(bool bFacadeWillUseBlockingPlayback) override;
	
	//deprecated: but needed to compile
	virtual FTimespan GetTime() const override;

public:
	void SetState(EMediaState iState);

private:
	TWeakPtr<class FOdysseyAnimationMediaPlayer> mPlayer;
	EMediaState mState;
	bool mPlaybackIsBlocking;
	bool mIsLooping;
	float mRate;
};
