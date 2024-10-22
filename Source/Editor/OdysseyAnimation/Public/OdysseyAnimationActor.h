// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "OdysseyAnimationActor.generated.h"

class UOdysseyAnimation;
class UOdysseyAnimationComponent;
class UOdysseyAnimationPlayer;
//class UOdysseyAnimationTexture;

UENUM()
enum class EOdysseyAnimationActorMode
{
	Animation,
	Player
	//Texture
};

/**
 * An instance of a UOdysseyAnimation in a level.
 *
 * This actor is created when you drag a animation asset from the content browser into the level, and
 * it is just a thin wrapper around a UOdysseyAnimationComponent that actually references the asset.
 */
UCLASS(ComponentWrapperClass, meta=(PrioritizeCategories="Actions"))
class ODYSSEYANIMATION_API AOdysseyAnimationActor : public AActor
{
	GENERATED_UCLASS_BODY()

public:
	// AActor interface
#if WITH_EDITOR
	virtual bool GetReferencedContentObjects(TArray<UObject*>& Objects) const override;
#endif
	// End of AActor interface

public:
	UFUNCTION(Category="Actions", CallInEditor)
	void FaceCamera();

	UFUNCTION(Category="Actions", CallInEditor)
	void MoveInFrontOfCamera();

	UFUNCTION(Category="Actions", CallInEditor)
	void Play();

	UFUNCTION(Category="Actions", CallInEditor)
	void Stop();

public:
	UOdysseyAnimationPlayer* GetActivePlayer() const;

public:
	virtual void PostInitProperties() override;
	virtual void PostLoad() override;

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Animation")
	TObjectPtr<class UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
	EOdysseyAnimationActorMode Mode = EOdysseyAnimationActorMode::Animation;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="Animation", meta=(EditCondition = "Mode==EOdysseyAnimationActorMode::Animation", EditConditionHides))
	TObjectPtr<UOdysseyAnimation> Animation;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="Animation", meta=(EditCondition = "Mode==EOdysseyAnimationActorMode::Player", EditConditionHides))
	TObjectPtr<UOdysseyAnimationPlayer> Player;

private:
	UPROPERTY()
	TObjectPtr<UOdysseyAnimationPlayer> DefaultPlayer;

	//UPROPERTY()
	//TObjectPtr<UOdysseyAnimationTexture> DefaultTexture;

	UPROPERTY()
	TObjectPtr<UOdysseyAnimationPlayer> PreviousPlayer;
};
