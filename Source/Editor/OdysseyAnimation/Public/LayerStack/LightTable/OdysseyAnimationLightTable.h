// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "OdysseyAnimationLightTable.generated.h"

UENUM()
enum class EOdysseyLightTableDisplayPosition
{
    AboveLayer,
    UnderLayer
};

class ODYSSEYANIMATION_API FOdysseyAnimationLightTable
    : public TSharedFromThis<FOdysseyAnimationLightTable>
    , public FOdysseyAnimationImageRenderingAbility
{
public:
    virtual ~FOdysseyAnimationLightTable();
    FOdysseyAnimationLightTable(UOdysseyAnimationLayer* iLayer);

public:
    UOdysseyAnimationLayer* GetLayer() const;
    EOdysseyLightTableDisplayPosition GetDisplayPosition() const;

public:
    bool GetKeyIsActivated(int iIndex) const;
    int GetKeyOffset(int iIndex) const;
    float GetKeyOpacity(int iIndex) const;
    ::ULIS::FColor GetKeyColor(int iIndex) const;
    const FLinearColor& GetNextKeysColor() const;
    const FLinearColor& GetPreviousKeysColor() const;
    float GetNextKeysContrast() const;
    float GetPreviousKeysContrast() const;

    struct FKey
    {
        friend class FOdysseyAnimationLightTableKeyImport;
        friend class FOdysseyAnimationLightTableKeyExport;

        bool mIsActivated;
        float mOpacity;
    };

    const FKey* GetKey(int iIndex) const;
    int GetRange() const;

public:
	//FOdysseyImageRenderingAbility overrides
	virtual TSharedPtr<IOdysseyImageRenderer> BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame, FImageRendererFilter iFilter = FImageRendererFilter()) const override;
	virtual TArray<FGuid> GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex) const override;
	virtual TArray<::ULIS::FRectI> GetImageRenderingRects() const override;

    void Serialize(FArchive& Ar);

private:
    FKey* GetKey(int iIndex);

private:
    friend class FOdysseyAnimationLightTableMutator;
    friend class FOdysseyAnimationLightTableImport;
    friend class FOdysseyAnimationLightTableExport;

    UOdysseyAnimationLayer* mLayer;
    EOdysseyLightTableDisplayPosition mDisplayPosition;
    FLinearColor mPreviousKeysColor;
    FLinearColor mNextKeysColor;
    float mPreviousKeysContrast;
    float mNextKeysContrast;

    TArray<FKey> mKeys;
};
