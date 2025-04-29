// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyLayerCell.h"

#include "OdysseyLayerCellImageStagger.generated.h"

UENUM(BlueprintType)
enum class EOdysseyLayerCellImageStaggerBehaviour : uint8
{
    Loop,
    PingPong
};

UCLASS(BlueprintType)
class ODYSSEYLAYERSTACK_API UOdysseyLayerCellImageStagger
    : public UOdysseyLayerCell
{
    GENERATED_BODY()

public:
    //IOdysseyRenderingAbility overrides
    virtual bool BuildRenderPipelineInternal(
        FFrameNumber iFrame,
        uint64 iType,
        IOdysseyTextureRenderingAbility::FRenderFunction& oRenderFunction,
        const IOdysseyTextureRenderingAbility::FCanRenderFunction& iCanRenderFunction,
        const TArray<const IOdysseyTextureRenderingAbility*>& iParents
    ) const override;
    virtual TArray<FGuid> GetRenderingComposition(uint64 iRenderType, int iFrameIndex) const override;
    virtual FIntRect GetDefaultRenderRect() const override;
    virtual void OldSerialize(FArchive& Ar) override; //DEPRECATED: Keep that for compatibility with early versions of Odyssey

public:
    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    int GetReferenceFrameAtFrame(int Frame) const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    UOdysseyLayerCell* GetReferenceCellAtFrame(int Frame, bool Recursive = true) const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    EOdysseyLayerCellImageStaggerBehaviour GetBehaviour() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    int GetReach() const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    void SetBehaviour(EOdysseyLayerCellImageStaggerBehaviour Value);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    void SetReach(int Value, bool IsInteractive);

public:
#if WITH_EDITOR
    virtual UOdysseyLayerCell* Break(int Frame, bool bClear) override;
#endif

protected:
    friend class FOdysseyLayerCellImageStaggerImport;

    UPROPERTY()
    EOdysseyLayerCellImageStaggerBehaviour Behaviour = EOdysseyLayerCellImageStaggerBehaviour::Loop;

    UPROPERTY()
    int Reach = 0;
};
