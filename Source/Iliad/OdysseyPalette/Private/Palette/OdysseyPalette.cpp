// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPalette.h"

#include "AssetRegistry/AssetData.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Misc/PackageName.h"
#include "Misc/TransactionObjectEvent.h"
#include "UObject/ObjectSaveContext.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Notifications/SNotificationList.h"

#include "OdysseyPaletteEntryFolder.h"
#include "OdysseyPaletteReferencer.h"

UOdysseyPalette::UOdysseyPalette()
{
}

UOdysseyPalette::FOnHierarchyChanged& UOdysseyPalette::OnHierarchyChanged()
{
    static FOnHierarchyChanged onHierarchyChanged;
    return onHierarchyChanged;
}

UOdysseyPalette::FOnSetsChanged& UOdysseyPalette::OnSetsChanged()
{
    static FOnSetsChanged onSetsChanged;
    return onSetsChanged;
}

UOdysseyPalette::FOnCurrentSetChanged& UOdysseyPalette::OnCurrentSetChanged()
{
    static FOnCurrentSetChanged onCurrentSetChanged;
    return onCurrentSetChanged;
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

    //Duplicate the entry
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

    bool bChangeParent = iEntry->Parent != iParentEntry;

    int index = FMath::Clamp(iIndexInParent, 0, iParentEntry->Children.Num());
    iEntry->Parent->Children.Remove(iEntry);
    iParentEntry->Children.Insert(iEntry, (iEntry->Parent == iParentEntry && oldIndex < index) ? index - 1 : index);
    iEntry->Parent = iParentEntry;
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

    int index = FMath::Clamp(iIndexInParent, 0, iParentEntry->Children.Num());
    for (UOdysseyPaletteEntry* entry : iEntries)
    {
        UOdysseyPaletteEntry* parent = entry->Parent;

        bool bChangeParent = entry->Parent != parent;

        if (bChangeParent)
        {
            entry->Modify();
            entry->Parent->Modify();
        }
        parent->Modify();

        int oldIndex = parent->Children.Find(entry);
        parent->Children.Remove(entry);
        iParentEntry->Children.Insert(entry, (parent == iParentEntry && oldIndex < index) ? index - 1 : index);
        entry->Parent = iParentEntry;

        if (parent == iParentEntry && oldIndex > index)
            index++;
    }

    HierarchyChanged();
}

const TArray<FName>&
UOdysseyPalette::GetSets() const
{
    return Sets;
}

const TMap<FGuid, FName>&
UOdysseyPalette::GetSetsIDs() const
{
    return SetsIDs;
}

void
UOdysseyPalette::RenameSet(FGuid iSetId, const FName& iName)
{
    if( !SetsIDs.Contains(iSetId) )
        return;

    SetsIDs[iSetId] = iName;
}

FGuid UOdysseyPalette::DuplicateSet(FGuid iSetId, const FName& iName)
{
    if (!SetsIDs.Contains(iSetId))
        return FGuid();

    FGuid newId = FGuid::NewGuid();

    TArray<UOdysseyPaletteEntry*> entries = GetEntries();
    for (int i = 0; i < entries.Num(); i++)
    {
        entries[i]->DuplicateSetAt(iSetId, newId);
    }

    SetsIDs.Add(newId, iName);

    return newId;
}

void UOdysseyPalette::RemoveSet(FGuid iSetId)
{
    if (!SetsIDs.Contains(iSetId))
        return;

    TArray<UOdysseyPaletteEntry*> entries = GetEntries();
    for (int i = 0; i < entries.Num(); i++)
    {
        entries[i]->Modify();
        entries[i]->RemoveSet(iSetId);
    }

    SetsIDs.Remove(iSetId);
}

FGuid UOdysseyPalette::GetDefaultSetID()
{
    if (SetsIDs.Num() > 0)
    {
        TMap<FGuid, FName>::TIterator It(SetsIDs);
        return It.Key();
    }
    return FGuid();
}

void UOdysseyPalette::HierarchyChanged()
{
    OnHierarchyChanged().Broadcast(this);
}

void
UOdysseyPalette::PropertyChanged(const FName& iPropertyName)
{
}

void UOdysseyPalette::PostPropertyChanged(const FName& iPropertyName)
{
    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPalette, Sets))
        OnSetsChanged().Broadcast(this);
}

void UOdysseyPalette::PostLoad()
{
    Super::PostLoad();

    // Legacy, to delete next version

    //We can't dirty the asset in the PostLoad, because it's part of the serialization (loading) of the asset, therefore, Unreal ignore Modify() and MarkPackageDirty() in it.
    //This is why we use a boolean that is checked when this asset is loaded in a texture/animation or palette editor, and dirty the package there if needed

    //We don't delete Sets the first time, because we need it to keep the ordering correct for the entries in the palette.
    //The second time this palette is loaded, though, we delete sets, and don't do anything more. And even if Sets is empty and SetsIDs is not, the loop after isn't triggered
    if(SetsIDs.Num() > 0 && Sets.Num() > 0)
    {
        Sets.Empty();
        NeedsSavingAfterUpgrade = true;
    }

    for (int i = 0; i < Sets.Num(); i++)
    {
        SetsIDs.Add(FGuid::NewGuid(), Sets[i]);
        NeedsSavingAfterUpgrade = true;
    }
    //---
}

#if WITH_EDITOR
void UOdysseyPalette::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    PropertyChanged(PropertyChangedEvent.GetPropertyName());
    PostPropertyChanged(PropertyChangedEvent.GetPropertyName());
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
        PostPropertyChanged( propertyName );
    }
}
#endif //WITH_EDITOR

void UOdysseyPalette::PostInitProperties()
{
    Super::PostInitProperties();

    if (HasAnyFlags(RF_ClassDefaultObject))
        return;

    if (!HasAnyFlags( RF_NeedLoad | RF_WasLoaded))
    {
        SetsIDs.Add(FGuid::NewGuid(), "Default Set"); //Only add a default set if it's a newly created asset
    }

    PaletteRoot = NewObject<UOdysseyPaletteEntry>(this, UOdysseyPaletteEntryFolder::StaticClass(), NAME_None, RF_Public | RF_Transactional);
    PaletteRoot->OnCreated();

    UsedSet_DEPRECATED = 0;
}

UOdysseyPaletteEntry* UOdysseyPalette::CreateEntry(UClass* iEntryType)
{
    //Create the entry
    UOdysseyPaletteEntry* entry = NewObject<UOdysseyPaletteEntry>(this, iEntryType, NAME_None, RF_Public | RF_Transactional);
    if (!entry)
        return nullptr;

    //Name the entry
    FString name = entry->DefaultName.ToString() + TEXT(" ") + FString::FromInt(GetEntries().Num() + 1);
    entry->EntryName = FText::FromString(name);

    //Initialize the entry
    entry->OnCreated();

    return entry;
}

void UOdysseyPalette::AddEntriesToHierarchy(TArray<UOdysseyPaletteEntry*> iEntries, UOdysseyPaletteEntry* iParent, int iIndexInParent)
{
    iParent->Modify();
    for ( UOdysseyPaletteEntry* entry : iEntries)
    {
        entry->Modify();
    }

    for (UOdysseyPaletteEntry* entry : iEntries)
    {
        entry->Children.Empty();
    }

    //Add entries to parent's children
    iParent->Children.Insert(iEntries, FMath::Clamp(iIndexInParent, 0, iParent->Children.Num()));

    for (UOdysseyPaletteEntry* entry : iEntries)
    {
        //Make sure entry is accessible in the hierarchy map
        entry->Parent = iParent;
    }

    HierarchyChanged();
}

void UOdysseyPalette::RemoveEntriesFromHierarchy(TArray<UOdysseyPaletteEntry*> iEntries)
{
    TArray<UOdysseyPaletteEntry*> parents;
    TArray<UOdysseyPaletteEntry*> entriesToRemove = iEntries;// UOdysseyLayerStackFunctionLibrary::FilterTopmostLayers(iEntries);

    for (UOdysseyPaletteEntry* entry : entriesToRemove)
    {
        parents.AddUnique(entry->Parent);
    }

    for (UOdysseyPaletteEntry* entry : entriesToRemove )
        entry->Parent = nullptr;

    for (UOdysseyPaletteEntry* parent : parents )
        for (UOdysseyPaletteEntry* entry : entriesToRemove )
            parent->Children.Remove(entry);
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

#if WITH_EDITOR
void
UOdysseyPalette::PreSave(FObjectPreSaveContext SaveContext)
{
    Super::PreSave(SaveContext);
    RefreshReferencedAssets();
}

void UOdysseyPalette::RefreshReferencedAssets()
{
    TArray<FName> assetNames = GetReferencedAssetsViaAssetRegistry();
    if( assetNames.IsEmpty() )
        return;

    for (FName assetName : assetNames)
    {
        FString pathStr = assetName.ToString();
        FString assetType = FPackageName::GetShortName(pathStr);
        FString fullObjectPath = FString::Printf(TEXT("%s.%s"), *pathStr, *assetType);

        FSoftObjectPath softPath(fullObjectPath);
        TSoftObjectPtr<UObject> referencedAsset(softPath);

        if (!referencedAsset.IsValid())
            continue;

        if (!referencedAsset->Implements<UOdysseyPaletteReferencer>())
            continue;

        IOdysseyPaletteReferencer* paletteReferencer = Cast<IOdysseyPaletteReferencer>(referencedAsset.Get());
        if (!paletteReferencer)
            return;

        paletteReferencer->OnRefreshReferencedPalette(this);

        /* if (texture.IsValid()) // The asset is valid AND loaded
        {

        } */
    }

    FText confirmText = FText::Format(
    NSLOCTEXT("PaletteEditor", "RefreshReferencedAsset", "{0} assets have been refreshed"),
    FText::AsNumber(assetNames.Num())
    );

    FNotificationInfo Info(confirmText);
    Info.ExpireDuration = 5.0f;
    TSharedPtr<SNotificationItem> notification = FSlateNotificationManager::Get().AddNotification(Info);
    if (notification)
        notification->SetCompletionState(SNotificationItem::CS_Success);
}

TArray<FName>
UOdysseyPalette::GetReferencedAssetsViaAssetRegistry()
{
    TArray<FName> outDependencies;

    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

    FAssetData AssetData = AssetRegistry.GetAssetByObjectPath(FSoftObjectPath(GetPathName()));
    FName AssetPath;

    if (AssetData.IsValid())
    {
        AssetPath = AssetData.PackageName;
    }

    AssetRegistry.GetReferencers(AssetPath, outDependencies);

    return outDependencies;
}
#endif
