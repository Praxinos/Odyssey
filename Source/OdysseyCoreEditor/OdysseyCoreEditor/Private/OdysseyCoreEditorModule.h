// IDDN.FR.001.060015.008.S.X.2019.000.00000
// Odyssey is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

class FOdysseyCoreEditorModule : public IModuleInterface
{
public:
	// IModuleInterface interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void RegisterCommands();
	void UnregisterCommands();

	void ExtendHelpMenu();

private:
	void OpenAboutWindow();
	void GotoUserDocumentation();
};
