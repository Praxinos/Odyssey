// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "OdysseyAnimationConfiguration.h"
#include "OdysseyRasterBlock.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "BaseMediaSource.h"
#include <ULIS>

#include "OdysseyAnimation.generated.h"

UCLASS(config=EditorPerProjectUserSettings, PerObjectConfig)
class ODYSSEYANIMATION_API UOdysseyAnimation : public UBaseMediaSource
{
	GENERATED_BODY()

public:
	void Init(const FOdysseyAnimationConfiguration& iConfiguration);

public:

	//~ IMediaOptions interface
	virtual bool GetMediaOption(const FName& Key, bool DefaultValue) const override;
	virtual bool HasMediaOption(const FName& Key) const override;

public:

	//~ UMediaSource interface
	virtual FString GetUrl() const override;
	virtual bool Validate() const override;

public:
	uint32 Width() const;
	uint32 Height() const;
	::ULIS::eFormat Format() const;

	FTimespan GetDuration() const;
	uint32 GetFrameCount() const;
	double GetFramesPerSecond() const;
	uint32 GetFrameIndexAtTime(FTimespan iTime) const;
	TRange<FTimespan> GetFrameTimeRange(uint32 iFrameIndex) const;

	TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> GetBlockAtIndex(uint32 iIndex);

	UOdysseyAnimationLayerStack* GetLayerStack() const;

public:
    /**
     * @brief Preloads the given frame and keeps it preloaded until the handle is destroyed
     */
    TSharedPtr<IOdysseyHandle> Preload(int iFrame);

public:
	//CurrentFrame is specific to the user, not to the animation itself
    //So we save it in user's config, instead of the animation
	UPROPERTY(config, DuplicateTransient)
	int CurrentFrame = 0;

private:
	UPROPERTY()
	uint32 mWidth = -1;

	UPROPERTY()
	uint32 mHeight = -1;

	UPROPERTY()
	uint32 mFormat = ::ULIS::Format_RGBA8;

	UPROPERTY()
	float mFramesPerSecond = 24.0f;

	//Maybe add a length to each block to avoid loading those blocks a lot
	UPROPERTY()
	TArray<TObjectPtr<UOdysseyRasterBlock>> mRasterBlocks;

	UPROPERTY(meta=(LoadBehavior = "LazyOnDemand"))
	TObjectPtr<UOdysseyAnimationLayerStack> mLayerStack;
};