// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationProxy.h"
#include "OdysseyRasterBlockMutator.h"

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
    if (!mFramesToBlockData.Contains(iFrameIndex))
        return nullptr;

    TSharedPtr<FBlockData> blockData = mFramesToBlockData[iFrameIndex];
    TSharedPtr<::ULIS::FBlock> block = blockData->GetBlock(); //To keep the block in memory
    blockData->Render(true); //Renders only if needed, so this is safe

    return block;
}

TSharedPtr<IOdysseyHandle>
FOdysseyAnimationProxy::Preload(int iFrameIndex) const
{
    if (!mFramesToBlockData.Contains(iFrameIndex))
        return nullptr;
    
    TSharedPtr<FBlockData> blockData = mFramesToBlockData[iFrameIndex];
    return blockData->GetRasterBlock()->Preload();
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

    TArray<TSharedPtr<FBlockData>> blockDataToEnqueue;
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
                TSharedPtr<FOdysseyRasterBlock> block = MakeShared<FOdysseyRasterBlock>(mAnimation);
                block->SetBlock(MakeShared<::ULIS::FBlock>(mAnimation->Width(), mAnimation->Height(), mAnimation->Format()));
                blockData = MakeShared<FBlockData>(mAnimation, composition, block);
                mBlockData.Add(blockData);
                blockDataToEnqueue.Add(blockData);
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
    }

    for ( TSharedPtr<FBlockData> blockData : blockDataToEnqueue )
    {
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
        blockData->Render(false);
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
FOdysseyAnimationProxy::Serialize(FArchive& Ar)
{
    //TODO: Save/Load proxy blocks and Ids
    //How about the undos ?
}

void
FOdysseyAnimationProxy::OnImageRenderingPreChanged(const FOdysseyImageRenderingChangedEvent& iEvent)
{
    if (iEvent.IsCommit())
        return;

    const FGuid& id = iEvent.GetId();

    if (iEvent.GetType() == FOdysseyImageRenderingChangedEvent::eEventType::kValueChange)
    {
        const TArray<::ULIS::FRectI>& rects = iEvent.GetRects();
        for (TSharedPtr<FBlockData> blockData : mBlockData)
        {
            if (blockData->GetComposition().Contains(id))
            {
                blockData->LockPending(id);
                blockData->AppendInvalidRects(rects);
            }
        }
    }
    else if(iEvent.GetType() == FOdysseyImageRenderingChangedEvent::eEventType::kCompositionChange)
    {
        for (TSharedPtr<FBlockData> blockData : mBlockData)
        {
            if (blockData->GetComposition().Contains(id))
                blockData->LockPending(id);
        }

        FInt32Range range = mAnimation->GetFrameRange();

        TArray<FInt32Range> rangesToRemove;
        if ( !mAnimationRange.GetUpperBound().IsOpen() && !mAnimationRange.GetLowerBound().IsOpen() )
            rangesToRemove = FInt32Range::Difference(mAnimationRange, range);

        TMap<TSharedPtr<FBlockData>, TArray<int>> frameIndexesToRemove;
        TMap<TSharedPtr<FBlockData>, TArray<int>> frameIndexesToAdd;

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
                    TSharedPtr<FOdysseyRasterBlock> block = MakeShared<FOdysseyRasterBlock>(mAnimation);
                    block->SetBlock(MakeShared<::ULIS::FBlock>(mAnimation->Width(), mAnimation->Height(), mAnimation->Format()));
                    blockData = MakeShared<FBlockData>(mAnimation, composition, block);
                    blockData->LockPending(id);
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
    }
}

void
FOdysseyAnimationProxy::OnImageRenderingChanged(const FOdysseyImageRenderingChangedEvent& iEvent)
{
    if (!iEvent.IsCommit())
        return;

    FGuid id = iEvent.GetId();
    
    for (TSharedPtr<FBlockData> blockData : mBlockData)
    {
        if (blockData->GetComposition().Contains(id))
        {
            if (blockData->UnlockPending(id))
                mPendingBlockData.Enqueue(blockData); //mPendingBlockData is ThreadSafe
        }
    }
}

//=================================================================

FBlockData::FBlockData(UOdysseyAnimation* iAnimation, const TArray<FGuid>& iComposition, TSharedPtr<FOdysseyRasterBlock> iRasterBlock)
    : mAnimation(iAnimation)
    , mComposition(iComposition)
    , mState((int)eState::kInvalid | (int)eState::kPending)
    , mRasterBlock(iRasterBlock)
    //, mULISBlock(iRasterBlock->GetBlock())
    , mInvalidTileMap(64, iRasterBlock->GetWidth(), iRasterBlock->GetHeight())
    , mFrameIndexes()
{
    mInvalidTileMap.Invalidate(::ULIS::FRectI::FromXYWH(0, 0, iRasterBlock->GetWidth(), iRasterBlock->GetHeight()));
}

TSharedPtr<::ULIS::FBlock>
FBlockData::GetBlock()
{
    FScopeLock renderLock(&mRenderMutex);
    TSharedPtr<::ULIS::FBlock> block = mRasterBlock->GetBlock();
    return block;
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
FBlockData::LockPending(const FGuid& iId)
{
    FScopeLock Lock(&mEditMutex);
    mLockPendingIds.Add(iId);
    mState = mState & ~((int)eState::kPending);
}

bool
FBlockData::UnlockPending(const FGuid& iId)
{
    FScopeLock Lock(&mEditMutex);

    mLockPendingIds.Remove(iId);
    if ( mLockPendingIds.IsEmpty())
    {
        if (!mInvalidTileMap.InvalidTiles().IsEmpty())
            mState = ((int)eState::kInvalid) | ((int)eState::kPending);
    }

    return mState & ((int)eState::kPending);
}

void
FBlockData::AppendInvalidRects(const TArray<::ULIS::FRectI>& iInvalidRects)
{
    FScopeLock Lock(&mEditMutex);
    mInvalidTileMap.Invalidate(iInvalidRects);
    mState = (int)eState::kInvalid; //not pending anymore, just invalid until SetPending() is called
}

TSharedPtr<IOdysseyImageRenderer>
FBlockData::BuildRenderer()
{
    TArray<::ULIS::FRectI> rects = { ::ULIS::FRectI::FromXYWH(0, 0, mAnimation->Width(), mAnimation->Height()) };
    return MakeShared<FOdysseyAnimationImageRenderer>(mAnimation, mFrameIndexes.Array()[0], IOdysseyImageRenderer::eRenderType::Render, rects );
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
    return mState & ((int)eState::kInvalid);
}

bool
FBlockData::IsPending() const
{
    return mState & ((int)eState::kPending);
}

void
FBlockData::Render(bool iForceRender)
{
    FScopeLock renderLock(&mRenderMutex);
    mEditMutex.Lock(); //Lock any other thread from editing values (like mInvalidRects)

    //If it is already rendered, don't need to render it again
    if (!IsInvalid())
    {
        mEditMutex.Unlock();
        return;
    }

    if (!iForceRender && !IsPending())
    {
        mEditMutex.Unlock();
        return;
    }

    //If not used, it is useless to render it
    if (mFrameIndexes.IsEmpty())
    {
        mEditMutex.Unlock();
        return;
    }

    //Get all variables we need to render, to ensure the values we use are not modified during the process
    TArray<::ULIS::FRectI> invalidRects = mInvalidTileMap.InvalidRects();
    TSharedPtr<IOdysseyImageRenderer> renderer = BuildRenderer();
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = mRasterBlock;

    //Clear the invalid rects, before rendering so we can detect if new invalid rects are present when we are done
    mInvalidTileMap.Clear();
    mEditMutex.Unlock();

    //Render until there is no invalid rects to render anymore
    while (!invalidRects.IsEmpty())
    {   
        FOdysseyRasterBlockMutator rasterBlockMutator(rasterBlock, false);
        rasterBlockMutator.EditTilesFromRects(
            invalidRects,
            FOdysseyRasterBlockMutator::FEditDelegate::CreateLambda(
                [&](const FULISInvalidTileMap& iTileMap)
                {
                    TSharedPtr<::ULIS::FBlock> block = rasterBlock->GetBlock();
                    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(block->Format());
                    TArray<::ULIS::FRectI> rects = iTileMap.InvalidRects();

                    TArray<::ULIS::FEvent> clearEvents;
                    for (const ::ULIS::FRectI& rect : rects)
                    {    
                        ::ULIS::FEvent eventClearBlock = FULISEventBuilder().RetainBlock(block).Build();
                        ctx.Clear(*block, rect, ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 0, nullptr, &eventClearBlock);
                        clearEvents.Add(eventClearBlock);
                    }

                    return renderer->Copy(block, rects, clearEvents);
                }
            )
        );
        rasterBlockMutator.Commit();

        //We are done, check if there is new rectangles to render
        mEditMutex.Lock();

        //No invalid rects to render
        if ( mInvalidTileMap.InvalidTiles().IsEmpty() )
        {
            mState = (int)eState::kValid;
            mEditMutex.Unlock();
            break;
        }

        //new invalid rects to render, get the new renderer and invalid rects
        mInvalidTileMap.Invalidate(invalidRects);
        invalidRects = mInvalidTileMap.InvalidRects();
        mInvalidTileMap.Clear();

        if (!iForceRender && !IsPending())
        {
            mInvalidTileMap.Invalidate(invalidRects);
            mEditMutex.Unlock();
            break;
        }

        renderer = BuildRenderer();
        mEditMutex.Unlock();
    }   
}
