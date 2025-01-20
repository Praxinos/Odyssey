// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyModuleLoaderModule.h"
#include "interfaces/IPluginManager.h"
#include "Modules/ModuleManager.h"

void
FOdysseyModuleLoaderModule::StartupModule()
{
    ActivateModules();
}

void
FOdysseyModuleLoaderModule::ActivateModules()
{
    #define OUT_MODULES(iCond) (iCond ? activeModules : inactiveModules)

    TMap<FName, ELoadingPhase::Type> activeModules;
    TMap<FName, ELoadingPhase::Type> inactiveModules;

    FModuleManager& moduleManager = FModuleManager::Get();
    bool bPaper2DExists = moduleManager.ModuleExists(TEXT("Paper2D"));
    OUT_MODULES( bPaper2DExists ).Add("OdysseyFlipbook", ELoadingPhase::PostDefault);
    OUT_MODULES( bPaper2DExists ).Add("OdysseyFlipbookEditor", ELoadingPhase::PostDefault);

    IPluginManager& pluginManager = IPluginManager::Get();
    TSharedPtr<IPlugin> Odyssey = pluginManager.FindPlugin("Odyssey");
    FPluginDescriptor& OdysseyDescriptor = const_cast<FPluginDescriptor&>(Odyssey->GetDescriptor());

    for ( FModuleDescriptor& OdysseyModule : OdysseyDescriptor.Modules )
    {
        if ( activeModules.Contains( OdysseyModule.Name) )
            OdysseyModule.LoadingPhase = activeModules[OdysseyModule.Name];

        if ( inactiveModules.Contains( OdysseyModule.Name ) )
            OdysseyModule.LoadingPhase = ELoadingPhase::None;
    }
}

void FOdysseyModuleLoaderModule::ShutdownModule()
{
}

IMPLEMENT_MODULE(FOdysseyModuleLoaderModule, OdysseyModuleLoader );
