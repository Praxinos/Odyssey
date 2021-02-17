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
class FOdysseyFlipbookEditor;
class FOdysseyFlipbookEditorController;
class FOdysseyFlipbookEditorTimelineTab;

/**
 * Implements an Editor toolkit for Flipbooks.
 */
class ODYSSEYFLIPBOOKEDITOR_API FOdysseyFlipbookEditorGUI :
	public FOdysseyPainterEditorGUI
{
public:
    // Construction / Destruction
    virtual ~FOdysseyFlipbookEditorGUI();
    FOdysseyFlipbookEditorGUI(FOdysseyFlipbookEditor* iEditor);

protected:
    //Init
	virtual void CreateTabs() override;
	virtual void InitTabs() override;

public:
	void InitOdysseyFlipbookEditorGUI(FOdysseyFlipbookEditor* iEditor, TSharedPtr<FOdysseyFlipbookEditorController>& iController);

public:
    // Getters
    TSharedPtr<SOdysseyLayerStackView>& GetLayerStackTab();
	TSharedPtr<SOdysseyTextureDetails>& GetTextureDetailsTab();
	TSharedPtr<FOdysseyFlipbookEditorTimelineTab>& GetTimelineTab();

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
    void CreateLayerStackTab(FOdysseyFlipbookEditor* iEditor, TSharedPtr<FOdysseyFlipbookEditorController>& iController);
    // void CreateTimelineTab(FOdysseyFlipbookEditor* iEditor, TSharedPtr<FOdysseyFlipbookEditorController>& iController);
	void CreateTextureDetailsTab(FOdysseyFlipbookEditor* iEditor, TSharedPtr<FOdysseyFlipbookEditorController>& iController);

private:
	// Spawner callbacks

	// Callback for spawning the LayerStack tab.
	TSharedRef<SDockTab> HandleTabSpawnerSpawnLayerStack(const FSpawnTabArgs& iArgs);

	// Callback for spawning the Timeline tab.
	// TSharedRef<SDockTab> HandleTabSpawnerSpawnTimeline(const FSpawnTabArgs& iArgs);

	// Callback for spawning the TextureDetails tab.
	TSharedRef<SDockTab> HandleTabSpawnerSpawnTextureDetails(const FSpawnTabArgs& iArgs);

private:
	FOdysseyFlipbookEditor*						mEditor;

protected:
    //Tabs
    TSharedPtr<SOdysseyLayerStackView>          mLayerStackTab;
	TSharedPtr<SOdysseyTextureDetails>          mTextureDetailsTab;
	TSharedPtr<FOdysseyFlipbookEditorTimelineTab>    mTimelineTab;

private:
    /** Tabs IDs */
    static const FName smLayerStackTabId;
	static const FName smTextureDetailsTabId;
	// static const FName smTimelineTabId;
};

