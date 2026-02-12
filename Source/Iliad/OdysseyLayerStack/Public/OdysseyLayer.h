// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyBlendingMode.h"
#include "OdysseyMediaProvider.h"
#include "OdysseyTextureRenderingAbility.h"
#include "Textures/SlateIcon.h"
#include "Layout/Margin.h"
#include "Templates/SubclassOf.h"

#if WITH_EDITOR
#include "OdysseyLighttable.h"
#endif

#include "OdysseyLayer.generated.h"

class UOdysseyLayerCell;
class UOdysseyLayerStack;

UENUM(BlueprintType)
enum  class  EGetLayerChildrenMethod : uint8
{
    DepthFirst,
    BreadthFirst
};

UENUM(BlueprintType)
enum class EOdysseyLayerImagePostBehaviour : uint8
{
    None,
    Hold,
    Loop,
    PingPong
};

UCLASS(Abstract, HideDropdown, BlueprintType, config=EditorPerProjectUserSettings, PerObjectConfig)
class ODYSSEYLAYERSTACK_API UOdysseyLayer
    : public UObject
    , public IOdysseyTextureRenderingAbility
{
    GENERATED_BODY()

public:
#if WITH_EDITOR
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnDisplayChildrenChanged, UOdysseyLayer*);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnDisplayOptionsChanged, UOdysseyLayer*);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnDisplayCellNamesChanged, UOdysseyLayer*);

    static FOnDisplayChildrenChanged& OnDisplayChildrenChanged();
    static FOnDisplayOptionsChanged& OnDisplayOptionsChanged();
    static FOnDisplayCellNamesChanged& OnDisplayCellNamesChanged();
    FSimpleMulticastDelegate& OnLighttableChanged();
#endif

    FSimpleMulticastDelegate& OnCellsChanged();

public:
#if WITH_EDITOR
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    bool IsEditable() const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void SetIsLocked(bool Value);

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    bool IsLocked() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    bool IsLockedRecursively() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    bool ShouldDisplayChildren() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    bool ShouldDisplayOptions() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    bool ShouldDisplayCellNames() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    FOdysseyLighttable GetLighttable() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    bool HasLighttable() const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void SetLighttable(FOdysseyLighttable Value);
#endif

public:
    //Setters

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void AddChild(UOdysseyLayer* Layer, int IndexInParent = -1);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void AddChildren(TArray<UOdysseyLayer*> Layers, int IndexInParent = -1);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void RemoveChild(UOdysseyLayer* Layer);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void RemoveChildren(TArray<UOdysseyLayer*> Layers);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void SetLayerName(FText Value);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void SetIsActivated(bool Value);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void SetBlendMode(EOdysseyBlendingMode Value);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void SetOpacity(float Value);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void SetCellsOffset(int Value);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void SetPreBehaviour(EOdysseyLayerImagePostBehaviour Value);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void SetPostBehaviour(EOdysseyLayerImagePostBehaviour Value);

public:
    //Getters
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    virtual UOdysseyLayerStack* GetLayerStack() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    TSubclassOf<UOdysseyLayerCell> GetDefaultCellClass() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    FText GetLayerTypeName() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    FText GetDescription() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    FText GetDefaultName() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    UOdysseyLayer* GetParent() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    TArray<UOdysseyLayer*> GetParents() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    bool CanHaveChildren() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    const TArray<UOdysseyLayer*>& GetChildren() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    TArray<UOdysseyLayer*> GetChildrenRecursively(EGetLayerChildrenMethod Method = EGetLayerChildrenMethod::DepthFirst) const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    int GetIndexInParent() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    bool IsChildOf(UOdysseyLayer* Layer) const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    virtual FInt32Range GetFrameRange() const override;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    FText GetLayerName() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    bool IsActivated() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    bool IsActivatedRecursively() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    EOdysseyBlendingMode GetBlendMode() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    float GetOpacity() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    int GetCellsOffset() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    EOdysseyLayerImagePostBehaviour GetPreBehaviour() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    EOdysseyLayerImagePostBehaviour GetPostBehaviour() const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    const TArray<TSubclassOf<UOdysseyLayerCell>>& GetSupportedCellTypes() const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    const TArray<UOdysseyLayerCell*>& GetCells() const;

public:
    // Advanced Getters

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    int GetPreBehaviourFrame(EOdysseyLayerImagePostBehaviour Behaviour, int Frame) const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    int GetPostBehaviourFrame(EOdysseyLayerImagePostBehaviour Behaviour, int Frame) const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    UOdysseyLayerCell* GetCellAtFrame(int Frame) const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    int GetCellIndexAtFrame(int Frame) const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    bool HasCellAtFrame(int Frame) const;

public:
    //Advanced Setters

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer", meta=(DeterminesOutputType = "CellType"))
    UOdysseyLayerCell* AddCell(TSubclassOf<UOdysseyLayerCell> CellType, int Index = -1 );

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer", meta=(DeterminesOutputType = "CellType"))
    TArray<UOdysseyLayerCell*> AddCells(TSubclassOf<UOdysseyLayerCell> CellType, int Index = -1, int Count = 1 );

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void RemoveCell(UOdysseyLayerCell* Cell); //Prevent Empty Layer ?

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void RemoveCells(const TArray<UOdysseyLayerCell*>& Cells); //Prevent Empty Layer ?

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void RemoveCellAtIndex(int Index);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void RemoveAllCells();

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    UOdysseyLayerCell* CopyCell(UOdysseyLayerCell* Cell, int Index = -1);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    UOdysseyLayerCell* CopyCellToLayer(UOdysseyLayerCell* Cell, UOdysseyLayer* ToLayer, int Index = -1) const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    TArray<UOdysseyLayerCell*> CopyCells(TArray<UOdysseyLayerCell*> Cells, int Index = -1);

public:
    //Merge

    /**
     * @brief Returns the layer types generated by this layer if a merge is called on it right now
     * Can change depending on the layer content or configuration (ex: layer folder children layer types)
     *
     * @return TSet<UClass*>
     */
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer" )
    virtual TSet<UClass*> GetMergeDefaultLayerTypes() const;

    /**
     * @brief Returns the layer types generated by this layer if it is merged as part of one of the given layer types
     * Can change depending on the layer content or configuration (ex: layer folder children layer types)
     *
     * @return TSet<UClass*>
     */
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer" )
    virtual TSet<UClass*> GetMergeLayerTypesFromTypes(TSet<UClass*> LayerTypes) const;

    /**
     * @brief Merges the given layers into this layer
     * Only works with Layer class being a child of classes returned by GetMergeLayerTypes()
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    virtual void Merge(const TArray<UOdysseyLayer*>& Layers);

public:
    //Invalidate the frame ranges of all cells
    //Used for performance optimisation to avoid iterating over all cells each time we need a cell's frame range
#if WITH_EDITOR
    void SetLighttableInteractive(FOdysseyLighttable Value);
    const FSlateIcon& GetIcon() const;
    const FSlateIcon& GetIconExpanded() const;
    void SetDisplayChildren(bool Value);
    void SetDisplayOptions(bool Value);
    void SetDisplayCellNames(bool Value);
    void AddCellInteractive(int Index = -1 );
    void AddCellsInteractive(int Index = -1, int Count = 1 );
    void SetOpacityInteractive(float Value);
    void SetCellsOffsetInteractive(float Value);
    void CellsChangedInteractive();
#endif

    void InvalidateCellsFrameRanges();
    const TArray<FInt32Range>& GetCellsFrameRanges() const;
    void UpdateCellsIndexInLayer();
    bool ReverseCells( const TArray<UOdysseyLayerCell*>& iCellsToReverse );
    static bool AreCellsContiguous( const TArray<UOdysseyLayerCell*>& iCellsToReverse );
    static bool AreCellsContiguous( const TArray<UOdysseyLayerCell*>& iCellsToReverse
                                  , TArray<UOdysseyLayerCell*>& oSortedCells  );

public:
#if WITH_EDITOR
    virtual TArray<FName> GetRows() const;
    virtual int GetRowHeight(FName iSubRowName) const;
    virtual bool IsRowVisible(FName iSubRowName) const;
    virtual FMargin GetRowPadding(FName iSubRowName) const;
    virtual FOdysseyMediaProvider GetMediaProvider(uint32 iFrameIndex) const;
#endif

    virtual void PostInitProperties() override;
    virtual void PostLoad() override;
    virtual void PostDuplicate(EDuplicateMode::Type iDuplicateMode) override;
    virtual TArray<FGuid> GetRenderingComposition(uint64 iRenderType, int iFrame) const override;
    virtual FIntRect GetDefaultRenderRect() const override;

    virtual UTextureRenderTarget2D* CreateRenderingRenderTarget() const override;
#if WITH_EDITOR
    virtual UTexture2D* CreateExportTexture(UObject* Outer, FName Name, EObjectFlags Flags = RF_NoFlags) override;
#endif
    virtual bool BuildRenderPipelineInternal(FFrameNumber iFrame, uint64 iType, IOdysseyTextureRenderingAbility::FRenderFunction& oRenderFunction, const IOdysseyTextureRenderingAbility::FCanRenderFunction& iCanRenderFunction, const TArray<const IOdysseyTextureRenderingAbility*>& iParents) const override;

protected:
    bool BuildRenderChildrenPipeline(FFrameNumber iFrame, uint64 iType, IOdysseyTextureRenderingAbility::FRenderFunction& oRenderFunction, const IOdysseyTextureRenderingAbility::FCanRenderFunction& iCanRenderFunction, const TArray<const IOdysseyTextureRenderingAbility*>& iParents) const;
    //Property changed methods
    virtual void CellsChanged();

#if WITH_EDITOR
    bool BuildLighttableRenderPipeline(FFrameNumber iFrame, uint64 iType, IOdysseyTextureRenderingAbility::FRenderFunction& oRenderFunction, const IOdysseyTextureRenderingAbility::FCanRenderFunction& iCanRenderFunction, const TArray<const IOdysseyTextureRenderingAbility*>& iParents) const;
    TArray<FGuid> GetLighttableImageRenderingComposition(int iFrameIndex) const;
    // UObject overrides
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;
    // overide for UObject::IsSelected() which calls IsSelectedInEditor()
    virtual bool IsSelectedInEditor() const override;
#endif

protected:
    //Default properties
#if WITH_EDITORONLY_DATA
    FSimpleMulticastDelegate mOnLighttableChanged;
    FSlateIcon Icon;
    FSlateIcon IconExpanded;
#endif
    UPROPERTY()
    FText LayerTypeName = FText::FromString(TEXT("Unnamed Layer Type"));

    UPROPERTY()
    FText Description = FText::FromString(TEXT(""));

    UPROPERTY()
    FText DefaultName = FText::FromString(TEXT("Layer"));

    //Defaults Properties
    UPROPERTY()
    bool bCanHaveChildren = false;

    UPROPERTY(Transient, DuplicateTransient)
    UOdysseyLayer* Parent = nullptr;

    UPROPERTY()
    TArray<TObjectPtr<UOdysseyLayer>> Children;

protected:
//Instance properties

#if WITH_EDITORONLY_DATA
    UPROPERTY()
    bool bIsLocked = false;

    UPROPERTY(NonTransactional)
    bool bDisplayChildren = true;

    UPROPERTY(NonTransactional)
    bool bDisplayOptions = true;

    UPROPERTY(NonTransactional)
    bool bDisplayCellNames = false;

    UPROPERTY(NonTransactional)
    FOdysseyLighttable Lighttable;

    UPROPERTY(NonTransactional)
    bool bHasLighttable = true;
#endif

    UPROPERTY()
    FText Name;

    UPROPERTY()
    bool bIsActivated = true;

    UPROPERTY()
    EOdysseyBlendingMode BlendMode = EOdysseyBlendingMode::kNormal;

    UPROPERTY()
    float Opacity = 1.0f;

    UPROPERTY()
    TSubclassOf<UOdysseyLayerCell> DefaultCellClass = nullptr;

    UPROPERTY()
    int CellsOffset = 0;

    UPROPERTY()
    EOdysseyLayerImagePostBehaviour PreBehaviour = EOdysseyLayerImagePostBehaviour::None;

    UPROPERTY()
    EOdysseyLayerImagePostBehaviour PostBehaviour = EOdysseyLayerImagePostBehaviour::None;

    UPROPERTY()
    TArray<TSubclassOf<UOdysseyLayerCell>> SupportedCellTypes;

    UPROPERTY()
    TArray<TObjectPtr<UOdysseyLayerCell>> Cells;

private:
    mutable TArray<FInt32Range> mCellsFrameRanges;
    FSimpleMulticastDelegate mOnCellsChanged;
};
