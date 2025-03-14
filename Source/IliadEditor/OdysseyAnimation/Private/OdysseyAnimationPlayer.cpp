// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationPlayer.h"
#include "OdysseyAnimation.h"
#include "OdysseyRectUtils.h"
#include "RenderCommandFence.h"
#include "RHITypes.h"
#include "Engine/Texture2D.h"
#include "TextureCompiler.h"
#include "Misc/TransactionObjectEvent.h"
#include "Misc/OdysseyUndoDelegates.h"

#include "ULISLoaderModule.h"

FSimpleMulticastDelegate&
UOdysseyAnimationPlayer::OnAnimationChanged()
{
    return mOnAnimationChanged;
}

FSimpleMulticastDelegate&
UOdysseyAnimationPlayer::OnTextureChanged()
{
    return mOnTextureChanged;
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
UOdysseyAnimationPlayer::SeekToFrameImmediate(int iFrameIndex)
{
    if (mCurrentFrame == iFrame)
        return;

    mCurrentFrame = iFrame;
    UpdateTexture();
    mOnCurrentFrameChanged.Broadcast();
}

UTexture2D*
UOdysseyAnimationPlayer::GetTexture()
{
    if( !Texture )
        AnimationChanged();

    return Texture;
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

/* bool
UOdysseyAnimationPlayer::GetTimeInPlayerBounds(FTimespan iTime, FTimespan& oTime) const
{
    FTimespan duration;
    if (!GetDuration(duration))
        return false;

    if (mIgnoreAnimationBounds)
    {
        oTime = iTime;
        return true;
    }

    if(iTime < 0)
    {
        return ApplyPreBehaviour(iTime, oTime);
    }
    else if (iTime >= duration)
    {
        return ApplyPostBehaviour(iTime, oTime);
    }

    oTime = iTime;
    return true;
} */

/* bool
UOdysseyAnimationPlayer::GetCurrentTimeInPlayerBounds(FTimespan& oTime) const
{
    return GetTimeInPlayerBounds(mCurrentTime, oTime);
} */

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

    //oFrame += leftBound;

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
        case EOdysseyAnimationPlayerPostBehaviour::None:
        {
            return false;
        }
        break;

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
        case EOdysseyAnimationPlayerPostBehaviour::None:
        {
            return false;
        }
        break;

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

/* bool
UOdysseyAnimationPlayer::ApplyPreBehaviour(FTimespan iTime, FTimespan& oTime) const
{
    if (!UsePreBehaviour)
        return false;

    switch(PreBehaviour)
    {
        case EOdysseyAnimationPlayerPostBehaviour::None:
        {
            return false;
        }
        break;

        case EOdysseyAnimationPlayerPostBehaviour::Hold:
        {
            oTime = FTimespan(0);
        }
        break;

        case EOdysseyAnimationPlayerPostBehaviour::Loop:
        {
            FTimespan duration;
            if (!GetDuration(duration))
                return false;

            FTimespan positiveTime = (mCurrentTime * -1);
            oTime = duration - (positiveTime % duration);
        }
        break;
    }
    return true;
}

bool
UOdysseyAnimationPlayer::ApplyPostBehaviour(FTimespan iTime, FTimespan& oTime) const
{
    if (!UsePostBehaviour)
        return false;

    switch(PostBehaviour)
    {
        case EOdysseyAnimationPlayerPostBehaviour::None:
        {
            return false;
        }
        break;

        case EOdysseyAnimationPlayerPostBehaviour::Hold:
        {
            FTimespan duration;
            if (!GetDuration(duration))
                return false;
            oTime = FMath::Max(FTimespan(0), duration - FTimespan(1));
        }
        break;

        case EOdysseyAnimationPlayerPostBehaviour::Loop:
        {
            FTimespan duration;
            if (!GetDuration(duration))
                return false;

            oTime = mCurrentTime % duration;
        }
        break;
    }

    return true;
} */

bool
UOdysseyAnimationPlayer::IsBackward() const
{
    return mIsBackward;
}

void
UOdysseyAnimationPlayer::SetRenderType(IOdysseyImageRenderer::eRenderType iRenderType)
{
    mRenderType = iRenderType;
}

IOdysseyImageRenderer::eRenderType
UOdysseyAnimationPlayer::GetRenderType() const
{
    return mRenderType;
}

void
UOdysseyAnimationPlayer::Tick(float iDeltaTime)
{
    if (GetFlags() & RF_ClassDefaultObject)
        return;

    if (!Animation || !Texture)
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

    TArray<FGuid> imageRenderingComposition = Animation->GetImageRenderingComposition(mRenderType, frame.GetFrame().Value);
    if ( imageRenderingComposition != mImageRenderingComposition )
    {
        mImageRenderingComposition = imageRenderingComposition;
        TSharedPtr<IOdysseyImageRenderer> renderer = Animation->BuildImageRenderer(mRenderType, frame.GetFrame().Value);
        renderer->Init();
        mRenderer = renderer; //Init Renderer before assigning mRenderer to avoid caching (raster / vector blocks) when unneeded

        ::ULIS::FRectI rect = ::ULIS::FRectI::FromXYWH(0, 0, Animation->GetWidth(), Animation->GetHeight());
        TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(Animation->GetWidth(), Animation->GetHeight(), Animation->GetFormat());

        {
            FOdysseyImageRendererCopyParams params(block, { block->Rect() });
            mRenderer->Copy(params, {});

            ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(Animation->GetFormat());
            ctx.Finish();
        }

        CopyBlocksToTexture({ block }, { rect });
        mInvalidTileMap.Clear();
        return;
    }

    if (!mInvalidTileMap.InvalidTiles().IsEmpty())
    {
        TSharedPtr<IOdysseyImageRenderer> renderer = Animation->BuildImageRenderer(mRenderType, frame.GetFrame().Value);
        renderer->Init();
        mRenderer = renderer; //Init Renderer before assigning mRenderer to avoid caching (raster / vector blocks) when unneeded

        TArray<TSharedPtr<::ULIS::FBlock>> blocks;
        TArray<::ULIS::FRectI> invalidRects = mInvalidTileMap.InvalidRects();

        {
            TRACE_CPUPROFILER_EVENT_SCOPE(UOdysseyAnimationPlayer::UpdateTexture::Copy);
            for ( const ::ULIS::FRectI& rect : invalidRects )
            {
                TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(rect.w, rect.h, Animation->GetFormat());
                FOdysseyImageRendererCopyParams params(block, { block->Rect() }, rect.Position());
                mRenderer->Copy(params, {});
                blocks.Add(block);
            }

            ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(Animation->GetFormat());
            ctx.Finish();
        }

        CopyBlocksToTexture(blocks, invalidRects);

        mInvalidTileMap.Clear();
    }
}

void
UOdysseyAnimationPlayer::OnImageRenderingChanged(const FOdysseyImageRenderingChangedEvent& iEvent)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UOdysseyAnimationPlayer::OnImageRenderingChanged);
    if ( !Animation )
        return;

    if (iEvent.GetType() == FOdysseyImageRenderingChangedEvent::eEventType::kValueChange)
    {
        if (mImageRenderingComposition.Contains(iEvent.GetId()))
        {
            mInvalidTileMap.Invalidate(iEvent.GetRects());
        }
        return;
    }

    if (iEvent.GetType() == FOdysseyImageRenderingChangedEvent::eEventType::kCompositionChange)
    {
        if ( !mImageRenderingComposition.Contains(iEvent.GetId()) )
            return;

        FFrameTime frame;
        if (!GetCurrentFrameInAnimationBounds(frame))
            return;

        TArray<FGuid> imageRenderingComposition = Animation->GetImageRenderingComposition(mRenderType, frame.GetFrame().Value);
        if ( imageRenderingComposition == mImageRenderingComposition )
            return;

        mInvalidTileMap.Invalidate(::ULIS::FRectI::FromXYWH(0, 0, Animation->GetWidth(), Animation->GetHeight()));
    }
}

void
UOdysseyAnimationPlayer::CopyBlocksToTexture(const TArray<TSharedPtr<::ULIS::FBlock>>& iBlocks, const TArray<::ULIS::FRectI>& iRects)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UOdysseyAnimationPlayer::CopyBlocksToTexture);
    if ( iBlocks.IsEmpty() )
        return;

    FTextureCompilingManager::Get().FinishCompilation({ Texture });

    ::ULIS::eFormat format = iBlocks[0]->Format();

    //convert block to BGRA8 if needed*
    if ( format == ::ULIS::Format_BGRA8 )
    {
        TRACE_CPUPROFILER_EVENT_SCOPE(UOdysseyAnimationPlayer::CopyBlocks);
        TArray<TSharedPtr<FUpdateTextureRegion2D>> regions; //Keeps region object alive until fence.Wait()
        for ( int i = 0; i < iBlocks.Num(); i++ )
        {
            const TSharedPtr<::ULIS::FBlock>& block = iBlocks[i];
            const ::ULIS::FRectI& rect = iRects[i];
            regions.Add(MakeShared<FUpdateTextureRegion2D>(rect.x, rect.y, 0, 0, block->Rect().w, block->Rect().h));
            Texture->UpdateTextureRegions(0, 1, regions.Last().Get(), block->BytesPerScanLine(), block->BytesPerPixel(), block->Bits());
        }

        FRenderCommandFence fence;
        fence.BeginFence();
        fence.Wait();

        return;
    }

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_BGRA8);
    TArray<TSharedPtr<::ULIS::FBlock>> convBlocks;
    TArray<TSharedPtr<FUpdateTextureRegion2D>> regions;
    for ( int i = 0; i < iBlocks.Num(); i++ )
    {
        const TSharedPtr<::ULIS::FBlock>& block = iBlocks[i];
        const ::ULIS::FRectI& rect = iRects[i];

        TSharedPtr<::ULIS::FBlock> convBlock = MakeShared<::ULIS::FBlock>(rect.w, rect.h, ::ULIS::Format_BGRA8);
        ctx.ConvertFormat(*block, *convBlock, block->Rect(), ::ULIS::FVec2I(0), ::ULIS::FSchedulePolicy::AsyncCacheEfficient);
        convBlocks.Add(convBlock);
        regions.Add(MakeShared<FUpdateTextureRegion2D>(rect.x, rect.y, 0, 0, rect.w, rect.h));
    }

    ctx.Finish();

    for ( int i = 0; i < regions.Num(); i++ )
    {
        Texture->UpdateTextureRegions(
            0,
            1,
            regions[i].Get(),
            convBlocks[i]->BytesPerScanLine(),
            convBlocks[i]->BytesPerPixel(),
            convBlocks[i]->Bits()
        );
    }

    FRenderCommandFence fence;
    fence.BeginFence();
    fence.Wait();
}

void
UOdysseyAnimationPlayer::AnimationChanged()
{
    UOdysseyAnimation::OnImageRenderingChangedDelegate().RemoveAll(this);
    if (!Animation)
    {
        Texture = nullptr;
        mRenderer = nullptr;
        return;
    }

    Texture = NewObject<UTexture2D>(this);
    Texture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;

    FTextureFormatSettings textureFormatSettings;
    Texture->GetLayerFormatSettings(0, textureFormatSettings);
    textureFormatSettings.CompressionNone = 1;
    Texture->SetLayerFormatSettings(0, textureFormatSettings);

    Texture->Source.Init(Animation->GetWidth(), Animation->GetHeight(), 1, 1, TSF_BGRA8, nullptr);
    Texture->UpdateResource();
    FTextureCompilingManager::Get().FinishCompilation({ Texture });

    mInvalidTileMap = FULISInvalidTileMap(64, Animation->GetWidth(), Animation->GetHeight());

    UOdysseyAnimation::OnImageRenderingChangedDelegate().AddUObject(this, &UOdysseyAnimationPlayer::OnImageRenderingChanged);
}

void
UOdysseyAnimationPlayer::TextureChanged()
{
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

    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationPlayer, Texture) )
        TextureChanged();

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
        mOnTextureChanged.Broadcast();
    }

    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationPlayer, Texture) )
    {
        mOnTextureChanged.Broadcast();
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

    if( !Texture )
        AnimationChanged();

    //will create the texture if needed
    if (Animation)
    {
        mInvalidTileMap = FULISInvalidTileMap(64, Animation->GetWidth(), Animation->GetHeight());
        UOdysseyAnimation::OnImageRenderingChangedDelegate().AddUObject(this, &UOdysseyAnimationPlayer::OnImageRenderingChanged);
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
