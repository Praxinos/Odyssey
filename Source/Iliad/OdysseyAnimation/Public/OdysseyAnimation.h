// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "UObject/AssetRegistryTagsContext.h"
#include "BaseMediaSource.h"
#include "OdysseyTextureRenderingAbility.h"
#include "UObject/SoftObjectPath.h"
#include "OdysseyPaletteReferencer.h"

#include "OdysseyAnimation.generated.h"

class UOdysseyLayerStack;
class UTexture2D;
class UOdysseyPaletteSet;

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
    , public IOdysseyPaletteReferencer
{
    GENERATED_BODY()

public:
#if WITH_EDITOR
    //Property changed methods
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;
    virtual void PropertyChanged(const FName& iPropertyName);
    virtual void PreSave(FObjectPreSaveContext SaveContext) override;
    virtual void CollectSaveOverrides(FObjectCollectSaveOverridesContext SaveContext) override;
    virtual void GetAssetRegistryTagMetadata( TMap<FName, FAssetRegistryTagMetadata>& OutMetadata ) const override;
    virtual void GetAssetRegistryTags( FAssetRegistryTagsContext ioContext ) const override;
#endif

public:
    //~ UMediaSource interface
    virtual FString GetUrl() const override;
    virtual bool Validate() const override;

    //Size and Format
    int GetWidth() const;
    int GetHeight() const;
    /** Compute the width corresponding to given height and respecting the animation ratio */
    int GetWidthFromHeightKeepingRatio( int iHeight );
    /** Compute the height corresponding to given width and respecting the animation ratio */
    int GetHeightFromWidthKeepingRatio( int iWidth );
    EOdysseyAnimationFormat GetFormat() const;
    float GetFramesPerSecond() const;

    /**
     * Return the duration of the animation.
     *
     * Computed from its frame count and its frames per second.
     */
    UFUNCTION(BlueprintPure, Category="Odyssey|Animation")
    FTimespan GetDuration() const;

    /**
     * Return the range of frames of the animation.
     *
     * From the left bound to the right bound, both included.
     */
    UFUNCTION(BlueprintPure, Category="Odyssey|Animation")
    virtual FInt32Range GetFrameRange() const override;

    /** Return the number of frames between the left and right bounds. */
    UFUNCTION(BlueprintPure, Category="Odyssey|Animation")
    int GetFrameCount() const;

    int GetFrameIndexAtFrame(int iFrameIndex) const;

    //Time range of the frame at iFrameIndex
    TRange<FTimespan> GetFrameTimeRange(int iFrameIndex) const; //TODO: find a better way to have this function, only used by Media and ViewportDrawingEditor

    /** Return how the left bound (first frame) of the animation is defined. */
    UFUNCTION(BlueprintPure, Category = "Odyssey|Animation")
    EOdysseyAnimationBoundMode GetLeftBoundMode() const;

    /** Return how the right bound (last frame) of the animation is defined. */
    UFUNCTION(BlueprintPure, Category = "Odyssey|Animation")
    EOdysseyAnimationBoundMode GetRightBoundMode() const;

    /**
     * Return the first frame of the animation.
     *
     * In Automatic mode, this is the first frame of the layer stack;
     * in Manual mode, it is the Left Bound value.
     */
    UFUNCTION(BlueprintPure, Category = "Odyssey|Animation")
    int GetLeftBoundValue() const;

    /**
     * Return the last frame of the animation.
     *
     * In Automatic mode, this is the last frame of the layer stack;
     * in Manual mode, it is the Right Bound value.
     */
    UFUNCTION(BlueprintPure, Category = "Odyssey|Animation")
    int GetRightBoundValue() const;

    /** Set how the left bound (first frame) of the animation is defined. */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Animation")
    void SetLeftBoundMode(EOdysseyAnimationBoundMode iMode);

    /** Set how the right bound (last frame) of the animation is defined. */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Animation")
    void SetRightBoundMode(EOdysseyAnimationBoundMode iMode);

    /** Set the first frame of the animation, used in Manual mode. */
    UFUNCTION(BlueprintCallable, Category = "Odyssey|Animation")
    void SetLeftBoundValue(int iValue);

    /** Set the last frame of the animation, used in Manual mode. */
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
#endif
    UOdysseyLayerStack* GetLayerStack() const;

public:
    virtual TArray<FGuid> GetRenderingComposition(uint64 iRenderType, int iFrameIndex) const override;
    virtual bool BuildRenderPipelineInternal(
        FFrameNumber iFrame,
        uint64 iType,
        IOdysseyTextureRenderingAbility::FRenderFunction& oRenderFunction,
        const IOdysseyTextureRenderingAbility::FCanRenderFunction& iCanRenderFunction,
        const TArray<const IOdysseyTextureRenderingAbility*>& iParents
    ) const override;
    virtual FIntRect GetDefaultRenderRect() const override;
    virtual UTextureRenderTarget2D* CreateRenderingRenderTarget() const override;

#if WITH_EDITOR
    virtual UTexture2D* CreateExportTexture(const FString& iAssetName, const FString& iPackagePath, UClass* iAssetClass, UFactory* iFactory) override;
#endif

private:
#if WITH_EDITOR
    virtual void OnRefreshReferencedPalette(UOdysseyPalette* iPalette) override;
#endif

public:
    /** The framerate of the animation, in frames per second. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Odyssey|Animation", meta=(ClampMin=1, UIMin=1))
    float FramesPerSecond = 24.0f;

protected:
    /** Width of the animation, in pixels. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(DisplayName="Width"), Category="Odyssey|Animation")
    int mWidth = -1;

    /** Height of the animation, in pixels. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(DisplayName="Height"), Category="Odyssey|Animation")
    int mHeight = -1;

    /** Image format of the animation: BGRA8 (8 bit) or RGBA F (16 bit). */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Odyssey|Animation")
    EOdysseyAnimationFormat Format = EOdysseyAnimationFormat::BGRA8;

    UPROPERTY()
    TArray<FOdysseyAnimationFrame> Frames;

    /**
     * Keep the layer stack in cooked builds.
     *
     * If enabled, the layers and cells are packaged and rendered at runtime.
     * Otherwise, only the pre-rendered frames of the animation are packaged.
     */
    UPROPERTY(EditAnywhere, Category = "Odyssey|Animation", NonTransactional)
    bool PreserveLayerStackAtRuntime = false;

    /** The layer stack holding the layers and cells of the animation. */
    UPROPERTY(BlueprintReadOnly, Category = "Odyssey|Animation", meta = (DisplayName = "Layer Stack", LoadBehavior = "LazyOnDemand"))
    TObjectPtr<UOdysseyLayerStack> mLayerStack;

    /**
     * How the first frame of the animation is defined.
     *
     * Automatic: the first frame of the layer stack.
     * Manual: the frame set in Left Bound.
     */
    UPROPERTY(EditAnywhere, Category="Odyssey|Animation")
    EOdysseyAnimationBoundMode LeftBoundMode = EOdysseyAnimationBoundMode::Automatic;

    /** First frame of the animation, when Left Bound Mode is Manual. */
    UPROPERTY(EditAnywhere, Category="Odyssey|Animation", meta=(EditCondition="LeftBoundMode != EOdysseyAnimationBoundMode::Automatic", EditConditionHides))
    int LeftBound = 0;

    /**
     * How the last frame of the animation is defined.
     *
     * Automatic: the last frame of the layer stack.
     * Manual: the frame set in Right Bound.
     */
    UPROPERTY(EditAnywhere, Category="Odyssey|Animation")
    EOdysseyAnimationBoundMode RightBoundMode = EOdysseyAnimationBoundMode::Automatic;

    /** Last frame of the animation, when Right Bound Mode is Manual. */
    UPROPERTY(EditAnywhere, Category="Odyssey|Animation", meta=(EditCondition="RightBoundMode != EOdysseyAnimationBoundMode::Automatic", EditConditionHides))
    int RightBound = 0;

public:
    UPROPERTY()
    TArray<UOdysseyPaletteSet*> Palettes;

    // We store the path to the Tool Collections Assets, so that we don't have to include the painterEditorModule (where the tools and tool collections are) in the build.cs
    UPROPERTY()
    TArray<FSoftObjectPath> ToolCollections;

};
