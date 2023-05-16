// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationProxy.h"

FOdysseyAnimationProxy::~FOdysseyAnimationProxy()
{
    IOdysseyAnimationImageRenderingAbility::OnPreChanged().RemoveAll(this);
    IOdysseyAnimationImageRenderingAbility::OnPreCompositionChanged().RemoveAll(this);
    /*
    delete mThread;
    mThread = nullptr;
    */
}

FOdysseyAnimationProxy::FOdysseyAnimationProxy(UOdysseyAnimation* iAnimation)
    : mAnimation(iAnimation)
    //, mStopTaskCounter(0)
{
    IOdysseyAnimationImageRenderingAbility::OnPreChanged().AddRaw(this, &FOdysseyAnimationProxy::OnImageRenderingPreChanged);
    IOdysseyAnimationImageRenderingAbility::OnPreCompositionChanged().AddRaw(this, &FOdysseyAnimationProxy::OnImageRenderingPreCompositionChanged);
    //this line starts the thread which will execute Init() => Run()
    //mThread = FRunnableThread::Create(this, TEXT("FOdysseyAnimationProxy"), 0, TPri_BelowNormal); //windows default = 8mb for thread, could specify more
}

TSharedPtr<::ULIS::FBlock>
FOdysseyAnimationProxy::GetBlock(const TArray<FGuid>& iFrameComposition)
{
    //TODO: use available image if possible
    //TODO: Find the right frame or use iFrameIndex instead of iFrameComposition
    int frameIndex = 0;

    TSharedPtr<IOdysseyAnimationImageRenderingAbility> animationAbility = mAnimation->GetAbility<IOdysseyAnimationImageRenderingAbility>();
    if (!animationAbility)
        return nullptr;

    TSharedPtr<IOdysseyImageRenderer> renderer = MakeShared<FOdysseyAnimationImageRenderer>(mAnimation, frameIndex, false);
    TArray<::ULIS::FEvent> events;
    TSharedPtr<::ULIS::FBlock> block = renderer->RenderInNewBlock(mAnimation->Format(), ::ULIS::FRectI::FromXYWH(0, 0, mAnimation->Width(), mAnimation->Height()), events);
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(block->Format());
    ctx.Finish();
    return block;
}

bool
FOdysseyAnimationProxy::Init()
{
    return true;
}

uint32
FOdysseyAnimationProxy::Run()
{
    /*
    //While not told to stop this thread 
    while (StopTaskCounter.GetValue() == 0)
    {
        //Manage pausing the thread
        if(mPauseTaskCounter.GetValue() != 0)
        {
            FPlatformProcess::Sleep(0.03); //Arbitrary number
            continue;
        }

        //Sleep if there is nothing to work on
        if (mPending.IsEmpty())
        {
            FPlatformProcess::Sleep(0.03); //Arbitrary number
            continue;
        }

        //Get a pending frame to render
        FPendingFrame pendingFrame = mPending.Pop(); //FIFO
        if (IsFrameLocked(pendingFrame))
        {
            if (mPending.IsEmpty())
                FPlatformProcess::Sleep(0.03); //Arbitrary number
            
            mPending.Push(pendingFrame);
            continue;
        }

        //If frame id is already in mIds, then we don't have to render it again
        if (mIds.Contains(pendingFrame.mId))
            continue;

        //=======


        //Check if the frame id is still correct
        //then render
        if (pendingFrame.mId == mIds)

        mAnimation->Render();

        //Rendering the animation, means the hierarchy should not change
        //Ideally, we should generate a rendering pipeline
        // So that it holds the rendering path, blocks, options, etc...

        TSharedPtr<FOdysseyRenderingPipeline> renderingPipeline = imageRenderingAbility->GenerateRenderingPipeline();
        block = renderingPipeline->RenderInNewBlock();
        StoreForFrame(block);
    }
    */
    return 0;
}

void
FOdysseyAnimationProxy::Stop()
{
    /*
    mStopTaskCounter.Increment();
    WaitForCompletion();
    */
}

/*
void
FOdysseyAnimationProxy::Resume()
{
    mPauseTaskCounter.Decrement();
}

void
FOdysseyAnimationProxy::Pause()
{
    mPauseTaskCounter.Increment();
    WaitForPause();
}
*/

void
FOdysseyAnimationProxy::Serialize(FArchive& Ar)
{
    //TODO: Save/Load proxy blocks and Ids
    //How about the undos ?
}

void
FOdysseyAnimationProxy::OnImageRenderingPreChanged(const FGuid& iId, const TArray<::ULIS::FRectI>& iRects)
{
    //Invalidate iRects if iId is contained in the BlockData Map
}

void
FOdysseyAnimationProxy::OnImageRenderingPreCompositionChanged(const FGuid& iId)
{
    //Invalidate full animation rect if iId is contained in the BlockData Map
    //Make a full check of the animation, to find if some frames should be removed, or even added to the BlockData Map
}