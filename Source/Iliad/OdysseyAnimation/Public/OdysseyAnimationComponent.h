// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "Components/StaticMeshComponent.h"
#include "OdysseyAnimationComponent.generated.h"

class UOdysseyAnimation;
class UOdysseyAnimationPlayer;
class UMaterialInstanceDynamic;

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
    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationComponent")
    void InitializeFromAnimation(UOdysseyAnimation* iAnimation);

    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationComponent")
    void InitializeFromPlayer(UOdysseyAnimationPlayer* iPlayer);

    UFUNCTION(BlueprintPure, Category="Odyssey|AnimationComponent")
    UOdysseyAnimation* GetAnimation() const;

    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationComponent")
    UOdysseyAnimationPlayer* GetPlayer() const;

    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationComponent")
    EOdysseyAnimationComponentMode GetMode() const;

    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationComponent")
    UMaterialInterface* GetAnimationMaterial() const;

    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationComponent")
    void SetAnimation(UOdysseyAnimation* iAnimation);

    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationComponent")
    void SetPlayer(UOdysseyAnimationPlayer* iPlayer);

    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationComponent")
    void SetMode(EOdysseyAnimationComponentMode iMode);

    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationComponent")
    void SetAnimationMaterial(UMaterialInterface* iMaterial);

public:
    virtual void PostLoad() override;
    virtual void OnRegister() override;
    virtual void OnUnregister() override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;
    virtual void PropertyChanged(const FName& iPropertyName);
#endif

protected:
    //Property changed methods
    void Initialize();
    void ModeChanged();
    void AnimationChanged();
    void PlayerChanged();
    void LODGroupChanged();
    void MaterialChanged();
    void CreateMaterialInstance();

    void OnPlayerAnimationChanged();
    void RescaleToMatchAnimation(UOdysseyAnimation* iAnimation);

private:
    void RefreshMaterialTexture();

protected:
    UPROPERTY( EditAnywhere, Category="Animation")
    TObjectPtr<UMaterialInterface> Material;

    UPROPERTY(EditAnywhere, Category="Animation")
    EOdysseyAnimationComponentMode Mode = EOdysseyAnimationComponentMode::Animation;

    UPROPERTY( EditAnywhere, Category="Animation", meta=(EditCondition = "Mode==EOdysseyAnimationComponentMode::Animation", EditConditionHides))
    TObjectPtr<UOdysseyAnimation> Animation;

    UPROPERTY()
    TObjectPtr<UOdysseyAnimationPlayer> PreviousPlayer; //Only used to remove delegates from Player property when it changes

    UPROPERTY( EditAnywhere, Category="Animation", meta=(EditCondition = "Mode==EOdysseyAnimationComponentMode::Player", EditConditionHides))
    TObjectPtr<UOdysseyAnimationPlayer> Player;

    UPROPERTY( EditAnywhere, Category="Animation", meta=(DisplayName="Texture Group"), AssetRegistrySearchable )
    TEnumAsByte<enum TextureGroup> LODGroup = TEXTUREGROUP_Pixels2D;

private:
    UPROPERTY()
    TObjectPtr<UOdysseyAnimationPlayer> DefaultPlayer;

    UPROPERTY(Transient, DuplicateTransient)
    TObjectPtr<UMaterialInstanceDynamic> MaterialInstance;
};
