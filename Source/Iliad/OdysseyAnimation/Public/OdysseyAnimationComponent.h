// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "Components/StaticMeshComponent.h"
#include "Engine/TextureDefines.h"
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

UENUM()
enum class EOdysseyAnimationComponentMeasurement
{
    Height,
    Width
};

USTRUCT(BlueprintType)
struct FOdysseyAnimationComponentForceRatio
{
    GENERATED_BODY()

    /**
     * Size of the reference side of the animation, in scale units.
     *
     * The other side is scaled to keep the animation ratio.
     */
    UPROPERTY(EditAnywhere, Category="Transform")
    float BaseSize = 100.f;

    /** Side of the animation (height or width) that Base Size applies to. */
    UPROPERTY(EditAnywhere, Category="Transform")
    EOdysseyAnimationComponentMeasurement BaseMeasurement = EOdysseyAnimationComponentMeasurement::Height;
};

/**
 * A component containing an animation to attach to an actor
 */
UCLASS(Blueprintable, ClassGroup=(Animation), HideCategories=(Materials), editinlinenew, meta=(BlueprintSpawnableComponent, PrioritizeCategories="Animation"))
class ODYSSEYANIMATION_API UOdysseyAnimationComponent : public UStaticMeshComponent
{
    GENERATED_UCLASS_BODY()

public:
    /**
     * Initialize the component from an Animation 2D.
     *
     * The component displays the given Animation 2D using its own player,
     * and the Player property is cleared.
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationComponent")
    void InitializeFromAnimation(UOdysseyAnimation* Animation);

    /**
     * Initialize the component from an Animation Player.
     *
     * The component displays the given Animation Player,
     * and the Animation property is cleared.
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationComponent")
    void InitializeFromPlayer(UOdysseyAnimationPlayer* Player);

    /** Return the Animation 2D displayed by the component. */
    UFUNCTION(BlueprintPure, Category="Odyssey|AnimationComponent")
    UOdysseyAnimation* GetAnimation() const;

    /** Return the Animation Player used by the component, in Player mode. */
    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationComponent")
    UOdysseyAnimationPlayer* GetPlayer() const;

    /** Return whether the component uses an Animation or a Player. */
    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationComponent")
    EOdysseyAnimationComponentMode GetMode() const;

    /** Return the Material used to display the animation. */
    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationComponent")
    UMaterialInterface* GetAnimationMaterial() const;

    /** Return whether the animation ratio is forced on the component. */
    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationComponent")
    bool GetForceAnimationRatio() const;

    /** Return the settings used when forcing the animation ratio. */
    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationComponent")
    FOdysseyAnimationComponentForceRatio GetForceAnimationRatioParameters() const;

    /** Set the Animation 2D displayed by the component, in Animation mode. */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationComponent")
    void SetAnimation(UOdysseyAnimation* Animation);

    /** Set the Animation Player used by the component, in Player mode. */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationComponent")
    void SetPlayer(UOdysseyAnimationPlayer* Player);

    /** Set whether the component uses an Animation or a Player. */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationComponent")
    void SetMode(EOdysseyAnimationComponentMode Mode);

    /**
     * Set the Material used to display the animation.
     *
     * The Material should contain a Texture Sample Parameter
     * named "AnimationTexture".
     */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationComponent")
    void SetAnimationMaterial(UMaterialInterface* Material);

    /** Enable or disable forcing the animation ratio on the component. */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationComponent")
    void SetForceAnimationRatio(bool ForceRatio);

    /** Set the settings used when forcing the animation ratio. */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationComponent")
    void SetForceAnimationRatioParameters(FOdysseyAnimationComponentForceRatio Parameters);

public:
    virtual void PostLoad() override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;

    virtual void PropertyChanged(const FName& iPropertyName, bool iIsInteractive);
#endif
    virtual void OnRegister() override;
protected:
    //Property changed methods
    void ModeChanged();
    void AnimationChanged();
    void PlayerChanged();
    void LODGroupChanged();
    void MaterialChanged();
    void ForceAnimationRatioChanged();

    void OnPlayerAnimationChanged();
    void RescaleToMatchAnimation(UOdysseyAnimation* iAnimation);

private:
    virtual TStructOnScope<FActorComponentInstanceData> GetComponentInstanceData() const override;
    void ApplyComponentInstanceData(struct FOdysseyAnimationComponentInstanceData* ComponentInstanceData);
    void UpdateMaterialInstance( bool iForceRecreate = false );

protected:
    /**
     * Material used to display the animation.
     *
     * Make sure it contains at least one Texture Sample Parameter
     * named "AnimationTexture".
     */
    UPROPERTY( EditAnywhere, Category="Animation")
    TObjectPtr<UMaterialInterface> Material;

    /**
     * Source of the displayed animation.
     *
     * Animation: displays an Animation 2D asset directly.
     * Player: displays the output of an Animation Player.
     */
    UPROPERTY(EditAnywhere, Category="Animation")
    EOdysseyAnimationComponentMode Mode = EOdysseyAnimationComponentMode::Animation;

    /** The Animation 2D asset displayed by the component. */
    UPROPERTY( EditAnywhere, Category="Animation", meta=(EditCondition = "Mode==EOdysseyAnimationComponentMode::Animation", EditConditionHides))
    TObjectPtr<UOdysseyAnimation> Animation;

    UPROPERTY()
    TObjectPtr<UOdysseyAnimationPlayer> PreviousPlayer; //Only used to remove delegates from Player property when it changes

    /** The Animation Player whose output is displayed by the component. */
    UPROPERTY( EditAnywhere, Category="Animation", meta=(EditCondition = "Mode==EOdysseyAnimationComponentMode::Player", EditConditionHides))
    TObjectPtr<UOdysseyAnimationPlayer> Player;

    /**
     * Texture Group used to render the animation.
     *
     * Choose whether the pixels of the animation blend with the World
     * or are shown unfiltered (2D pixels), for instance for pixel art.
     */
    UPROPERTY( EditAnywhere, Category="Animation", meta=(DisplayName="Texture Group"), AssetRegistrySearchable )
    TEnumAsByte<enum TextureGroup> LODGroup = TEXTUREGROUP_Pixels2D;

    /** Enable Force Animation Ratio. */
    UPROPERTY(EditAnywhere, Category="Animation", meta=(InlineEditConditionToggle))
    bool bForceAnimationRatio = true;

    /**
     * Force the animation ratio.
     *
     * Resize the component to match the animation ratio, making it bigger
     * or smaller without changing the scale parameters in Transform.
     */
    UPROPERTY(EditAnywhere, Category="Animation", meta=(EditCondition = "bForceAnimationRatio"))
    FOdysseyAnimationComponentForceRatio ForceAnimationRatio;

private:
    UPROPERTY(Instanced)
    TObjectPtr<UOdysseyAnimationPlayer> DefaultPlayer;

private:
    friend struct FOdysseyAnimationComponentInstanceData;
};

//PATCH : COPIED FROM Engine\Source\Runtime\Engine\Classes\Components\StaticMeshComponent.h
//Needed because FStaticMeshComponentInstanceData does not set ENGINE_API on its functions
USTRUCT()
struct FOdysseyStaticMeshComponentInstanceDataPATCH : public FPrimitiveComponentInstanceData
{
    GENERATED_BODY()
public:
    FOdysseyStaticMeshComponentInstanceDataPATCH() = default;
    FOdysseyStaticMeshComponentInstanceDataPATCH(const UStaticMeshComponent* SourceComponent);
    virtual ~FOdysseyStaticMeshComponentInstanceDataPATCH() = default;

    virtual bool ContainsData() const override;

    virtual void ApplyToComponent(UActorComponent* Component, const ECacheApplyPhase CacheApplyPhase) override;

    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

    /** Add vertex color data for a specified LOD before RerunConstructionScripts is called */
    void AddVertexColorData(const struct FStaticMeshComponentLODInfo& LODInfo, uint32 LODIndex);

    /** Re-apply vertex color data after RerunConstructionScripts is called */
    bool ApplyVertexColorData(UStaticMeshComponent* StaticMeshComponent) const;

    /** Mesh being used by component */
    UPROPERTY()
    TObjectPtr<class UStaticMesh> StaticMesh = nullptr;

    /** Array of cached vertex colors for each LOD */
    UPROPERTY()
    TArray<FStaticMeshVertexColorLODData> VertexColorLODs;

    /** Used to store lightmap data during RerunConstructionScripts */
    UPROPERTY()
    TArray<FGuid> CachedStaticLighting;

    /** Texture streaming build data */
    UPROPERTY()
    TArray<FStreamingTextureBuildInfo> StreamingTextureData;

#if WITH_EDITORONLY_DATA
    /** Texture streaming editor data (for viewmodes) */
    UPROPERTY()
    TArray<uint32> MaterialStreamingRelativeBoxes;
#endif
};


/** Used to store data during RerunConstructionScripts */
USTRUCT()
struct FOdysseyAnimationComponentInstanceData : public FOdysseyStaticMeshComponentInstanceDataPATCH
{
    GENERATED_BODY()
public:
    FOdysseyAnimationComponentInstanceData() = default;
    explicit FOdysseyAnimationComponentInstanceData(const UOdysseyAnimationComponent* SourceComponent);

    virtual ~FOdysseyAnimationComponentInstanceData() override = default;

    virtual bool ContainsData() const override
    {
        return true;
    }

    virtual void ApplyToComponent(UActorComponent* Component, const ECacheApplyPhase CacheApplyPhase) override
    {
        Super::ApplyToComponent(Component, CacheApplyPhase);
        CastChecked<UOdysseyAnimationComponent>(Component)->ApplyComponentInstanceData(this);
    }

    UPROPERTY()
    TObjectPtr<UMaterialInterface> Material;

    UPROPERTY()
    EOdysseyAnimationComponentMode Mode = EOdysseyAnimationComponentMode::Animation;

    UPROPERTY()
    TObjectPtr<UOdysseyAnimation> Animation;

    UPROPERTY()
    TObjectPtr<UOdysseyAnimationPlayer> Player;

    UPROPERTY()
    TEnumAsByte<enum TextureGroup> LODGroup = TEXTUREGROUP_Pixels2D;

    UPROPERTY()
    TObjectPtr<UOdysseyAnimationPlayer> DefaultPlayer;

    UPROPERTY()
    bool bForceAnimationRatio = true;

    UPROPERTY()
    FOdysseyAnimationComponentForceRatio ForceAnimationRatio;
};
