// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyLayerCell.h"

#include "OdysseyLayerCellImageStagger.generated.h"

UENUM(BlueprintType)
enum class EOdysseyLayerCellImageStaggerBehaviour : uint8
{
    Loop,
    PingPong,
    Random
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
#if WITH_EDITOR
    virtual void OldSerialize(FArchive& Ar) override; //DEPRECATED: Keep that for compatibility with early versions of Odyssey
#endif

public:
    /**
     * Return the layer frame replicated by this Stagger Cell.
     *
     * Frame is relative to the start of the Stagger Cell. Return -1
     * if there is no frame to replicate.
     */
    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    int GetReferenceFrameAtFrame(int Frame) const;

    /**
     * Return the Cell replicated by this Stagger Cell at Frame.
     *
     * Frame is relative to the start of the Stagger Cell. If the
     * replicated Cell is itself a Stagger Cell, it is resolved too.
     */
    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    UOdysseyLayerCell* GetReferenceCellAtFrame(int Frame, bool Recursive = true) const;

    /** Return the stagger mode of this Cell: Loop, Ping-Pong or Random. */
    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    EOdysseyLayerCellImageStaggerBehaviour GetBehaviour() const;

    /**
     * Return how many previous frames are replicated by this Cell.
     *
     * A value of 0 replicates all previous frames of the layer.
     */
    UFUNCTION(BlueprintPure, Category="Odyssey|Cell")
    int GetReach() const;

    /** Set the stagger mode of this Cell: Loop, Ping-Pong or Random. */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    void SetBehaviour(EOdysseyLayerCellImageStaggerBehaviour Value);

    /**
     * Set how many previous frames are replicated by this Cell.
     *
     * A value of 0 replicates all previous frames of the layer.
     * IsInteractive should be true while the value is being dragged.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Cell")
    void SetReach(int Value, bool IsInteractive);

protected:
    void UpdateRandomIntegers( int iMaxValue ) const;

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

    UPROPERTY()
    int Seed = 0;

    private:
        // declared mutable to bypass the functions const-ness
        mutable TArray<int> mRandomIntegers;
        mutable int mRandomLimitValue = 0;
};
