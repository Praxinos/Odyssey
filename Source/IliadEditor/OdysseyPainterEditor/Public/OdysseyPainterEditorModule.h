// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "Modules/ModuleManager.h"

class FOdysseyPainterEditor;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorModule
    : public IModuleInterface
{
public:
    // IModuleInterface interface
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

public:
    virtual void OpenStandaloneEditorForAsset( UObject* iAsset );

    void AddOpenedEditor(FOdysseyPainterEditor* iEdtor);
    void RemoveOpenedEditor(FOdysseyPainterEditor* iEdtor);
    FOdysseyPainterEditor* GetOpenedEditorForAsset(UObject* iObject);

private:
    //Settings
    void RegisterSettings();
    void UnregisterSettings();

    //Commands
    void RegisterCommands();
    void UnregisterCommands();

    //BrushOverrides
    void RegisterBrushOverrides();
    void UnregisterBrushOverrides();

    //Level Editor Layout
    void RegisterLevelEditorLayoutExtensions();
    void UnregisterLevelEditorLayoutExtensions();

    void RegisterDetailCustomizations();
    void UnregisterDetailCustomization();
    //Editor Mode
    void RegisterEditorMode();
    void UnregisterEditorMode();

    //Shaders
    void RegisterShaders();
    void UnregisterShaders();

    //Property Module Customizations
    void RegisterPropertyModuleCustomizations();
    void UnregisterPropertyModuleCustomizations();

    void RegisterThumbnailRenderers();
    void UnregisterThumbnailRenderers();
    void RegisterPlacementFactories();
    void UnregisterPlacementFactories();
    void RegisterFactoryDelayed();
    void ReorderFactories();

public:
    //Tabs State Loading / Saving
    void SetOpenedTabIds(const FName& iEditorName, const TArray<FName>& iTabIds);
    const TArray<FName>& GetOpenedTabIds(const FName& iEditorName, const TArray<FName>& iDefaultOpenedTabIds);

private:
    FString GetOpenedTabIdsProjectPath() const;
    FString GetOpenedTabIdsSavedPath() const;
    void LoadOpenedTabIds(const FName& iEditorName, const TArray<FName>& iDefaultOpenedTabIds);
    void SaveOpenedTabIds(const FName& iEditorName);

private:
    TMap<FName, TArray<FName>> mOpenedTabIds; //Ids of tabs that should be opened when activating a Mode Editor
    FDelegateHandle mExtendLevelEditorLayout;
    TArray<FOdysseyPainterEditor*> mOpenedEditors;
};
