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

    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    void SetExposure(int Value);

#if WITH_EDITOR
    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    int GetMark() const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    void SetMark(int Value);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    virtual UOdysseyLayerCell* Break(int Frame, bool bClear);
#endif

public:
    virtual TSharedPtr<FOdysseyTextureRenderer> BuildTextureRenderer(FFrameNumber iFrame, TMap<const IOdysseyTextureRenderingAbility*, FGuid>* iIds = nullptr) const override;

#if WITH_EDITOR
    virtual FOdysseyMediaProvider GetMediaProvider(uint32 iFrameIndex) const;
#endif

public:
    // UObject overrides
#if WITH_EDITOR
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;
    virtual void OldSerialize(FArchive& Ar); //DEPRECATED: Keep that for compatibility with early versions of Odyssey
#endif

public:
#if WITH_EDITOR
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

#if WITH_EDITORONLY_DATA
    UPROPERTY()
    int Mark = -1;

    UPROPERTY(NonTransactional)
    bool ThumbnailIsDirty = false;
#endif

private:
#if WITH_EDITOR
    FSimpleMulticastDelegate mOnThumbnailChanged;
    FSimpleMulticastDelegate mOnThumbnailDirtied;
#endif
};
