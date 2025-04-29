// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyLayerCell.h"
#include "OdysseyAnimation.h"
#include <ULIS>

#include "OdysseyAnimationCell.generated.h"

USTRUCT(BlueprintType)
struct FOdysseyAnimationCellOutOfPegs
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
class ODYSSEYLAYERSTACKEDITOR_API UOdysseyAnimationCell
    : public UOdysseyLayerCell
{
    GENERATED_BODY()

public:
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnOutOfPegsChanged, bool /*iIsInteractive*/)

public:
    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    UOdysseyAnimation* GetAnimation() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    bool IsOutOfPegs() const;

public:
    //OutOfPegs
    ::ULIS::FMat3F OutOfPegsTransform() const;
    FOnOutOfPegsChanged& OnOutOfPegsChanged();

public:
    // UObject overrides
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;

protected:
    //Properties modifications
    void OutOfPegsChanged(bool iIsInteractive);

    virtual void PropertyChanged(const FName& iPropertyName, const FName& iMemberPropertyName, bool iIsInteractive);
    virtual void PostPropertyChanged(const FName& iPropertyName, bool iIsInteractive);

private:
    UFUNCTION(BlueprintSetter)
    void OutOfPegsBlueprintSetter(FOdysseyAnimationCellOutOfPegs Value);

public:
    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Cell", BlueprintSetter=OutOfPegsBlueprintSetter, NonTransactional, DuplicateTransient)
    FOdysseyAnimationCellOutOfPegs OutOfPegs;

private:
    FOnOutOfPegsChanged mOnOutOfPegsChanged;
};
