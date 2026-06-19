// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "UObject/GCObject.h"
#include "UObject/ObjectPtr.h"

class FBoardSequenceActions;
class FExtender;
class FShotSequenceActions;
class FUICommandList;
class USequencerSettings;

class FEposSequenceEditorModule
    : public IModuleInterface, public FGCObject
{
public:
    FEposSequenceEditorModule();

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

    void RegisterMenuExtensions();
    void UnregisterMenuExtensions();

    void RegisterLevelEditorExtensions();
    void UnregisterLevelEditorExtensions();

    void RegisterSettings();
    void UnregisterSettings();

    void RegisterSequenceCustomizations();
    void UnregisterSequenceCustomizations();

    void RegisterPropertyCustomizations();
    void UnregisterPropertyCustomizations();

    void RegisterMovieRenderer();
    void UnregisterMovieRenderer();

private:
    TSharedPtr<FBoardSequenceActions>   mBoardSequenceTypeActions;
    TSharedPtr<FShotSequenceActions>    mShotSequenceTypeActions;

    TSharedPtr<FUICommandList>          mCommandList;
    TSharedPtr<FExtender>               mCinematicsMenuExtender;

    TObjectPtr<USequencerSettings>      mSequencerSettings;

    FDelegateHandle                     mMovieRendererDelegate;
    FDelegateHandle                     mExportSequencerRendererDelegate;
};
