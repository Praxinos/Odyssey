// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyModeToolkit.h"
#include "LevelEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyModeToolkit"

/////////////////////////////////////////////////////
// FOdysseyModeToolkit
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyModeToolkit::~FOdysseyModeToolkit()
{
    TArray<UObject*> objects = mEditor->GetEditedObjects();
    for (int i = 0; i < objects.Num(); i++)
    {
        if (objects[i])
            OnRemoveEditedObject(objects[i]);
    }
    mEditor->OnAddEditedObjectDelegate().RemoveAll(this);
    mEditor->OnRemoveEditedObjectDelegate().RemoveAll(this);

    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
    mEditor->UnregisterTabSpawners(LevelEditorModule.GetLevelEditorTabManager()->AsShared());
}

FOdysseyModeToolkit::FOdysseyModeToolkit(const FName& iAppIdentifier, TSharedPtr<FOdysseyEditor> iEditor, class FEdMode* iEditorMode)
	: mEditor( iEditor )
	, mEditorMode(iEditorMode)
{
}

void FOdysseyModeToolkit::Init(const TSharedPtr<IToolkitHost>& iInitToolkitHost, TWeakObjectPtr<UEdMode> iOwningMode)
{
    TArray<UObject*> objects = mEditor->GetEditedObjects();
    for (int i = 0; i < objects.Num(); i++)
    {
        if (objects[i])
            OnAddEditedObject(objects[i]);
    }

    mEditor->OnAddEditedObjectDelegate().AddRaw(this, &FOdysseyModeToolkit::OnAddEditedObject);
    mEditor->OnRemoveEditedObjectDelegate().AddRaw(this, &FOdysseyModeToolkit::OnRemoveEditedObject);

    //Finish Initialization
    FModeToolkit::Init(iInitToolkitHost, iOwningMode);
    ExtendMenu();

    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
    mEditor->RegisterTabSpawners(LevelEditorModule.GetLevelEditorTabManager()->AsShared());

    mEditor->OnToolkitInitialized(this);
    mEditor->BindShortcuts(this);
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