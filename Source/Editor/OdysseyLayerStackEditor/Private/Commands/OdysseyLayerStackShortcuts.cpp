// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Commands/OdysseyLayerStackShortcuts.h"

#include "Framework/Commands/GenericCommands.h"
#include "Widgets/SOdysseyLayerStackTreeView.h"
#include "OdysseyLayerStack.h"
#include "OdysseyLayerStackClipboardData.h"
#include "UObject/OdysseyObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "FOdysseyLayerStackShortcuts"

FOdysseyLayerStackShortcuts::FOdysseyLayerStackShortcuts(TSharedPtr<SOdysseyLayerStackTreeView> iTreeView, UOdysseyLayerStack* iLayerStack)
    : mCommandList(MakeShared<FUICommandList>())
    , mTreeView(iTreeView)
    , mLayerStack(iLayerStack)
{
    MapActionsToCommandList();
}

TSharedRef<FUICommandList>
FOdysseyLayerStackShortcuts::GetCommandList() const
{
    return mCommandList;
}

void
FOdysseyLayerStackShortcuts::MapActionsToCommandList()
{
    mCommandList->MapAction(
        FGenericCommands::Get().Copy,
        FExecuteAction::CreateRaw(this, &FOdysseyLayerStackShortcuts::Action_Copy),
        FCanExecuteAction::CreateRaw(this, &FOdysseyLayerStackShortcuts::CanAction_Copy)
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Paste,
        FExecuteAction::CreateRaw(this, &FOdysseyLayerStackShortcuts::Action_Paste),
        FCanExecuteAction::CreateRaw(this, &FOdysseyLayerStackShortcuts::CanAction_Paste)
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Cut,
        FExecuteAction::CreateRaw(this, &FOdysseyLayerStackShortcuts::Action_Cut),
        FCanExecuteAction::CreateRaw(this, &FOdysseyLayerStackShortcuts::CanAction_Cut)
    );

    mCommandList->MapAction(
        FGenericCommands::Get().SelectAll,
        FExecuteAction::CreateRaw(this, &FOdysseyLayerStackShortcuts::Action_SelectAll),
        FCanExecuteAction::CreateRaw(this, &FOdysseyLayerStackShortcuts::CanAction_SelectAll)
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Delete,
        FExecuteAction::CreateRaw(this, &FOdysseyLayerStackShortcuts::Action_Delete),
        FCanExecuteAction::CreateRaw(this, &FOdysseyLayerStackShortcuts::CanAction_Delete)
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Duplicate,
        FExecuteAction::CreateRaw(this, &FOdysseyLayerStackShortcuts::Action_Duplicate),
        FCanExecuteAction::CreateRaw(this, &FOdysseyLayerStackShortcuts::CanAction_Duplicate)
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Rename,
        FExecuteAction::CreateRaw(this, &FOdysseyLayerStackShortcuts::Action_Rename),
        FCanExecuteAction::CreateRaw(this, &FOdysseyLayerStackShortcuts::CanAction_Rename)
    );

    mCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().MergeSelectedLayers,
        FExecuteAction::CreateRaw(this, &FOdysseyLayerStackShortcuts::Action_MergeSelectedLayers),
        FCanExecuteAction::CreateRaw(this, &FOdysseyLayerStackShortcuts::CanAction_MergeSelectedLayers)
    );

    mCommandList->MapAction(
        FOdysseyLayerStackEditorCommands::Get().FlattenSelectedLayers,
        FExecuteAction::CreateRaw(this, &FOdysseyLayerStackShortcuts::Action_FlattenSelectedLayers),
        FCanExecuteAction::CreateRaw(this, &FOdysseyLayerStackShortcuts::CanAction_FlattenSelectedLayers)
    );
}

//Common Shortcuts 
void
FOdysseyLayerStackShortcuts::Action_Rename()
{
    if ( !mLayerStack )
        return;

    if (!mLayerStack->CurrentLayer)
        return;

    TSharedPtr<SOdysseyLayerStackTreeView> treeView = mTreeView.Pin();
    if (!treeView)
        return;

    treeView->SetIsRenamePending(true); //has to come before ScrollItemIntoView() in case the item is already into view, which will trigger OnItemScrolledIntoView() immediately
    treeView->RequestScrollIntoView(mLayerStack->CurrentLayer.Get());
}

void
FOdysseyLayerStackShortcuts::Action_Copy()
{
    TSharedPtr<SOdysseyLayerStackTreeView> treeView = mTreeView.Pin();
    if (!treeView)
        return;

    TSharedPtr<FOdysseyLayerStackClipboardData> clipboardData = MakeShared<FOdysseyLayerStackClipboardData>(treeView->GetSelectedItems());
    FOdysseyClipboard::Get().SetData(clipboardData);
}

void
FOdysseyLayerStackShortcuts::Action_Cut()
{
    TSharedPtr<SOdysseyLayerStackTreeView> treeView = mTreeView.Pin();
    if (!treeView)
        return;

    TSharedPtr<FOdysseyLayerStackClipboardData> clipboardData = MakeShared<FOdysseyLayerStackClipboardData>(treeView->GetSelectedItems());
    FOdysseyClipboard::Get().SetData(clipboardData);

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("LayerStack", "Cut Layers"));
#endif
    Action_Delete();
}

void
FOdysseyLayerStackShortcuts::Action_Paste()
{
    TSharedPtr<FOdysseyLayerStackClipboardData> clipboardData = FOdysseyClipboard::Get().GetData<FOdysseyLayerStackClipboardData>();
    if (!clipboardData)
        return;

    TSharedPtr<SOdysseyLayerStackTreeView> treeView = mTreeView.Pin();
    if (!treeView)
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("LayerStack", "Paste Layers"));
#endif

    TArray<UOdysseyLayer*> pastedLayers = clipboardData->Paste(mLayerStack);
    treeView->SetItemSelection(pastedLayers, true);
}

void
FOdysseyLayerStackShortcuts::Action_SelectAll()
{
    if ( !mLayerStack )
        return;
    
    TSharedPtr<SOdysseyLayerStackTreeView> treeView = mTreeView.Pin();
    if (!treeView)
        return;

    //ItemsSource is the ListView::ItemsSource, which contains all displayed items, even deep children
    //It is NOT the same as TreeItemsSource or mRootLayers which only contain root elements
    treeView->SetItemSelection(treeView->SListView<UOdysseyLayer*>::GetItems(), true);
}

void
FOdysseyLayerStackShortcuts::Action_Delete()
{
    if ( !mLayerStack )
        return;
    
    TSharedPtr<SOdysseyLayerStackTreeView> treeView = mTreeView.Pin();
    if (!treeView)
        return;

    TArray<UOdysseyLayer*> selectedLayers = treeView->GetSelectedItems();
    mLayerStack->RemoveLayers(selectedLayers);
}

void
FOdysseyLayerStackShortcuts::Action_Duplicate()
{
    if ( !mLayerStack )
        return;
        
    TSharedPtr<SOdysseyLayerStackTreeView> treeView = mTreeView.Pin();
    if (!treeView)
        return;

    TArray<UOdysseyLayer*> selectedLayers = treeView->GetSelectedItems();
    if (selectedLayers.Num() <= 0)
        return;

	//manage current layer seperately
	TArray<UOdysseyLayer*> duplicatedLayers = mLayerStack->DuplicateLayers(selectedLayers);
}


bool
FOdysseyLayerStackShortcuts::CanAction_Rename()
{
    return true;
}

bool
FOdysseyLayerStackShortcuts::CanAction_Copy()
{
    return true;
}

bool
FOdysseyLayerStackShortcuts::CanAction_Cut()
{
    return true;
}

bool
FOdysseyLayerStackShortcuts::CanAction_Paste()
{
    //First check if the copied layers can be pasted in this layerstack
    TSharedPtr<FOdysseyLayerStackClipboardData> clipboardData = FOdysseyClipboard::Get().GetData<FOdysseyLayerStackClipboardData>();
    if (!clipboardData)
        return false;

    return clipboardData->CanPaste(mLayerStack);
}

bool
FOdysseyLayerStackShortcuts::CanAction_SelectAll()
{
    return true;
}

bool
FOdysseyLayerStackShortcuts::CanAction_Delete()
{
    if ( !mLayerStack )
        return false;
        
    TSharedPtr<SOdysseyLayerStackTreeView> treeView = mTreeView.Pin();
    if (!treeView)
        return false;

    TArray<UOdysseyLayer*> selectedLayers = treeView->GetSelectedItems();
    if (selectedLayers.Num() <= 0)
        return false;

    //If one of the root layers is not selected, we can delete selected layers
    const TArray<UOdysseyLayer*>& rootLayers = mLayerStack->GetRootLayers();
    for (UOdysseyLayer* rootLayer : rootLayers)
    {
        if (!selectedLayers.Contains(rootLayer))
            return true;
    }
    
    return false;
}

bool
FOdysseyLayerStackShortcuts::CanAction_Duplicate()
{
    return true;
}


//Layers Specific Shortcuts
void
FOdysseyLayerStackShortcuts::Action_MergeSelectedLayers()
{
    if ( !mLayerStack )
        return;
        
    TSharedPtr<SOdysseyLayerStackTreeView> treeView = mTreeView.Pin();
    if (!treeView)
        return;

    TArray<UOdysseyLayer*> selectedLayers = treeView->GetSelectedItems();
    if (selectedLayers.Num() <= 0)
        return;

    mLayerStack->MergeLayers(selectedLayers);
}

void
FOdysseyLayerStackShortcuts::Action_FlattenSelectedLayers()
{
    if ( !mLayerStack )
        return;
        
    TSharedPtr<SOdysseyLayerStackTreeView> treeView = mTreeView.Pin();
    if (!treeView)
        return;

    TArray<UOdysseyLayer*> selectedLayers = treeView->GetSelectedItems();
    if (selectedLayers.Num() <= 0)
        return;

    mLayerStack->FlattenLayers(selectedLayers);
}


bool
FOdysseyLayerStackShortcuts::CanAction_MergeSelectedLayers()
{
    if ( !mLayerStack )
        return false;
        
    TSharedPtr<SOdysseyLayerStackTreeView> treeView = mTreeView.Pin();
    if (!treeView)
        return false;

    TArray<UOdysseyLayer*> selectedLayers = treeView->GetSelectedItems();
    if (selectedLayers.Num() <= 0)
        return false;

    return mLayerStack->CanMergeLayers(selectedLayers);
}

bool
FOdysseyLayerStackShortcuts::CanAction_FlattenSelectedLayers()
{
    if ( !mLayerStack )
        return false;
        
    TSharedPtr<SOdysseyLayerStackTreeView> treeView = mTreeView.Pin();
    if (!treeView)
        return false;

    TArray<UOdysseyLayer*> selectedLayers = treeView->GetSelectedItems();
    if (selectedLayers.Num() <= 0)
        return false;

    return mLayerStack->CanFlattenLayers(selectedLayers);
}

#undef LOCTEXT_NAMESPACE