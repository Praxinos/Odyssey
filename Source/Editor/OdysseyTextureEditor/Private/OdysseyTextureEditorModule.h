// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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
