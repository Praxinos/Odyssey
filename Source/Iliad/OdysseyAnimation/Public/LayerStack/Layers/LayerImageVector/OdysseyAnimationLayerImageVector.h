// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "Image/OdysseyBlendingMode.h"

#include <ULIS>

#include "OdysseyVector.h"

#include "OdysseyAnimationLayerImageVector.generated.h"

UCLASS(BlueprintType)
class ODYSSEYANIMATION_API UOdysseyAnimationLayerImageVector
    : public UOdysseyAnimationLayer
{
    GENERATED_BODY()

public:
    /**
     * @brief Delegate called when something changed the result of RenderImage()
     *
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnIsColoredChanged, UOdysseyAnimationLayerImageVector*)
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnIsWireframeChanged, UOdysseyAnimationLayerImageVector*)

public:
    static FOnIsColoredChanged& OnIsColoredChanged();
    static FOnIsWireframeChanged& OnIsWireframeChanged();

public:
    // UObject overrides
    virtual void PostInitProperties() override;
    virtual void Serialize(FArchive& Ar) override;

public:
    //UOdysseyLayer overrides
    virtual FOdysseyMediaProvider GetMediaProvider(uint32 iFrameIndex) const override;
    virtual void Merge(const TArray<UOdysseyLayer*>& Layers) override;

public:
    //FOdysseyImageRenderingAbility overrides
    virtual TSharedPtr<IOdysseyImageRenderer> BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame, FImageRendererFilter iFilter = FImageRendererFilter()) const override;
    virtual TArray<FGuid> GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex) const override;

protected:
    void IsColoredChanged();
    void IsWireframeChanged();
    virtual void PropertyChanged(const FName& iPropertyName, const FName& iMemberPropertyName, bool iIsInteractive) override;

private:
    TSharedPtr<IOdysseyMedia> CreateMediaVector(int iFrameIndex);
    TSharedPtr<IOdysseyMedia> GetCellMediaVector(uint32 iFrameIndex) const;
    void AutoCreateCell(int iFrameIndex);

private:
    UFUNCTION(BlueprintSetter)
    void IsWireframeBlueprintSetter(bool Value);

    UFUNCTION(BlueprintSetter)
    void IsColoredBlueprintSetter(bool Value);

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Odyssey|Layer", BlueprintSetter=IsWireframeBlueprintSetter, NonTransactional)
    bool IsWireframe = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Odyssey|Layer", BlueprintSetter=IsColoredBlueprintSetter, NonTransactional)
    bool IsColored = true;
};
