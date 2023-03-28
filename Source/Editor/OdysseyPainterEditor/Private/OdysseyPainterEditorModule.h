// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Modules/ModuleManager.h"

class FOdysseyPainterEditorModule
    : public IModuleInterface
{
public:
    // IModuleInterface interface
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    //Settings
	void RegisterSettings();
	void UnregisterSettings();

    //Commands
	void RegisterCommands();
	void UnregisterCommands();

    //BrushOverrides
	void RegisterBrushOverrides();
	void UnregisterBrushOverrides();
};