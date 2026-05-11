// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "IOdysseyStylusInputModule.h"

#include "CoreMinimal.h"
#include "ISettingsModule.h"
#include "OdysseyStylusInputSettings.h"

#define LOCTEXT_NAMESPACE "StylusInput"

//---

static const FName StylusInputDebugTabName = FName("StylusInputDebug");

class FOdysseyStylusInputModule
    : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        // register customizations
        FPropertyEditorModule& propertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

        propertyModule.RegisterCustomClassLayout(
            "OdysseyStylusInputSettings",
            FOnGetDetailCustomizationInstance::CreateStatic(&FOdysseyStylusInputSettingsCustomization::MakeInstance)
        );
    }

    virtual void ShutdownModule() override
    {
        // unregister customizations
        if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
        {
            FPropertyEditorModule& PropertyModule =
                FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

            PropertyModule.UnregisterCustomClassLayout("OdysseyStylusInputSettings");
        }
    }
};

IMPLEMENT_MODULE(FOdysseyStylusInputModule, OdysseyStylusInput)

#undef LOCTEXT_NAMESPACE
