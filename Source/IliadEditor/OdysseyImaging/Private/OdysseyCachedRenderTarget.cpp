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
UOdysseyCachedRenderTarget::Initialize(int InWidth, int InHeight, EOdysseyCachedRenderTargetFormat InFormat)
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

EOdysseyCachedRenderTargetFormat
UOdysseyCachedRenderTarget::GetFormat() const
{
    return Format;
}

EPixelFormat
UOdysseyCachedRenderTarget::GetPixelFormat() const
{
    switch(Format)
    {
        case EOdysseyCachedRenderTargetFormat::RGBA8: return PF_B8G8R8A8;
        case EOdysseyCachedRenderTargetFormat::RGBA16F: return PF_FloatRGBA;
        case EOdysseyCachedRenderTargetFormat::RGBA32F: return PF_A32B32G32R32F;
    }
    return PF_B8G8R8A8;
}

int
UOdysseyCachedRenderTarget::GetChannelsPerPixel() const
{
    switch(Format)
    {
        case EOdysseyCachedRenderTargetFormat::RGBA8: return 4;
        case EOdysseyCachedRenderTargetFormat::RGBA16F: return 4;
        case EOdysseyCachedRenderTargetFormat::RGBA32F: return 4;
    }
    return 4;
}

int
UOdysseyCachedRenderTarget::GetBytesPerChannel() const
{
    switch(Format)
    {
        case EOdysseyCachedRenderTargetFormat::RGBA8: return 1;
        case EOdysseyCachedRenderTargetFormat::RGBA16F: return 2;
        case EOdysseyCachedRenderTargetFormat::RGBA32F: return 4;
    }
    return 8;
}

int
UOdysseyCachedRenderTarget::GetBytesPerPixel() const
{
    return GetChannelsPerPixel() * GetBytesPerChannel();
}

int
UOdysseyCachedRenderTarget::GetStride() const
{
    return GetBytesPerPixel() * Width;
}

int
UOdysseyCachedRenderTarget::GetTotalBytes() const
{
    return GetStride() * Height;
}

void
UOdysseyCachedRenderTarget::BeginDraw()
{
    checkf(!IsDrawing, TEXT("BeginDraw() cannot be called twice in a row, Call EndDraw() first"));

    if (IsDrawing)
        return;

    LoadRenderTarget();

    mResetRenderTarget = TStrongObjectPtr(NewObject<UTextureRenderTarget2D>());
    mResetRenderTarget->ClearColor = FLinearColor::Transparent;
    mResetRenderTarget->InitCustomFormat(Width, Height, GetPixelFormat(), true);
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

    double start = FPlatformTime::Seconds() * 1000.f;

    ENQUEUE_RENDER_COMMAND(UOdysseyCachedRenderTarget_Draw)(
        [source = SourceTexture, destination = mRenderTarget, sourceRect, destinationRect](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder graphBuilder(RHICmdList);
            FRDGTextureRef sourceTexture = graphBuilder.RegisterExternalTexture(CreateRenderTarget(source->GetResource()->TextureRHI, TEXT("Source")));
            FRDGTextureRef destinationTexture = destination->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );

            for (int i = 0; i < 1000; i++)
            {
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
            }
            graphBuilder.Execute();
        }
    );

    FlushRenderingCommands();
    FRenderCommandFence fence;
    fence.BeginFence();
    fence.Wait();

    double end = FPlatformTime::Seconds() * 1000.f;
    UE_LOG(LogTemp, Warning, TEXT("Draw() in %f ms."), end-start);
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
    double start = FPlatformTime::Seconds() * 1000.f;
    if (mCacheState == eCacheState::DDC)
        LoadImage();

    FIntRect sourceRect = Rect;
    FIntRect fullRect(0, 0, Destination->SizeX, Destination->SizeY);
    FIntRect destinationRect(0, 0, Destination->SizeX, Destination->SizeY);

    sourceRect.Clip(fullRect - Position);
    destinationRect.Clip(Rect + Position);

    if (mCacheState == eCacheState::Image)
    {
        if (mImageFuture.IsValid())
        {
            ENQUEUE_RENDER_COMMAND(UOdysseyCachedRenderTarget_Draw)(
                [
                    imageFuture = mImageFuture,
                    width = Width,
                    height = Height,
                    destination = Destination,
                    pixelFormat = GetPixelFormat(),
                    stride = GetStride(),
                    sourceRect,
                    destinationRect
                ](FRHICommandListImmediate& RHICmdList)
                {
                    FRDGBuilder graphBuilder(RHICmdList);

                    FRDGTextureDesc renderTextureDesc = FRDGTextureDesc::Create2D(
                        FIntPoint(width, height),
                        pixelFormat,
                        FClearValueBinding::Transparent,
                        ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable
                    );
                    FRDGTextureRef renderTexture = graphBuilder.CreateTexture(renderTextureDesc, TEXT("UOdysseyCachedRenderTarget::RenderTexture"));

                    for (int i = 0; i < 100; i++)
                    {
                        auto* PassParameters = graphBuilder.AllocParameters<FRenderTargetParameters>();
                        PassParameters->RenderTargets[0] = FRenderTargetBinding(renderTexture, ERenderTargetLoadAction::ENoAction);

                        graphBuilder.AddPass(
                            RDG_EVENT_NAME("OdysseyCopyFromImage"),
                            PassParameters,
                            ERDGPassFlags::Raster,
                            [imageFuture, renderTexture, height, stride](FRHICommandListImmediate& RHICmdList)
                            {
                                const FUniqueBuffer& image = imageFuture.Get();
                                FTextureRHIRef destinationTexture = renderTexture->GetRHI();
                                FRHILockTextureArgs LockArgs = FRHILockTextureArgs::Lock2D(destinationTexture, 0, RLM_WriteOnly, false);
                                FRHILockTextureResult LockResult = RHICmdList.LockTexture(LockArgs);

                                for (uint32 y = 0; y < height; y++)
                                {
                                    void* src = (uint8*)(image.GetData()) + stride * y;
                                    void* dst = (uint8*)(LockResult.Data) + LockResult.Stride * y;
                                    FMemory::Memcpy(dst, src, stride);
                                }

                                RHICmdList.UnlockTexture(LockArgs);
                            }
                        );

                        FRDGTextureRef destinationTexture = destination->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );

                        AddDrawTexturePass(
                            graphBuilder,
                            FScreenPassViewInfo(),
                            renderTexture,
                            destinationTexture,
                            sourceRect.Min,
                            sourceRect.Size(),
                            destinationRect.Min,
                            destinationRect.Size()
                        );
                    }

                    graphBuilder.Execute();
                }
            );
        }
        else
        {
            ENQUEUE_RENDER_COMMAND(UOdysseyCachedRenderTarget_Draw)(
                [destination = Destination, destinationRect](FRHICommandListImmediate& RHICmdList)
                {
                    FRDGBuilder graphBuilder(RHICmdList);
                    FRDGTextureRef destinationTexture = destination->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );
                    AddClearRenderTargetPass(graphBuilder, destinationTexture, FLinearColor::Transparent, destinationRect);
                    graphBuilder.Execute();
                }
            );
        }
    }
    else if (mCacheState == eCacheState::RenderTarget)
    {
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

    FlushRenderingCommands();
    FRenderCommandFence fence;
    fence.BeginFence();
    fence.Wait();

    double end = FPlatformTime::Seconds() * 1000.f;
    UE_LOG(LogTemp, Warning, TEXT("Render() in %f ms."), end-start);
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

    FlushRenderingCommands();
    FRenderCommandFence fence;
    fence.BeginFence();
    fence.Wait();
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

    FlushRenderingCommands();
    FRenderCommandFence fence;
    fence.BeginFence();
    fence.Wait();
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
        TSharedRef<TPromise<FUniqueBuffer>> promise = MakeShared<TPromise<FUniqueBuffer>>();

        ENQUEUE_RENDER_COMMAND(UOdysseyCachedRenderTarget_Draw)(
            [
                promise,
                width = Width,
                height = Height,
                source = mRenderTarget,
                pixelFormat = GetPixelFormat(),
                stride = GetStride()
            ](FRHICommandListImmediate& RHICmdList)
            {
                FTextureRHIRef sourceTexture = source->GetResource()->TextureRHI;
                FRHILockTextureArgs LockArgs = FRHILockTextureArgs::Lock2D(sourceTexture, 0, RLM_ReadOnly, false);
                FRHILockTextureResult LockResult = RHICmdList.LockTexture(LockArgs);

                FUniqueBuffer image = FUniqueBuffer::Alloc(stride * height);
                for (uint32 y = 0; y < height; y++)
                {
                    void* src = (uint8*)(LockResult.Data) + LockResult.Stride * y;
                    void* dst = (uint8*)(image.GetData()) + stride * y;
                    FMemory::Memcpy(dst, src, stride);
                }
                RHICmdList.UnlockTexture(LockArgs);

                promise->SetValue(MoveTemp(image));
            }
        );

        mImageFuture = promise->GetFuture().Share();

        mIsImageCacheInvalid = false;
    }
    mRenderTarget = nullptr;

    eCacheState oldState = mCacheState;
    mCacheState = eCacheState::Image;
    FOdysseyCachedRenderTargetManager::Get().UpdateCacheState(this, oldState, mCacheState);

    FlushRenderingCommands();
    FRenderCommandFence fence;
    fence.BeginFence();
    fence.Wait();

    double end = FPlatformTime::Seconds() * 1000.f;
    UE_LOG(LogTemp, Warning, TEXT("UnloadRenderTarget() in %f ms."), end-start);
}

void
UOdysseyCachedRenderTarget::UnloadImage() const
{
    double start = FPlatformTime::Seconds() * 1000.f;
    if (mCacheState != eCacheState::Image)
        return;

    if (mIsDDCCacheInvalid && mImageFuture.IsValid())
    {
        const FUniqueBuffer& image = mImageFuture.Get();

        FOdysseyDiskCache cache(OdysseyCachedRenderTarget_CACHE_NAME, OdysseyCachedRenderTarget_CACHE_VERSION);
        FSharedBuffer sharedBuffer = FSharedBuffer::MakeView(image.GetData(), image.GetSize());
        cache.Save(mId.ToString(), sharedBuffer);

        mIsDDCCacheInvalid = false;
    }

    mImageFuture = TSharedFuture<FUniqueBuffer>();

    eCacheState oldState = mCacheState;
    mCacheState = eCacheState::DDC;
    FOdysseyCachedRenderTargetManager::Get().UpdateCacheState(this, oldState, mCacheState);

    double end = FPlatformTime::Seconds() * 1000.f;
    UE_LOG(LogTemp, Warning, TEXT("UnloadImage() in %f ms."), end-start);
}

void
UOdysseyCachedRenderTarget::LoadImage() const
{
    if (mCacheState != eCacheState::DDC)
        return;

    double start = FPlatformTime::Seconds() * 1000.f;
    double end = FPlatformTime::Seconds() * 1000.f;
    FOdysseyDiskCache cache(OdysseyCachedRenderTarget_CACHE_NAME, OdysseyCachedRenderTarget_CACHE_VERSION);

    FUniqueBuffer buffer;
    if (cache.Load(mId.ToString(), buffer))
    {
        end = FPlatformTime::Seconds() * 1000.f;
        UE_LOG(LogTemp, Warning, TEXT("#1 LoadImage() in %f ms."), end-start);
        start = FPlatformTime::Seconds() * 1000.f;

        TPromise<FUniqueBuffer> promise;
        promise.SetValue(MoveTemp(buffer));
        mImageFuture = promise.GetFuture().Share();

        end = FPlatformTime::Seconds() * 1000.f;
        UE_LOG(LogTemp, Warning, TEXT("#4 LoadImage() in %f ms."), end-start);
        start = FPlatformTime::Seconds() * 1000.f;
    }

    /* if (cache.ProbablyExists(mId.ToString()))
    {
        end = FPlatformTime::Seconds() * 1000.f;
        UE_LOG(LogTemp, Warning, TEXT("#1 LoadImage() in %f ms."), end-start);
        start = FPlatformTime::Seconds() * 1000.f;

        FUniqueBuffer image = FUniqueBuffer::Alloc(GetTotalBytes());

        end = FPlatformTime::Seconds() * 1000.f;
        UE_LOG(LogTemp, Warning, TEXT("#2 LoadImage() in %f ms."), end-start);
        start = FPlatformTime::Seconds() * 1000.f;

        if ( cache.LoadInto(mId.ToString(), image ) )
        {
            end = FPlatformTime::Seconds() * 1000.f;
            UE_LOG(LogTemp, Warning, TEXT("#3 LoadImage() in %f ms."), end-start);
            start = FPlatformTime::Seconds() * 1000.f;

            TPromise<FUniqueBuffer> promise;
            promise.SetValue(MoveTemp(image));
            mImageFuture = promise.GetFuture().Share();

            end = FPlatformTime::Seconds() * 1000.f;
            UE_LOG(LogTemp, Warning, TEXT("#4 LoadImage() in %f ms."), end-start);
            start = FPlatformTime::Seconds() * 1000.f;
        }
    } */

    eCacheState oldState = mCacheState;
    mCacheState = eCacheState::Image;
    FOdysseyCachedRenderTargetManager::Get().UpdateCacheState(this, oldState, mCacheState);

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
    mRenderTarget->ClearColor = FLinearColor::Transparent;
    mRenderTarget->InitCustomFormat(Width, Height, GetPixelFormat(), true);
    mRenderTarget->UpdateResourceImmediate();

    if (mImageFuture.IsValid())
    {
        ENQUEUE_RENDER_COMMAND(UOdysseyCachedRenderTarget_Draw)(
            [imageFuture = mImageFuture, height = Height, destination = mRenderTarget, stride = GetStride()](FRHICommandListImmediate& RHICmdList)
            {
                const FUniqueBuffer& image = imageFuture.Get();
                FTextureRHIRef destinationTexture = destination->GetRenderTargetResource()->TextureRHI;
                FRHILockTextureArgs LockArgs = FRHILockTextureArgs::Lock2D(destinationTexture, 0, RLM_WriteOnly, false);
                FRHILockTextureResult LockResult = RHICmdList.LockTexture(LockArgs);

                for (uint32 y = 0; y < height; y++)
                {
                    void* src = (uint8*)(image.GetData()) + stride * y;
                    void* dst = (uint8*)(LockResult.Data) + LockResult.Stride * y;
                    FMemory::Memcpy(dst, src, stride);
                }

                RHICmdList.UnlockTexture(LockArgs);
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

    FlushRenderingCommands();
    FRenderCommandFence fence;
    fence.BeginFence();
    fence.Wait();

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
    //TODO: Load/Save !
}
