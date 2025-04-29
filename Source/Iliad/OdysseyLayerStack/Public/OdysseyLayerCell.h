// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyTextureRenderingAbility.h"
#include "OdysseyMediaProvider.h"
#include "OdysseyBlendingMode.h"

#include "OdysseyLayerCell.generated.h"

UCLASS(Abstract, BlueprintType, HideDropdown)
class ODYSSEYLAYERSTACK_API UOdysseyLayerCell
    : public UObject
    , public IOdysseyTextureRenderingAbility
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    UOdysseyLayer* GetLayer() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    UOdysseyLayerStack* GetLayerStack() const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    FInt32Range GetFrameRange() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    int GetIndexInLayer() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    int GetExposure() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    int GetMark() const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    void SetExposure(int Value);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    void SetMark(int Value);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    virtual UOdysseyLayerCell* Break(int Frame, bool bClear);

public:
    virtual void RenderToTexture_RenderThread(FRDGBuilder& iGraphBuilder, FRDGTextureRef iDestinationTexture, ERHIFeatureLevel::Type iFeatureLevel, FFrameNumber iFrame, const FMatrix& iSrcTransform, const FIntRect& iSrcRect, const FIntRect& iDstRect) const override;
    virtual void BlendToTexture_RenderThread(FRDGBuilder& iGraphBuilder, FRDGTextureRef iDestinationTexture, ERHIFeatureLevel::Type iFeatureLevel, FFrameNumber iFrame, const FMatrix& iSrcTransform, const FIntRect& iSrcRect, const FIntRect& iDstRect, EOdysseyBlendingMode iBlendMode, float iOpacity) const;
    virtual FOdysseyMediaProvider GetMediaProvider(uint32 iFrameIndex) const;

public:
    // UObject overrides
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;
    virtual void OldSerialize(FArchive& Ar); //DEPRECATED: Keep that for compatibility with early versions of Odyssey

public:
#if WITH_EDITOR
    void SetExposureInteractive(int Value);
#endif
    FSimpleMulticastDelegate& OnThumbnailChanged();
    FSimpleMulticastDelegate& OnThumbnailDirtied();

protected:
    void DirtyThumbnail();
    void UndirtyThumbnail();
    bool IsThumbnailDirty() const;

protected:
    friend class UOdysseyLayer;
    friend class FOdysseyLayerCellImport;

    UPROPERTY()
    int IndexInLayer = -1;

    UPROPERTY()//TODO: meta (minvalue 1)
    int Exposure = 1;

    UPROPERTY()
    int Mark = -1;

private:
    UPROPERTY(NonTransactional)
    bool ThumbnailIsDirty = false;

private:
    FSimpleMulticastDelegate mOnThumbnailChanged;
    FSimpleMulticastDelegate mOnThumbnailDirtied;
};
