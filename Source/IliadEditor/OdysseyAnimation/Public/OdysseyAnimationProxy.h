// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Containers/Queue.h"
#include "HAL/Runnable.h"
#include "OdysseyRenderingAbility.h"
#include "OdysseyInvalidTileMap.h"

#include <ULIS>

class FBlockData;
class UOdysseyAnimationLayer;
class FOdysseyRasterBlock;
class IOdysseyImageRenderer;

class ODYSSEYANIMATION_API FOdysseyAnimationProxy
    : public FRunnable
{
public:
    virtual ~FOdysseyAnimationProxy();
    FOdysseyAnimationProxy(UOdysseyAnimationLayer* iLayer);

public:
    TSharedPtr<::ULIS::FBlock> GetBlock(int iFrameIndex);
    bool IsDone(int iFrameIndex) const;

    void PostLoad();

public:
    virtual bool Init() override;
    virtual uint32 Run() override;
    virtual void Stop() override;

    void Resume();
    void Pause();

private:
    TSharedPtr<FBlockData> GetBlockDataForComposition(const TArray<FGuid>& iComposition);
    void OnRenderingChanged(const FOdysseyRenderingChangedEvent& iEvent);
    void OnImageRenderingPreChanged(const FOdysseyRenderingChangedEvent& iEvent);

private:
    UOdysseyAnimationLayer* mLayer;

    // Thread to run the worker FRunnable on
    FRunnableThread* mThread;

    // Stop this thread? Uses Thread Safe Counter
    FThreadSafeCounter mStopTaskCounter;

    // Stop this thread? Uses Thread Safe Counter
    FThreadSafeCounter mPauseTaskCounter;

    FCriticalSection mPendingBlockMutex;

    TArray<TSharedPtr<FBlockData>> mBlockData;
    TMap<int, TSharedPtr<FBlockData>> mFramesToBlockData;
    TQueue<TSharedPtr<FBlockData>, EQueueMode::Mpsc> mPendingBlockData;
    FInt32Range mLayerRange;
};

class FBlockData
{
public:
    ~FBlockData();
    FBlockData(UOdysseyAnimationLayer* iLayer, const TArray<FGuid>& iComposition);

public:
    TSharedPtr<::ULIS::FBlock> GetBlock();
    const TArray<FGuid>& GetComposition() const;
    TSharedPtr<FOdysseyRasterBlock> GetRasterBlock() const;

    const TSet<int>& GetFrameIndexes();
    void AddFrameIndex(int iFrameIndex);
    void RemoveFrameIndex(int iFrameIndex);

    void PreChange(const FGuid& iId, const TArray<FIntRect>& iInvalidRects);
    bool PostChange(const FGuid& iId);

    bool IsInvalid() const;
    bool IsReadyToRender() const;

public:
    bool Render();

private:
    void Render(TSharedPtr<IOdysseyImageRenderer> iRenderer, TSharedPtr<FOdysseyRasterBlock> iRasterBlock, const TArray<FIntRect>& iInvalidRects);

private:
    UOdysseyAnimationLayer* mLayer;
    TArray<FGuid> mComposition;
    TArray<FGuid> mInvalidIds;
    TSharedPtr<FOdysseyRasterBlock> mRasterBlock;
    FOdysseyInvalidTileMap mInvalidTileMap;
    TSet<int> mFrameIndexes;
    TSharedPtr<IOdysseyImageRenderer> mRenderer;
    bool mIsReadyToRender = false;
    bool mIsInvalid;

    FCriticalSection mEditMutex;
    FCriticalSection mRenderMutex;
};
