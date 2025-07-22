// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyViewportDrawingEditorToolkit.h"
#include "OdysseyStyle.h"
#include "OdysseyEditorTab.h"
#include "OdysseyPainterEditorModule.h"
#include "LevelEditor.h"
#include "Widgets/SOdysseyViewportDrawingEditorMasterTab.h"
#include "Interfaces/IMainFrameModule.h"
#include "OdysseyViewportDrawingEditorExtension.h"
#include "Toolkits/AssetEditorModeUILayer.h"
#include "OdysseyPainterEditorToolsTab.h"
#include "OdysseyPainterEditorVectorSceneTreeViewTab.h"
#include "OdysseyPainterEditorAnimationDetailsTab.h"
#include "OdysseyPainterEditorTextureDetailsTab.h"
#include "OdysseyPainterEditorAnimationTimelineTab.h"
#include "OdysseyPainterEditorColorSelectorTab.h"
#include "OdysseyPainterEditorLayerStackTab.h"
#include "Serialization/JsonSerializer.h"

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
FOdysseyViewportDrawingEditorToolkit::LoadOpenedTabs()
{
    FString savedPath = GetOpenedTabIdsSavedPath();
    if (savedPath.IsEmpty())
        return;

    TArray<FName> tabIds = {
        FOdysseyPainterEditorToolsTab::StaticId(),
        FOdysseyPainterEditorVectorSceneTreeViewTab::StaticId(),
        FOdysseyPainterEditorAnimationDetailsTab::StaticId(),
        FOdysseyPainterEditorTextureDetailsTab::StaticId(),
        FOdysseyPainterEditorAnimationTimelineTab::StaticId(),
        FOdysseyPainterEditorColorSelectorTab::StaticId(),
        FOdysseyPainterEditorLayerStackTab::StaticId(),
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

    const TArray<TSharedPtr<FOdysseyEditorTab>>& tabs = mEditor->GetTabs();
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

    if (GEditor)
        GEditor->OnEditorClose().AddRaw(this, &FOdysseyViewportDrawingEditorToolkit::OnEditorClose);

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

    //ensure the editor layout is saved, so that SaveOpenedTabs is synchronized with the editor layout
    FGlobalTabmanager::Get()->SaveAllVisualState();

    SaveOpenedTabs();

    if (GEditor)
        GEditor->OnEditorClose().RemoveAll(this);

    mEditor->CloseAllTabs();
    mEditor->UnregisterTabSpawners(LevelEditorModule.GetLevelEditorTabManager()->AsShared());

    LevelEditorModule.GetMenuExtensibilityManager()->RemoveExtender(mLevelEditorMenuExtender);
    mLevelEditorMenuExtender = nullptr;
    RebuildLevelEditorMenu();
}

void
FOdysseyViewportDrawingEditorToolkit::OnEditorClose()
{
    SaveOpenedTabs();
}

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

FString
FOdysseyViewportDrawingEditorToolkit::GetOpenedTabIdsSavedPath() const
{
    FString filename = FString("OdysseyLayout.json");
    return FPaths::Combine(FPlatformProcess::UserSettingsDir(), FApp::GetEpicProductIdentifier(), TEXT("Editor"), TEXT("Odyssey"), filename);
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
