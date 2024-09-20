// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Layout/SScrollBox.h"
#include "OdysseyPaletteEntry.h"

#include "OdysseyPalette.generated.h"

/////////////////////////////////////////////////////
// OdysseyColorPalette
UCLASS(BlueprintType, config = EditorPerProjectUserSettings, PerObjectConfig)
class ODYSSEYPALETTE_API UOdysseyPalette : public UObject
{
    GENERATED_BODY()

public:
    UOdysseyPalette();

public:
    /* Called when the current entry changed */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentEntryChanged, UOdysseyPalette*)

    /* Called when the Entry hierarchy changed at some point */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnHierarchyChanged, UOdysseyPalette*);

    /* Called when the sets changed at some point */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnSetsChanged, UOdysseyPalette*);

public:
    //Delegates

    /**
     * @brief Returns the CurrentLayerChanged delegate
     */
    static FOnCurrentEntryChanged& OnCurrentEntryChanged();

    /**
     * @brief Returns the HierarchyChanged delegate
     */
    static FOnHierarchyChanged& OnHierarchyChanged();

    /**
     * @brief Returns the SetsChanged delegate
     */
    static FOnHierarchyChanged& OnSetsChanged();


public:
    //Entries management

    /**
     * @brief Adds an entry of EntryType to the Palette as child of ParentEntry at IndexInParent
     */
    UFUNCTION(BlueprintCallable, Category="Palette", meta = (DeterminesOutputType = "EntryType"))
    UOdysseyPaletteEntry* AddEntry(TSubclassOf<UOdysseyPaletteEntry> iEntryType, UOdysseyPaletteEntry* iParentEntry = nullptr, int iIndexInParent = 0);

    /**
     * @brief Removes Entry from the Palette
     */
    UFUNCTION(BlueprintCallable, Category="Palette")
    void RemoveEntry(UOdysseyPaletteEntry* iEntry);

    /**
     * @brief Removes Entries from the Palette
     *
     * @param Entries has to be Entries from this Palette
     */
    UFUNCTION(BlueprintCallable, Category="Palette")
    void RemoveEntries(TArray<UOdysseyPaletteEntry*> iEntries);

    /**
     * @brief Returns whether the given Entry is contained in the Palette
     *
     * @param Entry
     * @return bool
     */
    UFUNCTION(BlueprintPure, Category="Palette")
    bool ContainsEntry(const UOdysseyPaletteEntry* iEntry) const;

    /**
     * @brief Returns the root Entries only
     *
     * @return TArray<UOdysseyPaletteEntry*>
     */
    UFUNCTION(BlueprintPure, Category="Palette")
    const TArray<UOdysseyPaletteEntry*>& GetRootEntries() const;

    /**
     * @brief Returns the set used as a FName
     *
     * @return FName
     */
    FName GetUsedSet() const;

    /**
     * @brief Returns all Entries
     *
     * @return TArray<UOdysseyPaletteEntry*>
     */
    UFUNCTION(BlueprintPure, Category="Palette")
    TArray<UOdysseyPaletteEntry*> GetEntries() const;

    /**
     * @brief Duplicates Entry and position the resulting Entry directly above Entry
     *
     * @param Entry has to be a Entry from this Palette
     * @return UOdysseyPaletteEntry*
     */
    UFUNCTION(BlueprintCallable, Category="Palette", meta = (DeterminesOutputType = "Entry"))
    UOdysseyPaletteEntry* DuplicateEntry(UOdysseyPaletteEntry* iEntry);

    /**
     * @brief Duplicates Entries and position the resulting Entries directly above each Entries
     *
     * @param Entries
     * @return UOdysseyPaletteEntry*
     */
    UFUNCTION(BlueprintCallable, Category="Palette", meta = (DeterminesOutputType = "Entry"))
    TArray<UOdysseyPaletteEntry*> DuplicateEntries(TArray<UOdysseyPaletteEntry*> iEntries);

    /**
     * @brief Copies Entry and adds the resulting Entry as child of ParentEntry at IndexInParent
     *
     * @param Entry can be a Entry from another Palette
     * @param ParentEntry has to be a Entry from this Palette or nullptr
     * @param IndexInParent
     * @return UOdysseyPaletteEntry*
     */
    UFUNCTION(BlueprintCallable, Category="Palette", meta = (DeterminesOutputType = "Entry"))
    UOdysseyPaletteEntry* CopyEntry(UOdysseyPaletteEntry* iEntry, UOdysseyPaletteEntry* iParentEntry = nullptr, int iIndexInParent = 0);

    /**
     * @brief Copies Entry and adds the resulting Entry as child of ParentEntry at IndexInParent
     *
     * @param Entry can be a Entry from another Palette
     * @param ParentEntry has to be a Entry from this Palette or nullptr
     * @param IndexInParent
     * @return UOdysseyPaletteEntry*
     */
    UFUNCTION(BlueprintCallable, Category="Palette", meta = (DeterminesOutputType = "Entry"))
    TArray<UOdysseyPaletteEntry*> CopyEntries(TArray<UOdysseyPaletteEntry*> iEntries, UOdysseyPaletteEntry* iParentEntry = nullptr, int iIndexInParent = 0);

    /**
     * @brief Returns whether the given Entry can be moved in the given ParentEntry
     * 
     * @param Entry has to be a Entry from this Palette
     * @param ParentEntry has to be a Entry from this Palette or nullptr
     * @param IndexInParent 
     */
    UFUNCTION(BlueprintCallable, Category="Palette")
    bool CanMoveEntry(UOdysseyPaletteEntry* iEntry, UOdysseyPaletteEntry* iParentEntry) const;

	/**
	 * @brief Returns whether the given Entries can be moved in the given ParentEntry
	 *
	 * @param Entries 
	 * @param ParentEntry has to be a Entry from this Palette or nullptr
	 * @param IndexInParent
	 */
	UFUNCTION(BlueprintCallable, Category="Palette")
	bool CanMoveEntries(TArray<UOdysseyPaletteEntry*> iEntries, UOdysseyPaletteEntry* iParentEntry) const;

    /**
     * @brief Moves Entry to become child of ParentEntry at IndexInParent
     * 
     * @param Entry has to be a Entry from this Palette
     * @param ParentEntry has to be a Entry from this Palette or nullptr
     * @param IndexInParent 
     */
    UFUNCTION(BlueprintCallable, Category="Palette")
    void MoveEntry(UOdysseyPaletteEntry* iEntry, UOdysseyPaletteEntry* iParentEntry = nullptr, int iIndexInParent = 0);

	/**
	 * @brief Moves Entries to become children of ParentEntry at IndexInParent
	 *
	 * @param Entries 
	 * @param ParentEntry has to be a Entry from this Palette or nullptr
	 * @param IndexInParent
	 */
	UFUNCTION(BlueprintCallable, Category="Palette")
	void MoveEntries(TArray<UOdysseyPaletteEntry*> iEntries, UOdysseyPaletteEntry* iParentEntry = nullptr, int iIndexInParent = 0);

    void AddSet();
    void DuplicateSet();

    void RemoveSet(int iIndex = -1);

public:
    //Called by layers when there Parent or Children changed
    virtual void HierarchyChanged();

protected:
    //Property changed methods
    void CurrentEntryChanged();
    void SetsChanged();
    virtual void PropertyChanged(const FName& iPropertyName);

public:
    // UObject overrides

	/**
	 * Called when a property on this object has been modified
	 *
	 * @param PropertyThatChanged the property that was modified
	 */
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

    /**
     * @brief Allows us to know which property changed on an undo/redo
     * PostEditChangeProperty does not inform us of that on undo/redo
     * 
     * @param TransactionEvent 
     */
    virtual void PostTransacted(const FTransactionObjectEvent& TransactionEvent) override;

    virtual void PostInitProperties() override;

protected:
    //Internal
    UOdysseyPaletteEntry* CreateEntry(UClass* iEntryType);

    void AddEntriesToHierarchy(TArray<UOdysseyPaletteEntry*> iEntries, UOdysseyPaletteEntry* iParent, int iIndexInParent);

    void RemoveEntriesFromHierarchy(TArray<UOdysseyPaletteEntry*> iEntries);

    UOdysseyPaletteEntry* CopyEntryInternal(UOdysseyPaletteEntry* iEntry, UOdysseyPaletteEntry* iParent, int iIndexInParent);

    void GetEntriesUniqueParents(TArray<UOdysseyPaletteEntry*> iEntries, TArray<UOdysseyPaletteEntry*>& oParents);

public:
    UPROPERTY(config, DuplicateTransient, meta = (AllowedClasses = "/Script/OdysseyPaletteEntry.OdysseyPaletteEntry"))
    TSoftObjectPtr<UOdysseyPaletteEntry> CurrentEntry;

    UPROPERTY()
    TObjectPtr<UOdysseyPaletteEntry> PaletteRoot;
    
    UPROPERTY()
    TArray<FName> Sets;

    UPROPERTY()
    uint8 UsedSet;
};
