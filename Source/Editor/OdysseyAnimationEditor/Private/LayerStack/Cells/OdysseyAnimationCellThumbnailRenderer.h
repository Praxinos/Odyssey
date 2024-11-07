// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Containers/Queue.h"
#include "HAL/Runnable.h"
#include "ThumbnailRendering/ThumbnailRenderer.h"
#include "OdysseyImageRenderingAbility.h"
#include "UObject/GCObject.h"

#include "OdysseyAnimationCellThumbnailRenderer.generated.h"

class UOdysseyAnimationCell;
class UTexture2D;

struct FOdysseyAnimationCellThumbnailTask
{
    UOdysseyAnimationCell* mCell;
    int mCellWidth;
    int mCellHeight;
    ::ULIS::eFormat mCellFormat;
};

UCLASS()
class UOdysseyAnimationCellThumbnailRenderer
    : public UThumbnailRenderer
{
    GENERATED_BODY()

public:
    virtual void PostInitProperties() override;

public:
    virtual void GetThumbnailSize(UObject* Object, float Zoom, uint32& OutWidth, uint32& OutHeight) const override;
    virtual void Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget* Viewport, FCanvas* Canvas, bool bAdditionalViewFamily) override;
    virtual EThumbnailRenderFrequency GetThumbnailRenderFrequency(UObject* Object) const override { return EThumbnailRenderFrequency::Realtime; }

private:
    UPROPERTY()
    TMap<FVector2D, UTexture2D*> Textures;
    
    UPROPERTY()
    TObjectPtr<UTexture2D> mCheckerboardTexture;
};

class FOdysseyAnimationCellThumbnailProxy
    : public FRunnable
    , public FGCObject
{
public:
    static FOdysseyAnimationCellThumbnailProxy& Get();

    virtual ~FOdysseyAnimationCellThumbnailProxy();

private:
    FOdysseyAnimationCellThumbnailProxy();

public:
    virtual bool Init() override;
    virtual uint32 Run() override;
    virtual void Stop() override;

public:
    // FGCObject implementation
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
    virtual FString GetReferencerName() const override;

public:
    void InvalidateCell(UOdysseyAnimationCell* iCell);
    FCriticalSection& GetMutex();

private:
    // Thread to run the worker FRunnable on
    FRunnableThread* mThread;
    
    // Stop this thread? Uses Thread Safe Counter
    FThreadSafeCounter mStopTaskCounter;
    
    FCriticalSection mMutex;

    TMap<UOdysseyAnimationCell*, TSharedPtr<IOdysseyImageRenderer>> mRenderers;
    TQueue<FOdysseyAnimationCellThumbnailTask, EQueueMode::Mpsc> mQueue;

    FCriticalSection mFinishedCellsMutex;
    TArray<UOdysseyAnimationCell*> mFinishedCells;
};
