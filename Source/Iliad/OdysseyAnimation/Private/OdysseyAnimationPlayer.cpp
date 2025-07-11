// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationPlayer.h"
#include "OdysseyAnimation.h"
#include "RenderCommandFence.h"
#include "RHITypes.h"
#include "Engine/Texture2D.h"
#include "TextureCompiler.h"
#include "Misc/TransactionObjectEvent.h"
#include "Engine/TextureRenderTarget2D.h"

FSimpleMulticastDelegate&
UOdysseyAnimationPlayer::OnCursorFrameChanged()
{
    return mOnCursorFrameChanged;
}

FSimpleMulticastDelegate&
UOdysseyAnimationPlayer::OnCurrentFrameChanged()
{
    return mOnCurrentFrameChanged;
}

FSimpleMulticastDelegate&
UOdysseyAnimationPlayer::OnDisplayedFrameChanged()
{
    return mOnDisplayedFrameChanged;
}

UOdysseyAnimationPlayer::UOdysseyAnimationPlayer()
{
}

void
UOdysseyAnimationPlayer::PostInitProperties()
{
    Super::PostInitProperties();

    if (HasAnyFlags(RF_ClassDefaultObject))
        return;

    RenderTarget = NewObject<UTextureRenderTarget2D>(this, NAME_None, RF_Public | RF_Transient);
    RenderTarget->RenderTargetFormat = RTF_RGBA16f;
    RenderTarget->UpdateResource();
}

void
UOdysseyAnimationPlayer::Play(bool iBackward)
{
    if (Status == EOdysseyAnimationPlayerStatus::Playing && mIsBackward == iBackward)
        return;

    if (Status == EOdysseyAnimationPlayerStatus::Scrubbing)
        EndScrub();

    mIsBackward = iBackward;
    Status = EOdysseyAnimationPlayerStatus::Playing;

#if WITH_EDITOR
    if (mRange.IsSet())
        SeekToFrame(FMath::Clamp(mCursorFrame, mRange->GetLowerBoundValue(), mRange->GetUpperBoundValue()));
#endif
}

void
UOdysseyAnimationPlayer::Pause()
{
    if (Status == EOdysseyAnimationPlayerStatus::Paused)
        return;

    if (Status == EOdysseyAnimationPlayerStatus::Scrubbing)
        EndScrub();

    Status = EOdysseyAnimationPlayerStatus::Paused;
}

void
UOdysseyAnimationPlayer::Stop()
{
    if (Status == EOdysseyAnimationPlayerStatus::Stopped)
        return;

    if (Status == EOdysseyAnimationPlayerStatus::Scrubbing)
    {
        EndScrub();
        return;
    }

    Status = EOdysseyAnimationPlayerStatus::Stopped;
    SeekToFrame(mCurrentFrame);
}

void
UOdysseyAnimationPlayer::SeekToFrame(FFrameTime iFrame)
{
    bool cursorFrameChanged = false;
    bool displayedFrameChanged = false;
    bool currentFrameChanged = false;

    if (mCursorFrame != iFrame)
    {
        mCursorFrame = iFrame;
        cursorFrameChanged = true;
    }

    FFrameTime displayedFrame = GetFrameInAnimationBounds(mCursorFrame);
    if (mDisplayedFrame != displayedFrame)
    {
        mDisplayedFrame = displayedFrame;
        displayedFrameChanged = true;
    }

    if (Status == EOdysseyAnimationPlayerStatus::Stopped && mCurrentFrame != displayedFrame)
    {
        mCurrentFrame = displayedFrame;
        currentFrameChanged = true;
    }

    if (cursorFrameChanged)
        mOnCursorFrameChanged.Broadcast();

    if (displayedFrameChanged)
        mOnDisplayedFrameChanged.Broadcast();

    if (currentFrameChanged)
        mOnCurrentFrameChanged.Broadcast();
}

void
UOdysseyAnimationPlayer::SeekToFrameImmediate(FFrameTime iFrame)
{
    bool cursorFrameChanged = false;
    bool displayedFrameChanged = false;
    bool currentFrameChanged = false;

    if (mCursorFrame != iFrame)
    {
        mCursorFrame = iFrame;
        cursorFrameChanged = true;
    }

    FFrameTime displayedFrame = GetFrameInAnimationBounds(mCursorFrame);
    if (mDisplayedFrame != displayedFrame)
    {
        mDisplayedFrame = displayedFrame;
        displayedFrameChanged = true;
        UpdateTexture();
    }

    if (Status == EOdysseyAnimationPlayerStatus::Stopped && mCurrentFrame != displayedFrame)
    {
        mCurrentFrame = displayedFrame;
        currentFrameChanged = true;
    }

    if (cursorFrameChanged)
        mOnCursorFrameChanged.Broadcast();

    if (displayedFrameChanged)
        mOnDisplayedFrameChanged.Broadcast();

    if (currentFrameChanged)
        mOnCurrentFrameChanged.Broadcast();
}

UTextureRenderTarget2D*
UOdysseyAnimationPlayer::GetRenderTarget()
{
    return RenderTarget;
}

FFrameTime
UOdysseyAnimationPlayer::GetCursorFrame() const
{
    return mCursorFrame;
}

FFrameTime
UOdysseyAnimationPlayer::GetCurrentFrame() const
{
    return mCurrentFrame;
}

FFrameTime
UOdysseyAnimationPlayer::GetDisplayedFrame() const
{
    return mDisplayedFrame;
}

bool
UOdysseyAnimationPlayer::GetDuration(FFrameTime& oFrame) const
{
    if (!Animation)
        return false;

    int frameDuration = Animation->GetFrameCount();
    oFrame = FFrameTime(frameDuration);
    return true;
}

EOdysseyAnimationPlayerStatus
UOdysseyAnimationPlayer::GetStatus() const
{
    return Status;
}

FFrameTime
UOdysseyAnimationPlayer::GetFrameInAnimationBounds(FFrameTime iFrame) const
{
    if (!Animation)
        return iFrame;

#if WITH_EDITOR
    if (mIgnoreAnimationBounds)
        return iFrame;
#endif

    FInt32Range frameRange = Animation->GetFrameRange();
    FFrameTime leftBound = FFrameTime(frameRange.GetLowerBoundValue());
    FFrameTime rightBound = FFrameTime(frameRange.GetUpperBoundValue()) + FFrameTime(1);

    if(iFrame < leftBound)
    {
        return ApplyPreBehaviour(iFrame);
    }
    else if (iFrame >= rightBound)
    {
        return ApplyPostBehaviour(iFrame);
    }

    return iFrame;
}

void
UOdysseyAnimationPlayer::SetPreBehaviour(EOdysseyAnimationPlayerPostBehaviour iValue)
{
    PreBehaviour = iValue;
}

void
UOdysseyAnimationPlayer::SetPostBehaviour(EOdysseyAnimationPlayerPostBehaviour iValue)
{
    PostBehaviour = iValue;
}

FFrameTime
UOdysseyAnimationPlayer::ApplyPreBehaviour(FFrameTime iFrame) const
{
    if (!Animation)
        return iFrame;

#if WITH_EDITOR
    if (!mUsePreBehaviour)
        return iFrame;
#endif

    switch(PreBehaviour)
    {
        case EOdysseyAnimationPlayerPostBehaviour::Hold:
        {
            return FFrameTime(Animation->GetFrameRange().GetLowerBoundValue());
        }
        break;

        case EOdysseyAnimationPlayerPostBehaviour::Loop:
        {
            FFrameTime duration;
            if (!GetDuration(duration))
                return iFrame;

            if ( duration <= 0 )
                return iFrame;

            FInt32Range frameRange = Animation->GetFrameRange();
            FFrameTime leftBound = FFrameTime(frameRange.GetLowerBoundValue());
            FFrameTime rightBound = FFrameTime(frameRange.GetUpperBoundValue());
            /*FFrameTime positiveFrame = (iFrame - leftBound + 1) * -1;
            oFrame = rightBound - (positiveFrame % duration) + leftBound; */

            return rightBound - ((leftBound - iFrame) % duration);
        }
        break;
    }
    return iFrame;
}

FFrameTime
UOdysseyAnimationPlayer::ApplyPostBehaviour(FFrameTime iFrame) const
{
    if ( !Animation )
        return iFrame;

#if WITH_EDITOR
    if (!mUsePostBehaviour)
        return iFrame;
#endif

    switch(PostBehaviour)
    {
        case EOdysseyAnimationPlayerPostBehaviour::Hold:
        {
            return FFrameTime(Animation->GetFrameRange().GetUpperBoundValue());
        }
        break;

        case EOdysseyAnimationPlayerPostBehaviour::Loop:
        {
            FFrameTime duration;
            if (!GetDuration(duration))
                return iFrame;

            if ( duration <= 0 )
                return iFrame;

            FFrameTime leftBound = FFrameTime(Animation->GetFrameRange().GetLowerBoundValue());

            return (iFrame - leftBound) % duration + leftBound;
        }
        break;
    }

    return iFrame;
}

bool
UOdysseyAnimationPlayer::IsBackward() const
{
    return mIsBackward;
}

void
UOdysseyAnimationPlayer::SetAnimation(UOdysseyAnimation* iAnimation)
{
    if (iAnimation == Animation)
        return;

    Animation = iAnimation;
    AnimationChanged();
}

UOdysseyAnimation*
UOdysseyAnimationPlayer::GetAnimation() const
{
    return Animation;
}

void
UOdysseyAnimationPlayer::Tick(float iDeltaTime)
{
    if (GetFlags() & RF_ClassDefaultObject)
        return;

    if (!Animation || !RenderTarget)
        return;

    if (Status == EOdysseyAnimationPlayerStatus::Playing)
    {
        FFrameTime duration;
        if (!GetDuration(duration))
            return;

        //FFrameTime leftBound = mIgnoreAnimationBounds ? FFrameTime(Animation->GetFrameRange().GetLowerBoundValue()) : FFrameTime(Animation->GetLeftBoundValue());
        //FFrameTime rightBound = mIgnoreAnimationBounds ? FFrameTime(Animation->GetFrameRange().GetUpperBoundValue()) : FFrameTime(Animation->GetRightBoundValue()) + FFrameTime(1);

        FFrameTime leftBound = FFrameTime(Animation->GetFrameRange().GetLowerBoundValue());
        FFrameTime rightBound = FFrameTime(Animation->GetFrameRange().GetUpperBoundValue());

        bool bStop = false;
        FFrameTime newFrame = mDisplayedFrame;
        if ( mIsBackward )
        {
            newFrame -= FFrameTime::FromDecimal(iDeltaTime * PlayRate * Animation->GetFramesPerSecond());
#if WITH_EDITOR
            if (newFrame < leftBound)
            {
                if ( mIsLooping )
                {
                    while ( newFrame < leftBound )
                    {
                        newFrame += duration;
                    }
                }
                else
                if (!mUsePreBehaviour)
                {
                    newFrame = leftBound;
                    bStop = true;
                }
            }
#endif
        }
        else
        {
            newFrame += FFrameTime::FromDecimal(iDeltaTime * PlayRate * Animation->GetFramesPerSecond());
#if WITH_EDITOR
            if (newFrame >= rightBound + FFrameTime(1))
            {

                if ( mIsLooping )
                {
                    while ( newFrame >= rightBound + FFrameTime(1) )
                    {
                        newFrame -= duration;
                    }
                }
                else
                if (!mUsePostBehaviour)
                {
                    newFrame = rightBound;
                    bStop = true;
                }
            }
#endif
        }

        SeekToFrame(newFrame);
        if (bStop)
            Stop();
    }

    //If the frame has changed, render the whole frame
    UpdateTexture();
}

void
UOdysseyAnimationPlayer::UpdateTexture()
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UOdysseyAnimationPlayer::UpdateTexture);

    if (!Animation)
        return;

    FFrameTime frame = mDisplayedFrame;
    uint64 renderType = mRenderType.Get();
    TArray<FGuid> imageRenderingComposition = Animation->GetRenderingComposition(renderType, frame.GetFrame().Value);
    if ( imageRenderingComposition != mImageRenderingComposition )
    {
        //PATCH:
        if ( mPatchDelayFirstRender < 1 )
        {
            mPatchDelayFirstRender++;
            return;
        }

        mImageRenderingComposition = imageRenderingComposition;

        RenderTarget->WaitForPendingInitOrStreaming();
        Animation->Render_GameThread(RenderTarget, frame.GetFrame(), renderType );

        mInvalidTileMap.Clear();
        return;
    }

    if (!mInvalidTileMap.InvalidTiles().IsEmpty())
    {
        RenderTarget->WaitForPendingInitOrStreaming();

        TArray<FIntRect> invalidTiles = mInvalidTileMap.InvalidRects();
        for (const FIntRect& rect : invalidTiles)
        {
            Animation->Render_GameThread(RenderTarget, frame.GetFrame(), renderType, rect);
        }

        mInvalidTileMap.Clear();
    }
}

void
UOdysseyAnimationPlayer::OnRenderingChanged(const FOdysseyRenderingChangedEvent& iEvent)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UOdysseyAnimationPlayer::OnRenderingChanged);
    if ( !Animation )
        return;

    if (iEvent.GetType() == FOdysseyRenderingChangedEvent::eEventType::kValueChange)
    {
        if (mImageRenderingComposition.Contains(iEvent.GetId()))
        {
            mInvalidTileMap.Invalidate(iEvent.GetRects());
        }
        return;
    }

    if (iEvent.GetType() == FOdysseyRenderingChangedEvent::eEventType::kCompositionChange)
    {
        if ( !mImageRenderingComposition.Contains(iEvent.GetId()) )
            return;

        FFrameTime frame = mDisplayedFrame;
        TArray<FGuid> imageRenderingComposition = Animation->GetRenderingComposition(mRenderType.Get(), frame.GetFrame().Value);
        if ( imageRenderingComposition == mImageRenderingComposition )
            return;

        mInvalidTileMap.Invalidate(FIntRect(0, 0, Animation->GetWidth(), Animation->GetHeight()));
    }
}

void
UOdysseyAnimationPlayer::AnimationChanged()
{
    IOdysseyRenderingAbility::OnRenderingChangedDelegate().RemoveAll(this);
    if (!Animation)
    {
        RenderTarget->ResizeTarget(1, 1);
        return;
    }

    RenderTarget->ResizeTarget(Animation->GetWidth(), Animation->GetHeight());
    RenderTarget->UpdateResourceImmediate();

    mInvalidTileMap = FOdysseyInvalidTileMap(64, Animation->GetWidth(), Animation->GetHeight());

    IOdysseyRenderingAbility::OnRenderingChangedDelegate().AddUObject(this, &UOdysseyAnimationPlayer::OnRenderingChanged);
}

void
UOdysseyAnimationPlayer::BeginScrub()
{
    if ( Status == EOdysseyAnimationPlayerStatus::Scrubbing )
        return;

    if ( Status == EOdysseyAnimationPlayerStatus::Playing || Status == EOdysseyAnimationPlayerStatus::Paused )
        Stop();

    Status = EOdysseyAnimationPlayerStatus::Scrubbing;
}

void
UOdysseyAnimationPlayer::EndScrub()
{
    if ( Status != EOdysseyAnimationPlayerStatus::Scrubbing )
        return;

    Status = EOdysseyAnimationPlayerStatus::Stopped;
    SeekToFrame(mCursorFrame);
}

#if WITH_EDITOR

void
UOdysseyAnimationPlayer::SetFrameRange(const TOptional<TRange<FFrameTime>>& iRange)
{
    mRange = iRange;
}

void
UOdysseyAnimationPlayer::SetUsePreBehaviour(bool iValue)
{
    mUsePreBehaviour = iValue;
}

void
UOdysseyAnimationPlayer::SetUsePostBehaviour(bool iValue)
{
    mUsePostBehaviour = iValue;
}

bool
UOdysseyAnimationPlayer::UsePreBehaviour() const
{
    return mUsePreBehaviour;
}

bool
UOdysseyAnimationPlayer::UsePostBehaviour() const
{
    return mUsePreBehaviour;
}

bool
UOdysseyAnimationPlayer::IsLooping() const
{
    return mIsLooping;
}

void
UOdysseyAnimationPlayer::SetIsLooping(bool iIsLooping)
{
    mIsLooping = iIsLooping;
}

void
UOdysseyAnimationPlayer::SetRenderType(TAttribute<uint64> iRenderType)
{
    mRenderType = iRenderType;
}

uint64
UOdysseyAnimationPlayer::GetRenderType() const
{
    return mRenderType.Get();
}

void
UOdysseyAnimationPlayer::SetIgnoreAnimationBounds(bool iValue)
{
    mIgnoreAnimationBounds = iValue;
}

bool
UOdysseyAnimationPlayer::GetIgnoreAnimationBounds() const
{
    return mIgnoreAnimationBounds;
}

void
UOdysseyAnimationPlayer::PropertyChanged(const FName& iPropertyName)
{
    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationPlayer, Animation) )
        AnimationChanged();
}

void
UOdysseyAnimationPlayer::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (GetFlags() & RF_ClassDefaultObject)
        return;

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    PropertyChanged(PropertyChangedEvent.GetMemberPropertyName());
}

void
UOdysseyAnimationPlayer::PostTransacted(const FTransactionObjectEvent& iTransactionEvent)
{
    Super::PostTransacted(iTransactionEvent);

    if ( iTransactionEvent.GetEventType() != ETransactionObjectEventType::UndoRedo )
        return;

    const TArray<FName>& changedPropertyNames = iTransactionEvent.GetChangedProperties();
    for ( const FName& propertyName : changedPropertyNames )
    {
        PropertyChanged(propertyName);
    }
}
#endif

void
UOdysseyAnimationPlayer::PostLoad()
{
    Super::PostLoad();

    if (GetFlags() & RF_ClassDefaultObject)
        return;

    if (!RenderTarget)
    {
        RenderTarget = NewObject<UTextureRenderTarget2D>(this, NAME_None, RF_Public | RF_Transient);
        RenderTarget->RenderTargetFormat = RTF_RGBA16f;
    }

    //will create the texture if needed
    IOdysseyRenderingAbility::OnRenderingChangedDelegate().RemoveAll(this);
    if (!Animation)
        return;

    RenderTarget->ResizeTarget(Animation->GetWidth(), Animation->GetHeight());
    RenderTarget->UpdateResource();
    RenderTarget->UpdateResourceImmediate();

    mInvalidTileMap = FOdysseyInvalidTileMap(64, Animation->GetWidth(), Animation->GetHeight());

    IOdysseyRenderingAbility::OnRenderingChangedDelegate().AddUObject(this, &UOdysseyAnimationPlayer::OnRenderingChanged);
}

void
UOdysseyAnimationPlayer::PostDuplicate(EDuplicateMode::Type iDuplicateMode)
{
    Super::PostDuplicate(iDuplicateMode);

    if (GetFlags() & RF_ClassDefaultObject)
        return;

    if ( !RenderTarget )
    {
        RenderTarget = NewObject<UTextureRenderTarget2D>(this, NAME_None, RF_Public | RF_Transient);
        RenderTarget->RenderTargetFormat = RTF_RGBA16f;
    }

    //will create the texture if needed
    IOdysseyRenderingAbility::OnRenderingChangedDelegate().RemoveAll(this);
    if ( !Animation )
        return;

    RenderTarget->ResizeTarget(Animation->GetWidth(), Animation->GetHeight());
    RenderTarget->UpdateResource();
    RenderTarget->UpdateResourceImmediate();

    mInvalidTileMap = FOdysseyInvalidTileMap(64, Animation->GetWidth(), Animation->GetHeight());

    IOdysseyRenderingAbility::OnRenderingChangedDelegate().AddUObject(this, &UOdysseyAnimationPlayer::OnRenderingChanged);
    mImageRenderingComposition.Empty();
}
