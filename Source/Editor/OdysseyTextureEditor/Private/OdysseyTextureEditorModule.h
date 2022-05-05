// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Modules/ModuleManager.h"
#include "Toolkits/IToolkitHost.h"

class FOdysseyTextureEditorModule
	: public IModuleInterface
{

public:
    // IModuleInterface interface
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

	//Settings
	void RegisterSettings();
	void UnregisterSettings();

	//Commands
	void RegisterCommands();
	void UnregisterCommands();
};
