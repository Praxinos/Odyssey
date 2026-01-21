// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "Modules/ModuleManager.h"

/* Gary
class FOdysseyPainterEditor;
*/

class ARIANEEDITOR_API FArianeEditorModule
    : public IModuleInterface
{
public:
    // IModuleInterface interface
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

public:
    virtual void OpenStandaloneEditorForAsset( UObject* iAsset );
/* Gary
    void AddOpenedEditor(FOdysseyPainterEditor* iEdtor);
    void RemoveOpenedEditor(FOdysseyPainterEditor* iEdtor);
    FOdysseyPainterEditor* GetOpenedEditorForAsset(UObject* iObject);
*/
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

private:
    FDelegateHandle mExtendLevelEditorLayout;
/* Gary
    TArray<FOdysseyPainterEditor*> mOpenedEditors;
*/
};
