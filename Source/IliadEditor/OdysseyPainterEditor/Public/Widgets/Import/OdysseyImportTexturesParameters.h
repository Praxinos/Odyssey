// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "UObject/GCObject.h"

#include "OdysseyAntiAliasing.h"
#include "OdysseyImportTexturesParameters.generated.h"

class UCurveBase;
class UCurveFloat;
class UTexture2D;
class UTextureRenderTarget2D;

UENUM()
enum class EOdysseyImportTextureScaling : uint8
{
    // Don't scale
    None,
    // Scale at 100%
    Full,
    // Scale with custom ratio
    Custom,
};

UENUM()
enum class EOdysseyImportTextureAlignment: uint8
{
    // Align image to top-left corner
    TopLeft,
    // Align image to top border
    Top,
    // Align image to top-right corner
    TopRight,
    // Align image to left border
    Left,
    // Center image
    Center,
    // Align image to right border
    Right,
    // Align image to bottom-left corner
    BottomLeft,
    // Align image to bottom border
    Bottom,
    // Align image to bottom-right corner
    BottomRight
};

class FOdysseyImportTexturesParametersGC
    : public FGCObject
{
public:
    FOdysseyImportTexturesParametersGC(struct FOdysseyImportTexturesParameters* iParameters);

private:
    // FGCObject API
    virtual void AddReferencedObjects( FReferenceCollector& ioCollector ) override;
    virtual FString GetReferencerName() const override;

private:
    struct FOdysseyImportTexturesParameters* mParameters;
};

USTRUCT(BlueprintType)
struct ODYSSEYPAINTEREDITOR_API FOdysseyImportTexturesParameters
{
    GENERATED_BODY()

public:
    ~FOdysseyImportTexturesParameters();
    FOdysseyImportTexturesParameters();

public:
    UTextureRenderTarget2D* CreateRT() const;
    void Render(UTextureRenderTarget2D* oRenderTarget, int iSourceTextureIndex) const;

    void Init(const TArray< UTexture2D* >& iTextures, uint32 iDestinationWidth, uint32 iDestinationHeight);
    void Init(const TArray<FString>& iFilenames, uint32 iDestinationWidth, uint32 iDestinationHeight);

    uint32 GetDestinationWidth() const;
    uint32 GetDestinationHeight() const;
    TArray< UTexture2D* > GetSourceTextures() const;
    EOdysseyImportTextureAlignment GetAlignment() const;
    EOdysseyImportTextureScaling GetScaling() const;
    bool GetScalingFullFit() const;
    FVector2D GetScalingCustomSize() const;
    EOdysseyAntiAliasing GetResamplingMethod() const;
    UCurveFloat* GetScanCleanerCurve() const;
    float GetScanCleanerColorSaturation() const;
    float GetScanCleanerColorValue() const;
    void SetScanCleanerColorSaturation(float iSaturation);
    void SetScanCleanerColorValue(float iValue);

    bool GetIsScanCleanerActivated() const;
    void SetIsScanCleanerActivated(bool iIsActivated);

    FVector2D GetTexturePosition(const FVector2D& iTextureSize) const;
    FVector2D GetTexturePosition(int iSourceTextureIndex) const;
    FVector2D GetTextureScaledSize(int iSourceTextureIndex) const;
    FString GetTextureName(int iSourceTextureIndex) const;

    void SetAlignment(EOdysseyImportTextureAlignment iAlignment);
    void SetScaling(EOdysseyImportTextureScaling iScaling);
    void SetScalingFullFit(bool iFit);
    void SetScalingCustomSize(FVector2D iSize);
    void SetResamplingMethod(EOdysseyAntiAliasing iMethod);

    void ResetScanCleaner();

private:
    void OnUpdateCurve( UCurveBase* Curve, EPropertyChangeType::Type ChangeType);
    void UpdateScanCleanerCurveTextures();

private:
    friend class FOdysseyImportTexturesParametersGC;
    FOdysseyImportTexturesParametersGC mGC;

    TArray< TObjectPtr<UTexture2D> > mSourceTextures;
    TArray< FString > mSourceTextureNames;
    uint32 mDestinationWidth;
    uint32 mDestinationHeight;

protected:
    // Positioning

    // Alignment of the imported image in the destination
    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Import")
    EOdysseyImportTextureAlignment mAlignment = EOdysseyImportTextureAlignment::Center;

    // Scaling behavior
    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Import")
    EOdysseyImportTextureScaling mScaling = EOdysseyImportTextureScaling::None;

    // Only for "Scale"
    // Keep the original ratio of the imported image
    UPROPERTY( BlueprintReadWrite, Category = "Odyssey|Import" )
    bool ScalingFullFit = true;

    // Only for "Custom"
    // Custom value of scaling width and height
    // 100% = original size
    UPROPERTY( BlueprintReadWrite, Category = "Odyssey|Import" )
    FVector2D ScalingCustomSize = FVector2D( 1.f, 1.f );

    // AntiAliasing behavior
    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Import")
    EOdysseyAntiAliasing mResamplingMethod = EOdysseyAntiAliasing::Bilinear;

    //---

    // Scan Cleaner
    bool mIsScanCleanerActivated = false;
    TObjectPtr<UCurveFloat> mScanCleanerCurve;
    TObjectPtr<UTextureRenderTarget2D> mScanCleanerCurveTexture;
    float mScanCleanerColorSaturation = 1.f;
    float mScanCleanerColorValue = 1.f;

};
