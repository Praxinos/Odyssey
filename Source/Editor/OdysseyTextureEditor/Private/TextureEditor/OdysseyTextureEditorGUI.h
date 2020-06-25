// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "OdysseyTextureEditorData.h"

#include "Brush/SOdysseyBrushExposedParameters.h"
#include "Brush/SOdysseyBrushSelector.h"
#include "Color/SOdysseyColorSelector.h"
#include "Color/SOdysseyColorSliders.h"
#include "LayerStack/SOdysseyLayerStackView.h"
#include "Mesh/SOdysseyMeshSelector.h"
#include "SOdysseyPaintModifiers.h"
#include "SOdysseyPerformanceOptions.h"
#include "SOdysseyStrokeOptions.h"
#include "UndoHistory/SOdysseyUndoHistory.h"
#include "OdysseyPainterEditorGUI.h"
#include "OdysseyTextureEditorGUI.generated.h"

class SDockableTab;
class STextBlock;
class SOdysseySurfaceViewport;
class FOdysseyTextureEditorController;

/////////////////////////////////////////////////////
// FOdysseyPerformanceOptions
USTRUCT()
struct FOdysseyTexturePerformanceOptions : public FOdysseyPerformanceOptions
{
	GENERATED_BODY()

		FOdysseyTexturePerformanceOptions()
		: LiveUpdate(true)
	{}

	/** Should the texture live-stream update in the 3D viewport ( can impact performances ). */
	UPROPERTY(EditAnywhere, Category = "Performances")
	bool    LiveUpdate;

	DECLARE_DELEGATE_OneParam(FOnLiveUpdateChanged, bool)

	FOnLiveUpdateChanged mOnLiveUpdateChanged;


	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged) override;

	virtual const UStruct* GetScriptStruct() override {
		return FOdysseyTexturePerformanceOptions::StaticStruct();
	}
};

/**
 * Implements an Editor toolkit for textures.
 */
class FOdysseyTextureEditorGUI :
	public FOdysseyPainterEditorGUI
{
public:
    // Construction / Destruction
    virtual ~FOdysseyTextureEditorGUI();
    FOdysseyTextureEditorGUI();

public:
    //Init
	void Init(TSharedPtr<FOdysseyTextureEditorData>& iData, TSharedPtr<FOdysseyTextureEditorController>& iController);

public:
    // Getters
    TSharedPtr<SOdysseyLayerStackView>& GetLayerStackTab();

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
    void CreateLayerStackTab(TSharedPtr<FOdysseyTextureEditorData>& iData, TSharedPtr<FOdysseyTextureEditorController>& iController);

private:
	// Spawner callbacks
	// Callback for spawning the LayerStack tab.
	TSharedRef<SDockTab> HandleTabSpawnerSpawnLayerStack(const FSpawnTabArgs& iArgs);

private:
    //Tabs
    TSharedPtr<SOdysseyLayerStackView>          mLayerStackTab;

private:
    /** Tabs IDs */
    static const FName smLayerStackTabId;
};

