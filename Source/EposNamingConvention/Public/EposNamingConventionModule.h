// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class UDefaultNamingFormatterBoard;
class UDefaultNamingFormatterShot;

class FEposNamingConventionModule
    : public IModuleInterface
    , public FGCObject
{
public:
    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    /** FGCObject interface */
    virtual void AddReferencedObjects( FReferenceCollector& Collector ) override;
    virtual FString GetReferencerName() const override;

private:
    void RegisterPropertyCustomizations();
    void UnregisterPropertyCustomizations();

    void RegisterNamingFormatter();
    void UnregisterNamingFormatter();

private:
    TObjectPtr<UDefaultNamingFormatterBoard> mNamingFormatterBoard;
    TObjectPtr<UDefaultNamingFormatterShot> mNamingFormatterShot;
};
