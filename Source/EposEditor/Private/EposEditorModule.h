// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

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
    static EAssetTypeCategories::Type GetAssetCategory();

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

    void RegisterMenuExtensions();
    void UnregisterMenuExtensions();

    static void OnCreateNewAssetWithSettings( UClass* iClass );

    void RegisterSettings();
    void UnregisterSettings();

    void RegisterSequenceCustomizations();
    void UnregisterSequenceCustomizations();

private:
    static EAssetTypeCategories::Type   mEposAssetCategory;
    TSharedPtr<FBoardSequenceActions>   mBoardSequenceTypeActions;
    TSharedPtr<FShotSequenceActions>    mShotSequenceTypeActions;

    TSharedPtr<FUICommandList>          mCommandList;
    TSharedPtr<FExtender>               mCinematicsMenuExtender;

    USequencerSettings*                 mSequencerSettings;
};
