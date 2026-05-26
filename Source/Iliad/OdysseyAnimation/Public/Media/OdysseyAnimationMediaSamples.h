// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "IMediaSamples.h"
#include "Tickable.h"
#include "TickableEditorObject.h"
#include "UObject/StrongObjectPtr.h"

#include "OdysseyRenderingAbility.h"
#include "OdysseyInvalidTileMap.h"

class FOdysseyAnimationMediaPlayer;
class FOdysseyAnimationMediaControls;
class UOdysseyAnimation;
class UTextureRenderTarget2D;

#if WITH_EDITOR
    /*
    * Allows us to react to Tick events at runtime
    *
    * We expplicitely use FTickableEditorObject here because
    * using only FTickableEditorObject and setting IsTickableInEditor()
    * would prevent ticking while dragging sliders or while in a blocking window
    */
    using FOdysseyTickClass = FTickableEditorObject;
#else
    //Allows us to react to Tick events at runtime
    using FOdysseyTickClass = FTickableGameObject;
#endif

class ODYSSEYANIMATION_API FOdysseyAnimationMediaSamples
    : public IMediaSamples
    , public FOdysseyTickClass //Allows us to react to Tick events
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
    void Update(int iFrameIndex, int64 iSequenceIndex, int64 iLoopIndex);
    uint64 GetRenderType() const;
    void SetRenderType(uint64 iRenderType);

protected:
    // FTickableGameObject implementation
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
