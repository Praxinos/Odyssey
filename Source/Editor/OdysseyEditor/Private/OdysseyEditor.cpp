// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyEditor.h"

#include "OdysseyEditorGUI.h"
#include "OdysseyEditorTab.h"
#include "ToolMenus.h"
#include "ToolMenuOwner.h"
#include "Serialization/BufferArchive.h"

/////////////////////////////////////////////////////
// FOdysseyEditor
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

FOdysseyEditor::~FOdysseyEditor()
{
}

FOdysseyEditor::FOdysseyEditor(const FText& iName, UObject* iEditedObject)
    : mName(iName)
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

TArray<UObject*>
FOdysseyEditor::GetAdditionalEditedObjects()
{
    return mAdditionalEditedObjects;
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Listeners

void
FOdysseyEditor::BindShortcuts(FBaseToolkit* iToolkit)
{
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

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Interface

void
FOdysseyEditor::ExtendMenu( FToolMenuOwner iOwner, FName iMenuName )
{
    for (const TSharedPtr<FOdysseyEditorTab> tab : mTabs)
    {
        tab->ExtendMenu( iOwner, iMenuName );
    }
}

void
FOdysseyEditor::AddTab(TSharedRef<FOdysseyEditorTab> iTab)
{
    mTabs.Add(iTab);
    iTab->Init();
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
FOdysseyEditor::SetTabsSaveFilename(const FString& iFilename)
{
    mTabsSaveFilename = iFilename;
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
FOdysseyEditor::SaveOpenedTabs()
{
    FString tabsOpenedPath = FPaths::Combine(FPaths::EngineSavedDir(), *mTabsSaveFilename);
    IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
    FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*tabsOpenedPath);
    IFileHandle* fileHandle = platformFile.OpenWrite(*tabsOpenedPath);

    if( !fileHandle )
        return;

    FBufferArchive buffer;
    FString str;

    const TArray<TSharedPtr<FOdysseyEditorTab>>& tabs = GetTabs();
    for (TSharedPtr<FOdysseyEditorTab> tab : tabs)
    {
        if (tab->IsOpened())
        {
            str = tab->GetId().ToString();
            buffer << str;
        }
    }

    fileHandle->Seek(0);
    fileHandle->Write(buffer.GetData(), buffer.Num());

    fileHandle->Flush(true);
    delete fileHandle;

}

void
FOdysseyEditor::LoadOpenedTabs()
{
    FString tabsOpenedPath = FPaths::Combine(FPaths::EngineSavedDir(), *mTabsSaveFilename);
    IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
    IFileHandle* fileHandle = platformFile.OpenRead(*tabsOpenedPath, true);

    if( !fileHandle )
        return;

    FBufferArchive buffer;
    buffer.SetNum( fileHandle->Size() );

    fileHandle->Seek(0);
    fileHandle->Read(buffer.GetData(), fileHandle->Size() );

    TArray<FString> tabNames;
    int start = 4;
    while (start < fileHandle->Size() )
    {
        tabNames.Add( FString( (char*) buffer.GetData() + start ) );
        start += tabNames[tabNames.Num() - 1].Len() + 5;
    }

    fileHandle->Flush(true);
    delete fileHandle;

    const TArray<TSharedPtr<FOdysseyEditorTab>>& tabs = GetTabs();
    for (TSharedPtr<FOdysseyEditorTab> tab : tabs)
    {
        if (tabNames.Contains(tab->GetId()))
            tab->Open();
    }
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