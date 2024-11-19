// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "OdysseyAnimationComponent.generated.h"

class UOdysseyAnimation;
class UOdysseyAnimationPlayer;

UENUM()
enum class EOdysseyAnimationComponentMode
{
    Animation,
    Player
};

/**
 * A component containing an animation to attach to an actor
 */
UCLASS(Blueprintable, ClassGroup=(Animation), editinlinenew, meta=(BlueprintSpawnableComponent, PrioritizeCategories="Actions"))
class ODYSSEYANIMATION_API UOdysseyAnimationComponent : public UStaticMeshComponent
{
    GENERATED_UCLASS_BODY()

public:
    UFUNCTION(Category="Actions", CallInEditor)
    void Play();

    UFUNCTION(Category="Actions", CallInEditor)
    void Stop();

public:
    UOdysseyAnimation* GetActiveAnimation() const;
    UOdysseyAnimationPlayer* GetActivePlayer() const;

    FSimpleMulticastDelegate& OnAnimationChanged();
    FSimpleMulticastDelegate& OnPlayerChanged();
    FSimpleMulticastDelegate& OnModeChanged();

public:
    virtual void PostInitProperties() override;
    virtual void PostLoad() override;
    virtual void PostDuplicate(bool bDuplicateForPIE) override;

    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;

protected:
    //Property changed methods
    virtual void PropertyChanged(const FName& iPropertyName);

    virtual void ModeChanged();
    virtual void AnimationChanged();
    virtual void PlayerChanged();

private:
    void RefreshMaterialTexture();

    void OnDefaultPlayerTextureChanged();
    void OnPlayerTextureChanged();

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
    EOdysseyAnimationComponentMode Mode = EOdysseyAnimationComponentMode::Animation;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="Animation", meta=(EditCondition = "Mode==EOdysseyAnimationComponentMode::Animation", EditConditionHides))
    TObjectPtr<UOdysseyAnimation> Animation;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="Animation", meta=(EditCondition = "Mode==EOdysseyAnimationComponentMode::Player", EditConditionHides))
    TObjectPtr<UOdysseyAnimationPlayer> Player;

private:
    UPROPERTY(Transient)
    TObjectPtr<UOdysseyAnimationPlayer> DefaultPlayer;

    UPROPERTY()
    TObjectPtr<UOdysseyAnimationPlayer> PreviousPlayer;

    FSimpleMulticastDelegate mOnAnimationChanged;
    FSimpleMulticastDelegate mOnPlayerChanged;
    FSimpleMulticastDelegate mOnModeChanged;
};
