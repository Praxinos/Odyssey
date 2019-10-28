// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "IOdysseyStyleModule.h"
#include "SlateOdysseyStyle.h"


/**
 * Implements the Editor style module, loaded by SlateApplication dynamically at startup.
 */
class FOdysseyStyleModule
    : public IOdysseyStyleModule
{
public:

    // IEditorStyleModule interface

    virtual void StartupModule( ) override
    {
        FSlateOdysseyStyle::Initialize();
    }

    virtual void ShutdownModule( ) override
    {
        FSlateOdysseyStyle::Shutdown();
    }

    virtual TSharedRef<class FSlateStyleSet> CreateOdysseyStyleInstance( ) const override
    {
        return FSlateOdysseyStyle::Create();
    }

    // End IModuleInterface interface
};


IMPLEMENT_MODULE(FOdysseyStyleModule, OdysseyStyle)

