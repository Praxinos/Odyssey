// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class UNamingFormatter;

class FEposSequenceModule
    : public IModuleInterface
{
public:
    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

public:
    EPOSSEQUENCE_API void RegisterNamingFormatter( UNamingFormatter* iFormatter );
    EPOSSEQUENCE_API void UnregisterNamingFormatter( UNamingFormatter* iFormatter );

public:
    template<typename Formatter>
    UNamingFormatter* GetNamingFormatter();

private:
    TArray<UNamingFormatter*> mNamingFormatters;
};
