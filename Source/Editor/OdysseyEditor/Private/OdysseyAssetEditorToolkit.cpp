// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAssetEditorToolkit.h"

#include "OdysseyEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyAssetEditorToolkit"

/////////////////////////////////////////////////////
// FOdysseyAssetEditorToolkit
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyAssetEditorToolkit::~FOdysseyAssetEditorToolkit()
{
}

FOdysseyAssetEditorToolkit::FOdysseyAssetEditorToolkit(const FName& iAppIdentifier, TSharedPtr<FOdysseyEditor> iEditor) :
    TOdysseyToolkit<FAssetEditorToolkit>(iAppIdentifier, iEditor)
{
}

void
FOdysseyAssetEditorToolkit::Initialize()
{
    mEditor->OnAddEditedObjectDelegate().AddRaw(this, &FOdysseyAssetEditorToolkit::OnAddEditedObject);
    mEditor->OnRemoveEditedObjectDelegate().AddRaw(this, &FOdysseyAssetEditorToolkit::OnRemoveEditedObject);
    
    TArray<UObject*> editedObjects = mEditor->GetEditedObjects();
    FAssetEditorToolkit::InitAssetEditor( EToolkitMode::Standalone, NULL, mAppIdentifier, mEditor->GetLayout(), true, false, editedObjects);
    ExtendMenu();

    //Finish Initialization
    TOdysseyToolkit<FAssetEditorToolkit>::Initialize();
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------- FAssetEditorToolkit interface

void
FOdysseyAssetEditorToolkit::SaveAssetAs_Execute()
{
    UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
    FDelegateHandle openAssetHandle = AssetEditorSubsystem->OnAssetEditorRequestedOpen().AddRaw(this, &FOdysseyAssetEditorToolkit::OpenAsset );

	FAssetEditorToolkit::SaveAssetAs_Execute();

	AssetEditorSubsystem->OnAssetEditorRequestedOpen().Remove(openAssetHandle);
}

bool
FOdysseyAssetEditorToolkit::OnRequestClose()
{
    return mEditor->OnCloseRequested();
}

void
FOdysseyAssetEditorToolkit::RegisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager)
{
    FAssetEditorToolkit::RegisterTabSpawners(iTabManager);
    WorkspaceMenuCategory = mEditor->RegisterTabSpawners(iTabManager);
}

void
FOdysseyAssetEditorToolkit::UnregisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager)
{
    FAssetEditorToolkit::UnregisterTabSpawners(iTabManager);
	mEditor->UnregisterTabSpawners(iTabManager);
}

bool
FOdysseyAssetEditorToolkit::CanReimport() const
{
	return false;
}

bool
FOdysseyAssetEditorToolkit::CanReimport(UObject* EditingObject) const
{
	return false;
}

FText
FOdysseyAssetEditorToolkit::GetToolkitName() const
{
	return GetLabelForObject(GetEditingObjects()[0]);
}

FText
FOdysseyAssetEditorToolkit::GetToolkitToolTipText() const
{
	return GetToolTipTextForObject(GetEditingObjects()[0]);
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Commands building

void
FOdysseyAssetEditorToolkit::ExtendMenu()
{
    mEditor->ExtendMenu( this, GetToolMenuName() );
}

void
FOdysseyAssetEditorToolkit::OnAddEditedObject(UObject* iObject)
{
    AddEditingObject(iObject);
}

void
FOdysseyAssetEditorToolkit::OnRemoveEditedObject(UObject* iObject)
{
    AddEditingObject(iObject);
}

#undef LOCTEXT_NAMESPACE