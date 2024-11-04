// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyLayer.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"
#include "OdysseyLayer.h"
#include "Templates/SubclassOf.h"

#include "OdysseyAnimationLayer.generated.h"

class UOdysseyAnimation;
class UOdysseyAnimationCell;

UENUM(BlueprintType)
enum class EOdysseyAnimationLayerImagePostBehaviour : uint8
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
	virtual void PostInitProperties() override;
	virtual UOdysseyAnimationLayerStack* GetLayerStack() const;

public:
    //Invalidate the frame ranges of all cells
    //Used for performance optimisation to avoid iterating over all cells each time we need a cell's frame range
    void InvalidateCellsFrameRanges();
    const TArray<FInt32Range>& GetCellsFrameRanges() const;

public:
    //Getters
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    UOdysseyAnimation* GetAnimation() const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    virtual FInt32Range GetFrameRange() const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    int GetPreBehaviourFrame(EOdysseyAnimationLayerImagePostBehaviour Behaviour, int Frame) const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    int GetPostBehaviourFrame(EOdysseyAnimationLayerImagePostBehaviour Behaviour, int Frame) const;

public:
    //Cells
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    const TArray<TSubclassOf<UOdysseyAnimationCell>>& GetSupportedCellTypes() const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    const TArray<UOdysseyAnimationCell*>& GetCells() const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    UOdysseyAnimationCell* GetCellAtFrame(int Frame) const;
    
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
    UOdysseyAnimationCell* CopyCell(UOdysseyAnimationCell* Cell, int Index = -1);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    TArray<UOdysseyAnimationCell*> CopyCells(TArray<UOdysseyAnimationCell*> Cells, int Index = -1);

public:
#ifdef WITH_EDITOR
	virtual TArray<FName> GetRows() const override;
	virtual int GetRowHeight(FName iSubRowName) const override;
	virtual bool IsRowVisible(FName iSubRowName) const override;
#endif

protected:
    //Property changes
    virtual void LightTableChanged(bool iIsInteractive);
    virtual void PreBehaviourChanged();
    virtual void PostBehaviourChanged();
    virtual void CellsChanged(bool iIsInteractive);
    virtual void CellsOffsetChanged(bool iIsInteractive);
    virtual void PropertyChanged(const FName& iPropertyName, const FName& iMemberPropertyName, bool iIsInteractive) override;

protected:
    TArray<FGuid> GetLighttableImageRenderingComposition(int iFrameIndex) const;
    void UpdateCellsIndexInLayer();

public:
    FSimpleMulticastDelegate& OnLightTableChanged();
    FSimpleMulticastDelegate& OnCellsChanged();

private:
    mutable TArray<FInt32Range> mCellsFrameRanges;

protected:
    friend class FOdysseyAnimationCellsContainerImport;
    friend class FOdysseyAnimationLayerImageRasterImport;
    friend class FOdysseyAnimationLayerImageVectorImport;

    UPROPERTY()
    TArray<TSubclassOf<UOdysseyAnimationCell>> SupportedCellTypes;

    UPROPERTY()
    TArray<TObjectPtr<UOdysseyAnimationCell>> Cells;

private:
    UFUNCTION(BlueprintSetter)
    void CellsOffsetBlueprintSetter(int Value);

    UFUNCTION(BlueprintSetter)
    void PreBehaviourBlueprintSetter(EOdysseyAnimationLayerImagePostBehaviour Value);

    UFUNCTION(BlueprintSetter)
    void PostBehaviourBlueprintSetter(EOdysseyAnimationLayerImagePostBehaviour Value);

    UFUNCTION(BlueprintSetter)
    void LighttableBlueprintSetter(FOdysseyAnimationLightTable Value);

public:
    UPROPERTY(BlueprintReadOnly, Category="Odyssey|Layer")
    TSubclassOf<UOdysseyAnimationCell> DefaultCellClass = nullptr;

    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Layer", BlueprintSetter=CellsOffsetBlueprintSetter)
    int CellsOffset = 0;

    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Layer", BlueprintSetter=PreBehaviourBlueprintSetter)
    EOdysseyAnimationLayerImagePostBehaviour PreBehaviour = EOdysseyAnimationLayerImagePostBehaviour::None;

    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Layer", BlueprintSetter=PostBehaviourBlueprintSetter)
    EOdysseyAnimationLayerImagePostBehaviour PostBehaviour = EOdysseyAnimationLayerImagePostBehaviour::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Odyssey|Layer", BlueprintSetter=LighttableBlueprintSetter, NonTransactional)
    FOdysseyAnimationLightTable Lighttable;

	UPROPERTY(BlueprintReadOnly, Category="Odyssey|Layer", NonTransactional)
	bool HasLighttable = true;

    FSimpleMulticastDelegate mOnLightTableChanged;
    FSimpleMulticastDelegate mOnCellsChanged;
};
