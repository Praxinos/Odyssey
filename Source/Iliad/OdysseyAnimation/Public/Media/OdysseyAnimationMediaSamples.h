// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "IMediaSamples.h"
#include "OdysseyRenderingAbility.h"
#include "Tickable.h"
#include "UObject/StrongObjectPtr.h"
#include "OdysseyInvalidTileMap.h"

class FOdysseyAnimationMediaPlayer;
class FOdysseyAnimationMediaControls;
class UOdysseyAnimation;

class ODYSSEYANIMATION_API FOdysseyAnimationMediaSamples
    : public IMediaSamples
    , public FTickableGameObject //Allows us to react to Tick events
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

    virtual EFetchBestSampleResult FetchBestVideoSampleForTimeRange(const TRange<FMediaTimeStamp>& TimeRange, TSharedPtr<IMediaTextureSample, ESPMode::ThreadSafe>& OutSample, bool bReverse, bool bConsistentResult) override;
    virtual bool PeekVideoSampleTime(FMediaTimeStamp & TimeStamp) override;

private:
    FTimespan FindMaxOverlapingFrame(FTimespan iStartTime, FTimespan iEndTime, int* oIndex);
    bool SanitizeTimeRange(TRange<FMediaTimeStamp>* oTimeRange, bool bReverse);

public:
    void Render();
    void Update(int iFrameIndex, int64 iSequenceIndex);
    uint64 GetRenderType() const;
    void SetRenderType(uint64 iRenderType);

protected:
    // FTickableGameObject implementation
    virtual bool IsTickableInEditor() const override { return true; }
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(FOdysseyAnimationMediaTextureSample, STATGROUP_Tickables); }

private:
    //Events
    void OnRenderingChanged(const FOdysseyRenderingChangedEvent& iEvent);
    int GetFrameIndexAtTime(FTimespan iTime) const;

private:
    UOdysseyAnimation* mAnimation;
    TWeakPtr<FOdysseyAnimationMediaPlayer> mPlayer;
    TWeakPtr<FOdysseyAnimationMediaControls> mControls;
    TSharedPtr<class FOdysseyAnimationMediaTextureSample> mSample;
    int mCurrentFrameIndex;
    TArray<FGuid> mImageRenderingComposition;
    TStrongObjectPtr<UTextureRenderTarget2D> mRenderTarget; //PATCH: Needs to be in this class, otherwise gets destriyed on the wrong thread
    FOdysseyInvalidTileMap mInvalidTileMap;
    uint64 mRenderType = EOdysseyRenderingType::Render;
};
