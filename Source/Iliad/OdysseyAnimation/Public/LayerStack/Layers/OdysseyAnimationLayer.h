// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyAnimationLayerStack.h"
#include "OdysseyLayer.h"
#include "OdysseyLighttable.h"

#include "OdysseyAnimationLayer.generated.h"

class UOdysseyAnimationCell;

UENUM(BlueprintType)
enum class EOdysseyLayerImagePostBehaviour : uint8
{
    None,
    Hold,
    Loop,
    PingPong
};

UCLASS(Abstract, BlueprintType, HideDropdown)
class ODYSSEYANIMATION_API UOdysseyAnimationLayer
    : public UOdysseyLayer
{
    GENERATED_BODY()

public:
#if WITH_EDITOR
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnDisplayCellNamesChanged, UOdysseyLayer*);
#endif
    DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCellsChanged, UOdysseyLayer*, bool);
#if WITH_EDITOR
    static FOnDisplayCellNamesChanged& OnDisplayCellNamesChanged();
#endif
    static bool AreCellsContiguous( const TArray<UOdysseyAnimationCell*>& iCellsToReverse );
    static bool AreCellsContiguous( const TArray<UOdysseyAnimationCell*>& iCellsToReverse
                                  , TArray<UOdysseyAnimationCell*>& oSortedCells  );
    /** Groups adjacent cells in ascending layer order. */
    static TArray<TArray<UOdysseyAnimationCell*>> GetCellsContiguousGroups( const TArray<UOdysseyAnimationCell*>& iCells );
    /** Called when cells change outside undo/redo. */
    static FOnCellsChanged& OnCellsChanged();
    virtual UOdysseyAnimationLayerStack* GetLayerStack() const override;
    virtual TArray<FGuid> GetRenderingComposition(uint64 iRenderType, int iFrame) const override;
    /** Resolves animated cells, pre/post behaviours and editor lighttable content. */
    virtual bool BuildRenderPipelineInternal(FRenderFunction& oRenderFunction,
        const TArray<const IOdysseyTextureRenderingAbility*>& iParents, const FOdysseyRenderContext* RenderContext) const override;


    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    UOdysseyAnimation* GetAnimation() const;

#if WITH_EDITOR
    FSimpleMulticastDelegate& OnLighttableChanged();
#endif
#if WITH_EDITOR

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    bool ShouldDisplayCellNames() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    FOdysseyLighttable GetLighttable() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    bool HasLighttable() const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void SetLighttable(FOdysseyLighttable Value);
#endif

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void SetCellsOffset(int Value);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void SetPreBehaviour(EOdysseyLayerImagePostBehaviour Value);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void SetPostBehaviour(EOdysseyLayerImagePostBehaviour Value);

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    TSubclassOf<UOdysseyAnimationCell> GetDefaultCellClass() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    int GetCellsOffset() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    EOdysseyLayerImagePostBehaviour GetPreBehaviour() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    EOdysseyLayerImagePostBehaviour GetPostBehaviour() const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    const TArray<TSubclassOf<UOdysseyAnimationCell>>& GetSupportedCellTypes() const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    const TArray<UOdysseyAnimationCell*>& GetCells() const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    int GetPreBehaviourFrame(EOdysseyLayerImagePostBehaviour Behaviour, int Frame) const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    int GetPostBehaviourFrame(EOdysseyLayerImagePostBehaviour Behaviour, int Frame) const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    UOdysseyAnimationCell* GetCellAtFrame(int Frame) const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    int GetCellIndexAtFrame(int Frame) const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    bool HasCellAtFrame(int Frame) const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer", meta=(DeterminesOutputType = "CellType"))
    UOdysseyAnimationCell* AddCell(TSubclassOf<UOdysseyAnimationCell> CellType, int Index = -1 );

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer", meta=(DeterminesOutputType = "CellType"))
    TArray<UOdysseyAnimationCell*> AddCells(TSubclassOf<UOdysseyAnimationCell> CellType, int Index = -1, int Count = 1 );

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void RemoveCell(UOdysseyAnimationCell* Cell); //Prevent Empty Layer ?

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void RemoveCells(const TArray<UOdysseyAnimationCell*>& Cells); //Prevent Empty Layer ?

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void RemoveCellAtIndex(int Index);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void RemoveAllCells();

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    UOdysseyAnimationCell* CopyCell(UOdysseyAnimationCell* Cell, int Index = -1);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    UOdysseyAnimationCell* CopyCellToLayer(UOdysseyAnimationCell* Cell, UOdysseyAnimationLayer* ToLayer, int Index = -1) const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    TArray<UOdysseyAnimationCell*> CopyCells(TArray<UOdysseyAnimationCell*> Cells, int Index = -1);
#if WITH_EDITOR
    void SetLighttableInteractive(FOdysseyLighttable Value);
    void SetDisplayCellNames(bool Value);
    void AddCellInteractive(int Index = -1 );
    void AddCellsInteractive(int Index = -1, int Count = 1 );
    void SetCellsOffsetInteractive(float Value);
    void CellsChangedInteractive();
#endif
    /** Invalidates the cached frame ranges after exposure or offset changes. */
    void InvalidateCellsFrameRanges();
    const TArray<FInt32Range>& GetCellsFrameRanges() const;
    void UpdateCellsIndexInLayer();
    bool ReverseCells( const TArray<UOdysseyAnimationCell*>& iCellsToReverse );
    virtual void PostInitProperties() override;
    virtual FInt32Range GetFrameRange() const override;
#if WITH_EDITOR
    virtual TArray<FName> GetRows() const override;
    virtual int GetRowHeight(FName iSubRowName) const override;
    virtual bool IsRowVisible(FName iSubRowName) const override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;
#endif
#if WITH_EDITOR
private:
    /** Builds the surrounding animation keys using the request's frame. */
    bool BuildLighttableRenderPipeline(FRenderFunction& oRenderFunction,
        const TArray<const IOdysseyTextureRenderingAbility*>& iParents, const FOdysseyRenderContext* RenderContext) const;
    TArray<FGuid> GetLighttableImageRenderingComposition(int iFrameIndex) const;

protected:
    friend class FOdysseyAnimationCellsContainerImport;
#endif
protected:
    /** Updates cell indices, cached exposures and rendering after an edit. */
    virtual void CellsChanged();

protected:
#if WITH_EDITORONLY_DATA

    UPROPERTY(NonTransactional)
    bool bDisplayCellNames = false;

    UPROPERTY(NonTransactional)
    FOdysseyLighttable Lighttable;

    UPROPERTY(NonTransactional)
    bool bHasLighttable = true;
#endif

    UPROPERTY()
    TSubclassOf<UOdysseyAnimationCell> DefaultCellClass = nullptr;

    UPROPERTY()
    int CellsOffset = 0;

    UPROPERTY()
    EOdysseyLayerImagePostBehaviour PreBehaviour = EOdysseyLayerImagePostBehaviour::None;

    UPROPERTY()
    EOdysseyLayerImagePostBehaviour PostBehaviour = EOdysseyLayerImagePostBehaviour::None;

    UPROPERTY()
    TArray<TSubclassOf<UOdysseyAnimationCell>> SupportedCellTypes;

    UPROPERTY()
    TArray<TObjectPtr<UOdysseyAnimationCell>> Cells;
#if WITH_EDITORONLY_DATA
    FSimpleMulticastDelegate mOnLighttableChanged;
#endif
private:
    mutable TArray<FInt32Range> mCellsFrameRanges;

};
