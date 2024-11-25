// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Cells/OdysseyAnimationCell.h"

#include "OdysseyAnimationCellImageStagger.generated.h"

UENUM(BlueprintType)
enum class EOdysseyAnimationCellImageStaggerBehaviour : uint8
{
    Loop,
    PingPong
};

UCLASS(BlueprintType)
class ODYSSEYANIMATION_API UOdysseyAnimationCellImageStagger
    : public UOdysseyAnimationCell
{
    GENERATED_BODY()

public:
    //FOdysseyImageRenderingAbility overrides
    virtual TSharedPtr<IOdysseyImageRenderer> BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame, FImageRendererFilter iFilter = FImageRendererFilter()) const override;
    virtual TArray<FGuid> GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex) const override;
    virtual TArray<::ULIS::FRectI> GetImageRenderingRects() const override;
    bool IsImageRenderingGameThreadOnly() const;

    virtual void OldSerialize(FArchive& Ar) override; //DEPRECATED: Keep that for compatibility with early versions of Odyssey

public:
    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    int GetReferenceFrameAtFrame(int Frame) const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    UOdysseyAnimationCell* GetReferenceCellAtFrame(int Frame, bool Recursive = true) const;

public:
    virtual UOdysseyAnimationCell* Break(int Frame, bool bClear) override;

protected:
    void BehaviourChanged();
    void ReachChanged(bool iIsInteractive);

    virtual void PropertyChanged(const FName& iPropertyName, const FName& iMemberPropertyName, bool iIsInteractive) override;

private:
    UFUNCTION(BlueprintSetter)
    void BehaviourBlueprintSetter(EOdysseyAnimationCellImageStaggerBehaviour Value);

    UFUNCTION(BlueprintSetter)
    void ReachBlueprintSetter(int Value);

public:
    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Cell")
    EOdysseyAnimationCellImageStaggerBehaviour Behaviour = EOdysseyAnimationCellImageStaggerBehaviour::Loop;

    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Cell")
    int Reach = 0;
};
