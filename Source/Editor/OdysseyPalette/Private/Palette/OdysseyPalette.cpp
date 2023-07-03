// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPalette.h"
#include "Widgets/Colors/SColorBlock.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "Misc/TransactionObjectEvent.h"

#define LOCTEXT_NAMESPACE "UOdysseyPalette"

UOdysseyPalette::UOdysseyPalette()
{
    FString text = FString("Set0");
    Sets.Add(FName(text));
}

UOdysseyPalette::FOnCurrentEntryChanged& UOdysseyPalette::OnCurrentEntryChanged()
{
    static FOnCurrentEntryChanged onCurrentEntryChanged;
    return onCurrentEntryChanged;
}

UOdysseyPalette::FOnHierarchyChanged& UOdysseyPalette::OnHierarchyChanged()
{
    static FOnHierarchyChanged onHierarchyChanged;
    return onHierarchyChanged;
}

UOdysseyPaletteEntry* UOdysseyPalette::AddEntry(TSubclassOf<UOdysseyPaletteEntry> iEntryType, UOdysseyPaletteEntry* iParentEntry /*= nullptr*/, int iIndexInParent /*= 0*/)
{
    UClass* entryType = iEntryType.Get();

    //No entryType
	if ( !entryType )
		return nullptr;

    //If the given parent can't have children or isn't contained in this palette
    if (!iParentEntry)
        iParentEntry = PaletteRoot;

    if (!iParentEntry->CanHaveChildren || !ContainsEntry(iParentEntry))
        return nullptr;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("PaletteTransaction", "Add Entry"));
#endif

    //Create the entry
	UOdysseyPaletteEntry* entry = CreateEntry(entryType);
    if (!entry)
        return nullptr;

    //Add the layer to the hierarchy
    AddEntriesToHierarchy({ entry }, iParentEntry, iIndexInParent);

    return entry;
}

void UOdysseyPalette::RemoveEntry(UOdysseyPaletteEntry* iEntry)
{
    //No entry or not contained by the palette
    if (!iEntry || !ContainsEntry(iEntry))
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("PaletteTransaction", "Remove Entries"));
#endif
    RemoveEntriesFromHierarchy({ iEntry });
}

void UOdysseyPalette::RemoveEntries(TArray<UOdysseyPaletteEntry*> iEntries)
{
    //Sanitize entries array
    iEntries.RemoveAll(
        [this](const UOdysseyPaletteEntry* entry)
        {
            return !entry || !ContainsEntry(entry);
        }
    );

    //No Layers
    if (iEntries.Num() <= 0)
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("PaletteTransaction", "Remove Entries"));
#endif

    RemoveEntriesFromHierarchy(iEntries);
}

bool UOdysseyPalette::ContainsEntry(const UOdysseyPaletteEntry* iEntry) const
{
    if (!iEntry)
        return false;

    if (iEntry == PaletteRoot)
        return true;

    return GetEntries().Contains(iEntry);
}

const TArray<UOdysseyPaletteEntry*>& UOdysseyPalette::GetRootEntries() const
{
    return PaletteRoot->Children;
}

TArray<UOdysseyPaletteEntry*> UOdysseyPalette::GetEntries() const
{
    return PaletteRoot->GetChildrenRecursively();
}

UOdysseyPaletteEntry* UOdysseyPalette::DuplicateEntry(UOdysseyPaletteEntry* iEntry)
{
    //No entry or not contained by the palette
    if(!iEntry || !ContainsEntry(iEntry))
        return nullptr;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("PaletteTransaction", "Duplicate Entries"));
#endif

    //Duplicate the layer
    UOdysseyPaletteEntry* entryDuplicate = CopyEntryInternal(iEntry, iEntry->Parent, iEntry->Parent->Children.Find(iEntry));

    return entryDuplicate;
}

TArray<UOdysseyPaletteEntry*> UOdysseyPalette::DuplicateEntries(TArray<UOdysseyPaletteEntry*> iEntries)
{
    TArray<UOdysseyPaletteEntry*> entriesDuplicates;

    //Sanitize Layers array
    iEntries.RemoveAll(
        [this](const UOdysseyPaletteEntry* iEntry)
        {
            return !iEntry || !ContainsEntry(iEntry);
        }
    );

    //No entry    
    if (iEntries.Num()<= 0)
        return entriesDuplicates;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("PaletteTransaction", "Duplicate Entries"));
#endif

    iEntries.Sort(
        [this](UOdysseyPaletteEntry& iEntryA, UOdysseyPaletteEntry& iEntryB)
        {
            //true => iLayerA is before iLayerB
            int depthA = iEntryA.GetParents().Num();
            int depthB = iEntryB.GetParents().Num();

            if (depthA < depthB)
                return true;

            if (depthA == depthB)
            {
                int indexA = iEntryA.GetIndexInParent();
                int indexB = iEntryB.GetIndexInParent();

                return indexA < indexB;
            }

            return false;
        }
    );


    for (UOdysseyPaletteEntry* entry : iEntries)
    {
        //Duplicate the entry
        UOdysseyPaletteEntry* entryCopy = CopyEntryInternal(entry, entry->Parent, entry->Parent->Children.Find(entry));
        entriesDuplicates.Add(entryCopy);
    }

    return entriesDuplicates;
}

UOdysseyPaletteEntry* UOdysseyPalette::CopyEntry(UOdysseyPaletteEntry* iEntry, UOdysseyPaletteEntry* iParentEntry /*= nullptr*/, int iIndexInParent /*= 0*/)
{
    //No entry
    if(!iEntry)
        return nullptr;

    if (!iParentEntry)
        iParentEntry = PaletteRoot;
    
    //If the given parent can't have children or isn't contained in this Palette
    if (iParentEntry && (!iParentEntry->CanHaveChildren || !ContainsEntry(iParentEntry)) )
        return nullptr;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("PaletteTransaction", "Copy Entries"));
#endif

    //Duplicate the entry
    UOdysseyPaletteEntry* entryCopy = CopyEntryInternal(iEntry, iParentEntry, iIndexInParent);
    return entryCopy;
}

TArray<UOdysseyPaletteEntry*> UOdysseyPalette::CopyEntries(TArray<UOdysseyPaletteEntry*> iEntries, UOdysseyPaletteEntry* iParentEntry /*= nullptr*/, int iIndexInParent /*= 0*/)
{
	TArray<UOdysseyPaletteEntry*> entryCopies;
    if (!iParentEntry)
        iParentEntry = PaletteRoot;

    //If the given parent can't have children or isn't contained in this palette
    if (iParentEntry && (!iParentEntry->CanHaveChildren || !ContainsEntry(iParentEntry)))
        return entryCopies;

    //Sanitize entries array
    iEntries.RemoveAll(
        [this](const UOdysseyPaletteEntry* iEntry)
        {
            return !iEntry;
        }
    );

    //No Layers
    if (iEntries.Num()<= 0)
        return entryCopies;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("PaletteTransaction", "Copy Entries"));
#endif

    iEntries.Sort(
        [this](UOdysseyPaletteEntry& iEntryA, UOdysseyPaletteEntry& iEntryB)
        {
            //true => iEntryA is before iEntryB
            int depthA = iEntryA.GetParents().Num();
            int depthB = iEntryB.GetParents().Num();

            if (depthA < depthB)
                return true;

            if (depthA == depthB)
            {
                int indexA = iEntryA.GetIndexInParent();
                int indexB = iEntryB.GetIndexInParent();

                return indexA < indexB;
            }

            return false;
        }
    );

    for (UOdysseyPaletteEntry* entry : iEntries)
    {   
        //Duplicate the entry
        UOdysseyPaletteEntry* entryCopy = CopyEntryInternal(entry, iParentEntry, iIndexInParent);
        entryCopies.Add(entryCopy);
    }

    return entryCopies;
}


bool UOdysseyPalette::CanMoveEntry(UOdysseyPaletteEntry* iEntry, UOdysseyPaletteEntry* iParentEntry) const
{
    //No entry or not contained by the palette
    if (!iEntry || !ContainsEntry(iEntry))
        return false;

    if (!iParentEntry)
        iParentEntry = PaletteRoot;

    //If the given parent can't have children or isn't contained in this palette
    if (!iParentEntry->CanHaveChildren || !ContainsEntry(iParentEntry))
        return false;

    //Does entry contain Parent entry
    if (iEntry == iParentEntry || iParentEntry->IsChildOf(iEntry))
        return false;

    return true;
}

bool UOdysseyPalette::CanMoveEntries(TArray<UOdysseyPaletteEntry*> iEntries, UOdysseyPaletteEntry* iParentEntry) const
{
    if ( !iParentEntry )
        iParentEntry = PaletteRoot;

    //If the given parent can't have children or isn't contained in this palette
    if (!iParentEntry->CanHaveChildren || !ContainsEntry(iParentEntry))
        return false;

    //Sanitize Layers array
    iEntries.RemoveAll(
        [this, iParentEntry](UOdysseyPaletteEntry* iEntry)
        {
            if(!iEntry || !ContainsEntry(iEntry))
                return true;

            if (iEntry == iParentEntry || iParentEntry->IsChildOf(iEntry))
                return true;

            return false;
        }
    );

    //No Entries
    if (iEntries.Num()<= 0)
        return false;

	return true;
}

void UOdysseyPalette::MoveEntry(UOdysseyPaletteEntry* iEntry, UOdysseyPaletteEntry* iParentEntry /*= nullptr*/, int iIndexInParent /*= 0*/)
{
	if ( !CanMoveEntry(iEntry, iParentEntry) )
		return;

    if ( !iParentEntry )
        iParentEntry = PaletteRoot;

    int oldIndex = iEntry->Parent->Children.Find(iEntry);
    if (iEntry->Parent == iParentEntry && oldIndex == iIndexInParent)
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("PaletteTransaction", "Move Entries"));
#endif

    bool bChangeParent = iEntry->Parent != iParentEntry;
    
    if (bChangeParent)
    {
        FOdysseyObjectEditorUtils::PreChangePropertyValue(iEntry, "Parent");
        FOdysseyObjectEditorUtils::PreChangePropertyValue(iEntry->Parent, "Children");
    }
    FOdysseyObjectEditorUtils::PreChangePropertyValue(iParentEntry, "Children");

    int index = FMath::Clamp(iIndexInParent, 0, iParentEntry->Children.Num());
    iEntry->Parent->Children.Remove(iEntry);
    iParentEntry->Children.Insert(iEntry, (iEntry->Parent == iParentEntry && oldIndex < index) ? index - 1 : index);
    iEntry->Parent = iParentEntry;

    if (bChangeParent)
    {
        FOdysseyObjectEditorUtils::PostChangePropertyValue(iEntry, "Parent", EPropertyChangeType::ValueSet);
        FOdysseyObjectEditorUtils::PostChangePropertyValue(iEntry, "Children", EPropertyChangeType::ArrayRemove);
    }
    FOdysseyObjectEditorUtils::PostChangePropertyValue(iParentEntry, "Children", EPropertyChangeType::ArrayAdd);
}

void UOdysseyPalette::MoveEntries(TArray<UOdysseyPaletteEntry*> iEntries, UOdysseyPaletteEntry* iParentEntry /*= nullptr*/, int iIndexInParent /*= 0*/)
{
    if ( !iParentEntry )
        iParentEntry = PaletteRoot;

	//If the given parent can't have children or isn't contained in this palette
	if ( !iParentEntry->CanHaveChildren || !ContainsEntry(iParentEntry))
		return;

	//Sanitize Entries array
	iEntries.RemoveAll(
		[this, iParentEntry](UOdysseyPaletteEntry* iEntry)
		{
			if ( !iEntry || !ContainsEntry(iEntry) )
				return true;

			if ( iEntry == iParentEntry || iParentEntry->IsChildOf(iEntry) )
				return true;

			return false;
		}
	);

    iEntries.Sort(
        [this](UOdysseyPaletteEntry& iEntryA, UOdysseyPaletteEntry& iEntryB)
        {
            //true => iEntryA is before iEntryB
            int depthA = iEntryA.GetParents().Num();
            int depthB = iEntryB.GetParents().Num();

            if (depthA < depthB)
                return true;

            if (depthA == depthB)
            {
                int indexA = iEntryA.GetIndexInParent();
                int indexB = iEntryB.GetIndexInParent();

                return indexA < indexB;
            }

            return false;
        }
    );

	//No Entries
	if ( iEntries.Num() <= 0 )
		return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("PaletteTransaction", "Move Entries"));
#endif

    int index = FMath::Clamp(iIndexInParent, 0, iParentEntry->Children.Num());
    for (UOdysseyPaletteEntry* entry : iEntries)
    {
        bool bChangeParent = entry->Parent != iParentEntry;
        
        if (bChangeParent)
        {
            FOdysseyObjectEditorUtils::PreChangePropertyValue(entry, "Parent");
            FOdysseyObjectEditorUtils::PreChangePropertyValue(entry->Parent, "Children");
        }
        FOdysseyObjectEditorUtils::PreChangePropertyValue(iParentEntry, "Children");

        UOdysseyPaletteEntry* parent = entry->Parent;

        int oldIndex = parent->Children.Find(entry);
        parent->Children.Remove(entry);
        iParentEntry->Children.Insert(entry, (parent == iParentEntry && oldIndex < index) ? index - 1 : index);
        entry->Parent = iParentEntry;

        if (parent == iParentEntry && oldIndex > index)
            index++;

        if (bChangeParent)
        {
            FOdysseyObjectEditorUtils::PostChangePropertyValue(entry, "Parent", EPropertyChangeType::ValueSet);
            FOdysseyObjectEditorUtils::PostChangePropertyValue(entry, "Children", EPropertyChangeType::ArrayRemove);
        }
        FOdysseyObjectEditorUtils::PostChangePropertyValue(iParentEntry, "Children", EPropertyChangeType::ArrayAdd);
    }
}

void UOdysseyPalette::AddSet()
{
    if (Sets.Num() < 8)
    {
        FString text = FString("Set") + FString::FromInt( Sets.Num() );
        Sets.Add(FName(text));

        TArray<UOdysseyPaletteEntry*> entries = GetEntries();
        for( int i = 0; i < entries.Num(); i++ )
            entries[i]->AddSet();
    }
}

void UOdysseyPalette::RemoveSet(int iIndex /*= -1 */)
{
/*
    if (NumSets > 1)
    {
        NumSets--;
        if (UsedSet >= NumSets)
        {
            UsedSet--;
        }
    }*/
}

void UOdysseyPalette::HierarchyChanged()
{
    OnHierarchyChanged().Broadcast(this);
}

void UOdysseyPalette::CurrentEntryChanged()
{
    OnCurrentEntryChanged().Broadcast(this);
}

void UOdysseyPalette::PropertyChanged(const FName& iPropertyName)
{
    if (iPropertyName == "CurrentEntry")
        CurrentEntryChanged();
}

void UOdysseyPalette::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    PropertyChanged(PropertyChangedEvent.GetPropertyName());
}

void UOdysseyPalette::PostTransacted(const FTransactionObjectEvent& iTransactionEvent)
{
    Super::PostTransacted(iTransactionEvent);

    if (iTransactionEvent.GetEventType() != ETransactionObjectEventType::UndoRedo)
        return;

    const TArray<FName>& changedPropertyNames = iTransactionEvent.GetChangedProperties();
    for (const FName& propertyName : changedPropertyNames)
    {
        PropertyChanged(propertyName);
    }
}

void UOdysseyPalette::PostInitProperties()
{
    Super::PostInitProperties();

    if (HasAnyFlags(RF_ClassDefaultObject))
        return;

    PaletteRoot = NewObject<UOdysseyPaletteEntry>(this, UOdysseyPaletteEntryFolder::StaticClass(), NAME_None, RF_Public | RF_Transactional);
    PaletteRoot->OnCreated();

    UsedSet = 0;
}

UOdysseyPaletteEntry* UOdysseyPalette::CreateEntry(UClass* iEntryType)
{
    //Create the Layer
    UOdysseyPaletteEntry* entry = NewObject<UOdysseyPaletteEntry>(this, iEntryType, NAME_None, RF_Public | RF_Transactional);
    if (!entry)
        return nullptr;
        
    //Name the layer
    FString name = entry->DefaultName.ToString() + TEXT(" ") + FString::FromInt(GetEntries().Num() + 1);
    entry->EntryName = FText::FromString(name);

    //Initialize the entry
    entry->OnCreated();

    return entry;
}

void UOdysseyPalette::AddEntriesToHierarchy(TArray<UOdysseyPaletteEntry*> iEntries, UOdysseyPaletteEntry* iParent, int iIndexInParent)
{
    //Call propertyPreChange in a stable state of the palette
    FOdysseyObjectEditorUtils::PreChangePropertyValue(iParent, "Children");
    for ( UOdysseyPaletteEntry* entry : iEntries)
    {
        FOdysseyObjectEditorUtils::PreChangePropertyValue(entry, "Parent");
    }

    //Add entries to parent's children
    iParent->Children.Insert(iEntries, FMath::Clamp(iIndexInParent, 0, iParent->Children.Num()));

    for (UOdysseyPaletteEntry* entry : iEntries)
    {
        //Make sure entry is accessible in the hierarchy map
        entry->Parent = iParent;
    }

    //Call propertyPostChange in a stable state of the palette
    FOdysseyObjectEditorUtils::PostChangePropertyValue(iParent, "Children", EPropertyChangeType::ArrayAdd);
    for (UOdysseyPaletteEntry* entry : iEntries)
    {
        FOdysseyObjectEditorUtils::PostChangePropertyValue(entry, "Parent", EPropertyChangeType::ValueSet);
    }
}

void UOdysseyPalette::RemoveEntriesFromHierarchy(TArray<UOdysseyPaletteEntry*> iEntries)
{
    TArray<UOdysseyPaletteEntry*> parents;
    TArray<UOdysseyPaletteEntry*> entriesToRemove = iEntries;// UOdysseyLayerStackFunctionLibrary::FilterTopmostLayers(iEntries);
    GetEntriesUniqueParents(entriesToRemove, parents);

    //Call propertyPreChange in a stable state of the palette
    for (UOdysseyPaletteEntry* entry : entriesToRemove )
        FOdysseyObjectEditorUtils::PreChangePropertyValue(entry, "Parent");

    for (UOdysseyPaletteEntry* parent : parents)
        FOdysseyObjectEditorUtils::PreChangePropertyValue(parent, "Children");

    for (UOdysseyPaletteEntry* entry : entriesToRemove )
        entry->Parent = nullptr;

    for (UOdysseyPaletteEntry* parent : parents )
        for (UOdysseyPaletteEntry* entry : entriesToRemove )
            parent->Children.Remove(entry);

    //Call propertyPostChange in a stable state of the palette
    for (UOdysseyPaletteEntry* entry : entriesToRemove )
        FOdysseyObjectEditorUtils::PostChangePropertyValue(entry, "Parent", EPropertyChangeType::ValueSet);

    for (UOdysseyPaletteEntry* parent : parents )
        FOdysseyObjectEditorUtils::PostChangePropertyValue(parent, "Children", EPropertyChangeType::ArrayRemove);
}

UOdysseyPaletteEntry* UOdysseyPalette::CopyEntryInternal(UOdysseyPaletteEntry* iEntry, UOdysseyPaletteEntry* iParent, int iIndexInParent)
{
    FObjectDuplicationParameters params(iEntry, this);
    UOdysseyPaletteEntry* duplicatedEntry = Cast<UOdysseyPaletteEntry>(StaticDuplicateObjectEx(params));
    if (!duplicatedEntry)
        return nullptr;

    AddEntriesToHierarchy({ duplicatedEntry }, iParent, iIndexInParent);

    for (UOdysseyPaletteEntry* child : iEntry->Children)
    {
        CopyEntryInternal(child, duplicatedEntry, iEntry->Children.Num());
    }

    return duplicatedEntry;
}

void UOdysseyPalette::GetEntriesUniqueParents(TArray<UOdysseyPaletteEntry*> iEntries, TArray<UOdysseyPaletteEntry*>& oParents)
{
    for (UOdysseyPaletteEntry* entry : iEntries)
    {
        oParents.AddUnique(entry->Parent);
    }
}

#undef LOCTEXT_NAMESPACE