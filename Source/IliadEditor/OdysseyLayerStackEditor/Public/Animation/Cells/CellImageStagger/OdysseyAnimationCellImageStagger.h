// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyAnimationCell.h"

#include "OdysseyAnimationCellImageStagger.generated.h"

UENUM(BlueprintType)
enum class EOdysseyAnimationCellImageStaggerBehaviour : uint8
{
    Loop,
    PingPong
};

UCLASS(BlueprintType)
class ODYSSEYLAYERSTACKEDITOR_API UOdysseyAnimationCellImageStagger
    : public UOdysseyAnimationCell
{
    GENERATED_BODY()

public:
    //IOdysseyRenderingAbility overrides
    virtual void RenderToTexture_RenderThread(FRDGBuilder& iGraphBuilder, FRDGTextureRef iDestinationTexture, ERHIFeatureLevel::Type iFeatureLevel, FFrameNumber iFrame, const FIntRect& iSrcRect, const FIntRect& iDstRect) const override;
    virtual TArray<FGuid> GetRenderingComposition(EOdysseyRenderingType iRenderType, int iFrameIndex) const override;
    virtual FIntRect GetDefaultRenderRect() const override;
    bool IsImageRenderingGameThreadOnly() const;

    virtual void OldSerialize(FArchive& Ar) override; //DEPRECATED: Keep that for compatibility with early versions of Odyssey

public:
    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    int GetReferenceFrameAtFrame(int Frame) const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    UOdysseyAnimationCell* GetReferenceCellAtFrame(int Frame, bool Recursive = true) const;

public:
    virtual UOdysseyLayerCell* Break(int Frame, bool bClear) override;

protected:
    void BehaviourChanged();
    void ReachChanged(bool iIsInteractive);

    virtual void PropertyChanged(const FName& iPropertyName, const FName& iMemberPropertyName, bool iIsInteractive) override;
    virtual void PostPropertyChanged(const FName& iPropertyName, bool iIsInteractive) override;

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
