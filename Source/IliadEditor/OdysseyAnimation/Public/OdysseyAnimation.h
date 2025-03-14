// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "BaseMediaSource.h"
#include "OdysseyImageRenderingAbility.h"
#include <ULIS>

#include "OdysseyAnimation.generated.h"

class FOdysseyAnimationProxy;
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
    , public FOdysseyImageRenderingAbility
    //, public FTickableEditorObject //Allows us to react to Tick events
{
    GENERATED_BODY()

public:
    /**
     * @brief Delegate called when CurrentFrame changes
     *
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentFrameChanged, UOdysseyAnimation*)
    static FOnCurrentFrameChanged& OnCurrentFrameChanged();

    /**
     * @brief Delegate called when FramesPerSecond changes
     *
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnFramesPerSecondChanged, UOdysseyAnimation*)
    static FOnFramesPerSecondChanged& OnFramesPerSecondChanged();

public:

    //~ UMediaSource interface
    virtual FString GetUrl() const override;
    virtual bool Validate() const override;

public:
    //Getters
    UOdysseyAnimationLayerStack* GetLayerStack() const;
    TSharedPtr<FOdysseyAnimationProxy> GetProxy() const;

    //Size and Format
    int GetWidth() const;
    int GetHeight() const;
    ::ULIS::eFormat GetFormat() const;

    //Duration and speed
    UFUNCTION(BlueprintPure, Category="Odyssey|Animation")
    FTimespan GetDuration() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Animation")
    FInt32Range GetFrameRange() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Animation")
    int GetFrameCount() const;

    double GetFramesPerSecond() const;

    EOdysseyAnimationBoundMode GetLeftBoundMode() const;
    EOdysseyAnimationBoundMode GetRightBoundMode() const;
    int GetLeftBoundValue() const;
    int GetRightBoundValue() const;

    void SetLeftBoundMode(EOdysseyAnimationBoundMode iMode);
    void SetRightBoundMode(EOdysseyAnimationBoundMode iMode);
    void SetLeftBoundValue(int iValue);
    void SetRightBoundValue(int iValue);

    //Time
    //Index to the frame at a given time
    UFUNCTION(BlueprintPure, Category="Odyssey|Animation")
    int GetFrameIndexAtTime(FTimespan Time) const;

    //Time range of the frame at iFrameIndex
    TRange<FTimespan> GetFrameTimeRange(int iFrameIndex) const;

public:
    //FOdysseyImageRenderingAbility overrides
    virtual TSharedPtr<IOdysseyImageRenderer> BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame, FImageRendererFilter iFilter = FImageRendererFilter()) const override;
    virtual TArray<FGuid> GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex) const override;
    virtual TArray<::ULIS::FRectI> GetImageRenderingRects() const override;

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

private:
    void OnImageRenderingChanged(const FOdysseyImageRenderingChangedEvent& iEvent);

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
    float TimelineSplitterPosition = 0.2f;

protected:
    friend class UOdysseyAnimationFactory;
    friend class UOdysseyPainterEditorAnimationFunctionLibrary;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(DisplayName="Width"), Category="Odyssey|Animation")
    int mWidth = -1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(DisplayName="Height"), Category="Odyssey|Animation")
    int mHeight = -1;

    UPROPERTY()
    int mFormat = ::ULIS::Format_BGRA8; //Deprecated: only present for compatibility, use Format instead

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Odyssey|Animation")
    EOdysseyAnimationBoundMode LeftBoundMode = EOdysseyAnimationBoundMode::Automatic;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Odyssey|Animation", meta=(EditCondition="LeftBoundMode != EOdysseyAnimationBoundMode::Automatic", EditConditionHides))
    int LeftBound = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Odyssey|Animation")
    EOdysseyAnimationBoundMode RightBoundMode = EOdysseyAnimationBoundMode::Automatic;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Odyssey|Animation", meta=(EditCondition="RightBoundMode != EOdysseyAnimationBoundMode::Automatic", EditConditionHides))
    int RightBound = 0;

    UPROPERTY(BlueprintReadOnly, Category="Odyssey|Animation")
    EOdysseyAnimationFormat Format = EOdysseyAnimationFormat::BGRA8;

    UPROPERTY(BlueprintReadOnly, Category="Odyssey|Animation", meta=(DisplayName="Layer Stack", LoadBehavior = "LazyOnDemand"))
    TObjectPtr<UOdysseyAnimationLayerStack> mLayerStack;

    TSharedPtr<FOdysseyAnimationProxy> mProxy;
};
