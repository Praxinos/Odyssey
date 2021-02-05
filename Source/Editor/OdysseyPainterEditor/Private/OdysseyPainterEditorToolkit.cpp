// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorToolkit.h"

#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorToolkit"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorToolkit
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorToolkit::~FOdysseyPainterEditorToolkit()
{
}

FOdysseyPainterEditorToolkit::FOdysseyPainterEditorToolkit(const FName& iAppIdentifier) :
    mEditor(nullptr),
    mAppIdentifier(iAppIdentifier)
{
}

void
FOdysseyPainterEditorToolkit::Init(TSharedPtr<IOdysseyPainterEditor> iEditor)
{
    mEditor = iEditor;
    mEditor->SetToolkit(SharedThis(this));
    mEditor->Init();
    
    TArray<UObject*> editedObjects = mEditor->GetAllEditedObjects();
    FAssetEditorToolkit::InitAssetEditor( EToolkitMode::Standalone, NULL, mAppIdentifier, mEditor->CreateLayout(), true, false, editedObjects);
    InitMenu();

    mEditor->OnToolkitInitialized();
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------- FAssetEditorToolkit interface

void
FOdysseyPainterEditorToolkit::SaveAssetAs_Execute()
{
    UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
    FDelegateHandle openAssetHandle = AssetEditorSubsystem->OnAssetEditorRequestedOpen().AddLambda([&](UObject* iObject) { OpenAsset(iObject); });

	FAssetEditorToolkit::SaveAssetAs_Execute();

	AssetEditorSubsystem->OnAssetEditorRequestedOpen().Remove(openAssetHandle);
}

bool
FOdysseyPainterEditorToolkit::OnRequestClose()
{
    return mEditor->OnCloseRequested();
}

FText
FOdysseyPainterEditorToolkit::GetToolkitName() const
{
	return GetLabelForObject(mEditor->GetEditedObject());
}

FText
FOdysseyPainterEditorToolkit::GetToolkitToolTipText() const
{
	return GetToolTipTextForObject(mEditor->GetEditedObject());
}

FLinearColor
FOdysseyPainterEditorToolkit::GetWorldCentricTabColorScale() const
{
    return FLinearColor( 0.3f, 0.2f, 0.5f, 0.5f );
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Commands building

void
FOdysseyPainterEditorToolkit::InitMenu()
{
    TArray<TSharedPtr<FExtender>> extenders = mEditor->CreateMenuExtenders();
    for( int i = 0; i < extenders.Num(); i++)
    {
        AddMenuExtender(extenders[i]);
    }
    RegenerateMenusAndToolbars(); //TODO: check if really needed
}

void
FOdysseyPainterEditorToolkit::RegisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager)
{
    FAssetEditorToolkit::RegisterTabSpawners(iTabManager);
    WorkspaceMenuCategory = mEditor->RegisterTabSpawners(iTabManager);
}

void
FOdysseyPainterEditorToolkit::UnregisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager)
{
    FAssetEditorToolkit::UnregisterTabSpawners(iTabManager);
	mEditor->UnregisterTabSpawners(iTabManager);
}

bool
FOdysseyPainterEditorToolkit::CanReimport() const
{
	return false;
}

bool
FOdysseyPainterEditorToolkit::CanReimport(UObject* EditingObject) const
{
	return false;
}

void
FOdysseyPainterEditorToolkit::AddEditingObject(UObject* Object)
{
    FAssetEditorToolkit::AddEditingObject(Object);
}

void
FOdysseyPainterEditorToolkit::RemoveEditingObject(UObject* Object)
{
    FAssetEditorToolkit::RemoveEditingObject(Object);
}

#undef LOCTEXT_NAMESPACE