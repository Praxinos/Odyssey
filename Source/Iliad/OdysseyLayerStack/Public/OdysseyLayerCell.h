// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyTextureRenderingAbility.h"
#include "OdysseyMediaProvider.h"
#include "OdysseyBlendingMode.h"

#include "OdysseyLayerCell.generated.h"

USTRUCT(BlueprintType)
struct FOdysseyLayerCellOutOfPegs
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Cell")
    FVector2D Pan = FVector2D(0, 0);

    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Cell")
    float Rotation = 0.f;

    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Cell")
    float Zoom = 100.f;
};

UCLASS(Abstract, BlueprintType, HideDropdown)
class ODYSSEYLAYERSTACK_API UOdysseyLayerCell
    : public UObject
    , public IOdysseyTextureRenderingAbility
{
    GENERATED_BODY()

#if WITH_EDITOR
public:
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnOutOfPegsChanged, bool /*iIsInteractive*/)
#endif

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

    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    void SetExposure(int Value);

    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    UTexture2D* GetTexture() const;

#if WITH_EDITOR
    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    int GetMark() const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    void SetMark(int Value);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    virtual UOdysseyLayerCell* Break(int Frame, bool bClear);

    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    bool IsOutOfPegs() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    FOdysseyLayerCellOutOfPegs GetOutOfPegs() const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    void SetOutOfPegs(FOdysseyLayerCellOutOfPegs Value);
#endif

public:
    virtual bool BuildRenderPipeline(
        FFrameNumber iFrame,
        uint64 iType,
        FOdysseyTextureRenderFunction& oRenderFunction
    ) const override;

#if WITH_EDITOR
    virtual FOdysseyMediaProvider GetMediaProvider(uint32 iFrameIndex) const;
#endif

public:
    // UObject overrides
    virtual void OldSerialize(FArchive& Ar); //DEPRECATED: Keep that for compatibility with early versions of Odyssey
#if WITH_EDITOR
    //Properties modifications
    void OutOfPegsChanged(bool iIsInteractive);

    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;
#endif

public:
#if WITH_EDITOR
    FMatrix OutOfPegsTransform() const;
    FOnOutOfPegsChanged& OnOutOfPegsChanged();
    void SetOutOfPegsInteractive(FOdysseyLayerCellOutOfPegs Value);
    void SetExposureInteractive(int Value);
    FSimpleMulticastDelegate& OnThumbnailChanged();
    FSimpleMulticastDelegate& OnThumbnailDirtied();
#endif

protected:
#if WITH_EDITOR
    void DirtyThumbnail();
    void UndirtyThumbnail();
    bool IsThumbnailDirty() const;
#endif

protected:
    friend class UOdysseyLayer;
    friend class FOdysseyLayerCellImport;

    UPROPERTY()
    int IndexInLayer = -1;

    UPROPERTY()//TODO: meta (minvalue 1)
    int Exposure = 1;

    UPROPERTY(NonTransactional)
    mutable TObjectPtr<UTexture2D> Texture; //mutable is temporary, will be removed when layers will be 100% GPU based and there's no more dependency on ULIS

#if WITH_EDITORONLY_DATA
    UPROPERTY()
    int Mark = -1;

    UPROPERTY(NonTransactional)
    bool ThumbnailIsDirty = false;

    UPROPERTY(NonTransactional, DuplicateTransient)
    FOdysseyLayerCellOutOfPegs OutOfPegs;
#endif

private:
#if WITH_EDITOR
    FSimpleMulticastDelegate mOnThumbnailChanged;
    FSimpleMulticastDelegate mOnThumbnailDirtied;

    FOnOutOfPegsChanged mOnOutOfPegsChanged;
#endif
};
