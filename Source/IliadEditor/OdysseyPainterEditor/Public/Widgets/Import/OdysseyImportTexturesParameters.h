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
    None UMETA(ToolTip="Don't scale"),
    Scale UMETA(ToolTip="Scales without preserving ratio"),
    ScaleAndFit UMETA(ToolTip="Scales and preserves ratio")
};

UENUM()
enum class EOdysseyImportTextureAlignment
{
    TopLeft,
    Top,
    TopRight,
    Left,
    Center,
    Right,
    BottomLeft,
    Bottom,
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
    void SetResamplingMethod(EOdysseyAntiAliasing iMethod);

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
    //Positioning
    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Import")
    EOdysseyImportTextureAlignment mAlignment = EOdysseyImportTextureAlignment::Center;

    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Import")
    EOdysseyImportTextureScaling mScaling = EOdysseyImportTextureScaling::None;

    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Import")
    EOdysseyAntiAliasing mResamplingMethod = EOdysseyAntiAliasing::Bilinear;

    //Scan Cleaner
    bool mIsScanCleanerActivated = false;
    TObjectPtr<UCurveFloat> mScanCleanerCurve;
    TObjectPtr<UTextureRenderTarget2D> mScanCleanerCurveTexture;
    float mScanCleanerColorSaturation = 1.f;
    float mScanCleanerColorValue = 1.f;

};
