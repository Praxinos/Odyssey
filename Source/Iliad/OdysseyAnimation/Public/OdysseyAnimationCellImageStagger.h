// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyAnimationCell.h"

#include "OdysseyAnimationCellImageStagger.generated.h"

UENUM(BlueprintType)
enum class EOdysseyAnimationCellImageStaggerBehaviour : uint8
{
    Loop,
    PingPong,
    Random
};

UCLASS(BlueprintType)
class ODYSSEYANIMATION_API UOdysseyAnimationCellImageStagger
    : public UOdysseyAnimationCell
{
    GENERATED_BODY()

public:
    //IOdysseyRenderingAbility overrides
    virtual bool BuildRenderPipelineInternal(
        IOdysseyTextureRenderingAbility::FRenderFunction& oRenderFunction,
        const TArray<const IOdysseyTextureRenderingAbility*>& iParents, const FOdysseyRenderContext* RenderContext
    ) const override;
    virtual TArray<FGuid> GetRenderingComposition(uint64 iRenderType, int iFrameIndex) const override;
    virtual FIntRect GetDefaultRenderRect() const override;
#if WITH_EDITOR
    virtual void OldSerialize(FArchive& Ar) override; //DEPRECATED: Keep that for compatibility with early versions of Odyssey
#endif

public:
    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    int GetReferenceFrameAtFrame(int Frame) const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    UOdysseyAnimationCell* GetReferenceCellAtFrame(int Frame, bool Recursive = true) const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    EOdysseyAnimationCellImageStaggerBehaviour GetBehaviour() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    int GetReach() const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    void SetBehaviour(EOdysseyAnimationCellImageStaggerBehaviour Value);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    void SetReach(int Value, bool IsInteractive);

protected:
    void UpdateRandomIntegers( int iMaxValue ) const;

public:
#if WITH_EDITOR
    virtual UOdysseyAnimationCell* Break(int Frame, bool bClear) override;
#endif

protected:
    friend class FOdysseyAnimationCellImageStaggerImport;

    UPROPERTY()
    EOdysseyAnimationCellImageStaggerBehaviour Behaviour = EOdysseyAnimationCellImageStaggerBehaviour::Loop;

    UPROPERTY()
    int Reach = 0;

    UPROPERTY()
    int Seed = 0;

    private:
        // declared mutable to bypass the functions const-ness
        mutable TArray<int> RandomIntegers;
        mutable int RandomLimitValue = 0;
};
