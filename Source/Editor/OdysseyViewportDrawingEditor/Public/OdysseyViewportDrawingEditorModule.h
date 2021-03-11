// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

/**
 * FOdysseyViewportDrawingEditor module interface
 */
class FOdysseyViewportDrawingEditorModule : public IModuleInterface
{
public:
	// IModuleInterface interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	//Editor Mode
	void RegisterEditorMode();
	void UnregisterEditorMode();

    //Commands
	void RegisterCommands();
	void UnregisterCommands();

    //Shaders
	void RegisterShaders();
	void UnregisterShaders();

    //Property Module Customizations
	void RegisterPropertyModuleCustomizations();
	void UnregisterPropertyModuleCustomizations();
};

