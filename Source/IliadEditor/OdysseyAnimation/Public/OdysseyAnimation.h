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

UCLASS(config=EditorPerProjectUserSettings, PerObjectConfig, HideCategories=(Platforms))
class ODYSSEYANIMATION_API UOdysseyAnimation
    : public UBaseMediaSource
    , public FOdysseyTextureRenderingAbility
{
    GENERATED_BODY()

public:
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentFrameChanged, UOdysseyAnimation*)
    static FOnCurrentFrameChanged& OnCurrentFrameChanged();

public:
    //UObject overrides
    /**
     * @brief Serialize this object
     *
     * @param Ar
     */
    virtual void PostInitProperties() override;
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;
    virtual void PostLoad() override;

    //Property changed methods
    virtual void PropertyChanged(const FName& iPropertyName);
    virtual void PostPropertyChanged(const FName& iPropertyName);

public:
    //~ UMediaSource interface
    virtual FString GetUrl() const override;
    virtual bool Validate() const override;

public:
    //Getters
    UOdysseyAnimationLayerStack* GetLayerStack() const;

    //Size and Format
    int GetWidth() const;
    int GetHeight() const;
    EOdysseyAnimationFormat GetFormat() const;
    double GetFramesPerSecond() const;

public:
    //Editor Only ?
    EOdysseyAnimationBoundMode GetLeftBoundMode() const;
    EOdysseyAnimationBoundMode GetRightBoundMode() const;
    int GetLeftBoundValue() const;
    int GetRightBoundValue() const;

    void SetLeftBoundMode(EOdysseyAnimationBoundMode iMode);
    void SetRightBoundMode(EOdysseyAnimationBoundMode iMode);
    void SetLeftBoundValue(int iValue);
    void SetRightBoundValue(int iValue);

public:
    UFUNCTION(BlueprintPure, Category="Odyssey|Animation")
    FTimespan GetDuration() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Animation")
    FInt32Range GetFrameRange() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Animation")
    int GetFrameCount() const;

    //Time range of the frame at iFrameIndex
    TRange<FTimespan> GetFrameTimeRange(int iFrameIndex) const; //TODO: find a better way to have this function, only used by Media and ViewportDrawingEditor

public:
    virtual void RenderToTextureFromRects(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, const TArray<FIntRect>& iRects) const override;
    virtual TArray<FGuid> GetRenderingComposition(EOdysseyRenderingType iRenderType, int iFrameIndex) const override;
    virtual TArray<FIntRect> GetRenderingRects() const override;

private:
    UFUNCTION(BlueprintSetter)
    void CurrentFrameBlueprintSetter(int Value);

    UFUNCTION(BlueprintSetter)
    void FramesPerSecondBlueprintSetter(float Value);

public:
    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Animation", BlueprintSetter=CurrentFrameBlueprintSetter, DuplicateTransient, NonTransactional, meta=(ClampMin=0, UIMin=0))
    int CurrentFrame = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Odyssey|Animation", BlueprintSetter=FramesPerSecondBlueprintSetter, meta=(ClampMin=1, UIMin=1))
    float FramesPerSecond = 24.0f;

    UPROPERTY(config)
    float TimelineSplitterPosition = 0.2f; //TODO: Move To Editor Only class

protected:
    friend class UOdysseyAnimationFactory;
    friend class UOdysseyPainterEditorAnimationFunctionLibrary;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(DisplayName="Width"), Category="Odyssey|Animation")
    int mWidth = -1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(DisplayName="Height"), Category="Odyssey|Animation")
    int mHeight = -1;

    UPROPERTY(BlueprintReadOnly, Category="Odyssey|Animation")
    EOdysseyAnimationFormat Format = EOdysseyAnimationFormat::BGRA8;

    UPROPERTY()
    int mFormat = 0; //Deprecated: only present for compatibility, use Format instead

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Odyssey|Animation")
    EOdysseyAnimationBoundMode LeftBoundMode = EOdysseyAnimationBoundMode::Automatic;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Odyssey|Animation", meta=(EditCondition="LeftBoundMode != EOdysseyAnimationBoundMode::Automatic", EditConditionHides))
    int LeftBound = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Odyssey|Animation")
    EOdysseyAnimationBoundMode RightBoundMode = EOdysseyAnimationBoundMode::Automatic;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Odyssey|Animation", meta=(EditCondition="RightBoundMode != EOdysseyAnimationBoundMode::Automatic", EditConditionHides))
    int RightBound = 0;

    UPROPERTY(BlueprintReadOnly, Category="Odyssey|Animation", meta=(DisplayName="Layer Stack", LoadBehavior = "LazyOnDemand"))
    TObjectPtr<UOdysseyAnimationLayerStack> mLayerStack; //Editor Only
};
