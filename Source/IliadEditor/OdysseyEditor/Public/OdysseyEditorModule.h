// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class FOdysseyClipboard;

class ODYSSEYEDITOR_API FOdysseyEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

public:
    TSharedPtr<FOdysseyClipboard> GetClipboard() const;

private:
    void OnEnginePreExit();

private:
    TSharedPtr<FOdysseyClipboard> mClipboard;
};
