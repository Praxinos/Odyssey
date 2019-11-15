// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

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
