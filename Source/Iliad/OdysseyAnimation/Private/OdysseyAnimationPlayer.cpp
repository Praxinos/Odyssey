// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationPlayer.h"
#include "OdysseyAnimation.h"
#include "RenderCommandFence.h"
#include "RHITypes.h"
#include "Engine/Texture2D.h"
#include "TextureCompiler.h"
#include "Misc/TransactionObjectEvent.h"
#include "Engine/TextureRenderTarget2D.h"

FSimpleMulticastDelegate&
UOdysseyAnimationPlayer::OnCurrentFrameChanged()
{
    return mOnCurrentFrameChanged;
}

FSimpleMulticastDelegate&
UOdysseyAnimationPlayer::OnStatusChanged()
{
    return mOnStatusChanged;
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
    RenderTarget->RenderTargetFormat = RTF_RGBA8;
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
        mCurrentFrame = FMath::Clamp(mCurrentFrame, mRange->GetLowerBoundValue(), mRange->GetUpperBoundValue());
#endif

    mOnStatusChanged.Broadcast();
}

void
UOdysseyAnimationPlayer::Pause()
{
    if (Status == EOdysseyAnimationPlayerStatus::Paused)
        return;

    if (Status == EOdysseyAnimationPlayerStatus::Scrubbing)
        EndScrub();

    Status = EOdysseyAnimationPlayerStatus::Paused;
    mOnStatusChanged.Broadcast();
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
    mOnStatusChanged.Broadcast();
}

void
UOdysseyAnimationPlayer::SeekToFrame(FFrameTime iFrame)
{
    if (mCurrentFrame == iFrame)
        return;

    mCurrentFrame = iFrame;
    mOnCurrentFrameChanged.Broadcast();
}

void
UOdysseyAnimationPlayer::SeekToFrameImmediate(FFrameTime iFrame)
{
    if (mCurrentFrame == iFrame)
        return;

    mCurrentFrame = iFrame;
    UpdateTexture();
    mOnCurrentFrameChanged.Broadcast();
}

UTextureRenderTarget2D*
UOdysseyAnimationPlayer::GetRenderTarget()
{
    return RenderTarget;
}

FFrameTime
UOdysseyAnimationPlayer::GetCurrentFrame() const
{
    return mCurrentFrame;
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

bool
UOdysseyAnimationPlayer::GetFrameInAnimationBounds(FFrameTime iFrame, FFrameTime& oFrame) const
{
    if (!Animation)
        return false;

#if WITH_EDITOR
    if (mIgnoreAnimationBounds)
    {
        oFrame = iFrame;
        return true;
    }
#endif

    FInt32Range frameRange = Animation->GetFrameRange();
    FFrameTime leftBound = FFrameTime(frameRange.GetLowerBoundValue());
    FFrameTime rightBound = FFrameTime(frameRange.GetUpperBoundValue()) + FFrameTime(1);

    oFrame = iFrame;
    if(iFrame < leftBound)
    {
        if (!ApplyPreBehaviour(iFrame, oFrame))
            return false;
    }
    else if (iFrame >= rightBound)
    {
        if (!ApplyPostBehaviour(iFrame, oFrame))
            return false;
    }

    return true;
}

bool
UOdysseyAnimationPlayer::GetCurrentFrameInAnimationBounds(FFrameTime& oFrame) const
{
    return GetFrameInAnimationBounds(mCurrentFrame, oFrame);
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

bool
UOdysseyAnimationPlayer::ApplyPreBehaviour(FFrameTime iFrame, FFrameTime& oFrame) const
{
    if (!Animation)
        return false;

#if WITH_EDITOR
    if (!mUsePreBehaviour)
        return false;
#endif

    switch(PreBehaviour)
    {
        case EOdysseyAnimationPlayerPostBehaviour::Hold:
        {
            oFrame = FFrameTime(Animation->GetFrameRange().GetLowerBoundValue());
        }
        break;

        case EOdysseyAnimationPlayerPostBehaviour::Loop:
        {
            FFrameTime duration;
            if (!GetDuration(duration))
                return false;

            if ( duration <= 0 )
                return false;

            FInt32Range frameRange = Animation->GetFrameRange();
            FFrameTime leftBound = FFrameTime(frameRange.GetLowerBoundValue());
            FFrameTime rightBound = FFrameTime(frameRange.GetUpperBoundValue());
            FFrameTime positiveFrame = (iFrame - leftBound + 1) * -1;
            oFrame = rightBound - (positiveFrame % duration) + leftBound;
        }
        break;
    }
    return true;
}

bool
UOdysseyAnimationPlayer::ApplyPostBehaviour(FFrameTime iFrame, FFrameTime& oFrame) const
{
    if ( !Animation )
        return false;

#if WITH_EDITOR
    if (!mUsePostBehaviour)
        return false;
#endif

    switch(PostBehaviour)
    {
        case EOdysseyAnimationPlayerPostBehaviour::Hold:
        {
            oFrame = FFrameTime(Animation->GetFrameRange().GetUpperBoundValue());
        }
        break;

        case EOdysseyAnimationPlayerPostBehaviour::Loop:
        {
            FFrameTime duration;
            if (!GetDuration(duration))
                return false;

            if ( duration <= 0 )
                return false;

            FFrameTime leftBound = FFrameTime(Animation->GetFrameRange().GetLowerBoundValue());

            oFrame = (iFrame - leftBound) % duration + leftBound;
        }
        break;
    }

    return true;
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
        FFrameTime newFrame = mCurrentFrame;
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

    FFrameTime frame;
    if (!GetCurrentFrameInAnimationBounds(frame))
        return;

    TArray<FGuid> imageRenderingComposition = Animation->GetRenderingComposition(mRenderType, frame.GetFrame().Value);
    if ( imageRenderingComposition != mImageRenderingComposition )
    {
        mImageRenderingComposition = imageRenderingComposition;

        IOdysseyTextureRenderingAbility::Execute_Render(Animation, RenderTarget, frame.GetFrame());
        mInvalidTileMap.Clear();
        return;
    }

    if (!mInvalidTileMap.InvalidTiles().IsEmpty())
    {
        IOdysseyTextureRenderingAbility::Execute_RenderRects(Animation, RenderTarget, frame.GetFrame(), mInvalidTileMap.InvalidRects());
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

        FFrameTime frame;
        if (!GetCurrentFrameInAnimationBounds(frame))
            return;

        TArray<FGuid> imageRenderingComposition = Animation->GetRenderingComposition(mRenderType, frame.GetFrame().Value);
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
    mOnStatusChanged.Broadcast();
}

void
UOdysseyAnimationPlayer::EndScrub()
{
    if ( Status != EOdysseyAnimationPlayerStatus::Scrubbing )
        return;

    Status = EOdysseyAnimationPlayerStatus::Stopped;
    mOnStatusChanged.Broadcast();
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
UOdysseyAnimationPlayer::SetRenderType(EOdysseyRenderingType iRenderType)
{
    mRenderType = iRenderType;
}

EOdysseyRenderingType
UOdysseyAnimationPlayer::GetRenderType() const
{
    return mRenderType;
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

    AnimationChanged();

    //will create the texture if needed
    if (Animation)
    {
        mInvalidTileMap = FOdysseyInvalidTileMap(64, Animation->GetWidth(), Animation->GetHeight());
        IOdysseyRenderingAbility::OnRenderingChangedDelegate().AddUObject(this, &UOdysseyAnimationPlayer::OnRenderingChanged);
    }
}

void
UOdysseyAnimationPlayer::PostDuplicate(EDuplicateMode::Type iDuplicateMode)
{
    Super::PostDuplicate(iDuplicateMode);

    if (GetFlags() & RF_ClassDefaultObject)
        return;

    //will create the texture if needed
    AnimationChanged();
    mImageRenderingComposition.Empty();
}
