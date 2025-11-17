// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/Palette/SOdysseyPainterEditorPaletteSetList.h"

#include "OdysseyPalette.h"
#include "OdysseyStyle.h"
#include "Palette/OdysseyPaletteEntryColor.h"
#include "Palette/OdysseyPaletteEntryFolder.h"

#include "SOdysseyPaletteTreeView.h"
#include "Widgets/SOdysseyPaletteSetComboBox.h"
#include "SPositiveActionButton.h"
#include "Widgets/Palette/SOdysseyPainterEditorPaletteRow.h"
#include "Widgets/Palette/SOdysseyPainterEditorPaletteColorRow.h"
#include "Widgets/Palette/SOdysseyPainterEditorPaletteFolderRow.h"

#include "PropertyCustomizationHelpers.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditor"

SLATE_IMPLEMENT_WIDGET(SOdysseyPainterEditorPaletteSetList)
void
SOdysseyPainterEditorPaletteSetList::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
    SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION(AttributeInitializer, mPaletteSets, EInvalidateWidgetReason::None)
    .OnValueChanged(FSlateAttributeDescriptor::FAttributeValueChangedDelegate::CreateLambda(
        [](SWidget& Widget)
        {
            static_cast<SOdysseyPainterEditorPaletteSetList&>(Widget).OnPaletteSetsChanged();
        }
    ));

    SLATE_ADD_MEMBER_ATTRIBUTE_DEFINITION(AttributeInitializer, mCurrentColorEntry, EInvalidateWidgetReason::None)
    .OnValueChanged(FSlateAttributeDescriptor::FAttributeValueChangedDelegate::CreateLambda(
        [](SWidget& Widget)
        {
            static_cast<SOdysseyPainterEditorPaletteSetList&>(Widget).OnCurrentColorEntryChanged();
        }
    ));
}

/////////////////////////////////////////////////////
// SOdysseyPainterEditorPaletteSetList
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

SOdysseyPainterEditorPaletteSetList::~SOdysseyPainterEditorPaletteSetList()
{
    UOdysseyPalette::OnHierarchyChanged().RemoveAll(this);
}

SOdysseyPainterEditorPaletteSetList::SOdysseyPainterEditorPaletteSetList()
    : mPaletteSets(*this, {})
    , mCurrentColorEntry(*this, nullptr)
{
    UOdysseyPalette::OnHierarchyChanged().AddRaw(this, &SOdysseyPainterEditorPaletteSetList::OnPaletteHierarchyChanged);
}

void SOdysseyPainterEditorPaletteSetList::Construct(const FArguments& InArgs)
{
    mPaletteSets.Assign(*this, InArgs._PaletteSets);
    mCurrentColorEntry.Assign(*this, InArgs._CurrentColorEntry);
    mCurrentSet = InArgs._CurrentSet;
    mColorType = InArgs._ColorType;
    mOnAddPaletteSet = InArgs._OnAddPaletteSet;
    mOnRemovePaletteSet = InArgs._OnRemovePaletteSet;
    mOnPaletteSetChanged = InArgs._OnPaletteSetChanged;
    mOnCurrentColorEntryChanged = InArgs._OnCurrentColorEntryChanged;

    mItemsSource = MakeShared<UE::Slate::Containers::TObservableArray<TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem>>>();

    TSharedRef<SHeaderRow> headerRow = SNew(SHeaderRow)
        .Visibility(EVisibility::Collapsed)
        + SHeaderRow::Column("Header")
        .DefaultLabel(FText())
        .VAlignCell(VAlign_Top)
        .FillWidth(0.5f)

        + SHeaderRow::Column("Color")
        .DefaultLabel(FText())
        .VAlignCell(VAlign_Top)
        .FillWidth(0.5f);

    ChildSlot
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew(SPositiveActionButton)
            .Text(LOCTEXT("palette.add-palette", "Add Palette"))
            .OnGetMenuContent(this, &SOdysseyPainterEditorPaletteSetList::OnGetAddPaletteMenuContent)
        ]
        + SVerticalBox::Slot()
        [
            SAssignNew(mTreeView, STreeView<TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem>>)
            .OnIsSelectableOrNavigable(this, &SOdysseyPainterEditorPaletteSetList::OnIsSelectableOrNavigable)
            .OnGetChildren(this, &SOdysseyPainterEditorPaletteSetList::OnGetChildren)
            .OnGenerateRow(this, &SOdysseyPainterEditorPaletteSetList::OnGenerateRow)
            .TreeItemsSource(mItemsSource)
            .HeaderRow(headerRow)
            .SelectionMode( ESelectionMode::Single )
            .OnSelectionChanged(this, &SOdysseyPainterEditorPaletteSetList::OnSelectionChanged)
        ]
    ];
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Private internal callbacks

void
SOdysseyPainterEditorPaletteSetList::OnPaletteHierarchyChanged(UOdysseyPalette* iPalette)
{
    TArray<UOdysseyPaletteSet*> paletteSets = mPaletteSets.Get();

    bool needRebuild = paletteSets.ContainsByPredicate(
        [iPalette](const UOdysseyPaletteSet* iPaletteSet)
        {
            return iPaletteSet->mPalette == iPalette;
        }
    );

    if (needRebuild)
    {
        RebuildItems();
    }
}

TSharedRef<SWidget>
SOdysseyPainterEditorPaletteSetList::OnGetAddPaletteMenuContent()
{
    TArray<UOdysseyPalette*> palettesAlreadyLoaded;
    for( UOdysseyPaletteSet* set : mPaletteSets.Get() )
    {
        palettesAlreadyLoaded.Add( set->mPalette );
    }

    FOnShouldFilterAsset filterPalette = FOnShouldFilterAsset::CreateLambda(
        [palettesAlreadyLoaded](const FAssetData& AssetData)
        {
            return palettesAlreadyLoaded.ContainsByPredicate([&](const FAssetData& PickedAsset)
                {
                    return PickedAsset.GetSoftObjectPath() == AssetData.GetSoftObjectPath();
                });
        });

    return PropertyCustomizationHelpers::MakeAssetPickerWithMenu(
        nullptr,
        false,
        false,
        { UOdysseyPalette::StaticClass() },
        {},
        PropertyCustomizationHelpers::GetNewAssetFactoriesForClasses({ UOdysseyPalette::StaticClass() }),
        filterPalette,
        FOnAssetSelected::CreateLambda(
            [this](const FAssetData& AssetData)
            {
                UOdysseyPalette* palette = Cast<UOdysseyPalette>(AssetData.GetAsset());
                mOnAddPaletteSet.ExecuteIfBound(palette);
            }
        ),
        FSimpleDelegate::CreateLambda(
            []()
            {
                FSlateApplication::Get().DismissAllMenus();
            }
        )
    );
}

bool
SOdysseyPainterEditorPaletteSetList::OnIsSelectableOrNavigable(TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem> iItem)
{
    if (!iItem)
        return false;

    if (iItem->mIsPalette)
        return false;

    if (!iItem->mEntry)
        return false;

    if (!iItem->mEntry->IsA<UOdysseyPaletteEntryColor>())
        return false;

    return true;
}

void
SOdysseyPainterEditorPaletteSetList::OnGetChildren(TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem> iItem, TArray<TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem>>& oChildren) const
{
    oChildren = iItem->mChildren;
}

TSharedRef<ITableRow>
SOdysseyPainterEditorPaletteSetList::OnGenerateRow( TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem> iItem, const TSharedRef<STableViewBase>& iOwnerTable )
{
    if (!iItem)
        return SNew(STableRow<TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem>>, iOwnerTable);

    if (iItem->mIsPalette)
    {
        return SNew(SOdysseyPainterEditorPaletteRow, iOwnerTable)
            .Palette_Lambda(
                [this, iItem]()
                {
                    return iItem->mPaletteSet->mPalette;
                }
            )
            .Set_Lambda(
                [this, iItem]()
                {
                    return iItem->mPaletteSet->mSet;
                }
            )
            .OnSetChanged_Lambda(
                [this, iItem](FGuid iSet)
                {
                    if( iItem->mPaletteSet->mSet != iSet )
                    {
                        iItem->mPaletteSet->mSet = iSet;
                        mOnPaletteSetChanged.ExecuteIfBound( iSet, iItem->mPaletteSet );
                    }

                    UOdysseyPaletteEntryColor* entryColor = mCurrentColorEntry.Get();
                    if (entryColor && entryColor->GetPalette() == iItem->mPaletteSet->mPalette)
                        mOnCurrentColorEntryChanged.ExecuteIfBound(entryColor, iSet);
                }
            )
            .OnDeleteButtonClicked_Lambda(
                [this, iItem]()
                {
                    mOnRemovePaletteSet.ExecuteIfBound(iItem->mPaletteSet);
                    return FReply::Handled();
                }
            );
    }
    else
    {
        if (iItem->mEntry->IsA<UOdysseyPaletteEntryColor>())
        {
            UOdysseyPaletteEntryColor* entryColor = Cast<UOdysseyPaletteEntryColor>(iItem->mEntry);
            return SNew(SOdysseyPainterEditorPaletteColorRow, iOwnerTable)
                .Entry_Lambda(
                    [this, entryColor]()
                    {
                        return entryColor;
                    }
                )
                .Set_Lambda(
                    [this, iItem]()
                    {
                        return iItem->mPaletteSet->mSet;
                    }
                )
                .ColorType( mColorType )
                .IsCurrent_Lambda(
                    [this, iItem]()
                    {
                        UOdysseyPaletteEntryColor* currentEntryColor = mCurrentColorEntry.Get();
                        if (!currentEntryColor)
                            return false;

                        FGuid currentEntrySet = mCurrentSet.Get();
                        FGuid entrySet = iItem->mPaletteSet->mSet;
                        return currentEntryColor == iItem->mEntry && currentEntrySet == entrySet;
                    }
                );
        }
        else if (iItem->mEntry->IsA<UOdysseyPaletteEntryFolder>())
        {
            UOdysseyPaletteEntryFolder* entryFolder = Cast<UOdysseyPaletteEntryFolder>(iItem->mEntry);
            return SNew(SOdysseyPainterEditorPaletteFolderRow, iOwnerTable)
                .Entry_Lambda(
                    [this, iItem]()
                    {
                        return iItem->mEntry;
                    }
                );
        }
    }

    return SNew(STableRow<TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem>>, iOwnerTable);
}

void
SOdysseyPainterEditorPaletteSetList::RebuildPaletteEntryItems(TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem> iItem, TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem> iRootItem)
{
    TArray<UOdysseyPaletteEntry*> childEntries = iItem->mEntry->GetChildren();
    TArray<TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem>> children;
    for (int i = 0; i < childEntries.Num(); i++)
    {
        UOdysseyPaletteEntry* entry = childEntries[i];

        TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem> entryItem = FindEntryItem(entry, iRootItem->mChildren);
        if (!entryItem)
        {
            entryItem = MakeShared<FOdysseyPainterEditorPaletteTreeViewItem>();
            entryItem->mEntry = entry;
            entryItem->mPaletteSet = iItem->mPaletteSet;
            entryItem->mIsPalette = false;
            mTreeView->SetItemExpansion(entryItem.ToSharedRef(), true);
        }

        RebuildPaletteEntryItems(entryItem, iRootItem);
        children.Add(entryItem);
    }
    iItem->mChildren = children;
}

TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem>
SOdysseyPainterEditorPaletteSetList::FindEntryItem(UOdysseyPaletteEntry* iEntry, const TArray<TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem>>& iChildren)
{
    const TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem>* entryItemPtr = iChildren.FindByPredicate(
        [iEntry](TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem> iChild)
        {
            return iChild->mEntry == iEntry;
        }
    );

    if (entryItemPtr)
        return *entryItemPtr;

    for (TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem> child : iChildren)
    {
        TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem> entryItem = FindEntryItem(iEntry, child->mChildren);
        if (entryItem)
            return entryItem;
    }

    return nullptr;
}

void
SOdysseyPainterEditorPaletteSetList::RebuildItems()
{
    TArray<UOdysseyPaletteSet*> paletteSets = mPaletteSets.Get();
    TArray<TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem>> rootItems;
    for (int i = 0; i < paletteSets.Num(); i++)
    {
        UOdysseyPaletteSet* paletteSet = mPaletteSets.Get()[i];
        bool setFound = false;
        for (int j = 0; j < mItemsSource->Num(); j++)
        {
            TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem> item = (*mItemsSource)[j];
            if (item->mIsPalette && item->mPaletteSet == paletteSet)
            {
                rootItems.Add(item);
                setFound = true;
                break;
            }
        }

        if (!setFound)
        {
            TSharedRef<FOdysseyPainterEditorPaletteTreeViewItem> paletteItem = MakeShared<FOdysseyPainterEditorPaletteTreeViewItem>();
            paletteItem->mIsPalette = true;
            paletteItem->mPaletteSet = paletteSet;
            mTreeView->SetItemExpansion(paletteItem, true);
            rootItems.Add(paletteItem);
        }
    }

    for (int i = 0; i < rootItems.Num(); i++)
    {
        TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem> rootItem = rootItems[i];
        UOdysseyPalette* palette = rootItem->mPaletteSet->mPalette;
        TArray<UOdysseyPaletteEntry*> rootEntries = palette->GetRootEntries();
        TArray<TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem>> children;
        for (int j = 0; j < rootEntries.Num(); j++)
        {
            UOdysseyPaletteEntry* rootEntry = rootEntries[j];

            TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem> entryItem = FindEntryItem(rootEntry, rootItem->mChildren);
            if (!entryItem)
            {
                entryItem = MakeShared<FOdysseyPainterEditorPaletteTreeViewItem>();
                entryItem->mEntry = rootEntries[j];
                entryItem->mPaletteSet = rootItem->mPaletteSet;
                entryItem->mIsPalette = false;
                mTreeView->SetItemExpansion(entryItem.ToSharedRef(), true);
            }

            RebuildPaletteEntryItems(entryItem, rootItem);
            children.Add(entryItem);
        }

        rootItem->mChildren = children;
    }

    mItemsSource->Reset();
    mItemsSource->Append(rootItems);
}

void
SOdysseyPainterEditorPaletteSetList::OnPaletteSetsChanged()
{
    RebuildItems();
}

void
SOdysseyPainterEditorPaletteSetList::OnCurrentColorEntryChanged()
{
    UOdysseyPaletteEntryColor* entry = mCurrentColorEntry.Get();
    if (!entry)
        mTreeView->ClearSelection();

    bool isAlreadySelected = mTreeView->GetSelectedItems().ContainsByPredicate(
        [this, entry](TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem> iItem)
        {
            return iItem->mEntry == entry && iItem->mPaletteSet->mSet == mCurrentSet.Get();
        }
    );

    if (isAlreadySelected)
        return;

    for (TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem> item : *mItemsSource)
    {
        TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem> entryItem = FindEntryItem(entry, item->mChildren);
        if (entryItem && mCurrentSet.Get() == entryItem->mPaletteSet->mSet)
        {
            mTreeView->SetItemSelection({entryItem}, true);
            return;
        }
    }
}

void
SOdysseyPainterEditorPaletteSetList::OnSelectionChanged(TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem> iItem, ESelectInfo::Type iSelectInfo)
{
    UOdysseyPaletteEntryColor* entry = mCurrentColorEntry.Get();
    if (entry)
    {
        if (!iItem || iItem->mIsPalette || !iItem->mEntry || !iItem->mEntry->IsA<UOdysseyPaletteEntryColor>())
        {
            for (TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem> item : *mItemsSource)
            {
                TSharedPtr<FOdysseyPainterEditorPaletteTreeViewItem> entryItem = FindEntryItem(entry, item->mChildren);
                if (entryItem)
                {
                    mTreeView->SetItemSelection({entryItem}, true);
                    return;
                }
            }
            mOnCurrentColorEntryChanged.ExecuteIfBound(nullptr, FGuid());
            return;
        }

        if (iItem->mEntry == entry && iItem->mPaletteSet->mSet == mCurrentSet.Get())
            return;

        UOdysseyPaletteEntryColor* entryColor = Cast<UOdysseyPaletteEntryColor>(iItem->mEntry);
        FGuid set = iItem->mPaletteSet->mSet;
        mOnCurrentColorEntryChanged.ExecuteIfBound(entryColor, set);
    }
    else
    {
        if (!iItem)
            return;

        if (iItem->mIsPalette || !iItem->mEntry || !iItem->mEntry->IsA<UOdysseyPaletteEntryColor>())
        {
            mTreeView->ClearSelection();
            return;
        }

        UOdysseyPaletteEntryColor* entryColor = Cast<UOdysseyPaletteEntryColor>(iItem->mEntry);
        FGuid set = iItem->mPaletteSet->mSet;
        mOnCurrentColorEntryChanged.ExecuteIfBound(entryColor, set);
    }
}

#undef LOCTEXT_NAMESPACE
