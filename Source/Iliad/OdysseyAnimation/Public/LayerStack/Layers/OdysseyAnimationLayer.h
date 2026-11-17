// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyAnimationLayerStack.h"
#include "OdysseyLayer.h"

#include "OdysseyAnimationLayer.generated.h"

UCLASS(Abstract, BlueprintType, HideDropdown)
class ODYSSEYANIMATION_API UOdysseyAnimationLayer
    : public UOdysseyLayer
{
    GENERATED_BODY()

public:

    virtual UOdysseyAnimationLayerStack* GetLayerStack() const;
    virtual TArray<FGuid> GetRenderingComposition(uint64 iRenderType, int iFrame) const override;
    /** Resolves animated cells, pre/post behaviours and editor lighttable content. */
    virtual bool BuildRenderPipelineInternal(FRenderFunction& oRenderFunction,
        const TArray<const IOdysseyTextureRenderingAbility*>& iParents, const FOdysseyRenderContext* RenderContext) const override;

public:
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    UOdysseyAnimation* GetAnimation() const;

#if WITH_EDITOR
private:
    /** Builds the surrounding animation keys using the request's frame. */
    bool BuildLighttableRenderPipeline(FRenderFunction& oRenderFunction,
        const TArray<const IOdysseyTextureRenderingAbility*>& iParents, const FOdysseyRenderContext* RenderContext) const;
    TArray<FGuid> GetLighttableImageRenderingComposition(int iFrameIndex) const;

protected:
    friend class FOdysseyAnimationCellsContainerImport;
#endif
};
