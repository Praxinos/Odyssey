// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "OdysseyAnimationLightTable.generated.h"

UENUM()
enum class EOdysseyLightTableDisplayMode
{
    Default,
    Color, //Full color keeps opacity
    Tint, //Tint uses Luminosity to define color
    ColorGradient, //Tint uses Luminosity to define color
    TintGradient, //Tint uses Luminosity to define color
};

UENUM()
enum class EOdysseyLightTableDisplayPosition
{
    AboveLayer,
    UnderLayer
};

class ODYSSEYANIMATION_API FOdysseyAnimationLightTable
    : public FOdysseyAbilityContainer
{
public:
    static TSharedRef<FOdysseyAnimationLightTable> Create(UOdysseyAnimationLayer* iLayer);

private:
    FOdysseyAnimationLightTable(UOdysseyAnimationLayer* iLayer);

public:
    enum eFrameDisplayMode
    {
        kDefault,
        kColor, //Full color keeps opacity
        kTint, //Tint uses Luminosity to define color
    };

public:
    UOdysseyAnimationLayer* GetOwnerLayer() const;
    UOdysseyAnimationLayer* GetSourceLayer() const;
    EOdysseyLightTableDisplayPosition GetDisplayPosition() const;
    EOdysseyLightTableDisplayMode GetDisplayMode() const;

public:
    bool GetKeyIsActivated(int iIndex) const;
    int GetKeyOffset(int iIndex) const;
    float GetKeyOpacity(int iIndex) const;
    eFrameDisplayMode GetKeyDisplayMode(int iIndex) const;
    ::ULIS::FColor GetKeyColor(int iIndex) const;

    struct FKeyData
    {
        bool mIsActivated;
        int mOffset;
        float mOpacity;
    };

    const TArray<FKeyData>& GetKeysData() const;

private:
    friend class FOdysseyAnimationLightTableMutator;

    UOdysseyAnimationLayer* mOwnerLayer;
    UOdysseyAnimationLayer* mSourceLayer;
    EOdysseyLightTableDisplayPosition mDisplayPosition;
    TEnumAsByte<EOdysseyLightTableDisplayMode> mDisplayMode;

    TArray<FKeyData> mKeysData;
};
