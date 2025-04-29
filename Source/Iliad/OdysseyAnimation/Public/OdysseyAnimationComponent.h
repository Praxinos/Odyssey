// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "Components/StaticMeshComponent.h"
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
UCLASS(Blueprintable, ClassGroup=(Animation), HideCategories=(Materials), editinlinenew, meta=(BlueprintSpawnableComponent, PrioritizeCategories="Actions"))
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
    EOdysseyAnimationComponentMode GetMode() const;

    void SetAnimation(UOdysseyAnimation* iAnimation);
    void SetPlayer(UOdysseyAnimationPlayer* iPlayer);
    void SetMode(EOdysseyAnimationComponentMode iMode);

public:
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

    void MaterialChanged();

private:
    void RefreshMaterialTexture();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
    EOdysseyAnimationComponentMode Mode = EOdysseyAnimationComponentMode::Animation;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="Animation", meta=(EditCondition = "Mode==EOdysseyAnimationComponentMode::Animation", EditConditionHides))
    TObjectPtr<UOdysseyAnimation> Animation;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="Animation", meta=(EditCondition = "Mode==EOdysseyAnimationComponentMode::Player", EditConditionHides))
    TObjectPtr<UOdysseyAnimationPlayer> Player;

    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="Animation")
    TObjectPtr<UMaterialInterface> Material;

private:
    UPROPERTY()
    TObjectPtr<UOdysseyAnimationPlayer> DefaultPlayer;

    UPROPERTY()
    TObjectPtr<UMaterialInstanceConstant> MaterialInstance;
};
