// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "UObject/AssetRegistryTagsContext.h"
#include "BaseMediaSource.h"
#include "Interfaces/Interface_AssetUserData.h"
#include "UObject/SoftObjectPath.h"

#include "OdysseyPaletteReferencer.h"
#include "OdysseyTextureRenderingAbility.h"

#include "OdysseyAnimation.generated.h"

class UAssetUserData;
class UTexture2D;
class UOdysseyPaletteSet;

UENUM()
enum class EOdysseyAnimationFormat : uint8
{
    BGRA8 UMETA(DisplayName = "BGRA 8"),
    RGBAF UMETA(DisplayName = "RGBA F")
};

#if WITH_EDITORONLY_DATA
UENUM()
enum class EOdysseyAnimationBoundMode : uint8
{
    Automatic,
    Manual
};
#endif

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
    , public IInterface_AssetUserData
{
    GENERATED_BODY()

public:
#if WITH_EDITOR
    /** Broadcast when an animation saved before the editor content moved to an asset user data is loaded */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnLegacyEditorDataLoaded, UOdysseyAnimation*);

    /**
     * @brief Delegate used by the editor module to migrate the legacy editor content of a loaded animation
     *
     * @return The delegate broadcast from PostLoad when HasLegacyEditorData() is true
     */
    static FOnLegacyEditorDataLoaded& OnLegacyEditorDataLoaded();
#endif

public:
    //~ Begin IInterface_AssetUserData Interface
    virtual void AddAssetUserData(UAssetUserData* InUserData) override;
    virtual void RemoveUserDataOfClass(TSubclassOf<UAssetUserData> InUserDataClass) override;
    virtual UAssetUserData* GetAssetUserDataOfClass(TSubclassOf<UAssetUserData> InUserDataClass) override;
    virtual const TArray<UAssetUserData*>* GetAssetUserDataArray() const override;
    //~ End IInterface_AssetUserData Interface

#if WITH_EDITOR
    /**
     * @brief Returns the asset user data holding the editable content of this animation
     *
     * @return The first asset user data implementing IOdysseyAnimationEditorData, or nullptr
     */
    UAssetUserData* GetEditorData() const;
#endif

#if WITH_EDITORONLY_DATA
    /**
     * @brief Tells if this animation still holds editor content saved before it moved to an asset user data
     */
    bool HasLegacyEditorData() const;

    /**
     * @brief Moves the legacy editor content out of this animation, leaving it empty
     *
     * @param oLayerStack Receives the legacy layer stack
     * @param oPalettes Receives the legacy palette sets
     * @param oToolCollections Receives the legacy tool collection paths
     */
    void TakeLegacyEditorData(UObject*& oLayerStack, TArray<UOdysseyPaletteSet*>& oPalettes, TArray<FSoftObjectPath>& oToolCollections);
#endif

public:
    virtual void PostLoad() override;

#if WITH_EDITOR
    //Property changed methods
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;
    virtual void PropertyChanged(const FName& iPropertyName);
    virtual void PreSave(FObjectPreSaveContext SaveContext) override;
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

    UFUNCTION(BlueprintPure, Category="Odyssey|Animation")
    FTimespan GetDuration() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Animation")
    virtual FInt32Range GetFrameRange() const override;

    UFUNCTION(BlueprintPure, Category="Odyssey|Animation")
    int GetFrameCount() const;

    int GetFrameIndexAtFrame(int iFrameIndex) const;

    //Time range of the frame at iFrameIndex
    TRange<FTimespan> GetFrameTimeRange(int iFrameIndex) const; //TODO: find a better way to have this function, only used by Media and ViewportDrawingEditor

#if WITH_EDITOR
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

public:
    void OnLeftBoundModeChanged();
    void OnRightBoundModeChanged();
    void OnLeftBoundChanged();
    void OnRightBoundChanged();

public:
    /**
     * @brief Sets the size, format and frame rate of a new animation
     *
     * The editable content is created separately, as an asset user data, by the editor module.
     */
    void Init(int iWidth, int iHeight, EOdysseyAnimationFormat iFormat, float iFramesPerSecond);
#endif

public:
    virtual TArray<FGuid> GetRenderingComposition(uint64 iRenderType, int iFrameIndex) const override;
    virtual bool BuildRenderPipelineInternal(
        IOdysseyTextureRenderingAbility::FRenderFunction& oRenderFunction,
        const TArray<const IOdysseyTextureRenderingAbility*>& iParents, const FOdysseyRenderContext* RenderContext
    ) const override;
    virtual FIntRect GetDefaultRenderRect() const override;
    virtual UTextureRenderTarget2D* CreateRenderingRenderTarget() const override;

#if WITH_EDITOR
    virtual UTexture2D* CreateExportTexture(const FString& iAssetName, const FString& iPackagePath, UClass* iAssetClass, UFactory* iFactory) override;
#endif

private:
#if WITH_EDITOR
    virtual void OnRefreshReferencedPalette(UOdysseyPalette* iPalette) override;

    /** Returns the rendering source of the editor data, or nullptr if there is none */
    const IOdysseyTextureRenderingAbility* GetEditorRenderingSource() const;
#endif

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

    /** Asset user data attached to this animation, among which its editor-only editable content */
    UPROPERTY(EditAnywhere, AdvancedDisplay, Instanced, Category="Odyssey|Animation")
    TArray<TObjectPtr<UAssetUserData>> AssetUserData;

#if WITH_EDITORONLY_DATA
    /** Legacy layer stack, moved to the editor data on load */
    UPROPERTY()
    TObjectPtr<UObject> mLayerStack_DEPRECATED;

    /** Legacy palette sets, moved to the editor data on load */
    UPROPERTY()
    TArray<UOdysseyPaletteSet*> Palettes_DEPRECATED;

    /** Legacy tool collection paths, moved to the editor data on load */
    UPROPERTY()
    TArray<FSoftObjectPath> ToolCollections_DEPRECATED;

    UPROPERTY(EditAnywhere, Category="Odyssey|Animation")
    EOdysseyAnimationBoundMode LeftBoundMode = EOdysseyAnimationBoundMode::Automatic;

    UPROPERTY(EditAnywhere, Category="Odyssey|Animation", meta=(EditCondition="LeftBoundMode != EOdysseyAnimationBoundMode::Automatic", EditConditionHides))
    int LeftBound = 0;

    UPROPERTY(EditAnywhere, Category="Odyssey|Animation")
    EOdysseyAnimationBoundMode RightBoundMode = EOdysseyAnimationBoundMode::Automatic;

    UPROPERTY(EditAnywhere, Category="Odyssey|Animation", meta=(EditCondition="RightBoundMode != EOdysseyAnimationBoundMode::Automatic", EditConditionHides))
    int RightBound = 0;
#endif
};
