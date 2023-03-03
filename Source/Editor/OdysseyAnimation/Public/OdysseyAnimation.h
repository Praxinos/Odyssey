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
class ODYSSEYANIMATION_API UOdysseyAnimation
	: public UBaseMediaSource
	, public FTickableEditorObject //Allows us to react to Tick events
{
	GENERATED_BODY()

public:
    /**
     * @brief Delegate called when CurrentFrame changes
     * 
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentFrameChanged, UOdysseyAnimation*)
    static FOnCurrentFrameChanged& OnCurrentFrameChanged();

    /**
     * @brief Delegate called when FramesPerSecond changes
     * 
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnFramesPerSecondChanged, UOdysseyAnimation*)
    static FOnFramesPerSecondChanged& OnFramesPerSecondChanged();

    /**
     * @brief Delegate called when pixels of a frame changed
     * 
     * @param UOdysseyAnimation* Animation
     * @param const TRange<int>& FrameRange
     * @param const TArray<::ULIS::FRectI>& Rects
     * @param bool IsInteractive
     */
    DECLARE_MULTICAST_DELEGATE_FourParams(FOnRenderImageChanged, UOdysseyAnimation*, const TRange<int>&, const TArray<::ULIS::FRectI>&, bool)
    static FOnRenderImageChanged& OnRenderImageChanged(); //Delegate

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
	UOdysseyAnimationLayerStack* GetLayerStack() const;

	FTimespan GetDuration() const;
	TRange<int> GetFrameRange() const;
	TRange<FTimespan> GetFrameTimeRange(int iFrameIndex) const;
	uint32 GetFrameCount() const;

	double GetFramesPerSecond() const;

	int GetFrameIndexAtTime(FTimespan iTime) const;
	FString GetFrameIdAtTime(FTimespan iTime) const;
	FString GetFrameId(int iFrameIndex) const;

	TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> GetBlockAtIndex(int iIndex);
	TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> GetBlockAtTime(FTimespan iTime);
	TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> GetBlockFromId(const FString& iId);

	void WaitForBlockUpdate(const FString& iFrameId);

public:
    /**
     * @brief Preloads the given frame and keeps it preloaded until the handle is destroyed
     */
    TSharedPtr<IOdysseyHandle> Preload(int iFrame);

public:
	//UObject overrides

	/**
	 * @brief Serialize this object
	 *
	 * @param Ar
	 */
	virtual void Serialize(FArchive& Ar) override;
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;
	virtual void PostInitProperties() override;

protected:
	// FTickableEditorObject implementation
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UOdysseyAnimation, STATGROUP_Tickables); }

private:
	void UpdateFrameBlocks();
	void GenerateFrameBlock(const FString& iId);

private:
	//Events
	void OnLayerStackRenderImageChanged(UOdysseyAnimationLayerStack* iLayerStack, const TRange<int>& iRange, const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive);
	

protected:
    //Property changed methods
    virtual void PropertyChanged(const FName& iPropertyName);
	
    virtual void CurrentFrameChanged();
	virtual void FramesPerSecondChanged();

public:
	//CurrentFrame is specific to the user, not to the animation itself
    //So we save it in user's config, instead of the animation
	UPROPERTY(config, DuplicateTransient)
	int CurrentFrame = 0;

	UPROPERTY()
	float FramesPerSecond = 24.0f;

private:
	UPROPERTY()
	uint32 mWidth = -1;

	UPROPERTY()
	uint32 mHeight = -1;

	UPROPERTY()
	uint32 mFormat = ::ULIS::Format_RGBA8;

	UPROPERTY(meta=(LoadBehavior = "LazyOnDemand"))
	TObjectPtr<UOdysseyAnimationLayerStack> mLayerStack;

	UPROPERTY()
	TArray<FString> mFrameIds;

	struct FFrameBlock
	{
		TSharedPtr<FOdysseyRasterBlock> mRasterBlock;
		TArray<::ULIS::FRectI> mInvalidRects;
		TArray<int> mFrameIndexes;
	};

	//Not UPROPERTY because of FOdysseyRasterBlock needing an owner on load (causes crash)
	TMap<FString, FFrameBlock> mFrameBlocks;
};