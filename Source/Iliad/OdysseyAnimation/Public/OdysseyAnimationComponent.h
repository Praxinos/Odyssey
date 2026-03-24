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

UENUM()
enum class EOdysseyAnimationComponentScaling
{
    AdjustWidth,
    AdjustHeight
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
    void InitializeFromAnimation(UOdysseyAnimation* Animation);

    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationComponent")
    void InitializeFromPlayer(UOdysseyAnimationPlayer* Player);

    UFUNCTION(BlueprintPure, Category="Odyssey|AnimationComponent")
    UOdysseyAnimation* GetAnimation() const;

    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationComponent")
    UOdysseyAnimationPlayer* GetPlayer() const;

    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationComponent")
    EOdysseyAnimationComponentMode GetMode() const;

    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationComponent")
    UMaterialInterface* GetAnimationMaterial() const;

    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationComponent")
    bool GetAutoScale() const;

    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationComponent")
    float GetAutoScaleSize() const;

    UFUNCTION(BlueprintPure, Category = "Odyssey|AnimationComponent")
    EOdysseyAnimationComponentScaling GetAutoScaleMode() const;

    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationComponent")
    void SetAnimation(UOdysseyAnimation* Animation);

    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationComponent")
    void SetPlayer(UOdysseyAnimationPlayer* Player);

    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationComponent")
    void SetMode(EOdysseyAnimationComponentMode Mode);

    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationComponent")
    void SetAnimationMaterial(UMaterialInterface* Material);

    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationComponent")
    void SetAutoScale(bool Value);

    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationComponent")
    void SetAutoScaleSize(float Size);

    UFUNCTION(BlueprintCallable, Category = "Odyssey|AnimationComponent")
    void SetAutoScaleMode(EOdysseyAnimationComponentScaling Mode);

public:
    virtual void PostInitProperties() override;
    virtual void PostReinitProperties() override;
    virtual void PostDuplicate(bool bDuplicateForPIE) override;
    virtual void PostLoad() override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;
    virtual void PostEditImport() override;

    virtual void PropertyChanged(const FName& iPropertyName, bool iIsInteractive);
#endif

    virtual void OnComponentCreated() override;
protected:
    //Property changed methods
    //void Initialize();
    void ModeChanged();
    void AnimationChanged();
    void PlayerChanged();
    void LODGroupChanged();
    void MaterialChanged();
    void AutoScaleChanged();

    void OnDefaultPlayerRenderTargetChanged();
    void OnPlayerAnimationChanged();
    void RescaleToMatchAnimation(UOdysseyAnimation* iAnimation);

private:
    virtual TStructOnScope<FActorComponentInstanceData> GetComponentInstanceData() const override;
    void ApplyComponentInstanceData(struct FOdysseyAnimationComponentInstanceData* ComponentInstanceData);
    void UpdateMaterialInstance();

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

    UPROPERTY(EditAnywhere, Category="Animation")
    bool AutoScale = true;

    UPROPERTY(EditAnywhere, Category="Animation", meta=(EditCondition = "AutoScale", EditConditionHides))
    float AutoScaleSize = 100.f;

    UPROPERTY(EditAnywhere, Category="Animation", meta=(EditCondition = "AutoScale", EditConditionHides))
    EOdysseyAnimationComponentScaling AutoScaleMode = EOdysseyAnimationComponentScaling::AdjustWidth;

private:
    UPROPERTY(Instanced)
    TObjectPtr<UOdysseyAnimationPlayer> DefaultPlayer;

    //bool bNeedUpdateMaterialInstance = true;

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

    UPROPERTY(EditAnywhere, Category="Animation")
    bool AutoScale = true;

    UPROPERTY(EditAnywhere, Category="Animation")
    float AutoScaleSize = 100.f;

    UPROPERTY(EditAnywhere, Category="Animation")
    EOdysseyAnimationComponentScaling AutoScaleMode = EOdysseyAnimationComponentScaling::AdjustWidth;
};
