// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAssetEditorToolkit.h"

#include "OdysseyEditor.h"

/////////////////////////////////////////////////////
// FOdysseyAssetEditorToolkit
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyAssetEditorToolkit::~FOdysseyAssetEditorToolkit()
{
}

FOdysseyAssetEditorToolkit::FOdysseyAssetEditorToolkit(const FName& iAppIdentifier)
    : mAppIdentifier(iAppIdentifier)
    , mEditor(nullptr)
{
}

void
FOdysseyAssetEditorToolkit::Initialize(UObject* iEditedObject, TSharedPtr<FOdysseyEditor> iEditor)
{
    //TArray<UObject*> editedObjects = mEditor->GetEditedObjects();
    mEditor = iEditor;
    mEditor->Initialize();
    mEditor->InitTabs();

    TArray<UObject*> editedObjects = mEditor->GetAdditionalEditedObjects(); //Editor can add some side edited objects
    editedObjects.Add(iEditedObject);

    FAssetEditorToolkit::InitAssetEditor( EToolkitMode::Standalone, NULL, mAppIdentifier, mEditor->CreateLayout(), true, false, editedObjects);

	//Add Odyssey Specific section to the main menu to add entries at the right place easier
	UToolMenu* fileMenu = UToolMenus::Get()->ExtendMenu(*(GetToolMenuName().ToString() + FString(".File")));
	fileMenu->FindOrAddSection("OdysseyFile");

	UToolMenu* editMenu = UToolMenus::Get()->ExtendMenu(*(GetToolMenuName().ToString() + FString(".Edit")));
	editMenu->FindOrAddSection("OdysseyEdit");

	UToolMenu* assetMenu = UToolMenus::Get()->ExtendMenu(*(GetToolMenuName().ToString() + FString(".Asset")));
	assetMenu->FindOrAddSection("OdysseyAsset");

	UToolMenu* windowMenu = UToolMenus::Get()->ExtendMenu(*(GetToolMenuName().ToString() + FString(".Window")));
	windowMenu->FindOrAddSection("OdysseyWindow");

	UToolMenu* toolsMenu = UToolMenus::Get()->ExtendMenu(*(GetToolMenuName().ToString() + FString(".Tools")));
	toolsMenu->FindOrAddSection("OdysseyTools");

	UToolMenu* helpMenu = UToolMenus::Get()->ExtendMenu(*(GetToolMenuName().ToString() + FString(".Help")));
	helpMenu->FindOrAddSection("OdysseyHelp");

	TSharedRef<FExtender> extender = MakeShared<FExtender>();
	mEditor->ExtendMenu( extender );
	AddMenuExtender(extender);

    mEditor->BindShortcuts( this );
    mEditor->OnAddEditedObjectDelegate().AddRaw(this, &FOdysseyAssetEditorToolkit::OnAddEditedObject);
    mEditor->OnRemoveEditedObjectDelegate().AddRaw(this, &FOdysseyAssetEditorToolkit::OnRemoveEditedObject);

	RegenerateMenusAndToolbars();
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------- FAssetEditorToolkit interface

FLinearColor
FOdysseyAssetEditorToolkit::GetWorldCentricTabColorScale() const
{
    return FLinearColor( 0.3f, 0.2f, 0.5f, 0.5f );
}

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
FOdysseyAssetEditorToolkit::OnClose()
{
    //Here is where we should clean everything prior to editor destruction
    mEditor->OnClose();
}

void
FOdysseyAssetEditorToolkit::RegisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager)
{
    FAssetEditorToolkit::RegisterTabSpawners(iTabManager);
    mEditor->RegisterTabSpawners(iTabManager);
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
FOdysseyAssetEditorToolkit::OnAddEditedObject(UObject* iObject)
{
    AddEditingObject(iObject);
}

void
FOdysseyAssetEditorToolkit::OnRemoveEditedObject(UObject* iObject)
{
    AddEditingObject(iObject);
}
