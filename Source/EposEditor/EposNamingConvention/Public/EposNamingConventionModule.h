// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "UObject/GCObject.h"
#include "UObject/ObjectPtr.h"

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
