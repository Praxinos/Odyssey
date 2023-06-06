// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPalette.h"
#include "Widgets/Colors/SColorBlock.h"

#define LOCTEXT_NAMESPACE "UOdysseyPalette"

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

}

void UOdysseyPalette::RemoveEntries(TArray<UOdysseyPaletteEntry*> iEntries)
{

}

bool UOdysseyPalette::ContainsEntry(const UOdysseyPaletteEntry* iEntry) const
{
    return false;
}

const TArray<UOdysseyPaletteEntry*>& UOdysseyPalette::GetRootEntries() const
{
    return mPaletteEntries;
}

TArray<UOdysseyPaletteEntry*> UOdysseyPalette::GetEntries() const
{
    return TArray<UOdysseyPaletteEntry*>();
}

UOdysseyPaletteEntry* UOdysseyPalette::DuplicateEntry(UOdysseyPaletteEntry* iEntry)
{
    return nullptr;
}

TArray<UOdysseyPaletteEntry*> UOdysseyPalette::DuplicateEntries(TArray<UOdysseyPaletteEntry*> iEntries)
{
    return TArray<UOdysseyPaletteEntry*>();
}

UOdysseyPaletteEntry* UOdysseyPalette::CopyEntry(UOdysseyPaletteEntry* iEntry, UOdysseyPaletteEntry* iParentEntry /*= nullptr*/, int iIndexInParent /*= 0*/)
{
    return nullptr;
}

TArray<UOdysseyPaletteEntry*> UOdysseyPalette::CopyEntries(TArray<UOdysseyPaletteEntry*> iEntries, UOdysseyPaletteEntry* iParentEntry /*= nullptr*/, int iIndexInParent /*= 0*/)
{
    return TArray<UOdysseyPaletteEntry*>();
}


bool UOdysseyPalette::CanMoveEntry(UOdysseyPaletteEntry* iEntry, UOdysseyPaletteEntry* iParentEntry) const
{
    return false;
}

bool UOdysseyPalette::CanMoveEntries(TArray<UOdysseyPaletteEntry*> iEntries, UOdysseyPaletteEntry* iParentEntry) const
{
    return false;
}

void UOdysseyPalette::MoveEntry(UOdysseyPaletteEntry* iEntry, UOdysseyPaletteEntry* iParentEntry /*= nullptr*/, int iIndexInParent /*= 0*/)
{

}

void UOdysseyPalette::MoveEntries(TArray<UOdysseyPaletteEntry*> iEntries, UOdysseyPaletteEntry* iParentLayer /*= nullptr*/, int iIndexInParent /*= 0*/)
{

}

void UOdysseyPalette::HierarchyChanged()
{

}

void UOdysseyPalette::CurrentEntryChanged()
{

}

void UOdysseyPalette::PropertyChanged(const FName& iPropertyName)
{

}

void UOdysseyPalette::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{

}

void UOdysseyPalette::PostTransacted(const FTransactionObjectEvent& TransactionEvent)
{

}

void UOdysseyPalette::PostInitProperties()
{

}

UOdysseyPaletteEntry* UOdysseyPalette::CreateEntry(UClass* iEntryType)
{
    return nullptr;
}

void UOdysseyPalette::AddEntriesToHierarchy(TArray<UOdysseyPaletteEntry*> iEntries, UOdysseyPaletteEntry* iParent, int iIndexInParent)
{

}

void UOdysseyPalette::RemoveEntriesFromHierarchy(TArray<UOdysseyPaletteEntry*> iEntries)
{

}

UOdysseyPaletteEntry* UOdysseyPalette::CopyEntryInternal(UOdysseyPaletteEntry* iEntry, UOdysseyPaletteEntry* iParent, int iIndexInParent)
{
    return nullptr;
}

void UOdysseyPalette::GetEntriesUniqueParents(TArray<UOdysseyPaletteEntry*> iEntries, TArray<UOdysseyPaletteEntry*>& oParents)
{

}

#undef LOCTEXT_NAMESPACE