// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Views/SListView.h"

#include "OdysseyEditorTab.h"
#include <ULIS>

class FOdysseyPainterEditor;
class UOdysseyPainterEditorTool;
class UOdysseyToolCollection;
class ITableRow;
class STableViewBase;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorToolCollectionTab :
    public FOdysseyEditorTab
{
public:
    static const FName& StaticId();

public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorToolCollectionTab();
    FOdysseyPainterEditorToolCollectionTab(FOdysseyPainterEditor* iEditor);

protected:
    // FOdysseyEditorTab interface
    virtual const FName& GetId() const override;
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;

    TSharedRef<ITableRow> OnGenerateCollectionRow(TWeakObjectPtr<UOdysseyToolCollection> InCollection, const TSharedRef<STableViewBase>& OwnerTable);
protected:
    // Event Listeners
    void OnAssetSelected(const FAssetData& AssetData);
    TSharedRef<SWidget> OnGetAddToolCollectionMenuContent();
    FReply OnLockClicked();

    TSharedRef<SWidget> GetCollectionMenuContent(TWeakObjectPtr<UOdysseyToolCollection> iCollection);
    void HandleSaveCollection(TWeakObjectPtr<UOdysseyToolCollection> iCollection);
    void HandleRemoveCollection(TWeakObjectPtr<UOdysseyToolCollection> iCollection);

protected:
    bool IsUnlocked() const;
    const FSlateBrush* GetLockIcon() const;
    void RefreshCollectionsGUI();
    void OnSourceChanged();
    void AddToolCollection(UOdysseyToolCollection* iToolCollection);
    void RemoveToolCollection(UOdysseyToolCollection* iToolCollection);

private:
    FOdysseyPainterEditor* mEditor;

    TArray<TWeakObjectPtr<UOdysseyToolCollection>> mSelectedCollections;

    /** Contains add collection button, lock, and filter options */
    TSharedPtr<SHorizontalBox> mTabOptions;

    TSharedPtr<SListView<TWeakObjectPtr<UOdysseyToolCollection>>> mCollectionsListView;
    TSharedPtr<SVerticalBox> mCollectionsWidget;

    bool bIsUnlocked = true;
};
