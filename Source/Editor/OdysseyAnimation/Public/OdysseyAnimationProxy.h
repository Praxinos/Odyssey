// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class FBlockData;

class ODYSSEYANIMATION_API FOdysseyAnimationProxy
    : public FRunnable
{
public:
    virtual ~FOdysseyAnimationProxy();
    FOdysseyAnimationProxy(UOdysseyAnimation* iAnimation);

public:
    TSharedPtr<::ULIS::FBlock> GetBlock(int iFrameIndex);
    TSharedPtr<IOdysseyHandle> Preload(int iFrameIndex) const;
    bool IsDone(int iFrameIndex) const;

    void PostLoad();

public:
    virtual bool Init() override;
    virtual uint32 Run() override;
    virtual void Stop() override;

    void Resume();
    void Pause();

	void Serialize(FArchive& Ar);

private:
    TSharedPtr<FBlockData> GetBlockDataForComposition(const TArray<FGuid>& iComposition);
    void OnImageRenderingPreChanged(const FGuid& iId, const TArray<::ULIS::FRectI>& iRects);
    void OnImageRenderingCommited(const FGuid& iId, const TArray<::ULIS::FRectI>& iRects);
    void OnImageRenderingCompositionPreChanged(const FGuid& iId);
    void OnImageRenderingCompositionCommited(const FGuid& iId);

private:
    UOdysseyAnimation* mAnimation;
    
    // Thread to run the worker FRunnable on
    FRunnableThread* mThread;
    
    // Stop this thread? Uses Thread Safe Counter
    FThreadSafeCounter mStopTaskCounter;

    // Stop this thread? Uses Thread Safe Counter
    FThreadSafeCounter mPauseTaskCounter;
    
    FCriticalSection mPendingBlockMutex;

    TArray<TSharedPtr<FBlockData>> mBlockData;
    TMap<int, TSharedPtr<FBlockData>> mFramesToBlockData;
    TQueue<TSharedPtr<FBlockData>> mPendingBlockData;
    FInt32Range mAnimationRange;
};

class FBlockData
{
public:
    FBlockData(UOdysseyAnimation* iAnimation, const TArray<FGuid>& iComposition, TSharedPtr<FOdysseyRasterBlock> iRasterBlock);

public:
    TSharedPtr<::ULIS::FBlock> GetBlock();
    const TArray<FGuid>& GetComposition() const;

    TSharedPtr<FOdysseyRasterBlock> GetRasterBlock() const;

    void AppendInvalidRects(const TArray<::ULIS::FRectI>& iInvalidRects);

    const TSet<int>& GetFrameIndexes();
    void AddFrameIndex(int iFrameIndex);
    void RemoveFrameIndex(int iFrameIndex);

    TSharedPtr<class IOdysseyImageRenderer> BuildRenderer();

    void LockPending(const FGuid& iId);
    bool UnlockPending(const FGuid& iId);

    bool IsInvalid() const;
    bool IsPending() const;

public:
    void Render(bool iForceRender);

private:
    enum class eState
    {
        kValid = 0,
        kInvalid = 1 << 0,
        kPending = 1 << 1
    };

    UOdysseyAnimation* mAnimation;
    TArray<FGuid> mComposition;
    int mState;
    TSharedPtr<FOdysseyRasterBlock> mRasterBlock;
    TSharedPtr<::ULIS::FBlock> mULISBlock; //DEBUG
    FULISInvalidTileMap mInvalidTileMap;
    TSet<int> mFrameIndexes;
    
    FCriticalSection mEditMutex;
    FCriticalSection mRenderMutex;
    TSet<FGuid> mLockPendingIds;
};