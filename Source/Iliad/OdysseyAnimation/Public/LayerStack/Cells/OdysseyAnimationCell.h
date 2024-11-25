// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyImageRenderingAbility.h"
#include "OdysseyMediaProvider.h"
#include "OdysseyRasterBlock.h"
#include "OdysseyAnimationCell.generated.h"

class UOdysseyAnimationLayer;
class FOdysseyRasterBlock;

USTRUCT(BlueprintType)
struct FOdysseyAnimationCellOutOfPegs
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Cell")
    FVector2D Pan = FVector2D(0, 0);

    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Cell")
    float Rotation = 0.f;

    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Cell")
    float Zoom = 100.f; //TODO:
};

UCLASS(Abstract, BlueprintType, HideDropdown)
class ODYSSEYANIMATION_API UOdysseyAnimationCell
    : public UObject
    , public FOdysseyImageRenderingAbility
{
    GENERATED_BODY()

public:
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnOutOfPegsChanged, bool /*iIsInteractive*/)

public:
    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    UOdysseyAnimationLayer* GetLayer() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    UOdysseyAnimation* GetAnimation() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    UOdysseyAnimationLayerStack* GetLayerStack() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    bool IsOutOfPegs() const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    FInt32Range GetFrameRange() const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    virtual UOdysseyAnimationCell* Break(int Frame, bool bClear);

public:
    virtual FOdysseyMediaProvider GetMediaProvider(uint32 iFrameIndex) const;

public:
    //OutOfPegs
    ::ULIS::FMat3F OutOfPegsTransform() const;
    FOnOutOfPegsChanged& OnOutOfPegsChanged();

public:
    // UObject overrides
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;
    virtual void OldSerialize(FArchive& Ar); //DEPRECATED: Keep that for compatibility with early versions of Odyssey

public:
    FSimpleMulticastDelegate& OnThumbnailChanged();
    FSimpleMulticastDelegate& OnThumbnailDirtied();

protected:
    void DirtyThumbnail();
    void UndirtyThumbnail();
    bool IsThumbnailDirty() const;

protected:
    //Properties modifications
    void OutOfPegsChanged(bool iIsInteractive);
    void ExposureChanged(bool iIsInteractive);

    virtual void PropertyChanged(const FName& iPropertyName, const FName& iMemberPropertyName, bool iIsInteractive);

private:
    UFUNCTION(BlueprintSetter)
    void ExposureBlueprintSetter(int Value);

    UFUNCTION(BlueprintSetter)
    void MarkBlueprintSetter(int Value);

    UFUNCTION(BlueprintSetter)
    void OutOfPegsBlueprintSetter(FOdysseyAnimationCellOutOfPegs Value);

public:
    UPROPERTY(BlueprintReadOnly, Category="Odyssey|Cell")
    int IndexInLayer = -1;

    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Cell", BlueprintSetter=ExposureBlueprintSetter)//TODO: meta (minvalue 1)
    int Exposure = 1;

    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Cell", BlueprintSetter=MarkBlueprintSetter) //TODO: GetOptions ? Is that possible ?
    int Mark = -1;

    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Cell", BlueprintSetter=OutOfPegsBlueprintSetter, NonTransactional, DuplicateTransient)
    FOdysseyAnimationCellOutOfPegs OutOfPegs;

private:
    friend class UOdysseyAnimationCellThumbnailRenderer;
    friend class FOdysseyAnimationCellThumbnailProxy;
    TSharedPtr<FOdysseyRasterBlock> mThumbnail;  //TODO: save/load mThumbnail AND mThumbnailIsDirty with the cell

    UPROPERTY(NonTransactional)
    bool ThumbnailIsDirty = false;

private:
    FOnOutOfPegsChanged mOnOutOfPegsChanged;
    FSimpleMulticastDelegate mOnThumbnailChanged;
    FSimpleMulticastDelegate mOnThumbnailDirtied;
};
