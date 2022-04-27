// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FBoardSequenceActions;
class FShotSequenceActions;
class USequencerSettings;

class FEposSequenceEditorModule
    : public IModuleInterface, public FGCObject
{
public:
    FEposSequenceEditorModule();

public:
    static EAssetTypeCategories::Type GetAssetCategory();

public:
    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    /** FGCObject interface */
    virtual void AddReferencedObjects( FReferenceCollector& Collector ) override;
    virtual FString GetReferencerName() const override;

private:
    void RegisterCommands();
    void UnregisterCommands();

    void RegisterAssetTools();
    void UnregisterAssetTools();

    void RegisterMenuExtensions();
    void UnregisterMenuExtensions();

    void RegisterLevelEditorExtensions();
    void UnregisterLevelEditorExtensions();

    static void OnCreateNewAssetWithSettings( UClass* iClass );

    void RegisterSettings();
    void UnregisterSettings();

    void RegisterSequenceCustomizations();
    void UnregisterSequenceCustomizations();

    void RegisterPropertyCustomizations();
    void UnregisterPropertyCustomizations();

    void RegisterMovieRenderer();
    void UnregisterMovieRenderer();

private:
    static EAssetTypeCategories::Type   mEposAssetCategory;
    TSharedPtr<FBoardSequenceActions>   mBoardSequenceTypeActions;
    TSharedPtr<FShotSequenceActions>    mShotSequenceTypeActions;

    TSharedPtr<FUICommandList>          mCommandList;
    TSharedPtr<FExtender>               mCinematicsMenuExtender;

    USequencerSettings*                 mSequencerSettings;

    FDelegateHandle                     mMovieRendererDelegate;
};
