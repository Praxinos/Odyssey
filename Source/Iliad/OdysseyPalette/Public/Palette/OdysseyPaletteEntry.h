// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateBrush.h"

#include "OdysseyPaletteEntry.generated.h"

class UOdysseyPalette;

UENUM(BlueprintType)
enum  class  EGetEntryChildrenMethod : uint8
{
    DepthFirst,
    BreadthFirst
};

/////////////////////////////////////////////////////
// UOdysseyPaletteEntry
UCLASS(Abstract, HideDropdown, BlueprintType, config = EditorPerProjectUserSettings, PerObjectConfig)
class ODYSSEYPALETTE_API UOdysseyPaletteEntry : public UObject
{
    GENERATED_BODY()

    public:
    /**
     * @brief Delegate called when something changed the name on this entry
     *
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnNameChanged, UOdysseyPaletteEntry*)

    /**
     * @brief Delegate called when something changed the activated state of this entry
     *
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnIsActivatedChanged, UOdysseyPaletteEntry*)

    /* IsExpandedChanged
     * - concerned Child
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnIsExpandedChanged, UOdysseyPaletteEntry*);

    /* ParentChanged
     * - concerned Child
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnParentChanged, UOdysseyPaletteEntry*);

    /* ChildrenChanged
     * - concerned Child
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnChildrenChanged, UOdysseyPaletteEntry*);


public:
    static FOnNameChanged& OnNameChanged();
    static FOnIsActivatedChanged& OnIsActivatedChanged();
    static FOnIsExpandedChanged& OnIsExpandedChanged();
    static FOnParentChanged& OnParentChanged();
    static FOnChildrenChanged& OnChildrenChanged();

public:
    // Events

    /**
     * @brief Called when the node has been created by the given Palette
     *
     */
    UFUNCTION(BlueprintNativeEvent, Category="Palette")
    void OnCreated();
    virtual void OnCreated_Implementation();

public:
    UFUNCTION(BlueprintPure, Category="Palette")
    UOdysseyPalette* GetPalette() const;

    /**
     * @brief Returns the entries children recursively
     *
     * @return int
     */
    UFUNCTION(BlueprintPure, Category="Palette")
    TArray<UOdysseyPaletteEntry*> GetChildrenRecursively(EGetEntryChildrenMethod iMethod = EGetEntryChildrenMethod::DepthFirst) const;

    /**
    * @brief Returns the index of the entry in its parent
    *
    * @return int
    */
    UFUNCTION(BlueprintPure, Category="Palette")
    int GetIndexInParent() const;


     /**
     * @brief Returns wether the given entry is a child of the given ParentEntry
     *
     * @return bool
     */
    UFUNCTION(BlueprintPure, Category="Palette")
    bool IsChildOf(UOdysseyPaletteEntry* iEntry) const;

     /**
     * @brief Returns the parent entry
     *
     * @return int
     */
    UFUNCTION(BlueprintPure, Category="Palette")
    UOdysseyPaletteEntry* GetParent() const;

    /**
     * @brief Returns the parents entries from direct parent to root parent
     *
     * @return int
     */
    UFUNCTION(BlueprintPure, Category="Palette")
    TArray<UOdysseyPaletteEntry*> GetParents() const;

     /**
     * @brief Returns the entry children recursively
     *
     * @return int
     */
    UFUNCTION(BlueprintPure, Category="Palette")
    const TArray<UOdysseyPaletteEntry*>& GetChildren() const;

    virtual void AddSet( FGuid iIndex );
    virtual void DuplicateSetAt( FGuid iIndexToCopy, FGuid iNewId );

    virtual void RemoveSet( FGuid iIndex );

protected:
    //Property changed methods
    virtual void NameChanged();
    virtual void IsActivatedChanged();
    virtual void IsExpandedChanged();
    virtual void ParentChanged();
    virtual void ChildrenChanged();

    virtual void PropertyChanged(const FName& iPropertyName);
    virtual void PostPropertyChanged(const FName& iPropertyName);

#if WITH_EDITOR
public:
    // UObject overrides
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;
#endif

public:
    UPROPERTY(EditAnywhere, Category="PaletteEntry")
    FText EntryName;

    UPROPERTY(EditDefaultsOnly, Category="PaletteEntry")
    FText EntryTypeName = FText::FromString(TEXT("Unnamed Entry Type"));

    UPROPERTY(EditDefaultsOnly, Category="PaletteEntry")
    FText DefaultName = FText::FromString(TEXT("Entry"));

    UPROPERTY(EditDefaultsOnly, Category="PaletteEntry")
    FText Description = FText::FromString(TEXT(""));

    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="PaletteEntry")
    bool IsActivated = true;

    //Defaults Properties
    UPROPERTY(EditDefaultsOnly, Category="PaletteEntry")
    bool CanHaveChildren = false;

    UPROPERTY(config, BlueprintReadWrite, Category="PaletteEntry", NonTransactional)
    bool IsExpanded = true;

    UPROPERTY()
    TObjectPtr<UOdysseyPaletteEntry> Parent;

    UPROPERTY()
    TArray< TObjectPtr<UOdysseyPaletteEntry> > Children;
};
