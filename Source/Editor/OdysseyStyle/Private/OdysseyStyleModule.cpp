// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyStyleModule.h"

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "SlateOdysseyStyle.h"

void 
FOdysseyStyleModule::StartupModule()
{
    FSlateOdysseyStyle::Initialize();
}

void 
FOdysseyStyleModule::ShutdownModule()
{
    FSlateOdysseyStyle::Shutdown();
}

IMPLEMENT_MODULE( FOdysseyStyleModule, OdysseyStyle )
