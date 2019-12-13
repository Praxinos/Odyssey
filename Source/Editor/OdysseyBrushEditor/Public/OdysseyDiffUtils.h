// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
#pragma once

#include "CoreMinimal.h"
#include "Widgets/SWidget.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Views/STreeView.h"
#include "PropertyPath.h"

class UBlueprint;
struct FRevisionInfo;

struct FResolvedProperty
{
    explicit FResolvedProperty()
        : Object(nullptr)
        , Property(nullptr)
    {
    }

    FResolvedProperty(const void* InObject, const UProperty* InProperty)
        : Object(InObject)
        , Property(InProperty)
    {
    }

    inline bool operator==(const FResolvedProperty& RHS) const
    {
        return Object == RHS.Object && Property == RHS.Property;
    }

    inline bool operator!=(const FResolvedProperty& RHS) const { return !(*this == RHS); }

    const void* Object;
    const UProperty* Property;
};

/**
 * FPropertySoftPath is a string of identifiers used to identify a single member of a UObject. It is primarily
 * used when comparing unrelated UObjects for Diffing and Merging, but can also be used as a key select
 * a property in a SDetailsView.
 */
struct FPropertySoftPath
{
    FPropertySoftPath()
    {
    }

    FPropertySoftPath(TArray<FName> InPropertyChain)
        : PropertyChain(InPropertyChain)
    {
    }

    FPropertySoftPath( FPropertyPath InPropertyPath )
    {
        for( int32 i = 0, end = InPropertyPath.GetNumProperties(); i != end; ++i )
        {
            PropertyChain.Push( InPropertyPath.GetPropertyInfo(i).Property->GetFName() );
        }
    }

    FPropertySoftPath(const FPropertySoftPath& SubPropertyPath, const UProperty* LeafProperty)
        : PropertyChain(SubPropertyPath.PropertyChain)
    {
        PropertyChain.Push(LeafProperty->GetFName());
    }

    FPropertySoftPath(const FPropertySoftPath& SubPropertyPath, int32 ContainerIndex)
        : PropertyChain(SubPropertyPath.PropertyChain)
    {
        PropertyChain.Push(FName(*FString::FromInt(ContainerIndex)));
    }

    FResolvedProperty Resolve(const UObject* Object) const;
    FPropertyPath ResolvePath(const UObject* Object) const;
    FString ToDisplayName() const;

    inline bool IsSubPropertyMatch(const FPropertySoftPath& PotentialBasePropertyPath) const
    {
        if (PropertyChain.Num() <= PotentialBasePropertyPath.PropertyChain.Num())
        {
            return false;
        }

        for (int32 CurChainElement = 0; CurChainElement < PotentialBasePropertyPath.PropertyChain.Num(); CurChainElement++)
        {
            if (PotentialBasePropertyPath.PropertyChain[CurChainElement] != PropertyChain[CurChainElement])
            {
                return false;
            }
        }

        return true;
    }

    inline bool operator==(FPropertySoftPath const& RHS) const
    {
        return PropertyChain == RHS.PropertyChain;
    }

    inline bool operator!=(FPropertySoftPath const& RHS ) const
    {
        return !(*this == RHS);
    }
private:
    friend uint32 GetTypeHash( FPropertySoftPath const& Path );
    TArray<FName> PropertyChain;
};

struct FSCSIdentifier
{
    FName Name;
    TArray< int32 > TreeLocation;
};

struct FSCSResolvedIdentifier
{
    FSCSIdentifier Identifier;
    const UObject* Object;
};

FORCEINLINE bool operator==( const FSCSIdentifier& A, const FSCSIdentifier& B )
{
    return A.Name == B.Name && A.TreeLocation == B.TreeLocation;
}

FORCEINLINE bool operator!=(const FSCSIdentifier& A, const FSCSIdentifier& B)
{
    return !(A == B);
}

FORCEINLINE uint32 GetTypeHash( FPropertySoftPath const& Path )
{
    uint32 Ret = 0;
    for( const auto& ProperytName : Path.PropertyChain )
    {
        Ret = Ret ^ GetTypeHash(ProperytName);
    }
    return Ret;
}

// Trying to restrict us to this typedef because I'm a little skeptical about hashing FPropertySoftPath safely
typedef TSet< FPropertySoftPath > FPropertySoftPathSet;

namespace EPropertyDiffType
{
    enum Type
    {
        Invalid,

        PropertyAddedToA,
        PropertyAddedToB,
        PropertyValueChanged,
    };
}

struct FSingleObjectDiffEntry
{
    FSingleObjectDiffEntry()
        : Identifier()
        , DiffType(EPropertyDiffType::Invalid)
    {
    }

    FSingleObjectDiffEntry(const FPropertySoftPath& InIdentifier, EPropertyDiffType::Type InDiffType )
        : Identifier(InIdentifier)
        , DiffType(InDiffType)
    {
    }

    FPropertySoftPath Identifier;
    EPropertyDiffType::Type DiffType;
};

namespace ETreeDiffType
{
    enum Type
    {
        NODE_ADDED,
        NODE_REMOVED,
        NODE_TYPE_CHANGED,
        NODE_PROPERTY_CHANGED,
        NODE_MOVED,
        /** We could potentially try to identify hierarchy reorders separately from add/remove */
    };
}

struct FSCSDiffEntry
{
    FSCSDiffEntry( const FSCSIdentifier& InIdentifier, ETreeDiffType::Type InDiffType, const FSingleObjectDiffEntry& InPropertyDiff )
        : TreeIdentifier(InIdentifier)
        , DiffType(InDiffType)
        , PropertyDiff(InPropertyDiff)
    {
    }

    FSCSIdentifier TreeIdentifier;
    ETreeDiffType::Type DiffType;
    FSingleObjectDiffEntry PropertyDiff;
};

struct FSCSDiffRoot
{
    // use indices in FSCSIdentifier::TreeLocation to find hierarchy..
    TArray< FSCSDiffEntry > Entries;
};

namespace DiffUtils
{
    ODYSSEYBRUSHEDITOR_API const UObject* GetCDO(const UBlueprint* ForOdysseyBrush);
    ODYSSEYBRUSHEDITOR_API void CompareUnrelatedObjects(const UObject* A, const UObject* B, TArray<FSingleObjectDiffEntry>& OutDifferingProperties);
    ODYSSEYBRUSHEDITOR_API void CompareUnrelatedSCS(const UBlueprint* Old, const TArray< FSCSResolvedIdentifier >& OldHierarchy, const UBlueprint* New, const TArray< FSCSResolvedIdentifier >& NewHierarchy, FSCSDiffRoot& OutDifferingEntries );
    ODYSSEYBRUSHEDITOR_API bool Identical(const FResolvedProperty& AProp, const FResolvedProperty& BProp, const FPropertySoftPath& RootPath, TArray<FPropertySoftPath>& DifferingProperties);
    TArray<FPropertySoftPath> GetVisiblePropertiesInOrderDeclared(const UObject* ForObj, const TArray<FName>& Scope = TArray<FName>());

    ODYSSEYBRUSHEDITOR_API TArray<FPropertyPath> ResolveAll(const UObject* Object, const TArray<FPropertySoftPath>& InSoftProperties);
    ODYSSEYBRUSHEDITOR_API TArray<FPropertyPath> ResolveAll(const UObject* Object, const TArray<FSingleObjectDiffEntry>& InDifferences);
}

DECLARE_DELEGATE(FOnDiffEntryFocused);
DECLARE_DELEGATE_RetVal(TSharedRef<SWidget>, FGenerateDiffEntryWidget);

class FOdysseyBrushDifferenceTreeEntry
{
public:
    FOdysseyBrushDifferenceTreeEntry( FOnDiffEntryFocused InOnFocus, FGenerateDiffEntryWidget InGenerateWidget, TArray< TSharedPtr<FOdysseyBrushDifferenceTreeEntry> > InChildren )
        : OnFocus(InOnFocus)
        , GenerateWidget(InGenerateWidget)
        , Children(InChildren)
    {
        check( InGenerateWidget.IsBound() );
    }

    /** The FOdysseyBrushDifferenceTreeEntry used to display a message to the user explaining that there are no differences: */
    ODYSSEYBRUSHEDITOR_API static TSharedPtr<FOdysseyBrushDifferenceTreeEntry> NoDifferencesEntry();
    ODYSSEYBRUSHEDITOR_API static TSharedPtr<FOdysseyBrushDifferenceTreeEntry> AnimBlueprintEntry();
    ODYSSEYBRUSHEDITOR_API static TSharedPtr<FOdysseyBrushDifferenceTreeEntry> WidGetBlueprintEntry();
    /** The FOdysseyBrushDifferenceTreeEntry used to label the defaults category: */
    ODYSSEYBRUSHEDITOR_API static TSharedPtr<FOdysseyBrushDifferenceTreeEntry> CreateDefaultsCategoryEntry(FOnDiffEntryFocused FocusCallback, const TArray< TSharedPtr<FOdysseyBrushDifferenceTreeEntry> >& Children, bool bHasDifferences );
    ODYSSEYBRUSHEDITOR_API static TSharedPtr<FOdysseyBrushDifferenceTreeEntry> CreateDefaultsCategoryEntryForMerge(FOnDiffEntryFocused FocusCallback, const TArray< TSharedPtr<FOdysseyBrushDifferenceTreeEntry> >& Children, bool bHasRemoteDifferences, bool bHasLocalDifferences, bool bHasConflicts );
    ODYSSEYBRUSHEDITOR_API static TSharedPtr<FOdysseyBrushDifferenceTreeEntry> CreateComponentsCategoryEntry(FOnDiffEntryFocused FocusCallback, const TArray< TSharedPtr<FOdysseyBrushDifferenceTreeEntry> >& Children, bool bHasDifferences);
    ODYSSEYBRUSHEDITOR_API static TSharedPtr<FOdysseyBrushDifferenceTreeEntry> CreateComponentsCategoryEntryForMerge(FOnDiffEntryFocused FocusCallback, const TArray< TSharedPtr<FOdysseyBrushDifferenceTreeEntry> >& Children, bool bHasRemoteDifferences, bool bHasLocalDifferences, bool bHasConflicts);

    FOnDiffEntryFocused OnFocus;
    FGenerateDiffEntryWidget GenerateWidget;
    TArray< TSharedPtr<FOdysseyBrushDifferenceTreeEntry> > Children;
};

namespace DiffTreeView
{
    ODYSSEYBRUSHEDITOR_API TSharedRef< STreeView<TSharedPtr< FOdysseyBrushDifferenceTreeEntry > > > CreateTreeView(TArray< TSharedPtr<FOdysseyBrushDifferenceTreeEntry> >* DifferencesList);
    ODYSSEYBRUSHEDITOR_API int32 CurrentDifference( TSharedRef< STreeView<TSharedPtr< FOdysseyBrushDifferenceTreeEntry > > > TreeView, const TArray< TSharedPtr<class FOdysseyBrushDifferenceTreeEntry> >& Differences );
    ODYSSEYBRUSHEDITOR_API void HighlightNextDifference(TSharedRef< STreeView<TSharedPtr< FOdysseyBrushDifferenceTreeEntry > > > TreeView, const TArray< TSharedPtr<class FOdysseyBrushDifferenceTreeEntry> >& Differences, const TArray< TSharedPtr<class FOdysseyBrushDifferenceTreeEntry> >& RootDifferences);
    ODYSSEYBRUSHEDITOR_API void HighlightPrevDifference(TSharedRef< STreeView<TSharedPtr< FOdysseyBrushDifferenceTreeEntry > > > TreeView, const TArray< TSharedPtr<class FOdysseyBrushDifferenceTreeEntry> >& Differences, const TArray< TSharedPtr<class FOdysseyBrushDifferenceTreeEntry> >& RootDifferences);
    ODYSSEYBRUSHEDITOR_API bool HasNextDifference(TSharedRef< STreeView<TSharedPtr< FOdysseyBrushDifferenceTreeEntry > > > TreeView, const TArray< TSharedPtr<class FOdysseyBrushDifferenceTreeEntry> >& Differences);
    ODYSSEYBRUSHEDITOR_API bool HasPrevDifference(TSharedRef< STreeView<TSharedPtr< FOdysseyBrushDifferenceTreeEntry > > > TreeView, const TArray< TSharedPtr<class FOdysseyBrushDifferenceTreeEntry> >& Differences);
}

struct FRevisionInfo;

namespace DiffViewUtils
{
    ODYSSEYBRUSHEDITOR_API FLinearColor LookupColor( bool bDiffers, bool bConflicts = false );
    ODYSSEYBRUSHEDITOR_API FLinearColor Differs();
    ODYSSEYBRUSHEDITOR_API FLinearColor Identical();
    ODYSSEYBRUSHEDITOR_API FLinearColor Missing();
    ODYSSEYBRUSHEDITOR_API FLinearColor Conflicting();

    ODYSSEYBRUSHEDITOR_API FText PropertyDiffMessage(FSingleObjectDiffEntry Difference, FText ObjectName);
    ODYSSEYBRUSHEDITOR_API FText SCSDiffMessage(const FSCSDiffEntry& Difference, FText ObjectName);
    ODYSSEYBRUSHEDITOR_API FText GetPanelLabel(const UBlueprint* OdysseyBrush, const FRevisionInfo& Revision, FText Label);

    ODYSSEYBRUSHEDITOR_API SHorizontalBox::FSlot& Box(bool bIsPresent, FLinearColor Color);
}
