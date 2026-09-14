// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyCachedRenderTarget.h"
#include "UObject/ObjectSaveContext.h"
#include "RenderGraphUtils.h"
#include "GenerateMips.h"
#include "OdysseyBlendShader.h"
#include "ImageCoreUtils.h"
#include "ImageUtils.h"
#include "ScreenPass.h"
#include "OdysseyDiskCache.h"
#include "OdysseyCachedRenderTargetManager.h"

#define OdysseyCachedRenderTarget_CACHE_NAME TEXT("OdysseyCachedRenderTarget")
#define OdysseyCachedRenderTarget_CACHE_VERSION TEXT("70ED9EAACC22420B8F1738352E5F3592")

UOdysseyCachedRenderTarget::~UOdysseyCachedRenderTarget()
{
    FOdysseyCachedRenderTargetManager::Get().Remove(this);
}

UOdysseyCachedRenderTarget::UOdysseyCachedRenderTarget()
    : mId(FGuid::NewGuid())
{
    FOdysseyCachedRenderTargetManager::Get().Add(this);
}

void
UOdysseyCachedRenderTarget::Initialize(int InWidth, int InHeight, ETextureRenderTargetFormat InFormat)
{
    Width = (uint32)InWidth;
    Height = (uint32)InHeight;
    Format = InFormat;

    mResetTileMap = FOdysseyInvalidTileMap(64, InWidth, InHeight);
}

int
UOdysseyCachedRenderTarget::GetWidth() const
{
    return Width;
}

int
UOdysseyCachedRenderTarget::GetHeight() const
{
    return Height;
}

ETextureRenderTargetFormat
UOdysseyCachedRenderTarget::GetFormat() const
{
    return Format;
}

void
UOdysseyCachedRenderTarget::BeginDraw()
{
    checkf(!IsDrawing, TEXT("BeginDraw() cannot be called twice in a row, Call EndDraw() first"));

    if (IsDrawing)
        return;

    LoadRenderTarget();

    mResetRenderTarget = TStrongObjectPtr(NewObject<UTextureRenderTarget2D>());
    mResetRenderTarget->RenderTargetFormat = Format;
    mResetRenderTarget->bForceLinearGamma = Format != RTF_RGBA8_SRGB;
    mResetRenderTarget->ClearColor = FLinearColor::Transparent;
    mResetRenderTarget->InitAutoFormat(Width, Height);
    mResetRenderTarget->UpdateResource();
    mResetRenderTarget->UpdateResourceImmediate();

    CopyRenderTargetToResetRT();

    IsDrawing = true;
}

void
UOdysseyCachedRenderTarget::Draw(UTexture* SourceTexture, FIntRect Rect, FIntPoint Position)
{
    checkf(IsDrawing, TEXT("Draw cannot be called if BeginDraw() has not been called first"));

    if (!IsDrawing)
        return;

    FIntRect sourceRect = Rect;
    FIntRect fullRect(0, 0, Width, Height);
    FIntRect destinationRect(0, 0, Width, Height);

    sourceRect.Clip(fullRect - Position);
    destinationRect.Clip(Rect + Position);

    mResetTileMap.Invalidate(destinationRect);
    mIsImageCacheInvalid = true;
    mIsDDCCacheInvalid = true;

    ENQUEUE_RENDER_COMMAND(UOdysseyCachedRenderTarget_Draw)(
        [source = SourceTexture, destination = mRenderTarget, sourceRect, destinationRect](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder graphBuilder(RHICmdList);
            FRDGTextureRef sourceTexture = graphBuilder.RegisterExternalTexture(CreateRenderTarget(source->GetResource()->TextureRHI, TEXT("Source")));
            FRDGTextureRef destinationTexture = destination->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );

            AddDrawTexturePass(
                graphBuilder,
                FScreenPassViewInfo(),
                sourceTexture,
                destinationTexture,
                sourceRect.Min,
                sourceRect.Size(),
                destinationRect.Min,
                destinationRect.Size()
            );
            graphBuilder.Execute();
        }
    );
}

void
UOdysseyCachedRenderTarget::ResetDraw()
{
    checkf(IsDrawing, TEXT("CommitDraw cannot be called if BeginDraw() has not been called first"));

    if (!IsDrawing)
        return;

    CopyResetRTToRenderTarget();
    mResetTileMap.Clear();
}

void
UOdysseyCachedRenderTarget::CommitDraw(bool IsUndoable)
{
    checkf(IsDrawing, TEXT("CommitDraw cannot be called if BeginDraw() has not been called first"));

    if (!IsDrawing)
        return;

    if (IsUndoable)
    {
        //TODO: Register Undo
    }
    mResetTileMap.Clear();

    CopyRenderTargetToResetRT();
}

void
UOdysseyCachedRenderTarget::EndDraw(bool IsUndoable)
{
    checkf(IsDrawing, TEXT("EndDraw cannot be called if BeginDraw() has not been called first"));

    if (!IsDrawing)
        return;

    if (IsUndoable)
    {
        //TODO: Register Undo
    }

    mResetTileMap.Clear();

    IsDrawing = false;
    mResetRenderTarget = nullptr;
    UnloadRenderTarget();
}

bool
UOdysseyCachedRenderTarget::GetIsDrawing() const
{
    return IsDrawing;
}

void
UOdysseyCachedRenderTarget::Render(UTextureRenderTarget2D* Destination, FIntRect Rect, FIntPoint Position) const
{
    LoadRenderTarget();

    FIntRect sourceRect = Rect;
    FIntRect fullRect(0, 0, Destination->SizeX, Destination->SizeY);
    FIntRect destinationRect(0, 0, Destination->SizeX, Destination->SizeY);

    sourceRect.Clip(fullRect - Position);
    destinationRect.Clip(Rect + Position);

    ENQUEUE_RENDER_COMMAND(UOdysseyCachedRenderTarget_Draw)(
        [source = mRenderTarget, destination = Destination, sourceRect, destinationRect](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder graphBuilder(RHICmdList);
            FRDGTextureRef sourceTexture = source->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );
            FRDGTextureRef destinationTexture = destination->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );

            AddDrawTexturePass(
                graphBuilder,
                FScreenPassViewInfo(),
                sourceTexture,
                destinationTexture,
                sourceRect.Min,
                sourceRect.Size(),
                destinationRect.Min,
                destinationRect.Size()
            );
            graphBuilder.Execute();
        }
    );
}

void
UOdysseyCachedRenderTarget::CopyRenderTargetToResetRT()
{
    FIntRect sourceRect(0, 0, Width, Height);
    FIntRect destinationRect(0, 0, Width, Height);

    ENQUEUE_RENDER_COMMAND(UOdysseyCachedRenderTarget_Draw)(
        [source = mRenderTarget, destination = mResetRenderTarget, sourceRect, destinationRect](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder graphBuilder(RHICmdList);
            FRDGTextureRef sourceTexture = source->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );
            FRDGTextureRef destinationTexture = destination->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );

            AddDrawTexturePass(
                graphBuilder,
                FScreenPassViewInfo(),
                sourceTexture,
                destinationTexture,
                sourceRect.Min,
                sourceRect.Size(),
                destinationRect.Min,
                destinationRect.Size()
            );
            graphBuilder.Execute();
        }
    );
}

void
UOdysseyCachedRenderTarget::CopyResetRTToRenderTarget()
{
    FIntRect sourceRect(0, 0, Width, Height);
    FIntRect destinationRect(0, 0, Width, Height);

    ENQUEUE_RENDER_COMMAND(UOdysseyCachedRenderTarget_Draw)(
        [source = mResetRenderTarget, destination = mRenderTarget, sourceRect, destinationRect](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder graphBuilder(RHICmdList);
            FRDGTextureRef sourceTexture = source->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );
            FRDGTextureRef destinationTexture = destination->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );

            AddDrawTexturePass(
                graphBuilder,
                FScreenPassViewInfo(),
                sourceTexture,
                destinationTexture,
                sourceRect.Min,
                sourceRect.Size(),
                destinationRect.Min,
                destinationRect.Size()
            );
            graphBuilder.Execute();
        }
    );
}

void
UOdysseyCachedRenderTarget::UnloadRenderTarget() const
{
    double start = FPlatformTime::Seconds() * 1000.f;
    if (IsDrawing)
        return;

    if (mCacheState != eCacheState::RenderTarget)
        return;

    if (mIsImageCacheInvalid)
    {
        if (!mImage)
            mImage = MakeShared<FImage>();

        if (!FImageUtils::GetRenderTargetImage(mRenderTarget.Get(), *mImage))
            return;

        mIsImageCacheInvalid = false;
    }
    mRenderTarget = nullptr;

    eCacheState oldState = mCacheState;
    mCacheState = eCacheState::Image;
    FOdysseyCachedRenderTargetManager::Get().UpdateCacheState(this, oldState, mCacheState);

    double end = FPlatformTime::Seconds() * 1000.f;
    UE_LOG(LogTemp, Warning, TEXT("UnloadRenderTarget() in %f ms."), end-start);
}

void
UOdysseyCachedRenderTarget::UnloadImage() const
{
    double start = FPlatformTime::Seconds() * 1000.f;
    if (mCacheState != eCacheState::Image)
        return;

    if (mIsDDCCacheInvalid && mImage)
    {
        FOdysseyDiskCache cache(OdysseyCachedRenderTarget_CACHE_NAME, OdysseyCachedRenderTarget_CACHE_VERSION);
        FSharedBuffer sharedBuffer = FSharedBuffer::MakeView(mImage->GetPixelPointer(0,0), mImage->GetImageSizeBytes());
        cache.Save(mId.ToString(), sharedBuffer);

        mImageFormat = mImage->Format;
        mImageGammaSpace = mImage->GammaSpace;

        mIsDDCCacheInvalid = false;
    }

    mImage = nullptr;

    eCacheState oldState = mCacheState;
    mCacheState = eCacheState::DDC;
    FOdysseyCachedRenderTargetManager::Get().UpdateCacheState(this, oldState, mCacheState);

    double end = FPlatformTime::Seconds() * 1000.f;
    UE_LOG(LogTemp, Warning, TEXT("UnloadImage() in %f ms."), end-start);
}

void
UOdysseyCachedRenderTarget::LoadImage() const
{
    double start = FPlatformTime::Seconds() * 1000.f;
    if (mCacheState != eCacheState::DDC)
        return;

    FOdysseyDiskCache cache(OdysseyCachedRenderTarget_CACHE_NAME, OdysseyCachedRenderTarget_CACHE_VERSION);
    if (cache.ProbablyExists(mId.ToString()))
    {
        mImage = MakeShared<FImage>(Width, Height, mImageFormat, mImageGammaSpace);
        FUniqueBuffer Buffer = FUniqueBuffer::MakeView(mImage->GetPixelPointer(0,0), mImage->GetImageSizeBytes());
        if ( !cache.LoadInto(mId.ToString(), Buffer ) )
        {
            //Empty Image
            mImage = nullptr;
        }
    }

    eCacheState oldState = mCacheState;
    mCacheState = eCacheState::Image;
    FOdysseyCachedRenderTargetManager::Get().UpdateCacheState(this, oldState, mCacheState);

    double end = FPlatformTime::Seconds() * 1000.f;
    UE_LOG(LogTemp, Warning, TEXT("LoadImage() in %f ms."), end-start);
}

void
UOdysseyCachedRenderTarget::LoadRenderTarget() const
{
    double start = FPlatformTime::Seconds() * 1000.f;

    if (mCacheState == eCacheState::RenderTarget)
    {
        FOdysseyCachedRenderTargetManager::Get().Touch(this);
        return;
    }

    if (mCacheState == eCacheState::DDC)
        LoadImage();

    mRenderTarget = TStrongObjectPtr(NewObject<UTextureRenderTarget2D>());
    mRenderTarget->RenderTargetFormat = Format;
    mRenderTarget->bForceLinearGamma = Format != RTF_RGBA8_SRGB;
    mRenderTarget->ClearColor = FLinearColor::Transparent;
    mRenderTarget->InitAutoFormat(Width, Height);
    mRenderTarget->UpdateResource();
    mRenderTarget->UpdateResourceImmediate();

    if (mImage)
    {
        ENQUEUE_RENDER_COMMAND(UOdysseyCachedRenderTarget_Draw)(
            [image = mImage, destination = mRenderTarget](FRHICommandListImmediate& RHICmdList)
            {
                FTextureRHIRef destinationTexture = destination->GetRenderTargetResource()->TextureRHI;
                FUpdateTextureRegion2D region(0, 0, 0, 0, image->SizeX, image->SizeY);
                RHIUpdateTexture2D(
                    destinationTexture,
                    0,
                    region,
                    image->GetStrideBytes(),
                    (const uint8*)image->GetPixelPointer(0,0)
                );

            }
        );
    }
    else
    {
        ENQUEUE_RENDER_COMMAND(UOdysseyCachedRenderTarget_Draw)(
            [destination = mRenderTarget](FRHICommandListImmediate& RHICmdList)
            {
                FRDGBuilder graphBuilder(RHICmdList);
                FRDGTextureRef destinationTexture = destination->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );
                AddClearRenderTargetPass(graphBuilder, destinationTexture);
                graphBuilder.Execute();
            }
        );
    }

    eCacheState oldState = mCacheState;
    mCacheState = eCacheState::RenderTarget;
    FOdysseyCachedRenderTargetManager::Get().UpdateCacheState(this, oldState, mCacheState);

    double end = FPlatformTime::Seconds() * 1000.f;
    UE_LOG(LogTemp, Warning, TEXT("LoadRenderTarget() in %f ms."), end-start);
}

UOdysseyCachedRenderTarget::eCacheState
UOdysseyCachedRenderTarget::GetCacheState() const
{
    return mCacheState;
}

void
UOdysseyCachedRenderTarget::Serialize(FArchive& Ar)
{
    Super::Serialize(Ar);

    FImage image;
    if (mCacheState == eCacheState::RenderTarget)
    {
        if (mIsImageCacheInvalid)
        {
            if (!FImageUtils::GetRenderTargetImage(mRenderTarget.Get(), image))
                return;

            mIsImageCacheInvalid = false;
        }
    }

    if (mCacheState == eCacheState::DDC)
        LoadImage();
    //TODO: Load/Save !
}
