// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationProxy.h"

#include "HAL/RunnableThread.h"
#include "OdysseyRasterBlockMutator.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationImageRenderer.h"
#include "ULISLoaderModule.h"

FOdysseyAnimationProxy::~FOdysseyAnimationProxy()
{
    FOdysseyImageRenderingAbility::OnImageRenderingPreChangedDelegate().RemoveAll(this);
    FOdysseyImageRenderingAbility::OnImageRenderingChangedDelegate().RemoveAll(this);

    delete mThread;
    mThread = nullptr;
}

FOdysseyAnimationProxy::FOdysseyAnimationProxy(UOdysseyAnimation* iAnimation)
    : mAnimation(iAnimation)
    , mStopTaskCounter(0)
    , mBlockData()
    , mFramesToBlockData()
{
    FOdysseyImageRenderingAbility::OnImageRenderingPreChangedDelegate().AddRaw(this, &FOdysseyAnimationProxy::OnImageRenderingPreChanged);
    FOdysseyImageRenderingAbility::OnImageRenderingChangedDelegate().AddRaw(this, &FOdysseyAnimationProxy::OnImageRenderingChanged);
    //this line starts the thread which will execute Init() => Run()
    mThread = FRunnableThread::Create(this, TEXT("FOdysseyAnimationProxy"), 0, TPri_BelowNormal); //windows default = 8mb for thread, could specify more
}

TSharedPtr<FBlockData>
FOdysseyAnimationProxy::GetBlockDataForComposition(const TArray<FGuid>& iComposition)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyAnimationProxy::GetBlockDataForComposition);
    for (TSharedPtr<FBlockData> blockData : mBlockData)
    {
        if (blockData->GetComposition() == iComposition)
            return blockData;
    }

    return nullptr;
}

TSharedPtr<::ULIS::FBlock>
FOdysseyAnimationProxy::GetBlock(int iFrameIndex)
{
    TSharedPtr<FBlockData> blockData = nullptr;
    if (mFramesToBlockData.Contains(iFrameIndex))
    {
        blockData = mFramesToBlockData[iFrameIndex];
    }
    else
    {
        TArray<FGuid> composition = mAnimation->GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Render, iFrameIndex);
        blockData = GetBlockDataForComposition(composition);
        if (!blockData)
            return nullptr;
    }

    TSharedPtr<::ULIS::FBlock> block = blockData->GetBlock(); //To keep the block in memory
    if (!blockData->Render()) //Renders only if needed, so this is safe
        return nullptr;

    return block;
}

bool
FOdysseyAnimationProxy::IsDone(int iFrameIndex) const
{
    if ( !mFramesToBlockData.Contains(iFrameIndex) )
        return false;

    TSharedPtr<FBlockData> blockData = mFramesToBlockData[iFrameIndex];
    return !blockData->IsInvalid();
}

void
FOdysseyAnimationProxy::PostLoad()
{
    mAnimationRange = mAnimation->GetFrameRange();

    TMap<TSharedPtr<FBlockData>, TArray<int>> frameIndexesToAdd;

    if ( !mAnimationRange.GetUpperBound().IsOpen() && !mAnimationRange.GetLowerBound().IsOpen() )
    {
        //Add new indexes
        int startFrame = mAnimationRange.GetLowerBound().IsInclusive() ? mAnimationRange.GetLowerBoundValue() : mAnimationRange.GetLowerBoundValue() + 1;
        int endFrame = mAnimationRange.GetUpperBound().IsInclusive() ? mAnimationRange.GetUpperBoundValue() : mAnimationRange.GetUpperBoundValue() - 1;
        for ( int i = startFrame; i <= endFrame; i++ )
        {
            TArray<FGuid> composition = mAnimation->GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Render, i);
            TSharedPtr<FBlockData> blockData = GetBlockDataForComposition(composition);
            if ( !blockData )
            {
                blockData = MakeShared<FBlockData>(mAnimation, composition);
                mBlockData.Add(blockData);
            }

            if ( !frameIndexesToAdd.Contains(blockData) )
            {
                frameIndexesToAdd.Add(blockData, { i });
            }
            else
            {
                frameIndexesToAdd[blockData].Add(i);
            }
        }
    }

    for (auto& element : frameIndexesToAdd)
    {
        TSharedPtr<FBlockData> blockData = element.Key;
        TArray<int> frameIndexes = element.Value;
        for (int frameIndex : frameIndexes)
        {
            blockData->AddFrameIndex(frameIndex);
            mFramesToBlockData.Add(frameIndex, blockData);
        }

        //Call PreChange with default Guid and PostChange with default Guid
        //to invalidate the blockdata and create its renderer
        //so that it is ready to be enqueued
        blockData->PreChange(FGuid(), {::ULIS::FRectI::FromXYWH(0, 0, mAnimation->GetWidth(), mAnimation->GetHeight())});
        blockData->PostChange(FGuid());
        mPendingBlockData.Enqueue(blockData); //mPendingBlockData is ThreadSafe
    }
}

bool
FOdysseyAnimationProxy::Init()
{
    return true;
}

uint32
FOdysseyAnimationProxy::Run()
{
    FOptionalTaskTagScope Scope(ETaskTag::EParallelGameThread);
    //While not told to stop this thread
    while (mStopTaskCounter.GetValue() == 0)
    {
        //Manage pausing the thread
        if(mPauseTaskCounter.GetValue() != 0)
        {
            FPlatformProcess::Sleep(0.03); //Arbitrary number
            continue;
        }

        TSharedPtr<FBlockData> blockData;
        if (!mPendingBlockData.Dequeue(blockData)) //PendingBlockData is a ThreadSafe queue
        {
            FPlatformProcess::Sleep(0.03); //Arbitrary number
            continue;
        }

        if (!blockData->Render()) //if render failed
        {
            mPendingBlockData.Enqueue(blockData);
        }
    }
    return 0;
}

void
FOdysseyAnimationProxy::Stop()
{
    mStopTaskCounter.Increment();
    mThread->WaitForCompletion();
}

void
FOdysseyAnimationProxy::Resume()
{
    mPauseTaskCounter.Decrement();
}

void
FOdysseyAnimationProxy::Pause()
{
    mPauseTaskCounter.Increment();
}

void
FOdysseyAnimationProxy::OnImageRenderingPreChanged(const FOdysseyImageRenderingChangedEvent& iEvent)
{
    //We use OnImageRenderingPreChanged because the proxy should be invalidated before anyone react to OnImageRenderingChanged
    //Some systems could react to OnImageRenderingChanged to ask the proxy for the block
    //But if you don't use OnImageRenderingPreChanged the proxy block could be in a valid state instead of an invalid state

    if(iEvent.GetType() == FOdysseyImageRenderingChangedEvent::eEventType::kCompositionChange)
        return;

    const FGuid& id = iEvent.GetId();
    bool isValueChange = iEvent.GetType() == FOdysseyImageRenderingChangedEvent::eEventType::kValueChange;
    TArray<::ULIS::FRectI> defaultRects = { ::ULIS::FRectI::FromXYWH(0, 0, mAnimation->GetWidth(), mAnimation->GetHeight())};
    for (TSharedPtr<FBlockData> blockData : mBlockData)
    {
        if (blockData->GetComposition().Contains(id))
            blockData->PreChange(id, isValueChange ? iEvent.GetRects() : defaultRects);
    }
}

void
FOdysseyAnimationProxy::OnImageRenderingChanged(const FOdysseyImageRenderingChangedEvent& iEvent)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyAnimationProxy::OnImageRenderingChanged);
    if (iEvent.IsInteractive())
        return;

    const FGuid& id = iEvent.GetId();
    if(iEvent.GetType() == FOdysseyImageRenderingChangedEvent::eEventType::kCompositionChange)
    {
        FInt32Range range = mAnimation->GetFrameRange();

        TArray<FInt32Range> rangesToRemove;
        if ( !mAnimationRange.GetUpperBound().IsOpen() && !mAnimationRange.GetLowerBound().IsOpen() )
            rangesToRemove = FInt32Range::Difference(mAnimationRange, range);

        TMap<TSharedPtr<FBlockData>, TArray<int>> frameIndexesToRemove;
        TMap<TSharedPtr<FBlockData>, TArray<int>> frameIndexesToAdd;
        TArray<TSharedPtr<FBlockData>> createdBlockData;

        //Remove unused indexes
        for ( const FInt32Range& rangeToRemove : rangesToRemove )
        {
            int startFrame = rangeToRemove.GetLowerBound().IsInclusive() ? rangeToRemove.GetLowerBoundValue() : rangeToRemove.GetLowerBoundValue() + 1;
            int endFrame = rangeToRemove.GetUpperBound().IsInclusive() ? rangeToRemove.GetUpperBoundValue() : rangeToRemove.GetUpperBoundValue() - 1;
            for ( int i = startFrame; i <= endFrame; i++ )
            {
                if ( !frameIndexesToRemove.Contains(mFramesToBlockData[i]) )
                {
                    frameIndexesToRemove.Add(mFramesToBlockData[i], { i });
                }
                else
                {
                    frameIndexesToRemove[mFramesToBlockData[i]].Add(i);
                }
            }
        }

        mAnimationRange = range;

        if ( !mAnimationRange.GetUpperBound().IsOpen() && !mAnimationRange.GetLowerBound().IsOpen() )
        {
            //Add new indexes
            int startFrame = mAnimationRange.GetLowerBound().IsInclusive() ? mAnimationRange.GetLowerBoundValue() : mAnimationRange.GetLowerBoundValue() + 1;
            int endFrame = mAnimationRange.GetUpperBound().IsInclusive() ? mAnimationRange.GetUpperBoundValue() : mAnimationRange.GetUpperBoundValue() - 1;
            for ( int i = startFrame; i <= endFrame; i++ )
            {
                TArray<FGuid> composition = mAnimation->GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Render, i);
                const TSharedPtr<FBlockData>* blockDataPtr = mFramesToBlockData.Find(i);
                TSharedPtr<FBlockData> blockData = blockDataPtr ? *blockDataPtr : nullptr;
                if ( blockData )
                {
                    const TArray<FGuid>& blockDataComposition = blockData->GetComposition();
                    if ( blockDataComposition == composition )
                        continue;

                    if ( !frameIndexesToRemove.Contains(blockData) )
                    {
                        frameIndexesToRemove.Add(blockData, { i });
                    }
                    else
                    {
                        frameIndexesToRemove[blockData].Add(i);
                    }
                }

                blockData = GetBlockDataForComposition(composition);
                if ( !blockData )
                {
                    blockData = MakeShared<FBlockData>(mAnimation, composition);
                    mBlockData.Add(blockData);
                    createdBlockData.Add(blockData);
                }

                if ( !frameIndexesToAdd.Contains(blockData) )
                {
                    frameIndexesToAdd.Add(blockData, { i });
                }
                else
                {
                    frameIndexesToAdd[blockData].Add(i);
                }
            }
        }

        for (auto& element : frameIndexesToRemove)
        {
            TSharedPtr<FBlockData> blockData = element.Key;
            TArray<int> frameIndexes = element.Value;
            for (int frameIndex : frameIndexes)
            {
                blockData->RemoveFrameIndex(frameIndex);
                mFramesToBlockData.Remove(frameIndex);
            }
        }

        for (auto& element : frameIndexesToAdd)
        {
            TSharedPtr<FBlockData> blockData = element.Key;
            TArray<int> frameIndexes = element.Value;
            for (int frameIndex : frameIndexes)
            {
                blockData->AddFrameIndex(frameIndex);
                mFramesToBlockData.Add(frameIndex, blockData);
            }
        }

        //Remove unused blockData
        mBlockData.RemoveAll(
            [](TSharedPtr<FBlockData> iBlockData)
            {
                return iBlockData->GetFrameIndexes().IsEmpty();
            }
        );

        for (TSharedPtr<FBlockData> blockData : createdBlockData)
        {
            blockData->PreChange(FGuid(), {::ULIS::FRectI::FromXYWH(0, 0, mAnimation->GetWidth(), mAnimation->GetHeight())});
            bool shouldEnqueue = blockData->PostChange(FGuid());
            if (shouldEnqueue)
                mPendingBlockData.Enqueue(blockData); //mPendingBlockData is ThreadSafe */
        }

        return;
    }

    for (TSharedPtr<FBlockData> blockData : mBlockData)
    {
        if (blockData->GetComposition().Contains(id))
        {
            bool shouldEnqueue = blockData->PostChange(id);
            if (shouldEnqueue)
                mPendingBlockData.Enqueue(blockData); //mPendingBlockData is ThreadSafe */
        }
    }
}

//=================================================================

FBlockData::~FBlockData()
{
}

FBlockData::FBlockData(UOdysseyAnimation* iAnimation, const TArray<FGuid>& iComposition)
    : mAnimation(iAnimation)
    , mComposition(iComposition)
    , mInvalidTileMap(64, iAnimation->GetWidth(), iAnimation->GetHeight())
    , mFrameIndexes()
    , mIsInvalid(false)
{
    mRasterBlock = MakeShared<FOdysseyRasterBlock>(mAnimation, mAnimation->GetWidth(), mAnimation->GetHeight(), mAnimation->GetFormat());
}

TSharedPtr<::ULIS::FBlock>
FBlockData::GetBlock()
{
    return mRasterBlock->GetBlock();
}

const TArray<FGuid>&
FBlockData::GetComposition() const
{
    return mComposition;
}

TSharedPtr<FOdysseyRasterBlock>
FBlockData::GetRasterBlock() const
{
    return mRasterBlock;
}

void
FBlockData::PreChange(const FGuid& iId, const TArray<::ULIS::FRectI>& iRects)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FBlockData::PreChange);
    //here we say that the iId part of the rendering will be modified
    //
    FScopeLock Lock(&mEditMutex);

    mRenderer = nullptr;
    mIsReadyToRender = false;
    mIsInvalid = true;
    mInvalidTileMap.Invalidate(iRects);
    mInvalidIds.AddUnique(iId); //ensures that if we get multiple chained events to OnImageRenderingChanged, we only really invalidate once the last one has been processed
}

bool
FBlockData::PostChange(const FGuid& iId)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FBlockData::PostChange);
    //here we say that the iId part of the rendering has been modified
    //optionnally specific rectangles to invalidate can be given
    //otherwise the full size of the rendering will be invalidated
    FScopeLock Lock(&mEditMutex);
    mInvalidIds.Remove(iId);

    if (mInvalidIds.IsEmpty() && !mInvalidTileMap.InvalidTiles().IsEmpty())
    {
        TArray<::ULIS::FRectI> rects = { ::ULIS::FRectI::FromXYWH(0, 0, mAnimation->GetWidth(), mAnimation->GetHeight()) };
        mRenderer = MakeShared<FOdysseyAnimationImageRenderer>(mAnimation, mFrameIndexes.Array()[0], IOdysseyImageRenderer::eRenderType::Render, rects );
        mIsReadyToRender = true;
        return true;
    }

    return false;
}

const TSet<int>&
FBlockData::GetFrameIndexes()
{
    return mFrameIndexes;
}

void
FBlockData::AddFrameIndex(int iFrameIndex)
{
    FScopeLock Lock(&mEditMutex);
    mFrameIndexes.Add(iFrameIndex);
}

void
FBlockData::RemoveFrameIndex(int iFrameIndex)
{
    FScopeLock Lock(&mEditMutex);
    mFrameIndexes.Remove(iFrameIndex);
}

bool
FBlockData::IsInvalid() const
{
    return mIsInvalid;
}

void
FBlockData::Render(TSharedPtr<IOdysseyImageRenderer> iRenderer, TSharedPtr<FOdysseyRasterBlock> iRasterBlock, const TArray<::ULIS::FRectI>& iInvalidRects)
{
    FOdysseyRasterBlockMutator rasterBlockMutator(iRasterBlock, false);
    rasterBlockMutator.EditTilesFromRects(
        iInvalidRects,
        [&](TSharedPtr<::ULIS::FBlock> iBlock, const FULISInvalidTileMap& iTileMap)
        {
            FOdysseyImageRendererCopyParams params(iBlock, iTileMap.InvalidRects());
            return iRenderer->Copy(params,  {});
        }
    );
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iRasterBlock->GetFormat());
    ctx.Finish();
    rasterBlockMutator.Commit();
}

bool
FBlockData::Render()
{
    FScopeLock renderLock(&mRenderMutex); //ensures other threads do not enter Render at the same time
    mEditMutex.Lock(); //Lock any other thread from editing values (like mInvalidRects)

    if (mFrameIndexes.IsEmpty())
    {
        mEditMutex.Unlock();
        return true;
    }

    if (IsInvalid() && !mIsReadyToRender) //the block contained by the blockdata is indeed invalid, but the block data is not yet in a state where it can be rendered
    {
        if (!IsInGameThread())
        {
            mEditMutex.Unlock();
            return false;
        }

        //specific case where
        //if we are in the game thread
        //we force rendering even if we are still editing the blockdata
        //this is safe only because we are on the gamethread
        TArray<::ULIS::FRectI> rects = { ::ULIS::FRectI::FromXYWH(0, 0, mAnimation->GetWidth(), mAnimation->GetHeight()) };
        mRenderer = MakeShared<FOdysseyAnimationImageRenderer>(mAnimation, mFrameIndexes.Array()[0], IOdysseyImageRenderer::eRenderType::Render, rects );
        mIsReadyToRender = true;
    }

    if (!IsInvalid()) //the block has already been rendered and is valid
    {
        mEditMutex.Unlock();
        return true;
    }

    while(mIsReadyToRender)
    {
        //Lock any data that could change while accessing
        mRenderer->Lock();

        if (!IsInGameThread() && mRenderer->IsGameThreadOnly())
        {
            mRenderer->Unlock();
            mEditMutex.Unlock();
            return false;
        }

        mRenderer->Init();

        //Get all variables we need to render, to ensure the values we use are not modified during the process
        TSharedPtr<IOdysseyImageRenderer> renderer = mRenderer; //renderer should be created in main thread to avoid crashes
        mRenderer = nullptr;
        TArray<::ULIS::FRectI> invalidRects = mInvalidTileMap.InvalidRects();
        TSharedPtr<FOdysseyRasterBlock> rasterBlock = mRasterBlock;

        //Clear the invalid rects, before rendering so we can detect if new invalid rects are present when we are done
        mIsReadyToRender = false;
        mIsInvalid = false;
        mInvalidTileMap.Clear();
        mEditMutex.Unlock();

        Render(renderer, rasterBlock, invalidRects);

        renderer->Unlock();

        AsyncTask(
            ENamedThreads::GameThread,
            [renderer]()
            {
                TSharedPtr<IOdysseyImageRenderer> r = renderer;
                r.Reset(); //reset the renderer on GameThread to avoid crashes
            }
        );

        mEditMutex.Lock();
    }
    mEditMutex.Unlock();
    return true;
}
