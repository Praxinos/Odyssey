// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyAntiAliasing.h"

UENUM()
enum class EOdysseyImportTextureScaling : uint8
{
    None UMETA(ToolTip="Don't scale"),
    Scale UMETA(ToolTip="Scales without preserving ratio"),
    ScaleAndFit UMETA(ToolTip="Scales and preserves ratio")
};

class FOdysseyImportTexturesData
{
public:
    enum class EAlignment
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

public:
    FOdysseyImportTexturesData();
    FOdysseyImportTexturesData(const TArray< UTexture2D* > iSourceTextures, uint32 iWidth, uint32 iHeight);

public:
    UTextureRenderTarget2D* CreateRT() const;
    void Render(UTextureRenderTarget2D* oRenderTarget, int iSourceTextureIndex) const;

    uint32 GetDestinationWidth() const;
    uint32 GetDestinationHeight() const;
    TArray< UTexture2D* > GetSourceTextures() const;
    EAlignment GetAlignment() const;
    EOdysseyImportTextureScaling GetScaling() const;
    EOdysseyAntiAliasing GetResamplingMethod() const;

    FVector2D GetTexturePosition(const FVector2D& iTextureSize) const;
    FVector2D GetTexturePosition(int iSourceTextureIndex) const;
    FVector2D GetTextureScaledSize(int iSourceTextureIndex) const;

    void SetAlignment(EAlignment iAlignment);
    void SetScaling(EOdysseyImportTextureScaling iScaling);
    void SetResamplingMethod(EOdysseyAntiAliasing iMethod);

private:
    TArray< UTexture2D* > mSourceTextures;
    uint32 mDestinationWidth;
    uint32 mDestinationHeight;

    EAlignment mAlignment = EAlignment::Center;
    EOdysseyImportTextureScaling mScaling = EOdysseyImportTextureScaling::None;
    EOdysseyAntiAliasing mResamplingMethod = EOdysseyAntiAliasing::Bilinear;
};
