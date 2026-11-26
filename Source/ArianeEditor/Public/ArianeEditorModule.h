// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "Modules/ModuleManager.h"

class AArianePainting3DActor;
class FArianeEditorViewportEdMode;
class ISequencer;

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

    // True while the open Sequencer plays or scrubs: the drawing changes every frame
    bool IsSequencerPlaying() const;
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

    //Level Editor Layout
    void RegisterLevelEditorLayoutExtensions();
    void UnregisterLevelEditorLayoutExtensions();

    void RegisterCustomizations();
    void UnregisterCustomization();
    //Editor Mode
    void RegisterEditorMode();
    void UnregisterEditorMode();

    void RegisterToolbarButton(TSharedPtr<class ILevelEditor> InLevelEditor);
    void UnregisterToolbarButton();
    //void AddToolbarButton( FToolBarBuilder &builder );
    void OnEngineInit();
    void ActivateEdMode( AArianePainting3DActor* Painting3DActor );

    //Shaders
    void RegisterShaders();
    void UnregisterShaders();

    //Property Module Customizations
    void RegisterPropertyModuleCustomizations();
    void UnregisterPropertyModuleCustomizations();

   // Menus
    void RegisterMenus();
    void RegisterActorMenu();
    void ConvertToStaticMesh();

    void RegisterThumbnailRenderers();
    void UnregisterThumbnailRenderers();
    void RegisterPlacementFactories();
    void UnregisterPlacementFactories();
    void RegisterFactoryDelayed();
    void ReorderFactories();


    FArianeEditorViewportEdMode* GetArianeEditorViewportEdMode();

    void OnSequencerCreated( TSharedRef<ISequencer> InSequencer );

private:
    FDelegateHandle mExtendLevelEditorLayout;
    FDelegateHandle ImageTrackHandle;
    FDelegateHandle SequencerCreatedHandle;

    // Last opened Sequencer: the level sequence editor opens one at a time
    TWeakPtr<ISequencer> Sequencer;
/* Gary
    TArray<FOdysseyPainterEditor*> mOpenedEditors;
*/
};
