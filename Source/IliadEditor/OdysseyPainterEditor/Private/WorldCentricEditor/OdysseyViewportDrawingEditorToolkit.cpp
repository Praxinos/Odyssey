// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyViewportDrawingEditorToolkit.h"
#include "OdysseyStyle.h"
#include "OdysseyEditorTab.h"
#include "OdysseyPainterEditorModule.h"
#include "LevelEditor.h"
#include "Widgets/SOdysseyViewportDrawingEditorMasterTab.h"
#include "Interfaces/IMainFrameModule.h"
#include "OdysseyViewportDrawingEditorExtension.h"
#include "Toolkits/AssetEditorModeUILayer.h"

#define LOCTEXT_NAMESPACE "ViewportDrawingEditor"

FOdysseyViewportDrawingEditorToolkit::~FOdysseyViewportDrawingEditorToolkit()
{
    TArray<UObject*> objects = mEditor->GetAdditionalEditedObjects();
    for (int i = 0; i < objects.Num(); i++)
    {
        if (objects[i])
            OnRemoveEditedObject(objects[i]);
    }
    mEditor->OnAddEditedObjectDelegate().RemoveAll(this);
    mEditor->OnRemoveEditedObjectDelegate().RemoveAll(this);
}

FOdysseyViewportDrawingEditorToolkit::FOdysseyViewportDrawingEditorToolkit(FEdMode* iEdMode)
    : mEditor(nullptr)
    , mEdMode(iEdMode)
    , mTabSaved(false)
{
}

TSharedPtr<FOdysseyPainterEditor>
FOdysseyViewportDrawingEditorToolkit::GetEditor() const
{
    return mEditor;
}

void
FOdysseyViewportDrawingEditorToolkit::Initialize(
    FEdMode* iEditorMode,
    const TSharedPtr<IToolkitHost>& iInitToolkitHost
)
{
    mEditor = MakeShared<FOdysseyPainterEditor>(SharedThis(this));

    mViewportDrawingExtension = MakeShared<FOdysseyViewportDrawingEditorExtension>(mEditor.Get());

    mEditor->AddExtension(mViewportDrawingExtension.ToSharedRef());

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
    //mEditor->RegisterTabSpawners(LevelEditorModule.GetLevelEditorTabManager()->AsShared());
    //mEditor->LoadOpenedTabs();

    mEditor->InitTabs();

    static FString menuName = TEXT("LevelEditor.MainMenu");

    //Add Odyssey Specific section to the main menu to add entries at the right place easier
    FToolMenuInsert fileMenuInsert = FToolMenuInsert("FileActors", EToolMenuInsertType::After);
    UToolMenu* fileMenu = UToolMenus::Get()->ExtendMenu(*(menuName + FString(".File")));
    fileMenu->FindOrAddSection("OdysseyFile", FText(), fileMenuInsert);

    UToolMenu* editMenu = UToolMenus::Get()->ExtendMenu(*(menuName + FString(".Edit")));
    editMenu->FindOrAddSection("OdysseyEdit");

    UToolMenu* windowMenu = UToolMenus::Get()->ExtendMenu(*(menuName + FString(".Window")));
    windowMenu->FindOrAddSection("OdysseyWindow");

    UToolMenu* toolsMenu = UToolMenus::Get()->ExtendMenu(*(menuName + FString(".Tools")));
    toolsMenu->FindOrAddSection("OdysseyTools");

    UToolMenu* buildMenu = UToolMenus::Get()->ExtendMenu(*(menuName + FString(".Build")));
    buildMenu->FindOrAddSection("OdysseyBuild");

    UToolMenu* selectMenu = UToolMenus::Get()->ExtendMenu(*(menuName + FString(".Select")));
    selectMenu->FindOrAddSection("OdysseySelect");

    UToolMenu* actorMenu = UToolMenus::Get()->ExtendMenu(*(menuName + FString(".Actor")));
    actorMenu->FindOrAddSection("OdysseyActor");

    UToolMenu* helpMenu = UToolMenus::Get()->ExtendMenu(*(menuName + FString(".Help")));
    helpMenu->FindOrAddSection("OdysseyHelp");

    mLevelEditorMenuExtender = MakeShared<FExtender>();
    mEditor->ExtendMenu( mLevelEditorMenuExtender.ToSharedRef() );
    LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(mLevelEditorMenuExtender);

    mEditor->BindShortcuts(this);
    mEditor->OnAddEditedObjectDelegate().AddRaw(this, &FOdysseyViewportDrawingEditorToolkit::OnAddEditedObject);
    mEditor->OnRemoveEditedObjectDelegate().AddRaw(this, &FOdysseyViewportDrawingEditorToolkit::OnRemoveEditedObject);

    RebuildLevelEditorMenu();
}



void
FOdysseyViewportDrawingEditorToolkit::ExtendSecondaryModeToolbar(UToolMenu* InModeToolbarMenu)
{
    mEditor->ExtendLevelEditorToolbar( InModeToolbarMenu );

    FName menuName = InModeToolbarMenu->GetMenuName();
    mEditor->OnRegenerateToolbarAndMenus().BindLambda(
        [menuName]()
        {
            UToolMenus::Get()->RefreshMenuWidget(menuName);
        }
    );
}

void
FOdysseyViewportDrawingEditorToolkit::RebuildLevelEditorMenu() const
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
    TSharedPtr<FExtender> Extenders = LevelEditorModule.GetMenuExtensibilityManager()->GetAllExtenders();
    TSharedPtr<ILevelEditor> levelEditor= LevelEditorModule.GetLevelEditorInstance().Pin();
    FToolMenuContext ToolMenuContext(levelEditor->GetLevelEditorActions(), Extenders.ToSharedRef());

    IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>( "MainFrame" );
    TSharedRef< SWidget > MenuBarWidget = MainFrameModule.MakeMainMenu( levelEditor->GetTabManager(), "LevelEditor.MainMenu", ToolMenuContext );
}

void
FOdysseyViewportDrawingEditorToolkit::OnAddEditedObject(UObject* iObject)
{
    GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->NotifyAssetOpened( iObject, this );
}

void
FOdysseyViewportDrawingEditorToolkit::OnRemoveEditedObject(UObject* iObject)
{
    GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->NotifyAssetClosed( iObject, this );
}

FName
FOdysseyViewportDrawingEditorToolkit::GetEditorName() const
{
    return GetToolkitFName();
}

void
FOdysseyViewportDrawingEditorToolkit::FocusWindow(UObject* ObjectToFocusOn)
{
    //---
}

bool
FOdysseyViewportDrawingEditorToolkit::CloseWindow()
{
    return mEditor->OnCloseRequested();
}

bool
FOdysseyViewportDrawingEditorToolkit::IsPrimaryEditor() const
{
    return true; //I don't know what this means
}

void
FOdysseyViewportDrawingEditorToolkit::InvokeTab(const struct FTabId& TabId)
{
    //---
}

FName
FOdysseyViewportDrawingEditorToolkit::GetToolbarTabId() const
{
    return GetToolkitFName();
}

TSharedPtr<class FTabManager>
FOdysseyViewportDrawingEditorToolkit::GetAssociatedTabManager()
{
    return TSharedPtr<class FTabManager>();
}

double
FOdysseyViewportDrawingEditorToolkit::GetLastActivationTime()
{
    return 0.0;
}

void
FOdysseyViewportDrawingEditorToolkit::RemoveEditingAsset(UObject* Asset)
{
    //---
}

void
FOdysseyViewportDrawingEditorToolkit::RequestModeUITabs()
{
    FModeToolkit::RequestModeUITabs();
}

void
FOdysseyViewportDrawingEditorToolkit::SaveOpenedTabs()
{
    TArray<FName> tabIds;
    const TArray<TSharedPtr<FOdysseyEditorTab>>& tabs = mEditor->GetTabs();
    for (TSharedPtr<FOdysseyEditorTab> tab : tabs)
    {
        if (tab->IsOpened())
            tabIds.Add(tab->GetId());
    }

    FOdysseyPainterEditorModule& odysseyEditorModule = FModuleManager::LoadModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
    odysseyEditorModule.SetOpenedTabIds(mEditor->GetId(), tabIds);
}

void
FOdysseyViewportDrawingEditorToolkit::LoadOpenedTabs()
{
    TArray<FName> defaultOpenedTabIds;
    const TArray<TSharedPtr<FOdysseyEditorTab>>& tabs = mEditor->GetTabs();
    for (TSharedPtr<FOdysseyEditorTab> tab : tabs)
    {
        if (tab->ShouldOpenByDefault())
            defaultOpenedTabIds.Add(tab->GetId());
    }

    FOdysseyPainterEditorModule& odysseyEditorModule = FModuleManager::LoadModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
    const TArray<FName>& tabIds = odysseyEditorModule.GetOpenedTabIds(mEditor->GetId(), defaultOpenedTabIds);
    for (TSharedPtr<FOdysseyEditorTab> tab : tabs)
    {
        if (!tabIds.Contains(tab->GetId()))
            continue;
        tab->Open();
    }
}

FName
FOdysseyViewportDrawingEditorToolkit::GetToolkitFName() const
{
    return FName("OdysseyMeshPaintMode");
}

FText
FOdysseyViewportDrawingEditorToolkit::GetBaseToolkitName() const
{
    return LOCTEXT("editor.name", "Odyssey Mesh Paint");
}

void
FOdysseyViewportDrawingEditorToolkit::InvokeUI()
{

    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
    mEditor->RegisterTabSpawners(LevelEditorModule.GetLevelEditorTabManager()->AsShared());

    FModeToolkit::InvokeUI();
    LoadOpenedTabs();

    //FIX: remove the default scrollbar of the main panel
    const TSharedPtr<SWidget> Content = GetInlineContent() ;
    if ( Content && InlineContentHolder.IsValid() )
        InlineContentHolder->SetContent( Content.ToSharedRef() );
}

void
FOdysseyViewportDrawingEditorToolkit::ShutdownUI()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));

    /* Save Opened Tabs Ids */
    SaveOpenedTabs();

    mEditor->CloseAllTabs();
    mEditor->UnregisterTabSpawners(LevelEditorModule.GetLevelEditorTabManager()->AsShared());

    LevelEditorModule.GetMenuExtensibilityManager()->RemoveExtender(mLevelEditorMenuExtender);
    mLevelEditorMenuExtender = nullptr;
    RebuildLevelEditorMenu();
}

/* void
FOdysseyViewportDrawingEditorToolkit::GetToolPaletteNames( TArray<FName>& ioPaletteNames ) const
{
    ioPaletteNames.Add( FName( "Odyssey Panels Manager" ));
} */

TSharedPtr<SWidget>
FOdysseyViewportDrawingEditorToolkit::GetInlineContent() const
{
    return SNew(SOdysseyViewportDrawingEditorMasterTab, mViewportDrawingExtension.Get());
}

FEdMode*
FOdysseyViewportDrawingEditorToolkit::GetEditorMode() const
{
    return mEdMode;
}

TSharedPtr<FOdysseyViewportDrawingEditorExtension>
FOdysseyViewportDrawingEditorToolkit::GetViewportDrawingExtension() const
{
    return mViewportDrawingExtension;
}

/* void
FOdysseyViewportDrawingEditorToolkit::BuildToolPalette( FName iPalette, class FToolBarBuilder& ioToolbarBuilder )
{
    const TArray<TSharedPtr<FOdysseyEditorTab>>& tabs = mEditor->GetTabs();
    for (TSharedPtr<FOdysseyEditorTab> tab : tabs)
    {
        FFormatNamedArguments Args;
        Args.Add("TabName", tab->GetName());

        FText description = FText::Format(
            LOCTEXT("master-tab.display-tab.tooltip", "Display {TabName}"),
            Args
        );

        ioToolbarBuilder.AddToolBarButton(
            FUIAction(FExecuteAction::CreateSP( tab.ToSharedRef(), &FOdysseyEditorTab::Open) ),
            NAME_None,
            tab->GetName(),
            description,
            tab->GetIcon()
        );
    }
} */

#undef LOCTEXT_NAMESPACE
