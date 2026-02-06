// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyAntiAliasing.h"

#include "OdysseyImportTexturesParameters.generated.h"

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

USTRUCT(BlueprintType)
struct ODYSSEYPAINTEREDITOR_API FOdysseyImportTexturesParameters
{
    GENERATED_BODY()

public:
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

    bool GetIsScanCleanerActivated() const;
    void SetIsScanCleanerActivated(bool iIsActivated);

    FVector2D GetTexturePosition(const FVector2D& iTextureSize) const;
    FVector2D GetTexturePosition(int iSourceTextureIndex) const;
    FVector2D GetTextureScaledSize(int iSourceTextureIndex) const;

    void SetAlignment(EOdysseyImportTextureAlignment iAlignment);
    void SetScaling(EOdysseyImportTextureScaling iScaling);
    void SetResamplingMethod(EOdysseyAntiAliasing iMethod);

private:
    TArray< UTexture2D* > mSourceTextures;
    uint32 mDestinationWidth;
    uint32 mDestinationHeight;

protected:
    //Positioning
    UPROPERTY(BlueprintReadWrite)
    EOdysseyImportTextureAlignment mAlignment = EOdysseyImportTextureAlignment::Center;

    UPROPERTY(BlueprintReadWrite)
    EOdysseyImportTextureScaling mScaling = EOdysseyImportTextureScaling::None;

    UPROPERTY(BlueprintReadWrite)
    EOdysseyAntiAliasing mResamplingMethod = EOdysseyAntiAliasing::Bilinear;

    //Scan Cleaner
    bool mIsScanCleanerActivated = false;
};
