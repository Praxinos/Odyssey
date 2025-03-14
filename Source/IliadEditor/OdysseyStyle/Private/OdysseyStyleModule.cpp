// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

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
