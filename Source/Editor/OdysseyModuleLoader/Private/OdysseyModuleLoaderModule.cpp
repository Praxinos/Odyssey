// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyModuleLoaderModule.h"
#include "interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "OdysseyModuleLoaderModule"

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
	bool bPaper2DExists = moduleManager.ModuleExists(L"Paper2D");
	OUT_MODULES( bPaper2DExists ).Add("OdysseyFlipbook", ELoadingPhase::PostDefault);
	OUT_MODULES( bPaper2DExists ).Add("OdysseyFlipbookEditor", ELoadingPhase::PostDefault);

	IPluginManager& pluginManager = IPluginManager::Get();
	TSharedPtr<IPlugin> iliad = pluginManager.FindPlugin("Iliad");
	FPluginDescriptor iliadDescriptor = iliad->GetDescriptor();
	
	for ( FModuleDescriptor& iliadModule : iliadDescriptor.Modules )
	{
		if ( activeModules.Contains( iliadModule.Name) )
			iliadModule.LoadingPhase = activeModules[iliadModule.Name];

		if ( inactiveModules.Contains( iliadModule.Name ) )
			iliadModule.LoadingPhase = ELoadingPhase::None;
	}

	FText failureReason;
	iliad->UpdateDescriptor( iliadDescriptor, failureReason );
}

void FOdysseyModuleLoaderModule::ShutdownModule()
{
}

IMPLEMENT_MODULE(FOdysseyModuleLoaderModule, OdysseyModuleLoaderModule );

#undef LOCTEXT_NAMESPACE
