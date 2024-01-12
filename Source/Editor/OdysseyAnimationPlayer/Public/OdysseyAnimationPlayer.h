// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

//#include "OdysseyAnimation.h"
#include "CoreMinimal.h"
#include "TickableEditorObject.h"
#include "ULISInvalidTileMap.h"
#include "Misc/OdysseyHandle.h"
#include "OdysseyImageRenderer.h"
#include "OdysseyImageRenderingAbility.h"

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

	void SetRenderType(IOdysseyImageRenderer::eRenderType iRenderType);
	IOdysseyImageRenderer::eRenderType GetRenderType() const;

	void SetRange(const TOptional<TRange<FTimespan>>& iRange);

	void SetFrameRange(const TOptional<FInt32Range>& iRange);

protected:
	// FTickableEditorObject implementation
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UOdysseyAnimation, STATGROUP_Tickables); }

private:
	void UpdateTexture();
	void CopyBlocksToTexture(const TArray<TSharedPtr<::ULIS::FBlock>>& iBlocks, const TArray<::ULIS::FRectI>& iRects);
	void OnImageRenderingChanged(const FOdysseyImageRenderingChangedEvent& iEvent);

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
	bool IsLooping = true;


private:
	bool mIsBackward = false;
	FTimespan mCurrentTime; 
	TOptional<TRange<FTimespan>> mRange;
	TArray<FGuid>   mImageRenderingComposition;
	FULISInvalidTileMap mInvalidTileMap;

	/**
	 * We keep the renderer in memory to ensure all blocks are loaded and ready to be used instead of being recached
	 */
	TSharedPtr<IOdysseyImageRenderer> mRenderer;
	IOdysseyImageRenderer::eRenderType mRenderType = IOdysseyImageRenderer::eRenderType::Render;

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