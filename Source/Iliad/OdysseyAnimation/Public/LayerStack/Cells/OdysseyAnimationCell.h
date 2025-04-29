// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyLayerCell.h"
#include "OdysseyAnimation.h"

#if WITH_EDITOR
#include <ULIS>
#endif

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
class ODYSSEYANIMATION_API UOdysseyAnimationCell
    : public UOdysseyLayerCell
{
    GENERATED_BODY()

public:
#if WITH_EDITOR
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnOutOfPegsChanged, bool /*iIsInteractive*/)
#endif

public:
    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    UOdysseyAnimation* GetAnimation() const;

#if WITH_EDITOR
public:
    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    bool IsOutOfPegs() const;

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

private:
    FOnOutOfPegsChanged mOnOutOfPegsChanged;
#endif

#if WITH_EDITORONLY_DATA
public:
    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Cell", BlueprintSetter=OutOfPegsBlueprintSetter, NonTransactional, DuplicateTransient)
    FOdysseyAnimationCellOutOfPegs OutOfPegs;
#endif
};
