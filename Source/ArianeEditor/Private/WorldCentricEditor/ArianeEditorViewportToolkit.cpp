// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianeEditorViewportToolkit.h"
#include "ArianeEditorViewportEdMode.h"
#include "ArianeEditor.h"
#include "SArianeEditorMasterPanel.h"

// Unreal headers
#include "LevelEditor.h"
#include "Widgets/SNullWidget.h"
#include "Toolkits/BaseToolkit.h"

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
    TArray<UObject*> objects = mEditor->GetAdditionalEditedObjects();
    for (int i = 0; i < objects.Num(); i++)
    {
        if (objects[i])
            OnRemoveEditedObject(objects[i]);
    }
    mEditor->OnAddEditedObjectDelegate().RemoveAll(this);
    mEditor->OnRemoveEditedObjectDelegate().RemoveAll(this);
*/
    mEditor = nullptr;
}

FArianeEditorViewportToolkit::FArianeEditorViewportToolkit( FArianeEditorViewportEdMode* iEdMode )
    : mEdMode( iEdMode )
    , mEditor( this )
/* Gary
    , mTabSaved(false)
*/
{
}

/* Gary
TSharedPtr<FOdysseyPainterEditor>
FArianeEditorViewportToolkit::GetEditor() const
{
    return mEditor;
}
*/

void
FArianeEditorViewportToolkit::ExtendMenu ( FMenuBuilder& MenuBuilder )
{
    UE_LOG(LogTemp, Warning, TEXT("TEST"));
}

void
FArianeEditorViewportToolkit::Init( const TSharedPtr<IToolkitHost>& iInitToolkitHost )
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
    FModeToolkit::Init( iInitToolkitHost );

    mEditor.Init();

    mLevelEditorMenuExtender = MakeShared<FExtender>();
    mLevelEditorMenuExtender->AddMenuExtension( "LevelEditor.ActorContextMenu"
                                              , EExtensionHook::After
                                              , nullptr
                                              , FMenuExtensionDelegate::CreateRaw( this, &FArianeEditorViewportToolkit::ExtendMenu ) );

    LevelEditorModule.GetMenuExtensibilityManager()->AddExtender( mLevelEditorMenuExtender );

/* Gary
    mEditor = MakeShared<FOdysseyPainterEditor>(SharedThis(this));

    mViewportDrawingExtension = MakeShared<FArianeViewportDrawingEditorExtension>(mEditor.Get());
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
    mEditor->OnAddEditedObjectDelegate().AddRaw(this, &FArianeEditorViewportToolkit::OnAddEditedObject);
    mEditor->OnRemoveEditedObjectDelegate().AddRaw(this, &FArianeEditorViewportToolkit::OnRemoveEditedObject);

    RebuildLevelEditorMenu();
*/
}

void
FArianeEditorViewportToolkit::ExtendSecondaryModeToolbar(UToolMenu* InModeToolbarMenu)
{
/* Gary
    mEditor->ExtendLevelEditorToolbar( InModeToolbarMenu );

    FName menuName = InModeToolbarMenu->GetMenuName();
    mEditor->OnRegenerateToolbarAndMenus().BindLambda(
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
    return mEditor;
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
    return mEditor->OnCloseRequested();
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

/* Gary
void
FArianeEditorViewportToolkit::SaveOpenedTabs()
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
FArianeEditorViewportToolkit::LoadOpenedTabs()
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
*/

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

void
FArianeEditorViewportToolkit::InvokeUI()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));

    mEditor.RegisterTabSpawners( LevelEditorModule.GetLevelEditorTabManager()->AsShared() );

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
}

void
FArianeEditorViewportToolkit::ShutdownUI()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>( TEXT( "LevelEditor" ) );

    //ensure the editor layout is saved, so that SaveOpenedTabs is synchronized with the editor layout
    FGlobalTabmanager::Get()->SaveAllVisualState();

/* Gary
    SaveOpenedTabs();
*/
    if (GEditor)
        GEditor->OnEditorClose().RemoveAll(this);

    mEditor.CloseAllTabs();
    mEditor.UnregisterTabSpawners(LevelEditorModule.GetLevelEditorTabManager()->AsShared());

/* Gary
    LevelEditorModule.GetMenuExtensibilityManager()->RemoveExtender(mLevelEditorMenuExtender);
    mLevelEditorMenuExtender = nullptr;
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
    return SNew(SArianeEditorMasterPanel, const_cast<FArianeEditor*>(&mEditor) );
}

FEdMode*
FArianeEditorViewportToolkit::GetEditorMode() const
{
    return mEdMode;
}

/* Gary
TSharedPtr<FArianeViewportDrawingEditorExtension>
FArianeEditorViewportToolkit::GetViewportDrawingExtension() const
{
    return mViewportDrawingExtension;
}
*/

/* Gary
FString
FArianeEditorViewportToolkit::GetOpenedTabIdsSavedPath() const
{
    FString filename = FString("OdysseyLayout.json");
    return FPaths::Combine(FPlatformProcess::UserSettingsDir(), FApp::GetEpicProductIdentifier(), TEXT("Editor"), TEXT("Odyssey"), filename);
}
*/

/* void
FArianeEditorViewportToolkit::BuildToolPalette( FName iPalette, class FToolBarBuilder& ioToolbarBuilder )
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
