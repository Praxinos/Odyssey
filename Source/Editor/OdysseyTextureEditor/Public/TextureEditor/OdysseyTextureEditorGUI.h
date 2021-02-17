// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "Brush/SOdysseyBrushExposedParameters.h"
#include "Brush/SOdysseyBrushSelector.h"
#include "Color/SOdysseyColorSelector.h"
#include "Color/SOdysseyColorSliders.h"
#include "LayerStack/SOdysseyLayerStackView.h"
#include "Mesh/SOdysseyMeshSelector.h"
#include "SOdysseyPaintModifiers.h"
#include "SOdysseyStrokeOptions.h"
#include "SOdysseyTextureDetails.h"
#include "UndoHistory/SOdysseyUndoHistory.h"
#include "OdysseyPainterEditorGUI.h"

class SDockableTab;
class STextBlock;
class SOdysseySurfaceViewport;
class FOdysseyTextureEditor;
class FOdysseyTextureEditorController;

/**
 * Implements an Editor toolkit for textures.
 */
class ODYSSEYTEXTUREEDITOR_API FOdysseyTextureEditorGUI :
	public FOdysseyPainterEditorGUI
{
public:
    // Construction / Destruction
    virtual ~FOdysseyTextureEditorGUI();
    FOdysseyTextureEditorGUI(FOdysseyTextureEditor* iEditor);

protected:
    //Init
	virtual void CreateTabs() override;
	virtual void InitTabs() override;

public:
    //Init
	void InitOdysseyTextureEditorGUI(FOdysseyTextureEditor* iEditor, TSharedPtr<FOdysseyTextureEditorController>& iController);

public:
    // Getters
    TSharedPtr<SOdysseyLayerStackView>& GetLayerStackTab();
	TSharedPtr<SOdysseyTextureDetails>& GetTextureDetailsTab();

public:
	virtual void RegisterTabSpawners(const TSharedRef< class FTabManager >& iTabManager, TSharedRef<FWorkspaceItem>& iWorkspaceMenuCategoryRef) override;
	virtual void UnregisterTabSpawners(const TSharedRef< class FTabManager >& iTabManager) override;

protected:
	// Internal widget creation
	virtual TSharedRef<FTabManager::FSplitter>	CreateMainSection();
	virtual TSharedRef<FTabManager::FSplitter>	CreateLeftSection();
	virtual TSharedRef<FTabManager::FSplitter>	CreateRightSection();
	virtual TSharedRef<FTabManager::FSplitter>	CreateMiddleSection();

private:
    // Internal widget creation
    void CreateLayerStackTab(FOdysseyTextureEditor* iEditor, TSharedPtr<FOdysseyTextureEditorController>& iController);
	void CreateTextureDetailsTab(FOdysseyTextureEditor* iEditor, TSharedPtr<FOdysseyTextureEditorController>& iController);

private:
	// Spawner callbacks
	// Callback for spawning the LayerStack tab.
	TSharedRef<SDockTab> HandleTabSpawnerSpawnLayerStack(const FSpawnTabArgs& iArgs);

	// Callback for spawning the TextureDetails tab.
	TSharedRef<SDockTab> HandleTabSpawnerSpawnTextureDetails(const FSpawnTabArgs& iArgs);

private:
	FOdysseyTextureEditor* mEditor;

private:
    //Tabs
    TSharedPtr<SOdysseyLayerStackView>          mLayerStackTab;
	TSharedPtr<SOdysseyTextureDetails>          mTextureDetailsTab;

private:
    /** Tabs IDs */
    static const FName smLayerStackTabId;
	static const FName smTextureDetailsTabId;
};

