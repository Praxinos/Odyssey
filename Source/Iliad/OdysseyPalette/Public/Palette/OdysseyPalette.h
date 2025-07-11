// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Layout/SScrollBox.h"
#include "OdysseyPaletteEntry.h"
#include "Templates/SubclassOf.h"

#include "OdysseyPalette.generated.h"

UCLASS()
class ODYSSEYPALETTE_API UOdysseyPaletteSet : public UObject
{
    GENERATED_BODY()

    bool
    operator==(const UOdysseyPaletteSet& iRhs) const
    {
        return mPalette == iRhs.mPalette && mSet == iRhs.mSet;
    }

public:
    UPROPERTY()
    TObjectPtr<UOdysseyPalette> mPalette = nullptr;

    UPROPERTY()
    FString mSet = FString();
};

/////////////////////////////////////////////////////
// OdysseyColorPalette
UCLASS(BlueprintType, config = EditorPerProjectUserSettings, PerObjectConfig)
class ODYSSEYPALETTE_API UOdysseyPalette : public UObject
{
    GENERATED_BODY()

public:
    UOdysseyPalette();

public:
    /* Called when the Entry hierarchy changed at some point */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnHierarchyChanged, UOdysseyPalette*);

    /* Called when the sets changed at some point */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnSetsChanged, UOdysseyPalette*);

    /* Called when the sets changed at some point */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentSetChanged, UOdysseyPalette*);

public:
    //Delegates
    /**
     * @brief Returns the HierarchyChanged delegate
     */
    static FOnHierarchyChanged& OnHierarchyChanged();

    /**
     * @brief Returns the SetsChanged delegate
     */
    static FOnSetsChanged& OnSetsChanged();

    /**
     * @brief Returns the SetsChanged delegate
     */
    static FOnCurrentSetChanged& OnCurrentSetChanged();


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

public:
    //Sets
    const TArray<FName>& GetSets() const;
    const TMap<FString, FName>& GetSetsIDs() const;
    void RenameSet(FString iSetId, const FName& iName);
    FString DuplicateSet(FString iSetId, const FName& iName );
    void RemoveSet(FString iSetId);
    FString GetDefaultSetID(); //Returns the first ID we find in the TMap

public:
    //Called by layers when there Parent or Children changed
    virtual void HierarchyChanged();


protected:
    virtual void PropertyChanged(const FName& iPropertyName);
    virtual void PostPropertyChanged(const FName& iPropertyName);
    virtual void PostLoad() override;

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

private:
    // Legacy, to delete next version
    UPROPERTY()
    TArray<FName> Sets;
    //---

    UPROPERTY()
    TMap<FString, FName> SetsIDs;

public:
    // Legacy, to delete next version
    UPROPERTY()
    bool NeedsSavingAfterUpgrade = false; //If it is true, it means we changed the storage of the sets from the previous version (Sets) to the actual version (SetsIDs), and the asset needs to be saved because of it
    //---

    UPROPERTY()
    TObjectPtr<UOdysseyPaletteEntry> PaletteRoot;

    UPROPERTY()
    uint8 UsedSet_DEPRECATED;
};
