// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyImageRenderer.h"
#include "OdysseyRenderingType.h"
#include <ULIS>

class UOdysseyAnimationCellImageRaster;

class ODYSSEYANIMATION_API FOdysseyAnimationCellImageRasterImageRenderer
    : public IOdysseyImageRenderer
    , public FGCObject
{
public:
    FOdysseyAnimationCellImageRasterImageRenderer(const UOdysseyAnimationCellImageRaster* iCell, int iFrame, EOdysseyRenderingType iRenderType, const TArray<FIntRect>& iDefaultRects, FImageRendererFilter iFilter);

public:
    virtual void Init() override;
    virtual void Lock() override;
    virtual void Unlock() override;
    virtual bool IsGameThreadOnly() override;

public:
    virtual TArray<::ULIS::FEvent> Blend(const FOdysseyImageRendererBlendParams& iParams, const TArray<::ULIS::FEvent>& iWaitList) override;
    virtual TArray<::ULIS::FEvent> Copy(const FOdysseyImageRendererCopyParams& iParams, const TArray<::ULIS::FEvent>& iWaitList) override;

public:
    // FGCObject implementation
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
    virtual FString GetReferencerName() const override;

public:
    const UOdysseyAnimationCellImageRaster* mCell;
    TSharedPtr<::ULIS::FBlock> mBlock;
    ::ULIS::FMat3F mOutOfPegsTransform;
};
