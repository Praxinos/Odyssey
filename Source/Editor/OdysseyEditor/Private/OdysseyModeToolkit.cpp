// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyModeToolkit.h"
#include "LevelEditor.h"
#include "Interfaces/IMainFrameModule.h"

#define LOCTEXT_NAMESPACE "OdysseyModeToolkit"

/////////////////////////////////////////////////////
// FOdysseyModeToolkit
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyModeToolkit::~FOdysseyModeToolkit()
{
    TArray<UObject*> objects = mEditor->GetAdditionalEditedObjects();
    for (int i = 0; i < objects.Num(); i++)
    {
        if (objects[i])
            OnRemoveEditedObject(objects[i]);
    }
    mEditor->OnAddEditedObjectDelegate().RemoveAll(this);
    mEditor->OnRemoveEditedObjectDelegate().RemoveAll(this);

    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));

    if (!mTabSaved)
    {
        mEditor->SaveOpenedTabs();
        mTabSaved = true;
    }
    mEditor->CloseAllTabs();
    mEditor->UnregisterTabSpawners(LevelEditorModule.GetLevelEditorTabManager()->AsShared());
}

FOdysseyModeToolkit::FOdysseyModeToolkit(TSharedRef<FOdysseyEditor> iEditor)
    : mEditor(iEditor)
    , mTabSaved(false)
{
}


void
FOdysseyModeToolkit::Initialize(
    FEdMode* iEditorMode,
    const TSharedPtr<IToolkitHost>& iInitToolkitHost
)
{
    mEditor->Initialize();

    TArray<UObject*> objects = mEditor->GetAdditionalEditedObjects();
    for (int i = 0; i < objects.Num(); i++)
    {
        if (objects[i])
            OnAddEditedObject(objects[i]);
    }

    //Finish Initialization
    Init(iInitToolkitHost);

    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
    mEditor->RegisterTabSpawners(LevelEditorModule.GetLevelEditorTabManager()->AsShared());
    //mEditor->LoadOpenedTabs();

    mEditor->ExtendMenu( this, FName("LevelEditor.MainMenu") );
    mEditor->BindShortcuts(this);

    mEditor->OnAddEditedObjectDelegate().AddRaw(this, &FOdysseyModeToolkit::OnAddEditedObject);
    mEditor->OnRemoveEditedObjectDelegate().AddRaw(this, &FOdysseyModeToolkit::OnRemoveEditedObject);

    UToolMenus::Get()->RefreshAllWidgets(); //Requested after ExtendMenu

    IMainFrameModule& mainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
    const TSharedPtr<SWindow>& mainFrameParentWindow = mainFrameModule.GetParentWindow();
    mainFrameParentWindow->SetOnWindowClosed(FOnWindowClosed::CreateSP(this, &FOdysseyModeToolkit::OnWindowClosed));
}

void
FOdysseyModeToolkit::OnWindowClosed(const TSharedRef<SWindow>& Window)
{
    //PATCH: Needed to save layout when closing Unreal Engine directly while being in ILIAD Mode
    if (!mTabSaved)
    {
        mEditor->SaveOpenedTabs();
        mTabSaved = true;
    }
}

TSharedPtr<SWidget>
FOdysseyModeToolkit::GetInlineContent() const
{
    //TODO: Create the widget in ViewportDrawingEditorToolkit
	//return mEditor->GetGUI()->GetWidget();
    return SNullWidget::NullWidget;
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
    
}

void
FOdysseyModeToolkit::RequestModeUITabs()
{
	FModeToolkit::RequestModeUITabs();
	if (TSharedPtr<FAssetEditorModeUILayer> modeUILayerPtr = ModeUILayer.Pin())
	{
        mEditor->BuildModeLayout(modeUILayerPtr);
    }
}

void
FOdysseyModeToolkit::InvokeUI()
{
	FModeToolkit::InvokeUI();
    mEditor->InvokeModeLayout();
}

#undef LOCTEXT_NAMESPACE // "OdysseyModeToolkit"