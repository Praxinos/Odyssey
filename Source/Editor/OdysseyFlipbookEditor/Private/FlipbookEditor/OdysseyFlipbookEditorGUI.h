// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "OdysseyFlipbookEditorData.h"

#include "Brush/SOdysseyBrushExposedParameters.h"
#include "Brush/SOdysseyBrushSelector.h"
#include "Color/SOdysseyColorSelector.h"
#include "Color/SOdysseyColorSliders.h"
#include "LayerStack/SOdysseyLayerStackView.h"
#include "SOdysseyFlipbookTimelineView.h"
#include "Mesh/SOdysseyMeshSelector.h"
#include "SOdysseyPaintModifiers.h"
#include "SOdysseyPerformanceOptions.h"
#include "SOdysseyStrokeOptions.h"
#include "UndoHistory/SOdysseyUndoHistory.h"
#include "OdysseyPainterEditorGUI.h"
#include "OdysseyFlipbookEditorGUI.generated.h"

class SDockableTab;
class STextBlock;
class SOdysseySurfaceViewport;
class FOdysseyFlipbookEditorController;

/////////////////////////////////////////////////////
// FOdysseyPerformanceOptions
USTRUCT()
struct FOdysseyFlipbookPerformanceOptions : public FOdysseyPerformanceOptions
{
	GENERATED_BODY()

		FOdysseyFlipbookPerformanceOptions()
	{}

	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged) override;

	virtual const UStruct* GetScriptStruct() override {
		return FOdysseyFlipbookPerformanceOptions::StaticStruct();
	}
};

/**
 * Implements an Editor toolkit for Flipbooks.
 */
class FOdysseyFlipbookEditorGUI :
	public FOdysseyPainterEditorGUI
{
public:
    // Construction / Destruction
    virtual ~FOdysseyFlipbookEditorGUI();
    FOdysseyFlipbookEditorGUI();

public:
    //Init
	void Init(TSharedPtr<FOdysseyFlipbookEditorData>& iData, TSharedPtr<FOdysseyFlipbookEditorController>& iController);

public:
    // Getters
    TSharedPtr<SOdysseyLayerStackView>& GetLayerStackTab();
	TSharedPtr<SOdysseyFlipbookTimelineView>& GetTimelineTab();

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
    void CreateLayerStackTab(TSharedPtr<FOdysseyFlipbookEditorData>& iData, TSharedPtr<FOdysseyFlipbookEditorController>& iController);
    void CreateTimelineTab(TSharedPtr<FOdysseyFlipbookEditorData>& iData, TSharedPtr<FOdysseyFlipbookEditorController>& iController);

private:
	// Spawner callbacks

	// Callback for spawning the LayerStack tab.
	TSharedRef<SDockTab> HandleTabSpawnerSpawnLayerStack(const FSpawnTabArgs& iArgs);

	// Callback for spawning the Timeline tab.
	TSharedRef<SDockTab> HandleTabSpawnerSpawnTimeline(const FSpawnTabArgs& iArgs);

private:
    //Tabs
    TSharedPtr<SOdysseyLayerStackView>          mLayerStackTab;
	TSharedPtr<SOdysseyFlipbookTimelineView>    mTimelineTab;

private:
    /** Tabs IDs */
    static const FName smLayerStackTabId;
	static const FName smTimelineTabId;
};

