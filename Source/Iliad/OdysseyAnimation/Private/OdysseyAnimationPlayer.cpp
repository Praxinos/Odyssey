// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationPlayer.h"
#include "OdysseyAnimation.h"
#include "RenderCommandFence.h"
#include "RHITypes.h"
#include "Engine/Texture2D.h"
#include "TextureCompiler.h"
#include "Misc/TransactionObjectEvent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "UObject/DevObjectVersion.h"

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
    RenderTarget->bAutoGenerateMips = true;
    RenderTarget->UpdateResource();
}

void
UOdysseyAnimationPlayer::Play(bool iBackward)
{
    if (!Animation)
        return;

    if (Status == EOdysseyAnimationPlayerStatus::Playing && mIsBackward == iBackward)
        return;

    if (Status == EOdysseyAnimationPlayerStatus::Scrubbing)
        EndScrub();

    mIsBackward = iBackward;
    Status = EOdysseyAnimationPlayerStatus::Playing;

    FFrameTime newFrame = mCursorFrame;
    switch(PlayRange)
    {
        case EOdysseyAnimationPlayerPlayRange::AnimationBounds:
        {
            newFrame = FMath::Clamp(newFrame, FFrameTime(Animation->GetFrameRange().GetLowerBoundValue()), FFrameTime(Animation->GetFrameRange().GetUpperBoundValue()));
        }
        break;

        case EOdysseyAnimationPlayerPlayRange::Custom:
        {
            newFrame = FMath::Clamp(newFrame, FFrameTime(CustomPlayRangeStartFrame), FFrameTime(CustomPlayRangeEndFrame));
        }
        break;
    }
    SeekToFrame(newFrame);
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

#if WITH_EDITOR
    FFrameTime displayedFrame = mCursorFrame;
    if (!mIgnorePrePostBehaviour)
        displayedFrame = ApplyPrePostBehaviour(mCursorFrame);
#else
    FFrameTime displayedFrame = ApplyPrePostBehaviour(mCursorFrame);
#endif

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

EOdysseyAnimationPlayerStatus
UOdysseyAnimationPlayer::GetStatus() const
{
    return Status;
}

FFrameTime
UOdysseyAnimationPlayer::ApplyPrePostBehaviour(FFrameTime iFrame) const
{
    if (!Animation)
        return iFrame;

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

    switch(PreBehaviour)
    {
        case EOdysseyAnimationPlayerPostBehaviour::Hold:
        {
            return FFrameTime(Animation->GetFrameRange().GetLowerBoundValue());
        }
        break;

        case EOdysseyAnimationPlayerPostBehaviour::Loop:
        {
            FFrameTime duration(Animation->GetFrameCount());
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

    switch(PostBehaviour)
    {
        case EOdysseyAnimationPlayerPostBehaviour::Hold:
        {
            return FFrameTime(Animation->GetFrameRange().GetUpperBoundValue());
        }
        break;

        case EOdysseyAnimationPlayerPostBehaviour::Loop:
        {
            FFrameTime duration(Animation->GetFrameCount());
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
        FFrameTime leftBound;
        FFrameTime rightBound;
        switch(PlayRange)
        {
            case EOdysseyAnimationPlayerPlayRange::AnimationBounds:
            {
                leftBound = FFrameTime(Animation->GetFrameRange().GetLowerBoundValue());
                rightBound = FFrameTime(Animation->GetFrameRange().GetUpperBoundValue());
            }
            break;

            case EOdysseyAnimationPlayerPlayRange::Custom:
            {
                leftBound = CustomPlayRangeStartFrame;
                rightBound = CustomPlayRangeEndFrame;
            }
            break;
        }
        FFrameTime duration = rightBound - leftBound + FFrameTime(1);

        bool bStop = false;
        FFrameTime newFrame = mDisplayedFrame;
        if ( mIsBackward )
        {
            newFrame -= FFrameTime::FromDecimal(iDeltaTime * PlayRate * Animation->GetFramesPerSecond());

            if (PlayRange != EOdysseyAnimationPlayerPlayRange::Infinite)
            {
                if (newFrame < leftBound)
                {
                    if ( IsLoopingInPlayRange )
                    {
                        while ( newFrame < leftBound )
                        {
                            newFrame += duration;
                        }
                    }
                    else
                    {
                        newFrame = leftBound;
                        bStop = true;
                    }
                }
            }
        }
        else
        {
            newFrame += FFrameTime::FromDecimal(iDeltaTime * PlayRate * Animation->GetFramesPerSecond());

            if (PlayRange != EOdysseyAnimationPlayerPlayRange::Infinite)
            {
                if (newFrame >= rightBound + FFrameTime(1))
                {
                    if ( IsLoopingInPlayRange )
                    {
                        while ( newFrame >= rightBound + FFrameTime(1) )
                        {
                            newFrame -= duration;
                        }
                    }
                    else
                    {
                        newFrame = rightBound;
                        bStop = true;
                    }
                }
            }
        }


        if (bStop)
        {
            Stop();
        }
        else
        {
            SeekToFrame(newFrame);
        }
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

    if( !RenderTarget->GameThread_GetRenderTargetResource() )
        return;

    FFrameTime frame = mDisplayedFrame;
    uint64 renderType = mRenderType.Get();
    TArray<FGuid> imageRenderingComposition = Animation->GetRenderingComposition(renderType, frame.GetFrame().Value);
    if ( imageRenderingComposition != mImageRenderingComposition )
    {
        mImageRenderingComposition = imageRenderingComposition;

        RenderTarget->WaitForPendingInitOrStreaming();
        Animation->Render_GameThread(RenderTarget, frame.GetFrame(), renderType );
        RenderTarget->UpdateResourceImmediate(false); //Update MipMaps

        /* FlushRenderingCommands();
        FRenderCommandFence fence;
        fence.BeginFence();
        fence.Wait(); */

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
        RenderTarget->UpdateResourceImmediate(false); //Update MipMaps

        /* FlushRenderingCommands();
        FRenderCommandFence fence;
        fence.BeginFence();
        fence.Wait(); */

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
    RenderTarget->UpdateResourceImmediate(false);

    mInvalidTileMap = FOdysseyInvalidTileMap(64, Animation->GetWidth(), Animation->GetHeight());

    UpdateTexture();

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

void
UOdysseyAnimationPlayer::SetIsLoopingInPlayRange(bool IsLooping)
{
    IsLoopingInPlayRange = IsLooping;
}

bool
UOdysseyAnimationPlayer::GetIsLoopingInPlayRange() const
{
    return IsLoopingInPlayRange;
}

void
UOdysseyAnimationPlayer::SetPlayRange(EOdysseyAnimationPlayerPlayRange InPlayRange)
{
    PlayRange = InPlayRange;
}

EOdysseyAnimationPlayerPlayRange
UOdysseyAnimationPlayer::GetPlayRange() const
{
    return PlayRange;
}

void
UOdysseyAnimationPlayer::SetCustomPlayRange(FFrameNumber StartFrame, FFrameNumber EndFrame)
{
    CustomPlayRangeStartFrame = StartFrame;
    CustomPlayRangeEndFrame = EndFrame;
}

void
UOdysseyAnimationPlayer::GetCustomPlayRange(FFrameNumber& StartFrame, FFrameNumber& EndFrame)
{
    StartFrame = CustomPlayRangeStartFrame;
    EndFrame = CustomPlayRangeEndFrame;
}

#if WITH_EDITOR

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
UOdysseyAnimationPlayer::SetIgnorePrePostBehaviour(bool iValue)
{
    mIgnorePrePostBehaviour = iValue;
}

bool
UOdysseyAnimationPlayer::GetIgnorePrePostBehaviour() const
{
    return mIgnorePrePostBehaviour;
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

    RenderTarget = NewObject<UTextureRenderTarget2D>(this, NAME_None, RF_Public | RF_Transient);
    RenderTarget->RenderTargetFormat = RTF_RGBA16f;
    RenderTarget->bAutoGenerateMips = true;

    //will create the texture if needed
    IOdysseyRenderingAbility::OnRenderingChangedDelegate().RemoveAll(this);
    if (!Animation)
        return;

    RenderTarget->ResizeTarget(Animation->GetWidth(), Animation->GetHeight());
    RenderTarget->UpdateResource();
    RenderTarget->UpdateResourceImmediate(false);

    mInvalidTileMap = FOdysseyInvalidTileMap(64, Animation->GetWidth(), Animation->GetHeight());

    UpdateTexture();

    IOdysseyRenderingAbility::OnRenderingChangedDelegate().AddUObject(this, &UOdysseyAnimationPlayer::OnRenderingChanged);
}

void
UOdysseyAnimationPlayer::PostDuplicate(EDuplicateMode::Type iDuplicateMode)
{
    Super::PostDuplicate(iDuplicateMode);

    if (GetFlags() & RF_ClassDefaultObject)
        return;

    RenderTarget = NewObject<UTextureRenderTarget2D>(this, NAME_None, RF_Public | RF_Transient);
    RenderTarget->RenderTargetFormat = RTF_RGBA16f;
    RenderTarget->bAutoGenerateMips = true;

    //will create the texture if needed
    IOdysseyRenderingAbility::OnRenderingChangedDelegate().RemoveAll(this);
    if ( !Animation )
        return;

    RenderTarget->ResizeTarget(Animation->GetWidth(), Animation->GetHeight());
    RenderTarget->UpdateResource();
    RenderTarget->UpdateResourceImmediate(false);

    mInvalidTileMap = FOdysseyInvalidTileMap(64, Animation->GetWidth(), Animation->GetHeight());
    mImageRenderingComposition.Empty();

    UpdateTexture();

    IOdysseyRenderingAbility::OnRenderingChangedDelegate().AddUObject(this, &UOdysseyAnimationPlayer::OnRenderingChanged);
}

struct FOdysseyAnimationPlayerObjectVersion
{
    enum Type
    {
        // Before any version changes were made
        BeforeCustomVersionWasAdded,

        // Added the Play Range system
        AddPlayRange,

        // -----<new versions can be added above this line>-------------------------------------------------
        VersionPlusOne,
        LatestVersion = VersionPlusOne - 1
    };

    // The GUID for this custom version number
    const static FGuid GUID;

private:
    FOdysseyAnimationPlayerObjectVersion() {}
};

const FGuid FOdysseyAnimationPlayerObjectVersion::GUID(0x19967258, 0x708644FE, 0x92B03E58, 0x23F1B6BC);
FDevVersionRegistration GRegisterOdysseyAnimationPlayerObjectVersion(FOdysseyAnimationPlayerObjectVersion::GUID, FOdysseyAnimationPlayerObjectVersion::LatestVersion, TEXT("OdysseyAnimationPlayer"));

void
UOdysseyAnimationPlayer::Serialize(FArchive& Ar)
{
    Super::Serialize(Ar);

    if( Ar.IsLoading() && Ar.CustomVer(FOdysseyAnimationPlayerObjectVersion::GUID) < FOdysseyAnimationPlayerObjectVersion::AddPlayRange )
    {
        PlayRange = EOdysseyAnimationPlayerPlayRange::Infinite;
    }
}
