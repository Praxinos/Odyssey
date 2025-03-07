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
UOdysseyAnimationPlayer::OnTextureUpdated()
{
    return mOnTextureUpdated;
}

FSimpleMulticastDelegate&
UOdysseyAnimationPlayer::OnIsLoopingChanged()
{
    return mOnIsLoopingChanged;
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
UOdysseyAnimationPlayer::OnPause()
{
    return mOnPause;
}

FSimpleMulticastDelegate&
UOdysseyAnimationPlayer::OnStop()
{
    return mOnStop;
}

void
UOdysseyAnimationPlayer::SetTimeRange(const TOptional<TRange<FTimespan>>& iRange)
{
    mRange = iRange;
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
    mOnPause.Broadcast();
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
    mCurrentTime = iTime;
    mOnCurrentTimeChanged.Broadcast();
}

void
UOdysseyAnimationPlayer::SeekToFrame(int iFrameIndex)
{
    FTimespan currentTime = FTimespan::FromSeconds(iFrameIndex / Animation->GetFramesPerSecond());
    if (currentTime == mCurrentTime)
        return;

    mCurrentTime = currentTime;
    mOnCurrentTimeChanged.Broadcast();
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
UOdysseyAnimationPlayer::GetTexture() const
{
    return Texture;
}

FTimespan
UOdysseyAnimationPlayer::GetCurrentTime() const
{
    return mCurrentTime;
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
        FTimespan lowerLimit = mRange.IsSet() ? mRange->GetLowerBoundValue() : FTimespan::Zero();
        FTimespan upperLimit = mRange.IsSet() ? mRange->GetUpperBoundValue() : Animation->GetDuration();

        bool bStop = false;
        FTimespan newTime = mCurrentTime;
        if ( mIsBackward )
        {
            newTime -= FTimespan::FromSeconds(iDeltaTime * FrameRate);
            if ( newTime < lowerLimit )
            {
                if ( IsLooping )
                {
                    while ( newTime < lowerLimit )
                    {
                        newTime += (upperLimit - lowerLimit);
                    }
                }
                else
                {
                    newTime = lowerLimit;
                    bStop = true;
                }
            }
        }
        else
        {
            newTime += FTimespan::FromSeconds(iDeltaTime * FrameRate);
            if ( newTime > upperLimit )
            {
                if ( IsLooping )
                {
                    while ( newTime > upperLimit )
                    {
                        newTime -= (upperLimit - lowerLimit);
                    }
                }
                else
                {
                    newTime = upperLimit;
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

    int frameIndex = Animation->GetFrameIndexAtTime(mCurrentTime);
    if ( frameIndex == INDEX_NONE )
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
        mOnTextureUpdated.Broadcast();
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
        {
            TRACE_CPUPROFILER_EVENT_SCOPE(UOdysseyAnimationPlayer::UpdateTexture::OnTextureUpdated);
            mOnTextureUpdated.Broadcast();
        }
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

        int frameIndex = Animation->GetFrameIndexAtTime(mCurrentTime);
        if (frameIndex == INDEX_NONE)
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

UOdysseyAnimationPlayer::UOdysseyAnimationPlayer()
{
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
UOdysseyAnimationPlayer::IsLoopingChanged()
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

    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationPlayer, IsLooping) )
        IsLoopingChanged();
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

    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationPlayer, Status) )
    {
        mOnStatusChanged.Broadcast();
    }

    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationPlayer, FrameRate) )
    {
        mOnFrameRateChanged.Broadcast();
    }

    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimationPlayer, IsLooping) )
    {
        mOnIsLoopingChanged.Broadcast();
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
