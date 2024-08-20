// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyLayer.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"

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

UCLASS(BlueprintType)
class ODYSSEYANIMATION_API UOdysseyAnimationLayer
    : public UOdysseyLayer
{
    GENERATED_BODY()

public:
	virtual void PostInitProperties() override;

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
	const TArray<UClass*>& GetSupportedCellTypes() const;

	UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
	const TArray<UOdysseyAnimationCell*>& GetCells() const;

	UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
	UOdysseyAnimationCell* GetCellAtFrame(int Frame) const;
	
	UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
	bool HasCellAtFrame(int Frame) const;

	UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
	UOdysseyAnimationCell* AddCell(TSubclassOf<UOdysseyAnimationCell> CellType, int Index = -1 );

	UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
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
    TArray<UOdysseyAnimationCell*> CopyCells(TArray<UOdysseyAnimationCell*> Layers, int Index = -1);

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
	TArray<TObjectPtr<UClass>> SupportedCellTypes;

	UPROPERTY()
	TArray<TObjectPtr<UOdysseyAnimationCell>> Cells;

public:
	UPROPERTY(BlueprintReadOnly, Category="Odyssey|Layer")
	TObjectPtr<UClass> DefaultCellClass = nullptr;

	UPROPERTY(BlueprintReadWrite, Category="Odyssey|Layer")
	int CellsOffset = 0;

    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Layer")
    EOdysseyAnimationLayerImagePostBehaviour PreBehaviour = EOdysseyAnimationLayerImagePostBehaviour::None;

    UPROPERTY(BlueprintReadWrite, Category="Odyssey|Layer")
    EOdysseyAnimationLayerImagePostBehaviour PostBehaviour = EOdysseyAnimationLayerImagePostBehaviour::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Odyssey|Layer", NonTransactional)
	FOdysseyAnimationLightTable Lighttable;

    FSimpleMulticastDelegate mOnLightTableChanged;
	FSimpleMulticastDelegate mOnCellsChanged;
};
