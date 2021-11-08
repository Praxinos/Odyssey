// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyModeToolkit.h"

#define LOCTEXT_NAMESPACE "OdysseyModeToolkit"

/////////////////////////////////////////////////////
// FOdysseyModeToolkit
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyModeToolkit::~FOdysseyModeToolkit()
{
}

FOdysseyModeToolkit::FOdysseyModeToolkit(const FName& iAppIdentifier, TSharedPtr<FOdysseyEditor> iEditor, class FEdMode* iEditorMode)
	: TOdysseyToolkit<FModeToolkit>(iAppIdentifier, iEditor)
	, mEditorMode(iEditorMode)
{
}

void
FOdysseyModeToolkit::Initialize()
{
    TArray<UObject*> objects = mEditor->GetEditedObjects();
    for(int i = 0; i < objects.Num(); i++)
    {
        if (objects[i])
            OnAddEditedObject(objects[i]);
    }

	mEditor->OnAddEditedObjectDelegate().AddRaw(this, &FOdysseyModeToolkit::OnAddEditedObject);
    mEditor->OnRemoveEditedObjectDelegate().AddRaw(this, &FOdysseyModeToolkit::OnRemoveEditedObject);

    //Finish Initialization
    TOdysseyToolkit<FModeToolkit>::Initialize();
}

class FEdMode* FOdysseyModeToolkit::GetEditorMode() const
{
	return mEditorMode;
}

TSharedPtr<SWidget> FOdysseyModeToolkit::GetInlineContent() const
{
	return mEditor->GetGUI()->GetWidget();
}

void
FOdysseyModeToolkit::OnAddEditedObject(UObject* iObject)
{
    GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->NotifyAssetOpened( iObject, this );
}

void
FOdysseyModeToolkit::OnRemoveEditedObject(UObject* iObject)
{
    GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->NotifyAssetClosed( iObject, this );
}

FName
FOdysseyModeToolkit::GetEditorName() const
{
    return GetToolkitFName();
}

void
FOdysseyModeToolkit::FocusWindow(UObject* ObjectToFocusOn)
{
    //---
}

bool
FOdysseyModeToolkit::CloseWindow()
{
	return mEditor->OnCloseRequested();
}

bool
FOdysseyModeToolkit::IsPrimaryEditor() const
{
    return true; //I don't know what this means
}

void
FOdysseyModeToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& iTabManager)
{
    FModeToolkit::RegisterTabSpawners( iTabManager );
    WorkspaceMenuCategory = mEditor->RegisterTabSpawners( iTabManager );
}

void
FOdysseyModeToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& iTabManager)
{
    FModeToolkit::UnregisterTabSpawners( iTabManager );
	mEditor->UnregisterTabSpawners(iTabManager);
}

void
FOdysseyModeToolkit::InvokeTab(const struct FTabId& TabId)
{
    //---
}

FName
FOdysseyModeToolkit::GetToolbarTabId() const
{
    return GetToolkitFName();
}

TSharedPtr<class FTabManager>
FOdysseyModeToolkit::GetAssociatedTabManager()
{
    return TSharedPtr<class FTabManager>();
}

double
FOdysseyModeToolkit::GetLastActivationTime()
{
    return 0.0;
}

void
FOdysseyModeToolkit::RemoveEditingAsset(UObject* Asset)
{
    //---
}


void FOdysseyModeToolkit::ExtendMenu()
{
    mEditor->ExtendMenu( this, FName("LevelEditor.MainMenu") );
}

#undef LOCTEXT_NAMESPACE // "OdysseyModeToolkit"