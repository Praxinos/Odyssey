// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Editor headers
#include "ArianeEditorViewportToolkit.h"
#include "ArianeEditorViewportEdMode.h"
#include "ArianeEditor.h"
#include "ArianeEditorTab.h"
#include "ArianeEditorLayerStackTab.h"
#include "ArianeEditorColorSelectorTab.h"
#include "SArianeEditorMasterPanel.h"
// Ariane headers
#include "ArianePainting3DActor.h"

// Unreal headers
#include "LevelEditor.h"
#include "Widgets/SNullWidget.h"
#include "Toolkits/BaseToolkit.h"
#include "Framework/Docking/LayoutExtender.h"

/* Gary
#include "OdysseyStyle.h"
#include "OdysseyEditorTab.h"
#include "OdysseyPainterEditorModule.h"
#include "Widgets/SArianeViewportDrawingEditorMasterTab.h"
#include "Interfaces/IMainFrameModule.h"
#include "ArianeViewportDrawingEditorExtension.h"
#include "Toolkits/AssetEditorModeUILayer.h"
#include "OdysseyPainterEditorToolsTab.h"
#include "OdysseyPainterEditorVectorSceneTreeViewTab.h"
#include "OdysseyPainterEditorAnimationDetailsTab.h"
#include "OdysseyPainterEditorTextureDetailsTab.h"
#include "OdysseyPainterEditorAnimationTimelineTab.h"
#include "OdysseyPainterEditorColorSelectorTab.h"
#include "OdysseyPainterEditorLayerStackTab.h"
#include "Serialization/JsonSerializer.h"
*/

#define LOCTEXT_NAMESPACE "ArianeEditorViewport"

FArianeEditorViewportToolkit::~FArianeEditorViewportToolkit()
{
/* Gary
    TArray<UObject*> objects = Editor.GetAdditionalEditedObjects();
    for (int i = 0; i < objects.Num(); i++)
    {
        if (objects[i])
            OnRemoveEditedObject(objects[i]);
    }
    Editor.OnAddEditedObjectDelegate().RemoveAll(this);
    Editor.OnRemoveEditedObjectDelegate().RemoveAll(this);
*/
    Editor = nullptr;
}

FArianeEditorViewportToolkit::FArianeEditorViewportToolkit( FArianeEditorViewportEdMode* iEdMode )
    : EdMode( iEdMode )
    , Editor( MakeShared<FArianeEditor>(this) )
/* Gary
    , mTabSaved(false)
*/
{
}

/* Gary
TSharedPtr<FOdysseyPainterEditor>
FArianeEditorViewportToolkit::GetEditor() const
{
    return Editor;
}
*/

void
FArianeEditorViewportToolkit::ExtendMenu ( FMenuBuilder& MenuBuilder )
{
}

/*
void
FArianeEditorViewportToolkit::ExtendMenus()
{
    UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu.AssetActionsSubMenu");
    FToolMenuSection& Section = Menu->FindOrAddSection("AssetContextMoveActions");

    FToolMenuEntry& Entry = Section.AddDynamicEntry("AssetManagerEditorViewCommands", FNewToolMenuSectionDelegate::CreateLambda([](FToolMenuSection& InSection)
    {
        UContentBrowserAssetContextMenuContext* Context = InSection.FindContext<UContentBrowserAssetContextMenuContext>();
        if (Context)
        {
            FToolUIActionChoice LockAction(FExecuteAction::CreateLambda([Context]()
            {
                FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

                for(FAssetData SelectedAsset : Context->SelectedAssets)
                {
                    if(SelectedAsset.GetPackage()->HasAnyPackageFlags(PKG_DisallowExport) == false)
                    {
                        SelectedAsset.GetPackage()->MarkPackageDirty();
                        SelectedAsset.GetPackage()->SetPackageFlags(PKG_DisallowExport);
                        AssetRegistryModule.Get().AssetFullyUpdateTags(SelectedAsset.GetAsset());
                    }
                }
            }));

            InSection.AddEntry(FToolMenuEntry::InitMenuEntry(FName("Lock"), FText::FromString("Lock Export"), FText::FromString("Lock this asset. Can't export if locked."), FSlateIcon(FMyStyle::Get().GetStyleSetName(), "MyIconName"), LockAction));
        }
    }));
}
*/

void
FArianeEditorViewportToolkit::AddActorMenuEntry( FToolMenuSection& InSection )
{
    InSection.AddMenuEntry(
          FName("MyEntry")
        , LOCTEXT("ariane-editor.add-actor-menu.add-painting3dactor.name", "Add Painting 3D Actor")
        , LOCTEXT("ariane-editor.add-actor-menu.add-painting3dactor.tooltip", "Add Painting 3D Actor")
        , FSlateIcon()
        , FUIAction( FExecuteAction::CreateLambda( [this]()
                                                   {
                                                       AArianePainting3DActor* Painting3DActor = Editor->AddPainting3DActor();

                                                       if( Painting3DActor )
                                                       {
                                                           // Select the actor
                                                           GEditor->SelectNone( true, true );
                                                           GEditor->SelectActor( Painting3DActor, true, true );
                                                       }
                                                   } ) ) );
}

void
FArianeEditorViewportToolkit::Init( const TSharedPtr<IToolkitHost>& iInitToolkitHost )
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
    FModeToolkit::Init( iInitToolkitHost );

    Editor->Init();

    Editor->RegisterTabSpawners();
    Editor->RegisterTools();

    // We create the Widgets now that we can can register to some delegates that they need to refresh themselves
    // just before Editor->Init() is called and will broadcast the delegates.

    //InlineContent = SNew(SArianeEditorMasterPanel, const_cast<FArianeEditor*>( Editor.Get()) );

    UToolMenu* addMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolbar.AddQuickMenu");
    FToolMenuSection& ArianeSection = addMenu->FindOrAddSection("Ariane 3D Painting");

    FToolMenuEntry& Entry = ArianeSection.AddDynamicEntry( "3D Painting Actor"
                                                         , FNewToolMenuSectionDelegate::CreateRaw( this, &FArianeEditorViewportToolkit::AddActorMenuEntry ) );

/*
    LevelEditorMenuExtender = MakeShared<FExtender>();
    LevelEditorMenuExtender->AddMenuExtension( "LevelEditor.LevelEditorToolbar.AddQuickMenu"
                                              , EExtensionHook::After
                                              , nullptr
                                              , FMenuExtensionDelegate::CreateRaw( this, &FArianeEditorViewportToolkit::ExtendMenu ) );

    LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender( LevelEditorMenuExtender );

    FToolMenuInsert actorMenuInsert = FToolMenuInsert("FileActors", EToolMenuInsertType::After);
    UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolbar.AddQuickMenu");
    fileMenu->FindOrAddSection("Ariane", FText(), fileMenuInsert);
    FToolMenuSection& ToolbarSection = ToolbarMenu->AddSection("PrefabSystemSection.BlueprintEditor.Component", INVTEXT("Pefabab System"));

    ToolbarSection.AddDynamicEntry(...);
*/

/* Gary
    Editor = MakeShared<FOdysseyPainterEditor>(SharedThis(this));

    mViewportDrawingExtension = MakeShared<FArianeViewportDrawingEditorExtension>(Editor.Get());
    Editor.AddExtension(mViewportDrawingExtension.ToSharedRef());
    Editor.Initialize();

    TArray<UObject*> objects = Editor.GetAdditionalEditedObjects();
    for (int i = 0; i < objects.Num(); i++)
    {
        if (objects[i])
            OnAddEditedObject(objects[i]);
    }

    //Finish Initialization
    Init(iInitToolkitHost);

    Editor.InitTabs();

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

    LevelEditorMenuExtender = MakeShared<FExtender>();
    Editor.ExtendMenu( LevelEditorMenuExtender.ToSharedRef() );
    LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(LevelEditorMenuExtender);

    Editor.BindShortcuts(this);
    Editor.OnAddEditedObjectDelegate().AddRaw(this, &FArianeEditorViewportToolkit::OnAddEditedObject);
    Editor.OnRemoveEditedObjectDelegate().AddRaw(this, &FArianeEditorViewportToolkit::OnRemoveEditedObject);

    RebuildLevelEditorMenu();
*/
}

void
FArianeEditorViewportToolkit::ExtendSecondaryModeToolbar(UToolMenu* InModeToolbarMenu)
{
    Editor->ExtendLevelEditorToolbar( InModeToolbarMenu );
/*
    FName menuName = InModeToolbarMenu->GetMenuName();
    Editor.OnRegenerateToolbarAndMenus().BindLambda(
        [menuName]()
        {
            UToolMenus::Get()->RefreshMenuWidget(menuName);
        }
    );
*/
}

/* Gary
void
FArianeEditorViewportToolkit::RebuildLevelEditorMenu() const
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
    TSharedPtr<FExtender> Extenders = LevelEditorModule.GetMenuExtensibilityManager()->GetAllExtenders();
    TSharedPtr<ILevelEditor> levelEditor= LevelEditorModule.GetLevelEditorInstance().Pin();
    FToolMenuContext ToolMenuContext(levelEditor->GetLevelEditorActions(), Extenders.ToSharedRef());

    IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>( "MainFrame" );
    TSharedRef< SWidget > MenuBarWidget = MainFrameModule.MakeMainMenu( levelEditor->GetTabManager(), "LevelEditor.MainMenu", ToolMenuContext );
}
*/

/* Gary
void
FArianeEditorViewportToolkit::OnAddEditedObject(UObject* iObject)
{
    GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->NotifyAssetOpened( iObject, this );
}

void
FArianeEditorViewportToolkit::OnRemoveEditedObject(UObject* iObject)
{
    GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->NotifyAssetClosed( iObject, this );
}
*/

FName
FArianeEditorViewportToolkit::GetEditorName() const
{
    return GetToolkitFName();
}

FArianeEditor&
FArianeEditorViewportToolkit::GetEditor()
{
    return *Editor.Get();
}

void
FArianeEditorViewportToolkit::FocusWindow(UObject* ObjectToFocusOn)
{
    //---
}

bool
FArianeEditorViewportToolkit::CloseWindow()
{
/* Gary
    return Editor.OnCloseRequested();
*/
    return false;
}

bool
FArianeEditorViewportToolkit::IsPrimaryEditor() const
{
    return true; //I don't know what this means
}

void
FArianeEditorViewportToolkit::InvokeTab(const struct FTabId& TabId)
{
    //---
}

FName
FArianeEditorViewportToolkit::GetToolbarTabId() const
{
    return GetToolkitFName();
}

TSharedPtr<class FTabManager>
FArianeEditorViewportToolkit::GetAssociatedTabManager()
{
    return TSharedPtr<class FTabManager>();
}

double
FArianeEditorViewportToolkit::GetLastActivationTime()
{
    return 0.0;
}

void
FArianeEditorViewportToolkit::RemoveEditingAsset(UObject* Asset)
{
    //---
}

void
FArianeEditorViewportToolkit::RequestModeUITabs()
{
    FModeToolkit::RequestModeUITabs();
}

void
FArianeEditorViewportToolkit::SaveOpenedTabs()
{
    TArray<FName> tabIds;
    const TArray<TSharedPtr<FArianeEditorTab>>& tabs = Editor->GetTabs();

    for (TSharedPtr<FArianeEditorTab> tab : tabs)
    {
        if ( tab->IsOpened() )
            tabIds.Add( tab->GetId() );
    }

    FString savedPath = GetOpenedTabIdsSavedPath();
    if (savedPath.IsEmpty())
        return;

    TArray<TSharedPtr<FJsonValue>> tabStringIds;
    for ( const FName& tabId : tabIds )
    {
        tabStringIds.Add(MakeShared<FJsonValueString>(tabId.ToString()));
    }

    TSharedRef<FJsonObject> jsonObject = MakeShared<FJsonObject>();
    jsonObject->SetArrayField(TEXT("OpenedTabs"), tabStringIds);

    FString FileContents;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&FileContents);
    if (!FJsonSerializer::Serialize(jsonObject, Writer))
        return;

    if (!FFileHelper::SaveStringToFile(FileContents, *savedPath))
        return;
}

void
FArianeEditorViewportToolkit::LoadOpenedTabs()
{
    FString savedPath = GetOpenedTabIdsSavedPath();
    if (savedPath.IsEmpty())
        return;

    TArray<FName> tabIds = {
        //FArianeEditorToolsTab::StaticId(),
        FArianeEditorLayerStackTab::StaticId(),

        //FOdysseyPainterEditorVectorSceneTreeViewTab::StaticId(),
        //FOdysseyPainterEditorAnimationDetailsTab::StaticId(),
        //FOdysseyPainterEditorTextureDetailsTab::StaticId(),
        //FOdysseyPainterEditorAnimationTimelineTab::StaticId(),
        //FOdysseyPainterEditorColorSelectorTab::StaticId(),
        //FOdysseyPainterEditorLayerStackTab::StaticId(),
    };

    FString FileContents;
    if (FFileHelper::LoadFileToString(FileContents, *savedPath))
    {
        TSharedPtr<FJsonObject> jsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FileContents);
        if (FJsonSerializer::Deserialize(Reader, jsonObject) && jsonObject.IsValid())
        {
            if (jsonObject->HasField(TEXT("OpenedTabs")))
            {
                tabIds.Empty();
                TArray<TSharedPtr<FJsonValue>> tabStringIds = jsonObject->GetArrayField(TEXT("OpenedTabs"));
                for (const TSharedPtr<FJsonValue>& tabStringId : tabStringIds)
                {
                    FString tabIdStr = tabStringId->AsString();
                    if (!tabIdStr.IsEmpty())
                    {
                        FName tabId(*tabIdStr);
                        if (!tabIds.Contains(tabId))
                            tabIds.Add(tabId);
                    }
                }
            }
        }
    }

    const TArray<TSharedPtr<FArianeEditorTab>>& tabs = Editor->GetTabs();
    for (TSharedPtr<FArianeEditorTab> tab : tabs)
    {
        if (!tabIds.Contains(tab->GetId()))
            continue;
        tab->Open();
    }
}

FName
FArianeEditorViewportToolkit::GetToolkitFName() const
{
    return FName("Ariane::Painting3D");
}

FText
FArianeEditorViewportToolkit::GetBaseToolkitName() const
{
    return LOCTEXT("ariane-editor.name", "Ariane::Painting3D");
}

// ShutdownUI is not called when reloading a layout. But Exit() will be, as we designed it.
void
FArianeEditorViewportToolkit::Exit()
{
    Editor->UnregisterTools();
    Editor->UnregisterTabSpawners();
}

void
FArianeEditorViewportToolkit::InvokeUI()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
    TSharedPtr<FTabManager> TabManager = LevelEditorModule.GetLevelEditorTabManager();

    TabManager->TryInvokeTab( FTabId( FArianeEditorColorSelectorTab::StaticId() ) );
    TabManager->TryInvokeTab( FTabId( FArianeEditorLayerStackTab::StaticId() ) );




/* Gary
    if (GEditor)
        GEditor->OnEditorClose().AddRaw(this, &FArianeEditorViewportToolkit::OnEditorClose);
*/

    FModeToolkit::InvokeUI();
/* Gary
    LoadOpenedTabs();
*/

    //FIX: remove the default scrollbar of the main panel
    const TSharedPtr<SWidget> Content = GetInlineContent() ;
    if ( Content && InlineContentHolder.IsValid() )
        InlineContentHolder->SetContent( Content.ToSharedRef() );

    Editor->PostInit();

/*
TSharedPtr<FTabManager::FLayout> CurrentLayout = TabManager->PersistLayout();
if (CurrentLayout.IsValid())
{
    FString LayoutString = CurrentLayout->ToString();
    UE_LOG(LogTemp, Warning, TEXT("LAYOUT DUMP: %s"), *LayoutString);
}
*/

    //MakeDefaultLayout();
}

void
FArianeEditorViewportToolkit::ShutdownUI()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>( TEXT( "LevelEditor" ) );

    //ensure the editor layout is saved, so that SaveOpenedTabs is synchronized with the editor layout
    FGlobalTabmanager::Get()->SaveAllVisualState();

    Editor->SetCurrentTool( nullptr, EToolShutdownType::Accept, true );

/* Gary
    SaveOpenedTabs();
*/
    Editor->CloseAllTabs();

    if (GEditor)
        GEditor->OnEditorClose().RemoveAll(this);


/* Gary
    LevelEditorModule.GetMenuExtensibilityManager()->RemoveExtender(LevelEditorMenuExtender);
    LevelEditorMenuExtender = nullptr;
    RebuildLevelEditorMenu();
*/
}

/* Gary
void
FArianeEditorViewportToolkit::OnEditorClose()
{
    SaveOpenedTabs();
}
*/


TSharedPtr<SWidget>
FArianeEditorViewportToolkit::GetInlineContent() const
{
    return SNew(SArianeEditorMasterPanel, const_cast<FArianeEditor*>( Editor.Get()) );
}


FArianeEditorViewportEdMode*
FArianeEditorViewportToolkit::GetEditorMode() const
{
    return EdMode;
}

/* Gary
TSharedPtr<FArianeViewportDrawingEditorExtension>
FArianeEditorViewportToolkit::GetViewportDrawingExtension() const
{
    return mViewportDrawingExtension;
}
*/

FString
FArianeEditorViewportToolkit::GetOpenedTabIdsSavedPath() const
{
    FString filename = FString("ArianeLayout.json");
    return FPaths::Combine(FPlatformProcess::UserSettingsDir(), FApp::GetEpicProductIdentifier(), TEXT("Editor"), TEXT("Ariane"), filename);
}

/* void
FArianeEditorViewportToolkit::BuildToolPalette( FName iPalette, class FToolBarBuilder& ioToolbarBuilder )
{
    const TArray<TSharedPtr<FArianeEditorTab>>& tabs = Editor.GetTabs();
    for (TSharedPtr<FArianeEditorTab> tab : tabs)
    {
        FFormatNamedArguments Args;
        Args.Add("TabName", tab->GetName());

        FText description = FText::Format(
            LOCTEXT("master-tab.display-tab.tooltip", "Display {TabName}"),
            Args
        );

        ioToolbarBuilder.AddToolBarButton(
            FUIAction(FExecuteAction::CreateSP( tab.ToSharedRef(), &FArianeEditorTab::Open) ),
            NAME_None,
            tab->GetName(),
            description,
            tab->GetIcon()
        );
    }
} */

#undef LOCTEXT_NAMESPACE
