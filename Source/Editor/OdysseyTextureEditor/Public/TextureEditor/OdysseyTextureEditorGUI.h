// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "Brush/SOdysseyBrushExposedParameters.h"
#include "Brush/SOdysseyBrushSelector.h"
#include "Color/SOdysseyColorSelector.h"
#include "Color/SOdysseyColorSliders.h"
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

class FOdysseyTextureEditorLayerStackTab;
class FOdysseyTextureEditorTextureDetailsTab;

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

public:
    //Init
	// void InitOdysseyTextureEditorGUI(FOdysseyTextureEditor* iEditor);

public:
    // Getters
    TSharedPtr<FOdysseyTextureEditorLayerStackTab>& GetLayerStackTab();
	TSharedPtr<FOdysseyTextureEditorTextureDetailsTab>& GetTextureDetailsTab();

public:
	// virtual void RegisterTabSpawners(const TSharedRef< class FTabManager >& iTabManager, TSharedRef<FWorkspaceItem>& iWorkspaceMenuCategoryRef) override;
	// virtual void UnregisterTabSpawners(const TSharedRef< class FTabManager >& iTabManager) override;

protected:
	// Internal widget creation
	// virtual TSharedRef<FTabManager::FSplitter>	CreateMainSection();
	// virtual TSharedRef<FTabManager::FSplitter>	CreateLeftSection();
	virtual TSharedRef<FTabManager::FSplitter>	CreateRightSection();
	// virtual TSharedRef<FTabManager::FSplitter>	CreateMiddleSection();

private:
    // Internal widget creation
    // void CreateLayerStackTab(FOdysseyTextureEditor* iEditor, TSharedPtr<FOdysseyTextureEditorController>& iController);
	// void CreateTextureDetailsTab(FOdysseyTextureEditor* iEditor);

private:
	// Spawner callbacks
	// Callback for spawning the LayerStack tab.
	// TSharedRef<SDockTab> HandleTabSpawnerSpawnLayerStack(const FSpawnTabArgs& iArgs);

	// Callback for spawning the TextureDetails tab.
	// TSharedRef<SDockTab> HandleTabSpawnerSpawnTextureDetails(const FSpawnTabArgs& iArgs);

private:
	FOdysseyTextureEditor* mEditor;

private:
    //Tabs
    TSharedPtr<FOdysseyTextureEditorLayerStackTab>          mLayerStackTab;
	TSharedPtr<FOdysseyTextureEditorTextureDetailsTab>      mTextureDetailsTab;

private:
    /** Tabs IDs */
    // static const FName smLayerStackTabId;
	// static const FName smTextureDetailsTabId;
};

