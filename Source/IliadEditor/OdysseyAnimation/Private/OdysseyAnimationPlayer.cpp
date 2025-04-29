// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationPlayer.h"
#include "OdysseyAnimation.h"
#include "OdysseyRectUtils.h"
#include "RenderCommandFence.h"
#include "RHITypes.h"
#include "Engine/Texture2D.h"
#include "TextureCompiler.h"
#include "Misc/TransactionObjectEvent.h"
#include "Misc/OdysseyUndoDelegates.h"
#include "Engine/TextureRenderTarget2D.h"

FSimpleMulticastDelegate&
UOdysseyAnimationPlayer::OnAnimationChanged()
{
    return mOnAnimationChanged;
}

FSimpleMulticastDelegate&
UOdysseyAnimationPlayer::OnCurrentFrameChanged()
{
    return mOnCurrentFrameChanged;
}

FSimpleMulticastDelegate&
UOdysseyAnimationPlayer::OnPlay()
{
    return mOnPlay;
}

FSimpleMulticastDelegate&
UOdysseyAnimationPlayer::OnStop()
{
    return mOnStop;
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

    RenderTarget = NewObject<UTextureRenderTarget2D>(this);
}

void
UOdysseyAnimationPlayer::SetFrameRange(const TOptional<TRange<FFrameTime>>& iRange)
{
    mRange = iRange;
}

void
UOdysseyAnimationPlayer::Play(bool iBackward)
{
    mIsBackward = iBackward;
    Status = EOdysseyAnimationPlayerStatus::Playing;

    if (mRange.IsSet())
        mCurrentFrame = FMath::Clamp(mCurrentFrame, mRange->GetLowerBoundValue(), mRange->GetUpperBoundValue());

    mOnPlay.Broadcast();
}

void
UOdysseyAnimationPlayer::Pause()
{
    Status = EOdysseyAnimationPlayerStatus::Paused;
}

void
UOdysseyAnimationPlayer::Stop()
{
    Status = EOdysseyAnimationPlayerStatus::Stopped;
    mOnStop.Broadcast();
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

    int frameDuration = Animation->GetRightBoundValue() - Animation->GetLeftBoundValue() + 1;
    if (mIgnoreAnimationBounds)
        frameDuration = Animation->GetFrameCount();

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

    FFrameTime leftBound = FFrameTime(Animation->GetLeftBoundValue());
    FFrameTime rightBound = FFrameTime(Animation->GetRightBoundValue()) + FFrameTime(1);

    if (mIgnoreAnimationBounds)
    {
        oFrame = iFrame;
    }
    else if(iFrame < leftBound)
    {
        if (!ApplyPreBehaviour(iFrame, oFrame))
            return false;
    }
    else if (iFrame >= rightBound)
    {
        if (!ApplyPostBehaviour(iFrame, oFrame))
            return false;
    }
    else
    {
        oFrame = iFrame;
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

    if (!UsePreBehaviour)
        return false;

    switch(PreBehaviour)
    {
        case EOdysseyAnimationPlayerPostBehaviour::Hold:
        {
            oFrame = FFrameTime(Animation->GetLeftBoundValue());
        }
        break;

        case EOdysseyAnimationPlayerPostBehaviour::Loop:
        {
            FFrameTime duration;
            if (!GetDuration(duration))
                return false;

            FFrameTime leftBound = FFrameTime(Animation->GetLeftBoundValue());
            FFrameTime rightBound = FFrameTime(Animation->GetRightBoundValue());
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
    if (!UsePostBehaviour)
        return false;

    switch(PostBehaviour)
    {
        case EOdysseyAnimationPlayerPostBehaviour::Hold:
        {
            oFrame = FFrameTime(Animation->GetRightBoundValue());
        }
        break;

        case EOdysseyAnimationPlayerPostBehaviour::Loop:
        {
            FFrameTime duration;
            if (!GetDuration(duration))
                return false;

            FFrameTime leftBound = FFrameTime(Animation->GetLeftBoundValue());

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

        FFrameTime leftBound = mIgnoreAnimationBounds ? FFrameTime(Animation->GetFrameRange().GetLowerBoundValue()) : FFrameTime(Animation->GetLeftBoundValue());
        FFrameTime rightBound = mIgnoreAnimationBounds ? FFrameTime(Animation->GetFrameRange().GetUpperBoundValue()) : FFrameTime(Animation->GetRightBoundValue()) + FFrameTime(1);

        bool bStop = false;
        FFrameTime newFrame = mCurrentFrame;
        if ( mIsBackward )
        {
            newFrame -= FFrameTime::FromDecimal(iDeltaTime * PlayRate * Animation->GetFramesPerSecond());
            if (newFrame < leftBound)
            {
                if ( IsLooping )
                {
                    while ( newFrame < leftBound )
                    {
                        newFrame += duration;
                    }
                }
                else if (!UsePreBehaviour)
                {
                    newFrame = leftBound;
                    bStop = true;
                }
            }
        }
        else
        {
            newFrame += FFrameTime::FromDecimal(iDeltaTime * PlayRate * Animation->GetFramesPerSecond());
            if (newFrame >= rightBound + FFrameTime(1))
            {
                if ( IsLooping )
                {
                    while ( newFrame >= rightBound + FFrameTime(1) )
                    {
                        newFrame -= duration;
                    }
                }
                else if (!UsePostBehaviour)
                {
                    newFrame = rightBound;
                    bStop = true;
                }
            }
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

        Animation->RenderToTexture(RenderTarget, frame.GetFrame());
        mInvalidTileMap.Clear();
        return;
    }

    if (!mInvalidTileMap.InvalidTiles().IsEmpty())
    {
        Animation->RenderToTextureFromRects(RenderTarget, frame.GetFrame(), mInvalidTileMap.InvalidRects());
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
    FOdysseyRenderingAbility::OnRenderingChangedDelegate().RemoveAll(this);
    if (!Animation)
    {
        RenderTarget->ResizeTarget(1, 1);
        mRenderer = nullptr;
        return;
    }

    RenderTarget->ResizeTarget(Animation->GetWidth(), Animation->GetHeight());
    RenderTarget->UpdateResourceImmediate();

    mInvalidTileMap = FOdysseyInvalidTileMap(64, Animation->GetWidth(), Animation->GetHeight());

    FOdysseyRenderingAbility::OnRenderingChangedDelegate().AddUObject(this, &UOdysseyAnimationPlayer::OnRenderingChanged);
}

void
UOdysseyAnimationPlayer::StatusChanged()
{
}

void
UOdysseyAnimationPlayer::PlayRateChanged()
{
}


void
UOdysseyAnimationPlayer::PropertyChanged(const FName& iPropertyName)
{
    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationPlayer, Animation) )
        AnimationChanged();

    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationPlayer, Status) )
        StatusChanged();

    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationPlayer, PlayRate) )
        PlayRateChanged();
}

void
UOdysseyAnimationPlayer::PostPropertyChanged(const FName& iPropertyName)
{
    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationPlayer, Animation) )
    {
        mOnAnimationChanged.Broadcast();
    }
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
    PostPropertyChanged(PropertyChangedEvent.GetMemberPropertyName());
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
        FOdysseyUndoDelegates::Get().OnAfterUndoRedo().AddLambda(
            [this, propertyName](bool iIsRedo)
            {
                PostPropertyChanged(propertyName);
            }
        );
    }
}

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
        FOdysseyRenderingAbility::OnRenderingChangedDelegate().AddUObject(this, &UOdysseyAnimationPlayer::OnRenderingChanged);
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

#if WITH_EDITOR
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
#endif
