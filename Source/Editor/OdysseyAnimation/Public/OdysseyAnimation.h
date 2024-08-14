// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "Widgets/SOdysseyAnimationConfigureWindow.h"
#include "OdysseyRasterBlock.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "BaseMediaSource.h"
#include "OdysseyAnimationProxy.h"
#include "OdysseyAnimationImageRenderingAbility.h"
#include <ULIS>

#include "OdysseyAnimation.generated.h"

UCLASS(config=EditorPerProjectUserSettings, PerObjectConfig, HideCategories=(Platforms))
class ODYSSEYANIMATION_API UOdysseyAnimation
	: public UBaseMediaSource
    , public FOdysseyAnimationImageRenderingAbility
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
	void Init(const FOdysseyAnimationConfiguration& iConfiguration);

public:

	//~ UMediaSource interface
	virtual FString GetUrl() const override;
	virtual bool Validate() const override;

public:
	//Getters
	UOdysseyAnimationLayerStack* GetLayerStack() const;
	TSharedPtr<FOdysseyAnimationProxy> GetProxy() const;

	//Size and Format
	int Width() const;
	int Height() const;
	::ULIS::eFormat Format() const;

	//Duration and speed
	UFUNCTION(BlueprintPure, Category="Odyssey|2D Animation")
	FTimespan GetDuration() const;

	UFUNCTION(BlueprintPure, Category="Odyssey|2D Animation")
	FInt32Range GetFrameRange() const;

	UFUNCTION(BlueprintPure, Category="Odyssey|2D Animation")
	int GetFrameCount() const;

	double GetFramesPerSecond() const;

	//Time
	//Index to the frame at a given time
	UFUNCTION(BlueprintPure, Category="Odyssey|2D Animation")
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
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;
	virtual void PostInitProperties() override;
	virtual void PostLoad() override;
	
protected:
    //Property changed methods
    virtual void PropertyChanged(const FName& iPropertyName);
	
    virtual void CurrentFrameChanged();
	virtual void FramesPerSecondChanged();

private:
	void OnImageRenderingChanged(const FOdysseyImageRenderingChangedEvent& iEvent);

public:
	UPROPERTY(BlueprintReadWrite, Category = "Animation", DuplicateTransient, NonTransactional, meta=(ClampMin=0, UIMin=0))
	int CurrentFrame = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=1, UIMin=1), Category="Animation")
	float FramesPerSecond = 24.0f;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(DisplayName="Width"), Category = "Animation")
	int mWidth = -1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(DisplayName="Height"), Category = "Animation")
	int mHeight = -1;

	UPROPERTY(BlueprintReadOnly, meta=(DisplayName="Format"), Category = "Animation")
	int mFormat = ::ULIS::Format_RGBA8;

	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta=(DisplayName="Layer Stack", LoadBehavior = "LazyOnDemand"))
	TObjectPtr<UOdysseyAnimationLayerStack> mLayerStack;

	TSharedPtr<FOdysseyAnimationProxy> mProxy;
};