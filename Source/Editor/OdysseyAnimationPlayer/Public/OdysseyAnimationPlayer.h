// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

//#include "OdysseyAnimation.h"
#include "CoreMinimal.h"
#include "TickableEditorObject.h"

#include <ULIS>

#include "OdysseyAnimationPlayer.generated.h"

class UOdysseyAnimation;

UENUM()
enum class EOdysseyAnimationPlayerStatus
{
	Playing,
	Paused,
	Stopped
};

UCLASS()
class ODYSSEYANIMATIONPLAYER_API UOdysseyAnimationPlayer
	: public UObject
	, public FTickableEditorObject //Allows us to react to Tick events
{
	GENERATED_BODY()

//TODO: Events
// - Animation Changed
// - Texture Changed
// - FramesPerSecond Changed
// - OnSeek Changed
// - OnPlay Changed
// - OnPause Changed
// - OnStop Changed
public:
	//Events
	FSimpleMulticastDelegate& OnAnimationChanged();
	FSimpleMulticastDelegate& OnTextureChanged();
	FSimpleMulticastDelegate& OnTextureUpdated();
	FSimpleMulticastDelegate& OnFramesPerSecondChanged();
	FSimpleMulticastDelegate& OnIsLoopingChanged();
	FSimpleMulticastDelegate& OnCurrentTimeChanged();
	FSimpleMulticastDelegate& OnPlay();
	FSimpleMulticastDelegate& OnPause();
	FSimpleMulticastDelegate& OnStop();

public:
	void SetAnimation(UOdysseyAnimation* iAnimation);
	UOdysseyAnimation* GetAnimation() const;

	UTexture2D* GetTexture() const;

	void Play(bool iBackward = false);
	void Pause();
	void Stop();

	void SeekToTime(FTimespan iTime);
	void SeekToFrame(int iFrameIndex);

	void SetFramesPerSecond(double iFramesPerSecond);
	double GetFramesPerSecond() const;

	void SetIsLooping(bool iIsLooping);
	bool GetIsLooping() const;

	EOdysseyAnimationPlayerStatus GetStatus() const;

	FTimespan GetCurrentTime() const;
	bool IsBackward() const;

public:
	//Uobject Overrides
	virtual void PostInitProperties() override;

protected:
	// FTickableEditorObject implementation
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UOdysseyAnimation, STATGROUP_Tickables); }

private:
	void UpdateTexture();
	void CopyBlockToTexture(TSharedPtr<::ULIS::FBlock> iBlock, const TArray<::ULIS::FRectI>& iRects);
	void OnAnimationRenderImageDataChanged(UOdysseyAnimation* iAnimation, const FOdysseyAnimationRenderImageId& iFrameId, const TArray<::ULIS::FRectI>& iRects);
	void OnAnimationRenderImageIdChanged(UOdysseyAnimation* iAnimation);

private:
	UPROPERTY()
	UOdysseyAnimation* 	Animation = nullptr;

	UPROPERTY()
	UTexture2D*	Texture = nullptr;

	UPROPERTY()
	EOdysseyAnimationPlayerStatus Status = EOdysseyAnimationPlayerStatus::Stopped;

	UPROPERTY()
	double FramesPerSecond = 0.0f;

	UPROPERTY()
	bool IsLooping = false;


private:
	bool mIsBackward = false;
	FTimespan mCurrentTime; 
	FString   mFrameId;
	TArray<::ULIS::FRectI> mInvalidRects;

	//The current block to display, so that it stays in memory.
	//Makes a huge improvement in performance when drawing, be cause we don't have to retieve it from cache at each tick
	TSharedPtr<::ULIS::FBlock> mBlock; 

private:
	//Events
	FSimpleMulticastDelegate mOnAnimationChanged;
	FSimpleMulticastDelegate mOnTextureChanged;
	FSimpleMulticastDelegate mOnTextureUpdated;
	FSimpleMulticastDelegate mOnFramesPerSecondChanged;
	FSimpleMulticastDelegate mOnIsLoopingChanged;
	FSimpleMulticastDelegate mOnCurrentTimeChanged;
	FSimpleMulticastDelegate mOnPlay;
	FSimpleMulticastDelegate mOnPause;
	FSimpleMulticastDelegate mOnStop;
};