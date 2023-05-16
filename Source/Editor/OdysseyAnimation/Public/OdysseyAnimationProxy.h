// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class ODYSSEYANIMATION_API FOdysseyAnimationProxy
    : public FRunnable
{
public:
    virtual ~FOdysseyAnimationProxy();
    FOdysseyAnimationProxy(UOdysseyAnimation* iAnimation);

public:
    TSharedPtr<::ULIS::FBlock> GetBlock(const TArray<FGuid>& iFrameComposition);

public:
    virtual bool Init() override;
    virtual uint32 Run() override;
    virtual void Stop() override;

    /*
    void Resume();
    void Pause();
    */

	void Serialize(FArchive& Ar);

private:
    void OnImageRenderingPreChanged(const FGuid& iId, const TArray<::ULIS::FRectI>& iRects);
    void OnImageRenderingPreCompositionChanged(const FGuid& iId);

private:
    UOdysseyAnimation* mAnimation;

    /* struct FBlockData
    {
        TSharedPtr<FOdysseyRasterBlock> block;
        TArray<::ULIS::FRectI> mInvalidRects;
    };

    TMap<TArray<FGuid>, FBlockData> mBlockData; */

    /*
    struct FBlockToFrames
    {
        TSharedPtr<FOdysseyRasterBlock> mBlock;
        TArray<int> mIndexes;
    };

    TMap<TArray<FGuid>, FBlockToFrame> mBlocks;
    TMap<int, TArray<FGuid>> mIds;
    
    // Thread to run the worker FRunnable on
    FRunnableThread* mThread;
    
    // Stop this thread? Uses Thread Safe Counter
    FThreadSafeCounter mStopTaskCounter;

    // Stop this thread? Uses Thread Safe Counter
    FThreadSafeCounter mPauseTaskCounter; */
};