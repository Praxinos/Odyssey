// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Palette/OdysseyPalette.h"
#include "Palette/OdysseyPaletteEntryColor.h"

class UOdysseyPaletteEntry;

struct FOdysseyPainterEditorPaletteTreeViewItem
{
    bool mIsPalette;
    UOdysseyPaletteSet* mPaletteSet;
    UOdysseyPaletteEntry* mEntry = nullptr;
    TArray<TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem>> mChildren;
};

/**
 * Implements the mesh selector
 */
class SOdysseyPainterEditorPaletteSetList : public SCompoundWidget
{
    SLATE_DECLARE_WIDGET(SOdysseyPainterEditorPaletteSetList, SCompoundWidget)

public:
    DECLARE_DELEGATE_TwoParams(FOnCurrentColorEntryChanged, UOdysseyPaletteEntryColor*, int)
    DECLARE_DELEGATE_OneParam(FOnAddPaletteSet, UOdysseyPalette*)
    DECLARE_DELEGATE_OneParam(FOnRemovePaletteSet, UOdysseyPaletteSet*)
    DECLARE_DELEGATE_TwoParams(FOnPaletteSetChanged, int, UOdysseyPaletteSet* )

public:
    ~SOdysseyPainterEditorPaletteSetList();
    SOdysseyPainterEditorPaletteSetList();

    SLATE_BEGIN_ARGS(SOdysseyPainterEditorPaletteSetList)
        {}
        SLATE_ATTRIBUTE(TArray<UOdysseyPaletteSet*>, PaletteSets)
        SLATE_ATTRIBUTE(UOdysseyPaletteEntryColor*, CurrentColorEntry)
        SLATE_ATTRIBUTE(int, CurrentSet)
        SLATE_EVENT(FOnAddPaletteSet, OnAddPaletteSet)
        SLATE_EVENT(FOnRemovePaletteSet, OnRemovePaletteSet)
        SLATE_EVENT(FOnPaletteSetChanged, OnPaletteSetChanged)
        SLATE_EVENT(FOnCurrentColorEntryChanged, OnCurrentColorEntryChanged)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct(const FArguments& InArgs);

private:
    // Private internal callbacks
    void OnPaletteSetsChanged();
    void OnCurrentColorEntryChanged();

    TSharedRef<SWidget> OnGetAddPaletteMenuContent();
    TSharedRef<ITableRow> OnGenerateRow( TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem> iItem, const TSharedRef<STableViewBase>& iOwnerTable );
    void OnGetChildren(TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem> iParent, TArray<TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem>>& oChildren) const;
    void OnSelectionChanged(TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem> iItem, ESelectInfo::Type iSelectInfo);

    void RebuildPaletteEntryItems(TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem> iItem, TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem> iRootItem);
    TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem> FindEntryItem(UOdysseyPaletteEntry* iEntry, const TArray<TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem>>& iChildren);
    void RebuildItems();
    void OnPaletteHierarchyChanged(UOdysseyPalette* iPalette);
    bool OnIsSelectableOrNavigable(TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem> iItem);

private:
    // Private data
    TSlateAttribute<TArray<UOdysseyPaletteSet*>> mPaletteSets;
    TSlateAttribute<UOdysseyPaletteEntryColor*> mCurrentColorEntry;
    TAttribute<int> mCurrentSet;
    TSharedPtr<UE::Slate::Containers::TObservableArray<TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem>>> mItemsSource;
    FOnAddPaletteSet mOnAddPaletteSet;
    FOnRemovePaletteSet mOnRemovePaletteSet;
    FOnPaletteSetChanged mOnPaletteSetChanged;
    FOnCurrentColorEntryChanged mOnCurrentColorEntryChanged;

    TSharedPtr<STreeView<TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem>>> mTreeView;
};
