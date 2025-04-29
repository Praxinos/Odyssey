// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "BaseMediaSource.h"
#include "OdysseyTextureRenderingAbility.h"

#include "OdysseyAnimation.generated.h"

class UOdysseyLayerStack;

UENUM()
enum class EOdysseyAnimationFormat : uint8
{
    BGRA8 UMETA(DisplayName = "BGRA 8"),
    RGBAF UMETA(DisplayName = "RGBA F")
};

UENUM()
enum class EOdysseyAnimationBoundMode : uint8
{
    Automatic,
    Manual
};

USTRUCT()
struct FOdysseyAnimationFrame
{
    GENERATED_BODY()

    UPROPERTY()
    UTexture2D* Texture = nullptr;

    UPROPERTY()
    int Exposure = 0;

    UPROPERTY()
    TArray<FGuid> RenderingComposition;
};

UCLASS(HideCategories=(Platforms))
class ODYSSEYANIMATION_API UOdysseyAnimation
    : public UBaseMediaSource
    , public IOdysseyTextureRenderingAbility
{
    GENERATED_BODY()

public:
#if WITH_EDITOR
    //Property changed methods
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;
    virtual void PropertyChanged(const FName& iPropertyName);
    virtual void PreSave(FObjectPreSaveContext SaveContext) override;
#endif

public:
    //~ UMediaSource interface
    virtual FString GetUrl() const override;
    virtual bool Validate() const override;

    //Size and Format
    int GetWidth() const;
    int GetHeight() const;
    EOdysseyAnimationFormat GetFormat() const;
    float GetFramesPerSecond() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Animation")
    FTimespan GetDuration() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Animation")
    virtual FInt32Range GetFrameRange() const override;

    UFUNCTION(BlueprintPure, Category="Odyssey|Animation")
    int GetFrameCount() const;

    int GetFrameIndexAtFrame(int iFrameIndex) const;

    //Time range of the frame at iFrameIndex
    TRange<FTimespan> GetFrameTimeRange(int iFrameIndex) const; //TODO: find a better way to have this function, only used by Media and ViewportDrawingEditor

    UFUNCTION(BlueprintPure, Category = "Odyssey|Animation")
    EOdysseyAnimationBoundMode GetLeftBoundMode() const;

    UFUNCTION(BlueprintPure, Category = "Odyssey|Animation")
    EOdysseyAnimationBoundMode GetRightBoundMode() const;

    UFUNCTION(BlueprintPure, Category = "Odyssey|Animation")
    int GetLeftBoundValue() const;

    UFUNCTION(BlueprintPure, Category = "Odyssey|Animation")
    int GetRightBoundValue() const;

    UFUNCTION(BlueprintCallable, Category = "Odyssey|Animation")
    void SetLeftBoundMode(EOdysseyAnimationBoundMode iMode);

    UFUNCTION(BlueprintCallable, Category = "Odyssey|Animation")
    void SetRightBoundMode(EOdysseyAnimationBoundMode iMode);

    UFUNCTION(BlueprintCallable, Category = "Odyssey|Animation")
    void SetLeftBoundValue(int iValue);

    UFUNCTION(BlueprintCallable, Category = "Odyssey|Animation")
    void SetRightBoundValue(int iValue);

#if WITH_EDITOR
public:
    void OnLeftBoundModeChanged();
    void OnRightBoundModeChanged();
    void OnLeftBoundChanged();
    void OnRightBoundChanged();

public:
    void Init(int iWidth, int iHeight, EOdysseyAnimationFormat iFormat, float iFramesPerSecond);

    //Getters
    void SetLayerStack(UOdysseyLayerStack* iLayerStack);
    UOdysseyLayerStack* GetLayerStack() const;
#endif

public:
    virtual TArray<FGuid> GetRenderingComposition(EOdysseyRenderingType iRenderType, int iFrameIndex) const override;
    virtual bool BuildRenderPipeline(
        FFrameNumber iFrame,
        EOdysseyRenderingType iType,
        FOdysseyTextureRenderFunction& oRenderFunction
    ) const override;
    virtual FIntRect GetDefaultRenderRect() const override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Odyssey|Animation", meta=(ClampMin=1, UIMin=1))
    float FramesPerSecond = 24.0f;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(DisplayName="Width"), Category="Odyssey|Animation")
    int mWidth = -1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(DisplayName="Height"), Category="Odyssey|Animation")
    int mHeight = -1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Odyssey|Animation")
    EOdysseyAnimationFormat Format = EOdysseyAnimationFormat::BGRA8;

    UPROPERTY()
    TArray<FOdysseyAnimationFrame> Frames;

    UPROPERTY(EditAnywhere, Category = "Odyssey|Animation", NonTransactional)
    bool PreserveLayerStackAtRuntime = false;

    UPROPERTY(BlueprintReadOnly, Category = "Odyssey|Animation", meta = (DisplayName = "Layer Stack", LoadBehavior = "LazyOnDemand"))
    TObjectPtr<UOdysseyLayerStack> mLayerStack;

    UPROPERTY(EditAnywhere, Category="Odyssey|Animation")
    EOdysseyAnimationBoundMode LeftBoundMode = EOdysseyAnimationBoundMode::Automatic;

    UPROPERTY(EditAnywhere, Category="Odyssey|Animation", meta=(EditCondition="LeftBoundMode != EOdysseyAnimationBoundMode::Automatic", EditConditionHides))
    int LeftBound = 0;

    UPROPERTY(EditAnywhere, Category="Odyssey|Animation")
    EOdysseyAnimationBoundMode RightBoundMode = EOdysseyAnimationBoundMode::Automatic;

    UPROPERTY(EditAnywhere, Category="Odyssey|Animation", meta=(EditCondition="RightBoundMode != EOdysseyAnimationBoundMode::Automatic", EditConditionHides))
    int RightBound = 0;
};
