// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "OdysseyAnimationConfiguration.h"
#include "BaseMediaSource.h"
#include <ULIS>

#include "OdysseyAnimation.generated.h"

UCLASS()
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
	//~ UObject overrides
	//virtual void Serialize(FArchive& Ar) override;

public:
	uint32 Width() const;
	uint32 Height() const;
	::ULIS::eFormat Format() const;

	FTimespan GetDuration() const;
	uint32 GetFrameCount() const;
	double GetFramesPerSecond() const;
	uint32 GetFrameIndexAtTime(FTimespan iTime) const;
	TRange<FTimespan> GetFrameTimeRange(uint32 iFrameIndex) const;

	//Will certainly be removed 
	::ULIS::FBlock* GetBlockAtIndex(uint32 iIndex);

private:
	UPROPERTY()
	uint32 mWidth = -1;

	UPROPERTY()
	uint32 mHeight = -1;

	UPROPERTY()
	uint32 mFormat = ::ULIS::Format_RGBA8;

	UPROPERTY()
	float mFramesPerSecond = 24.0f;
	
	//Manages saving + caching of heavy parts of the animation asset (pixel blocks)
	//TMap<FGuid, UE::Serialization::FEditorBulkData> mBulkDatas;

	TArray<::ULIS::FBlock*> mBlocks;
};