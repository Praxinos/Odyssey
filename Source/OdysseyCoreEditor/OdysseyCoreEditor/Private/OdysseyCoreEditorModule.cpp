// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyCoreEditorModule.h"

#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"
#include "Preferences/UnrealEdOptions.h"
#include "OdysseyCommands.h"
#include "Interfaces/IMainFrameModule.h"
#include "ToolMenu.h"
#include "ToolMenuSection.h"
#include "SAboutWindow.h"
#include "Misc/App.h"
#include "ToolMenus.h"
#include "Framework/Docking/TabManager.h"
#include "OdysseyClipboard.h"

#define LOCTEXT_NAMESPACE "OdysseyCoreEditor"

void
FOdysseyCoreEditorModule::StartupModule()
{
    mClipboard = MakeShared<FOdysseyClipboard>();

    if (!IsRunningCommandlet())
        RegisterCommands();
}

void
FOdysseyCoreEditorModule::ShutdownModule()
{
    UnregisterCommands();
}

//---

void
FOdysseyCoreEditorModule::RegisterCommands()
{
    FOdysseyCommands::Register();

    TSharedRef<FUICommandList>& commandList = FModuleManager::LoadModuleChecked< IMainFrameModule >("MainFrame").GetMainFrameCommandBindings();

    commandList->MapAction(
        FOdysseyCommands::Get().OpenOdysseyAboutWindow,
        FUIAction(FExecuteAction::CreateRaw(this, &FOdysseyCoreEditorModule::OpenAboutWindow))
    );

    commandList->MapAction(
        FOdysseyCommands::Get().OpenOdysseyDocumentation,
        FUIAction(FExecuteAction::CreateRaw(this, &FOdysseyCoreEditorModule::GotoUserDocumentation))
    );

    ExtendHelpMenu();
}

void
FOdysseyCoreEditorModule::UnregisterCommands()
{
    FOdysseyCommands::Unregister();
}

TSharedPtr<FOdysseyClipboard>
FOdysseyCoreEditorModule::GetClipboard() const
{
    return mClipboard;
}

void
FOdysseyCoreEditorModule::ExtendHelpMenu()
{
    UToolMenu* HelpMenu = UToolMenus::Get()->ExtendMenu("MainFrame.MainMenu.Help");
    FToolMenuSection& section = HelpMenu->AddSection("Odyssey", LOCTEXT("main-menu.help.odyssey-section", "Odyssey"));
    section.AddMenuEntry(
        FOdysseyCommands::Get().OpenOdysseyAboutWindow
        , LOCTEXT("main-menu.help.about-odyssey.name", "About Odyssey")
        , LOCTEXT("main-menu.help.about-odyssey.tooltip", "To get more information about Odyssey, the team that created it, etc.")
        /*, FSlateIcon(FAppStyle::Get().GetStyleSetName(), "OdysseyLogo.Odyssey16")
        , NAME_None*/);
    section.AddMenuEntry(
        FOdysseyCommands::Get().OpenOdysseyDocumentation
        , LOCTEXT("main-menu.help.odyssey-documentation.name", "Odyssey Documentation ...")
        , LOCTEXT("main-menu.help.odyssey-documentation.tooltip", "To access Odyssey's Documentationget.")
        /*, FSlateIcon(FAppStyle::Get().GetStyleSetName(), "About.Manual16")
        , NAME_None*/ );
}

void
FOdysseyCoreEditorModule::OpenAboutWindow()
{
    TSharedPtr<SWindow> parentWindow = FGlobalTabmanager::Get()->GetRootWindow();
    SAboutWindow::Open(parentWindow);
}

void
FOdysseyCoreEditorModule::GotoUserDocumentation()
{
    FString URL = "https://praxinos.coop/odyssey-user-doc";
    FPlatformProcess::LaunchURL( *URL, NULL, NULL );
}


IMPLEMENT_MODULE(FOdysseyCoreEditorModule, OdysseyCoreEditor)

#undef LOCTEXT_NAMESPACE
