// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

class FOdysseyClipboard;

class ODYSSEYCOREEDITOR_API FOdysseyCoreEditorModule : public IModuleInterface
{
public:
    // IModuleInterface interface
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

public:
    TSharedPtr<FOdysseyClipboard> GetClipboard() const;

private:
    void RegisterCommands();
    void UnregisterCommands();

    void ExtendHelpMenu();

private:
    void OpenAboutWindow();
    void GotoUserDocumentation();

private:
    TSharedPtr<FOdysseyClipboard> mClipboard;
};
