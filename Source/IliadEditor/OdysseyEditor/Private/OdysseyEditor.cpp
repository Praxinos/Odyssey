// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyEditor.h"

#include "OdysseyEditorTab.h"
#include "ToolMenus.h"
#include "ToolMenuOwner.h"
#include "Serialization/BufferArchive.h"
#include "Toolkits/AssetEditorModeUILayer.h"

/////////////////////////////////////////////////////
// FOdysseyEditor
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

FOdysseyEditor::~FOdysseyEditor()
{
}

FOdysseyEditor::FOdysseyEditor(const FName& iId, const FText& iName, UObject* iEditedObject)
    : mId(iId)
    , mName(iName)
    , mEditedObject(iEditedObject)
{
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

FOdysseyEditor::FOnAddEditedObject&
FOdysseyEditor::OnAddEditedObjectDelegate()
{
    return mOnAddEditedObject;
}

FOdysseyEditor::FOnRemoveEditedObject&
FOdysseyEditor::OnRemoveEditedObjectDelegate()
{
    return mOnRemoveEditedObject;
}

FSimpleDelegate&
FOdysseyEditor::OnRegenerateToolbarAndMenus()
{
    return mOnRegenerateToolbarAndMenus;
}

TArray<UObject*>
FOdysseyEditor::GetAdditionalEditedObjects()
{
    return mAdditionalEditedObjects;
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Listeners

FOdysseyEditorShortcuts&
FOdysseyEditor::GetShortcuts()
{
    return mShortcuts;
}

void
FOdysseyEditor::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    mShortcuts.MapActionsToCommandList(toolkitCommands);

    //TODO: Use only mShortcuts instead of BindShortcuts (better coding style)
    for (TSharedPtr<FOdysseyEditorTab> tab : mTabs)
    {
        tab->BindShortcuts(iToolkit);
    }
}

bool
FOdysseyEditor::OnCloseRequested()
{
    return true;
}

void
FOdysseyEditor::OnClose()
{
    //Here is where we should clean everything prior to editor destruction
    mTabs.Empty(); //ensure all tabs are destroyed, because some need the editor on destruction
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Interface

void
FOdysseyEditor::ExtendMenu( TSharedRef<FExtender> iExtender )
{
    for (const TSharedPtr<FOdysseyEditorTab> tab : mTabs)
    {
        tab->ExtendMenu( iExtender );
    }
}

void
FOdysseyEditor::ExtendToolbar( UToolMenu* iToolbar )
{

}

void
FOdysseyEditor::InitToolMenuContext(FToolMenuContext& MenuContext)
{

}

void
FOdysseyEditor::AddTab(TSharedRef<FOdysseyEditorTab> iTab)
{
    mTabs.Add(iTab);
}

void
FOdysseyEditor::InitTabs()
{
    for (const TSharedPtr<FOdysseyEditorTab> tab : mTabs)
    {
        tab->Init();
    }
}

const TArray<TSharedPtr<FOdysseyEditorTab>>&
FOdysseyEditor::GetTabs() const
{
    return mTabs;
}

void
FOdysseyEditor::CloseAllTabs()
{
    for (TSharedPtr<FOdysseyEditorTab> tab : mTabs)
    {
        if (tab->IsOpened())
            tab->Close();
    }
}

void
FOdysseyEditor::RegisterTabSpawners( const TSharedRef< FTabManager >& iTabManager)
{
    TSharedPtr<FWorkspaceItem> workspaceMenuCategory = iTabManager->AddLocalWorkspaceMenuCategory(mName);
    TSharedRef<FWorkspaceItem> workspaceMenuCategoryRef = workspaceMenuCategory.ToSharedRef();
    for (TSharedPtr<FOdysseyEditorTab> tab : mTabs)
    {
        tab->SetTabManager(iTabManager);
        tab->Register(workspaceMenuCategoryRef);
    }
}

void
FOdysseyEditor::UnregisterTabSpawners( const TSharedRef< FTabManager >& iTabManager )
{
    for (TSharedPtr<FOdysseyEditorTab> tab : mTabs)
    {
        tab->Unregister();
    }
}

void
FOdysseyEditor::BuildModeLayout(TSharedPtr<FAssetEditorModeUILayer> iModeUILayerPtr)
{
    for (TSharedPtr<FOdysseyEditorTab> tab : mTabs)
    {
        iModeUILayerPtr->SetModePanelInfo(tab->GetId(), tab->GetMinorTabConfig());
    }
}

const FName&
FOdysseyEditor::GetId() const
{
    return mId;
}

UObject*
FOdysseyEditor::GetEditedObject() const
{
    return mEditedObject;
}

void
FOdysseyEditor::AddEditedObject(UObject* iObject)
{
    mAdditionalEditedObjects.Add(iObject);
    mOnAddEditedObject.Broadcast(iObject);
}

void
FOdysseyEditor::RemoveEditedObject(UObject* iObject)
{
    mAdditionalEditedObjects.Remove(iObject);
    mOnRemoveEditedObject.Broadcast(iObject);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- FGCObject implementation

void
FOdysseyEditor::AddReferencedObjects(FReferenceCollector& Collector)
{

}

FString
FOdysseyEditor::GetReferencerName() const
{
    return "FOdysseyEditor";
}

//--------------------------------------------------------------------------------------
//------------------------------------------------- FTickableEditorObject implementation

void
FOdysseyEditor::Tick(float DeltaTime)
{

}
