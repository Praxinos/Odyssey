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
    UOdysseyAnimationLayer* GetOwnerLayer() const;
    UOdysseyAnimationLayer* GetSourceLayer() const;
    EOdysseyLightTableDisplayPosition GetDisplayPosition() const;

public:
    bool GetKeyIsActivated(int iIndex) const;
    int GetKeyOffset(int iIndex) const;
    float GetKeyOpacity(int iIndex) const;
    ::ULIS::FColor GetKeyColor(int iIndex) const;
    const FLinearColor& GetNextKeysColor() const;
    const FLinearColor& GetPreviousKeysColor() const;

    struct FKeyData
    {
        bool mIsActivated;
        int mOffset;
        float mOpacity;
    };

    const TMap<int, FKeyData>& GetKeysData() const;
    int GetRange() const;

public:
	//FOdysseyImageRenderingAbility overrides
	virtual TSharedPtr<IOdysseyImageRenderer> BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame, FImageRendererFilter iFilter = FImageRendererFilter()) const override;
	virtual TArray<FGuid> GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex) const override;
	virtual TArray<::ULIS::FRectI> GetImageRenderingRects() const override;

private:
    friend class FOdysseyAnimationLightTableMutator;

    UOdysseyAnimationLayer* mOwnerLayer;
    UOdysseyAnimationLayer* mSourceLayer;
    EOdysseyLightTableDisplayPosition mDisplayPosition;
    FLinearColor mPreviousKeysColor;
    FLinearColor mNextKeysColor;

    TMap<int, FKeyData> mKeysData;
};
