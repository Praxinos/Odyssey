// IDDN.FR.001.060015.015.S.X.2019.000.00000
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

    DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCellsChanged, UOdysseyLayer*, bool /*IsInteractive*/);
    /**
     * Called when cells are changed
     * Not called on undo/redo
     */
    static FOnCellsChanged& OnCellsChanged();

public:
    /**
     * Return true if this layer can be edited.
     *
     * A layer is editable when neither it nor any of its parents is
     * locked, and when it and all of its parents are visible.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    bool IsEditable() const;

    /**
     * Lock or unlock this layer.
     *
     * Once locked, the layer can not be edited.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void SetIsLocked(bool Value);

    /** Return true if this layer itself is locked. */
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    bool IsLocked() const;

    /** Return true if this layer or any of its parents is locked. */
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    bool IsLockedRecursively() const;

#if WITH_EDITOR
    /** Return true if the children of this layer are expanded. */
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    bool ShouldDisplayChildren() const;

    /**
     * Return true if more details about this layer are shown.
     *
     * This is the state of the + / - icon of the layer.
     */
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    bool ShouldDisplayOptions() const;

    /** Return true if the Cells' names and exposure count are displayed. */
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    bool ShouldDisplayCellNames() const;

    /** Expand or collapse the children of this layer in the Layer Stack. */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Layer" )
    void SetDisplayChildren( bool Value );

    /**
     * Show more or less details about this layer in the Layer Stack.
     *
     * This is the same as clicking the + / - icon of the layer.
     */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Layer" )
    void SetDisplayOptions( bool Value );

    /** Show or hide the Cells' names and exposure count. */
    UFUNCTION( BlueprintCallable, Category = "Odyssey|Layer" )
    void SetDisplayCellNames( bool Value );

    /**
     * Return the Light Table settings of this layer.
     *
     * The Light Table displays by transparency the content of
     * previous and next Cells (also called onion skin).
     */
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    FOdysseyLighttable GetLighttable() const;

    /** Return true if this layer supports the Light Table. */
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    bool HasLighttable() const;

    /**
     * Set the Light Table settings of this layer.
     *
     * Has no effect if the layer does not support the Light Table.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void SetLighttable(FOdysseyLighttable Value);
#endif

public:
    //Setters
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void AddChild(UOdysseyLayer* Layer, int IndexInParent = -1);

    /**
     * Add the given layers as children of this layer.
     *
     * Layers are inserted at IndexInParent, or at the end if it is -1.
     * Layers that already have a parent are ignored.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void AddChildren(TArray<UOdysseyLayer*> Layers, int IndexInParent = -1);

    /** Remove the given child layer from this layer. */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void RemoveChild(UOdysseyLayer* Layer);

    /** Remove the given child layers from this layer. */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void RemoveChildren(TArray<UOdysseyLayer*> Layers);

    /** Set the name of this layer. */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void SetLayerName(FText Value);

    /**
     * Show or hide this layer.
     *
     * This is the same as clicking the eye icon of the layer.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void SetIsActivated(bool Value);

    /**
     * Return true if the given Blending mode can be applied to this layer.
     *
     * Stencil and Erase modes can not be set while Alpha inheritance
     * is enabled. Always false if the layer is not editable.
     */
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    bool CanSetBlendMode(EOdysseyBlendMode Value) const;

    /**
     * Set the Blending mode of this layer.
     *
     * Blending modes alter the way colors, tones and pixels of this layer
     * interact with those of the layers beneath.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void SetBlendMode(EOdysseyBlendMode Value);

    /** Set the opacity of this layer. */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void SetOpacity(float Value);

    /**
     * Set the frame at which the first Cell of this layer starts.
     *
     * Move the whole animation of the layer, so it starts sooner or
     * later, like the Moving tool of the Timeline.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void SetCellsOffset(int Value);

    /**
     * Set the Pre Behavior of this layer.
     *
     * Force a specific behavior before the animation starts:
     * - None
     * - Hold
     * - Loop
     * - Ping-pong
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void SetPreBehaviour(EOdysseyLayerImagePostBehaviour Value);

    /**
     * Set the Post Behavior of this layer.
     *
     * Force a specific behavior after the animation finishes:
     * - None
     * - Hold
     * - Loop
     * - Ping-pong
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void SetPostBehaviour(EOdysseyLayerImagePostBehaviour Value);

    /**
     * Return true if Alpha inheritance can be changed on this layer.
     *
     * Not possible while the layer uses the Stencil or Erase Blending
     * mode, or when the layer is not editable.
     */
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    bool CanSetInheritsAlpha() const;

    /**
     * Enable or disable Alpha inheritance on this layer.
     *
     * When enabled, the content of the layer is affected by the alpha
     * channel of the layers underneath (clipping mask).
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void SetInheritsAlpha(bool InheritsAlpha = true);

public:
    //Getters
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    virtual UOdysseyLayerStack* GetLayerStack() const;

    /** Return the type of Cell created by default in this layer. */
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    TSubclassOf<UOdysseyLayerCell> GetDefaultCellClass() const;

    /** Return the display name of this layer type. */
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    FText GetLayerTypeName() const;

    /** Return the description of this layer type. */
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    FText GetDescription() const;

    /** Return the default name given to new layers of this type. */
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    FText GetDefaultName() const;

    /** Return the parent layer of this layer, if any. */
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    UOdysseyLayer* GetParent() const;

    /**
     * Return all the ancestors of this layer.
     *
     * The direct parent comes first, followed by its own parents
     * up to the root of the stack.
     */
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    TArray<UOdysseyLayer*> GetParents() const;

    /** Return true if this layer can contain child layers (e.g. Folder). */
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    bool CanHaveChildren() const;

    /** Return the direct children of this layer. */
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    const TArray<UOdysseyLayer*>& GetChildren() const;

    /**
     * Return all the descendants of this layer.
     *
     * Method selects whether the hierarchy is traversed depth first
     * or breadth first.
     */
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    TArray<UOdysseyLayer*> GetChildrenRecursively(EGetLayerChildrenMethod Method = EGetLayerChildrenMethod::DepthFirst) const;

    /** Return the index of this layer among the children of its parent. */
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    int GetIndexInParent() const;

    /**
     * Return true if the given layer is a parent of this layer.
     *
     * Check all ancestors, not only the direct parent.
     */
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    bool IsChildOf(UOdysseyLayer* Layer) const;

    /**
     * Return the range of frames covered by this layer.
     *
     * Include the Cells of the layer and of all its children.
     */
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    virtual FInt32Range GetFrameRange() const override;

    /** Return the name of this layer. */
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    FText GetLayerName() const;

    /** Return true if this layer itself is visible. */
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    bool IsActivated() const;

    /** Return true if this layer and all of its parents are visible. */
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    bool IsActivatedRecursively() const;

    /** Return the Blending mode of this layer. */
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    EOdysseyBlendMode GetBlendMode() const;

    /** Return the opacity of this layer. */
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    float GetOpacity() const;

    /** Return the frame at which the first Cell of this layer starts. */
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    int GetCellsOffset() const;

    /**
     * Return the Pre Behavior of this layer.
     *
     * The behavior applied before the animation starts:
     * - None
     * - Hold
     * - Loop
     * - Ping-pong
     */
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    EOdysseyLayerImagePostBehaviour GetPreBehaviour() const;

    /**
     * Return the Post Behavior of this layer.
     *
     * The behavior applied after the animation finishes:
     * - None
     * - Hold
     * - Loop
     * - Ping-pong
     */
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    EOdysseyLayerImagePostBehaviour GetPostBehaviour() const;

    /** Return the types of Cells that can be added to this layer. */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    const TArray<TSubclassOf<UOdysseyLayerCell>>& GetSupportedCellTypes() const;

    /** Return the Cells of this layer, in timeline order. */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    const TArray<UOdysseyLayerCell*>& GetCells() const;

    /**
     * Return true if Alpha inheritance is enabled on this layer.
     *
     * When enabled, the layer is clipped by the alpha channel of the
     * layers underneath.
     */
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    bool GetInheritsAlpha() const;

public:
    // Advanced Getters

    /**
     * Return the frame displayed at Frame by the given Pre Behavior.
     *
     * Use it for frames located before the start of the layer.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    int GetPreBehaviourFrame(EOdysseyLayerImagePostBehaviour Behaviour, int Frame) const;

    /**
     * Return the frame displayed at Frame by the given Post Behavior.
     *
     * Use it for frames located after the end of the layer.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    int GetPostBehaviourFrame(EOdysseyLayerImagePostBehaviour Behaviour, int Frame) const;

    /** Return the Cell displayed at the given frame, if any. */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    UOdysseyLayerCell* GetCellAtFrame(int Frame) const;

    /**
     * Return the index of the Cell displayed at the given frame.
     *
     * Return -1 if there is no Cell at this frame.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    int GetCellIndexAtFrame(int Frame) const;

    /** Return true if a Cell is displayed at the given frame. */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    bool HasCellAtFrame(int Frame) const;

public:
    //Advanced Setters

    /**
     * Add a new Cell of the given type to this layer.
     *
     * The Cell is inserted at Index, or at the end if Index is -1.
     * Return the new Cell.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer", meta=(DeterminesOutputType = "CellType"))
    UOdysseyLayerCell* AddCell(TSubclassOf<UOdysseyLayerCell> CellType, int Index = -1 );

    /**
     * Add Count new Cells of the given type to this layer.
     *
     * The Cells are inserted at Index, or at the end if Index is -1.
     * Return the new Cells.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer", meta=(DeterminesOutputType = "CellType"))
    TArray<UOdysseyLayerCell*> AddCells(TSubclassOf<UOdysseyLayerCell> CellType, int Index = -1, int Count = 1 );

    /** Remove the given Cell from this layer. */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void RemoveCell(UOdysseyLayerCell* Cell); //Prevent Empty Layer ?

    /** Remove the given Cells from this layer. */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void RemoveCells(const TArray<UOdysseyLayerCell*>& Cells); //Prevent Empty Layer ?

    /** Remove the Cell at the given index from this layer. */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void RemoveCellAtIndex(int Index);

    /** Remove all the Cells of this layer. */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void RemoveAllCells();

    /**
     * Duplicate the given Cell into this layer.
     *
     * The copy is inserted at Index, or at the end if Index is -1.
     * Return the new Cell.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    UOdysseyLayerCell* CopyCell(UOdysseyLayerCell* Cell, int Index = -1);

    /**
     * Duplicate the given Cell into another layer.
     *
     * The copy is inserted in ToLayer at Index, or at the end if
     * Index is -1. Return the new Cell.
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    UOdysseyLayerCell* CopyCellToLayer(UOdysseyLayerCell* Cell, UOdysseyLayer* ToLayer, int Index = -1) const;

    /**
     * Duplicate the given Cells into this layer.
     *
     * The copies are inserted at Index, or at the end if Index is -1.
     * Return the new Cells.
     */
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
    /** Put contiguous cells together inside the same group
    *   ...[   C1   ][ C2 ]...[C10][     C11     ]...[    C22   ]...
    *   groups[0] = [C1, C2]
    *   groups[1] = [C10, C11]
    *   groups[2] = [C22]
    *
    *   If it returns a single group, it also means that all cells are contiguous
    */
    static TArray<TArray<UOdysseyLayerCell*>> GetCellsContiguousGroups( const TArray<UOdysseyLayerCell*>& iCells );

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
    virtual UTexture2D* CreateExportTexture( const FString& iAssetName, const FString& iPackagePath, UClass* iAssetClass, UFactory* iFactory ) override;
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
    bool bIsLocked = false;

    UPROPERTY()
    EOdysseyBlendingMode BlendMode_DEPRECATED = EOdysseyBlendingMode::kBlendingMode_Count;

    UPROPERTY()
    EOdysseyBlendMode LayerBlendMode = EOdysseyBlendMode::Normal;

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

    UPROPERTY()
    bool bInheritsAlpha = false;

private:
    mutable TArray<FInt32Range> mCellsFrameRanges;
};

#if WITH_EDITOR
namespace OdysseyLayerStackSelection {
    /** Callback for editor layer selection. This must be in runtime instead of editor for UOdysseyLayer::IsSelectedInEditor to work */
    extern ODYSSEYLAYERSTACK_API TFunction<bool(const UObject*)> GIsLayerSelectedInEditor;
}
#endif
