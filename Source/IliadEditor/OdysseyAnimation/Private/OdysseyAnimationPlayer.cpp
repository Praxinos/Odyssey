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
UOdysseyAnimationPlayer::OnCurrentTimeChanged()
{
    return mOnCurrentTimeChanged;
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
UOdysseyAnimationPlayer::SetFrameRange(const TOptional<FInt32Range>& iRange)
{
    if (iRange.IsSet())
    {
        mRange = TRange<FTimespan>::Inclusive(
            FTimespan::FromSeconds(iRange->GetLowerBoundValue() / Animation->GetFramesPerSecond()),
            FTimespan::FromSeconds((iRange->GetUpperBoundValue() + 1)  / Animation->GetFramesPerSecond())
        );
    }
    else
    {
        mRange = TOptional<TRange<FTimespan>>();
    }
}

void
UOdysseyAnimationPlayer::Play(bool iBackward)
{
    mIsBackward = iBackward;
    Status = EOdysseyAnimationPlayerStatus::Playing;

    if (mRange.IsSet())
        mCurrentTime = FMath::Clamp(mCurrentTime, mRange->GetLowerBoundValue(), mRange->GetUpperBoundValue());

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
UOdysseyAnimationPlayer::SeekToTime(FTimespan iTime)
{
    if (mCurrentTime == iTime)
        return;

    mCurrentTime = iTime;
    mOnCurrentTimeChanged.Broadcast();
}

void
UOdysseyAnimationPlayer::SeekToFrame(int iFrameIndex)
{
    if (!Animation)
        return;

    int leftBound = Animation->GetLeftBoundValue();
    if (mIgnoreAnimationBounds)
        leftBound = Animation->GetFrameRange().GetLowerBoundValue();

    SeekToTime(FTimespan::FromSeconds((iFrameIndex - leftBound) / Animation->GetFramesPerSecond()) + FTimespan(1));
}

void
UOdysseyAnimationPlayer::SeekToFrameImmediate(int iFrameIndex)
{
    FTimespan currentTime = FTimespan::FromSeconds(iFrameIndex / Animation->GetFramesPerSecond());
    if (currentTime == mCurrentTime)
        return;

    mCurrentTime = currentTime;
    UpdateTexture();
    mOnCurrentTimeChanged.Broadcast();
}

UTexture2D*
UOdysseyAnimationPlayer::GetTexture()
{
    if( !Texture )
        AnimationChanged();

    return Texture;
}

FTimespan
UOdysseyAnimationPlayer::GetCurrentTime() const
{
    return mCurrentTime;
}

bool
UOdysseyAnimationPlayer::GetDuration(FTimespan& oTime) const
{
    if (!Animation)
        return false;

    int frameDuration = Animation->GetRightBoundValue() - Animation->GetLeftBoundValue() + 1;
    if (mIgnoreAnimationBounds)
        frameDuration = Animation->GetFrameCount();

    oTime = FTimespan::FromSeconds(frameDuration / Animation->GetFramesPerSecond());
    return true;
}

EOdysseyAnimationPlayerStatus
UOdysseyAnimationPlayer::GetStatus() const
{
    return Status;
}

bool
UOdysseyAnimationPlayer::GetCurrentTimeInPlayerBounds(FTimespan& oTime) const
{
    FTimespan duration;
    if (!GetDuration(duration))
        return false;

    if (mIgnoreAnimationBounds)
    {
        oTime = mCurrentTime;
        return true;
    }

    if(mCurrentTime < 0)
    {
        return ApplyPreBehaviour(mCurrentTime, oTime);
    }
    else if (mCurrentTime >= duration)
    {
        return ApplyPostBehaviour(mCurrentTime, oTime);
    }

    oTime = mCurrentTime;
    return true;
}

bool
UOdysseyAnimationPlayer::GetCurrentFrameInAnimationBounds(int& oFrame) const
{
    if (!Animation)
        return false;

    FTimespan time;
    if (!GetCurrentTimeInPlayerBounds(time))
        return false;


    oFrame = FMath::FloorToInt( time.GetTotalSeconds() * Animation->GetFramesPerSecond() );
    if (mIgnoreAnimationBounds)
    {
        oFrame += Animation->GetFrameRange().GetLowerBoundValue();
    }
    else
    {
        oFrame += Animation->GetLeftBoundValue();
    }

    return true;
}

bool
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

    switch(PreBehaviour)
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
}

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
        FTimespan duration;
        if (!GetDuration(duration))
            return;

        bool bStop = false;
        FTimespan newTime = mCurrentTime;
        if ( mIsBackward )
        {
            newTime -= FTimespan::FromSeconds(iDeltaTime * FrameRate);
            if (newTime < 0)
            {
                if ( IsLooping )
                {
                    while ( newTime < 0 )
                    {
                        newTime += duration;
                    }
                }
                else if (!UsePreBehaviour)
                {
                    newTime = 0;
                    bStop = true;
                }
            }
        }
        else
        {
            newTime += FTimespan::FromSeconds(iDeltaTime * FrameRate);
            if (newTime >= duration)
            {
                if ( IsLooping )
                {
                    while ( newTime >= duration )
                    {
                        newTime -= duration;
                    }
                }
                else if (!UsePostBehaviour)
                {
                    newTime = duration - FTimespan(1);
                    bStop = true;
                }
            }
        }

        SeekToTime(newTime);
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

    int frameIndex = 0;
    if (!GetCurrentFrameInAnimationBounds(frameIndex))
        return;

    TArray<FGuid> imageRenderingComposition = Animation->GetImageRenderingComposition(mRenderType, frameIndex);
    if ( imageRenderingComposition != mImageRenderingComposition )
    {
        mImageRenderingComposition = imageRenderingComposition;
        TSharedPtr<IOdysseyImageRenderer> renderer = Animation->BuildImageRenderer(mRenderType, frameIndex);
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
        TSharedPtr<IOdysseyImageRenderer> renderer = Animation->BuildImageRenderer(mRenderType, frameIndex);
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

        int frameIndex;
        if (!GetCurrentFrameInAnimationBounds(frameIndex))
            return;

        TArray<FGuid> imageRenderingComposition = Animation->GetImageRenderingComposition(mRenderType, frameIndex);
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
UOdysseyAnimationPlayer::FrameRateChanged()
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

    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationPlayer, FrameRate) )
        FrameRateChanged();
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
