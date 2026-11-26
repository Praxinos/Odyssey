// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "HAL/IConsoleManager.h"
#include "Logging/StructuredLog.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "ToolMenus.h"

#include "OdysseyRasterStorageTagWriter.h"
#include "OdysseyTiledImageMigrationStartupOffer.h"
#include "OdysseyTiledImageMigrationWorkflow.h"
#include "OdysseyUserInteraction.h"
#include "OdysseyTiledImageMigrationLog.h"

#define LOCTEXT_NAMESPACE "OdysseyTiledImageMigration"

DEFINE_LOG_CATEGORY(LogOdysseyTiledImageMigration);

namespace UE::OdysseyTiledImageMigration::Private
{
/** Runs the conversion workflow when the editor can interact with the user. */
void RunFromCommand()
{
    if (!Odyssey::CanInteractWithUser())
    {
        UE_LOGFMT(LogOdysseyTiledImageMigration, Warning, "The conversion of the legacy Odyssey assets needs an interactive editor.");
        return;
    }
    Odyssey::TiledImageMigration::Run();
}
}

/**
 * Hosts the conversion of the legacy raster content of layers and cells to tiled images, project-wide:
 * the startup offer, a Tools menu entry and a console command. Transitional.
 */
class FOdysseyTiledImageMigrationModule
    : public IModuleInterface
{
public:
    // IModuleInterface interface
    /** Starts the services of the conversion and registers its Tools menu entry. */
    virtual void StartupModule() override;
    /** Unregisters the Tools menu entry and stops the services of the conversion. */
    virtual void ShutdownModule() override;
    // End of IModuleInterface interface

private:
    /** Adds the entry of the conversion to the Tools menu of the level editor. */
    void RegisterMenu();

private:
    /** Writes the raster storage tag on the saved assets while the module is loaded. */
    FOdysseyRasterStorageTagWriter StorageTagWriter;

    /** Offers the conversion once the asset registry has scanned the project. */
    FOdysseyTiledImageMigrationStartupOffer StartupOffer;

    /** The console command running the conversion workflow. */
    FAutoConsoleCommand ConvertCommand = FAutoConsoleCommand(
        TEXT("Odyssey.MigrateToTiledImages"),
        TEXT("Shows the legacy Odyssey assets of the project and its plugins, and converts those you choose."),
        FConsoleCommandDelegate::CreateStatic(&UE::OdysseyTiledImageMigration::Private::RunFromCommand));
};

void
FOdysseyTiledImageMigrationModule::StartupModule()
{
    StorageTagWriter.Start();
    StartupOffer.Start();
    const FSimpleMulticastDelegate::FDelegate MenuCallback = FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FOdysseyTiledImageMigrationModule::RegisterMenu);
    UToolMenus::RegisterStartupCallback(MenuCallback);
}

void
FOdysseyTiledImageMigrationModule::ShutdownModule()
{
    UToolMenus::UnRegisterStartupCallback(this);
    UToolMenus::UnregisterOwner(this);
    StartupOffer.Stop();
    StorageTagWriter.Stop();
}

void
FOdysseyTiledImageMigrationModule::RegisterMenu()
{
    FToolMenuOwnerScoped OwnerScoped(this);
    UToolMenus* ToolMenus = UToolMenus::Get();
    UToolMenu* Menu = ToolMenus->ExtendMenu("LevelEditor.MainMenu.Tools");
    FToolMenuSection& Section = Menu->FindOrAddSection("Odyssey", LOCTEXT("menu.section", "Odyssey"));
    const FExecuteAction ExecuteAction = FExecuteAction::CreateStatic(&UE::OdysseyTiledImageMigration::Private::RunFromCommand);
    const FUIAction Action(ExecuteAction);
    Section.AddMenuEntry(
        "OdysseyMigrateToTiledImages",
        LOCTEXT("menu.convert", "Convert Legacy Odyssey Assets..."),
        LOCTEXT("menu.convert-tooltip", "Shows the legacy Odyssey assets of the project and its plugins, and converts those you choose."),
        FSlateIcon(),
        Action);
}

IMPLEMENT_MODULE(FOdysseyTiledImageMigrationModule, OdysseyTiledImageMigration);

#undef LOCTEXT_NAMESPACE
