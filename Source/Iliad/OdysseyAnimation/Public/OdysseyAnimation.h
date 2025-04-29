// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "BaseMediaSource.h"
#include "OdysseyTextureRenderingAbility.h"

#include "OdysseyAnimation.generated.h"

class UOdysseyAnimationLayerStack;

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

    //Time range of the frame at iFrameIndex
    TRange<FTimespan> GetFrameTimeRange(int iFrameIndex) const; //TODO: find a better way to have this function, only used by Media and ViewportDrawingEditor

    void OnLeftBoundModeChanged();
    void OnRightBoundModeChanged();
    void OnLeftBoundChanged();
    void OnRightBoundChanged();

public:
#if WITH_EDITOR
    void Init(int iWidth, int iHeight, EOdysseyAnimationFormat iFormat, float iFramesPerSecond);

    //Getters
    void SetLayerStack(UObject* iLayerStack);
    UObject* GetLayerStack() const;

    EOdysseyAnimationBoundMode GetLeftBoundMode() const;
    EOdysseyAnimationBoundMode GetRightBoundMode() const;
    int GetLeftBoundValue() const;
    int GetRightBoundValue() const;

    void SetLeftBoundMode(EOdysseyAnimationBoundMode iMode);
    void SetRightBoundMode(EOdysseyAnimationBoundMode iMode);
    void SetLeftBoundValue(int iValue);
    void SetRightBoundValue(int iValue);
#endif

public:
    virtual TArray<FGuid> GetRenderingComposition(EOdysseyRenderingType iRenderType, int iFrameIndex) const override;

protected:
    virtual void RenderToTextureFromRects(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, const TArray<FIntRect>& iRects, const FIntPoint& iPos) const override;
    virtual TArray<FIntRect> GetRenderingRects() const override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Odyssey|Animation", meta=(ClampMin=1, UIMin=1))
    float FramesPerSecond = 24.0f;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(DisplayName="Width"), Category="Odyssey|Animation")
    int mWidth = -1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(DisplayName="Height"), Category="Odyssey|Animation")
    int mHeight = -1;

    UPROPERTY(BlueprintReadOnly, Category="Odyssey|Animation")
    EOdysseyAnimationFormat Format = EOdysseyAnimationFormat::BGRA8;

#if WITH_EDITORONLY_DATA
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Odyssey|Animation")
    EOdysseyAnimationBoundMode LeftBoundMode = EOdysseyAnimationBoundMode::Automatic;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Odyssey|Animation", meta=(EditCondition="LeftBoundMode != EOdysseyAnimationBoundMode::Automatic", EditConditionHides))
    int LeftBound = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Odyssey|Animation")
    EOdysseyAnimationBoundMode RightBoundMode = EOdysseyAnimationBoundMode::Automatic;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Odyssey|Animation", meta=(EditCondition="RightBoundMode != EOdysseyAnimationBoundMode::Automatic", EditConditionHides))
    int RightBound = 0;

    UPROPERTY(BlueprintReadOnly, Category="Odyssey|Animation", meta=(DisplayName="Layer Stack", LoadBehavior = "LazyOnDemand"))
    TObjectPtr<UObject> mLayerStack; //Editor Only
#endif
};
