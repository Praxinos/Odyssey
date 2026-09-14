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
#include "OdysseyTiledRLE.h"

#define OdysseyCachedRenderTarget_CACHE_NAME TEXT("OdysseyCachedRenderTarget")
#define OdysseyCachedRenderTarget_CACHE_VERSION TEXT("70ED9EAACC22420B8F1738352E5F3592")

DECLARE_STATS_GROUP(TEXT("OdysseyCachedRenderTarget"), STATGROUP_OdysseyCachedRenderTarget, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("OdysseyCachedRenderTarget Draw"), STAT_UOdysseyCachedRenderTarget_Draw, STATGROUP_OdysseyCachedRenderTarget);
DECLARE_CYCLE_STAT(TEXT("OdysseyCachedRenderTarget Render_Image"), STAT_UOdysseyCachedRenderTarget_Render_Image, STATGROUP_OdysseyCachedRenderTarget);
DECLARE_CYCLE_STAT(TEXT("OdysseyCachedRenderTarget Render_Empty"), STAT_UOdysseyCachedRenderTarget_Render_Empty, STATGROUP_OdysseyCachedRenderTarget);
DECLARE_CYCLE_STAT(TEXT("OdysseyCachedRenderTarget Render_RT"), STAT_UOdysseyCachedRenderTarget_Render_RT, STATGROUP_OdysseyCachedRenderTarget);
DECLARE_CYCLE_STAT(TEXT("OdysseyCachedRenderTarget CopyRenderTargetToResetRT"), STAT_UOdysseyCachedRenderTarget_CopyRenderTargetToResetRT, STATGROUP_OdysseyCachedRenderTarget);
DECLARE_CYCLE_STAT(TEXT("OdysseyCachedRenderTarget CopyResetRTToRenderTarget"), STAT_UOdysseyCachedRenderTarget_CopyResetRTToRenderTarget, STATGROUP_OdysseyCachedRenderTarget);
DECLARE_CYCLE_STAT(TEXT("OdysseyCachedRenderTarget UnloadRenderTarget"), STAT_UOdysseyCachedRenderTarget_UnloadRenderTarget, STATGROUP_OdysseyCachedRenderTarget);
DECLARE_CYCLE_STAT(TEXT("OdysseyCachedRenderTarget LoadRenderTarget"), STAT_UOdysseyCachedRenderTarget_LoadRenderTarget, STATGROUP_OdysseyCachedRenderTarget);
DECLARE_CYCLE_STAT(TEXT("OdysseyCachedRenderTarget LoadRenderTarget_Empty"), STAT_UOdysseyCachedRenderTarget_LoadRenderTarget_Empty, STATGROUP_OdysseyCachedRenderTarget);

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

    ENQUEUE_RENDER_COMMAND(UOdysseyCachedRenderTarget_Draw)(
        [source = SourceTexture, destination = mRenderTarget, sourceRect, destinationRect](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder graphBuilder(RHICmdList);

            SCOPE_CYCLE_COUNTER(STAT_UOdysseyCachedRenderTarget_Draw);
            DECLARE_GPU_STAT(UOdysseyCachedRenderTarget_Draw);
            {
                RDG_EVENT_SCOPE(graphBuilder, "UOdysseyCachedRenderTarget_Draw");
                RDG_GPU_STAT_SCOPE(graphBuilder, UOdysseyCachedRenderTarget_Draw);

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
            }
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
            ENQUEUE_RENDER_COMMAND(UOdysseyCachedRenderTarget_Render_Image)(
                [
                    //imageFuture = mImageFuture,
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

                    SCOPE_CYCLE_COUNTER(STAT_UOdysseyCachedRenderTarget_Render_Image);
                    DECLARE_GPU_STAT(UOdysseyCachedRenderTarget_Render_Image);
                    {
                        RDG_EVENT_SCOPE(graphBuilder, "UOdysseyCachedRenderTarget_Render_Image");
                        RDG_GPU_STAT_SCOPE(graphBuilder, UOdysseyCachedRenderTarget_Render_Image);

                        /*FRDGTextureDesc renderTextureDesc = FRDGTextureDesc::Create2D(
                            FIntPoint(width, height),
                            pixelFormat,
                            FClearValueBinding::Transparent,
                            ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable
                        );
                        FRDGTextureRef renderTexture = graphBuilder.CreateTexture(renderTextureDesc, TEXT("UOdysseyCachedRenderTarget::RenderTexture"));*/
                        FRDGTextureRef destinationTexture = destination->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );

                        /* auto* PassParameters = graphBuilder.AllocParameters<FRenderTargetParameters>();
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
                        ); */

                        FOdysseyTiledRLE::FRLEBuffer rleBuffer;
                        FOdysseyTiledRLE::FCompressionParams compressionParams;
                        compressionParams.TextureHeight = height;
                        compressionParams.TextureWidth = width;
                        compressionParams.TileWidth = 64;
                        compressionParams.TileHeight = 64;
                        compressionParams.PixelFormat = pixelFormat;

                        FRDGTextureRef renderTexture = FOdysseyTiledRLE::DecompressRenderThread(graphBuilder, rleBuffer, compressionParams);

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
            ENQUEUE_RENDER_COMMAND(UOdysseyCachedRenderTarget_Render_Empty)(
                [destination = Destination, destinationRect](FRHICommandListImmediate& RHICmdList)
                {
                    FRDGBuilder graphBuilder(RHICmdList);

                    SCOPE_CYCLE_COUNTER(STAT_UOdysseyCachedRenderTarget_Render_Empty);
                    DECLARE_GPU_STAT(UOdysseyCachedRenderTarget_Render_Empty);
                    {
                        RDG_EVENT_SCOPE(graphBuilder, "UOdysseyCachedRenderTarget_Render_Empty");
                        RDG_GPU_STAT_SCOPE(graphBuilder, UOdysseyCachedRenderTarget_Render_Empty);

                        FRDGTextureRef destinationTexture = destination->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );
                        AddClearRenderTargetPass(graphBuilder, destinationTexture, FLinearColor::Transparent, destinationRect);
                    }
                    graphBuilder.Execute();
                }
            );
        }
    }
    else if (mCacheState == eCacheState::RenderTarget)
    {
        ENQUEUE_RENDER_COMMAND(UOdysseyCachedRenderTarget_Render_RT)(
            [source = mRenderTarget, destination = Destination, sourceRect, destinationRect](FRHICommandListImmediate& RHICmdList)
            {
                FRDGBuilder graphBuilder(RHICmdList);

                SCOPE_CYCLE_COUNTER(STAT_UOdysseyCachedRenderTarget_Render_RT);
                DECLARE_GPU_STAT(UOdysseyCachedRenderTarget_Render_RT);
                {
                    RDG_EVENT_SCOPE(graphBuilder, "UOdysseyCachedRenderTarget_Render_RT");
                    RDG_GPU_STAT_SCOPE(graphBuilder, UOdysseyCachedRenderTarget_Render_RT);

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
                }
                graphBuilder.Execute();
            }
        );
    }
}

void
UOdysseyCachedRenderTarget::CopyRenderTargetToResetRT()
{
    FIntRect sourceRect(0, 0, Width, Height);
    FIntRect destinationRect(0, 0, Width, Height);

    ENQUEUE_RENDER_COMMAND(UOdysseyCachedRenderTarget_CopyRenderTargetToResetRT)(
        [source = mRenderTarget, destination = mResetRenderTarget, sourceRect, destinationRect](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder graphBuilder(RHICmdList);

            SCOPE_CYCLE_COUNTER(STAT_UOdysseyCachedRenderTarget_CopyRenderTargetToResetRT);
            DECLARE_GPU_STAT(UOdysseyCachedRenderTarget_CopyRenderTargetToResetRT);
            {
                RDG_EVENT_SCOPE(graphBuilder, "UOdysseyCachedRenderTarget_CopyRenderTargetToResetRT");
                RDG_GPU_STAT_SCOPE(graphBuilder, UOdysseyCachedRenderTarget_CopyRenderTargetToResetRT);

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
            }
            graphBuilder.Execute();
        }
    );
}

void
UOdysseyCachedRenderTarget::CopyResetRTToRenderTarget()
{
    FIntRect sourceRect(0, 0, Width, Height);
    FIntRect destinationRect(0, 0, Width, Height);

    ENQUEUE_RENDER_COMMAND(UOdysseyCachedRenderTarget_CopyResetRTToRenderTarget)(
        [source = mResetRenderTarget, destination = mRenderTarget, sourceRect, destinationRect](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder graphBuilder(RHICmdList);

            SCOPE_CYCLE_COUNTER(STAT_UOdysseyCachedRenderTarget_CopyResetRTToRenderTarget);
            DECLARE_GPU_STAT(UOdysseyCachedRenderTarget_CopyResetRTToRenderTarget);
            {
                RDG_EVENT_SCOPE(graphBuilder, "UOdysseyCachedRenderTarget_CopyResetRTToRenderTarget");
                RDG_GPU_STAT_SCOPE(graphBuilder, UOdysseyCachedRenderTarget_CopyResetRTToRenderTarget);

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
            }
            graphBuilder.Execute();
        }
    );
}

void
UOdysseyCachedRenderTarget::UnloadRenderTarget() const
{
    if (IsDrawing)
        return;

    if (mCacheState != eCacheState::RenderTarget)
        return;

    if (mIsImageCacheInvalid)
    {
        TSharedRef<TPromise<FUniqueBuffer>> promise = MakeShared<TPromise<FUniqueBuffer>>();

        ENQUEUE_RENDER_COMMAND(UOdysseyCachedRenderTarget_UnloadRenderTarget)(
            [
                promise,
                width = Width,
                height = Height,
                source = mRenderTarget,
                pixelFormat = GetPixelFormat(),
                stride = GetStride()
            ](FRHICommandListImmediate& RHICmdList)
            {
                SCOPE_CYCLE_COUNTER(STAT_UOdysseyCachedRenderTarget_UnloadRenderTarget);
                DECLARE_GPU_STAT(UOdysseyCachedRenderTarget_UnloadRenderTarget);

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
}

void
UOdysseyCachedRenderTarget::UnloadImage() const
{
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
}

void
UOdysseyCachedRenderTarget::LoadImage() const
{
    if (mCacheState != eCacheState::DDC)
        return;

    FOdysseyDiskCache cache(OdysseyCachedRenderTarget_CACHE_NAME, OdysseyCachedRenderTarget_CACHE_VERSION);
    FUniqueBuffer buffer;
    if (cache.Load(mId.ToString(), buffer))
    {
        TPromise<FUniqueBuffer> promise;
        promise.SetValue(MoveTemp(buffer));
        mImageFuture = promise.GetFuture().Share();
    }

    eCacheState oldState = mCacheState;
    mCacheState = eCacheState::Image;
    FOdysseyCachedRenderTargetManager::Get().UpdateCacheState(this, oldState, mCacheState);

}

void
UOdysseyCachedRenderTarget::LoadRenderTarget() const
{
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
        ENQUEUE_RENDER_COMMAND(UOdysseyCachedRenderTarget_LoadRenderTarget)(
            [imageFuture = mImageFuture, height = Height, destination = mRenderTarget, stride = GetStride()](FRHICommandListImmediate& RHICmdList)
            {
                SCOPE_CYCLE_COUNTER(STAT_UOdysseyCachedRenderTarget_LoadRenderTarget);
                DECLARE_GPU_STAT(UOdysseyCachedRenderTarget_LoadRenderTarget);

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
        ENQUEUE_RENDER_COMMAND(UOdysseyCachedRenderTarget_LoadRenderTarget_Empty)(
            [destination = mRenderTarget](FRHICommandListImmediate& RHICmdList)
            {
                SCOPE_CYCLE_COUNTER(STAT_UOdysseyCachedRenderTarget_LoadRenderTarget_Empty);
                DECLARE_GPU_STAT(UOdysseyCachedRenderTarget_LoadRenderTarget_Empty);

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
