// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FBoardSequenceActions;
class FShotSequenceActions;
class USequencerSettings;

class FEposEditorModule : public IModuleInterface, public FGCObject
{
public:
    FEposEditorModule();

public:
    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    /** FGCObject interface */
    virtual void AddReferencedObjects( FReferenceCollector& Collector ) override;

private:
    void RegisterCommands();
    void UnregisterCommands();

    void RegisterAssetTools();
    void UnregisterAssetTools();

    void RegisterSettings();
    void UnregisterSettings();

    void RegisterSequenceCustomizations();
    void UnregisterSequenceCustomizations();

private:
    TSharedPtr<FBoardSequenceActions>   mBoardSequenceTypeActions;
    TSharedPtr<FShotSequenceActions>    mShotSequenceTypeActions;

    USequencerSettings* mSettingsBoard;
    USequencerSettings* mSettingsShot;
};
