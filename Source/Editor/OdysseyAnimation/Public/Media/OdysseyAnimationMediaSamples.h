// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "IMediaSamples.h"
#include "OdysseyImageRenderer.h"
#include "OdysseyImageRenderingAbility.h"
#include "TickableEditorObject.h"
#include "UObject/StrongObjectPtr.h"
#include "ULISInvalidTileMap.h"

class FOdysseyAnimationMediaPlayer;
class FOdysseyAnimationMediaControls;
class UOdysseyAnimation;

class ODYSSEYANIMATION_API FOdysseyAnimationMediaSamples
    : public IMediaSamples
    , public FTickableEditorObject //Allows us to react to Tick events
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
    void CopyBlockToTexture(TSharedPtr<::ULIS::FBlock> iBlock, const TArray<::ULIS::FRectI>& iRects);
    IOdysseyImageRenderer::eRenderType GetRenderType() const;
    void SetRenderType(IOdysseyImageRenderer::eRenderType iRenderType);

protected:
    // FTickableEditorObject implementation
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(FOdysseyAnimationMediaTextureSample, STATGROUP_Tickables); }

private:
    //Events
    void OnImageRenderingChanged(const FOdysseyImageRenderingChangedEvent& iEvent);

private:
    UOdysseyAnimation* mAnimation;
    TWeakPtr<FOdysseyAnimationMediaPlayer> mPlayer;
    TWeakPtr<FOdysseyAnimationMediaControls> mControls;
    TSharedPtr<class FOdysseyAnimationMediaTextureSample> mSample;
    int mCurrentFrameIndex;
    TArray<FGuid> mImageRenderingComposition;
    TStrongObjectPtr<UTexture2D> mTexture; //PATCH: Needs to be in this class, otherwise gets destriyed on the wrong thread
    FULISInvalidTileMap mInvalidTileMap;
    IOdysseyImageRenderer::eRenderType mRenderType = IOdysseyImageRenderer::eRenderType::Render;
};
