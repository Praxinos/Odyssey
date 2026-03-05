// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "IOdysseyStylusInputModule.h"

#include "CoreMinimal.h"

#define LOCTEXT_NAMESPACE "StylusInput"

//---

static const FName StylusInputDebugTabName = FName("StylusInputDebug");

class FOdysseyStylusInputModule
    : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        // register settings
        ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
        if (settingsModule)
        {
            settingsModule->RegisterSettings("Editor", "Plugins", "OdysseyStylusInput"
                , LOCTEXT("settings.name", "Odyssey - Stylus Input")
                , LOCTEXT("settings.tooltip", "Configure the behaviour of stylus inputs in Odyssey.")
                , GetMutableDefault<UOdysseyStylusInputSettings>());
        }
    }

    virtual void ShutdownModule() override
    {
        // unregister settings
        ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

        if (settingsModule)
        {
            settingsModule->UnregisterSettings("Editor", "Plugins", "OdysseyStylusInput");
        }
    }
};

IMPLEMENT_MODULE(FOdysseyStylusInputModule, OdysseyStylusInput)

#undef LOCTEXT_NAMESPACE
