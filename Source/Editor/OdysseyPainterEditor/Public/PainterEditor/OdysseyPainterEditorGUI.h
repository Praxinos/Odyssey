// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyPainterEditorData.h"
#include "SOdysseyPerformanceOptions.h"
class SOdysseyBrushExposedParameters;
class SOdysseyBrushSelector;
class SOdysseyColorSelector;
class SOdysseyColorSliders;
class SOdysseyLayerStackView;
class SOdysseyMeshSelector;
class SOdysseyPaintModifiers;
// class SOdysseyPerformanceOptions;
class SOdysseyStrokeOptions;
class SOdysseyUndoHistory;

class SDockableTab;
class STextBlock;
class SOdysseySurfaceViewport;
class FOdysseyPainterEditorController;

/**
 * Implements an Editor toolkit for textures.
 */
class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorGUI :
	public TSharedFromThis<FOdysseyPainterEditorGUI>
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorGUI();
    FOdysseyPainterEditorGUI(const FName iLayoutName);

private:
    void CreateViewportTab(TSharedPtr<FOdysseyPainterEditorData>& iData, TSharedPtr<FOdysseyPainterEditorController>& iController);
    void CreateBrushSelectorTab(TSharedPtr<FOdysseyPainterEditorData>& iData, TSharedPtr<FOdysseyPainterEditorController>& iController);
    void CreateMeshSelectorTab(TSharedPtr<FOdysseyPainterEditorData>& iData, TSharedPtr<FOdysseyPainterEditorController>& iController);
    void CreateBrushExposedParametersTab(TSharedPtr<FOdysseyPainterEditorData>& iData, TSharedPtr<FOdysseyPainterEditorController>& iController);
    void CreateColorSelectorTab(TSharedPtr<FOdysseyPainterEditorData>& iData, TSharedPtr<FOdysseyPainterEditorController>& iController);
    void CreateColorSlidersTab(TSharedPtr<FOdysseyPainterEditorData>& iData, TSharedPtr<FOdysseyPainterEditorController>& iController);
    void CreateTopTab(TSharedPtr<FOdysseyPainterEditorData>& iData, TSharedPtr<FOdysseyPainterEditorController>& iController);
    void CreateToolsTab(TSharedPtr<FOdysseyPainterEditorData>& iData, TSharedPtr<FOdysseyPainterEditorController>& iController);
    void CreateStrokeOptionsTab(TSharedPtr<FOdysseyPainterEditorData>& iData, TSharedPtr<FOdysseyPainterEditorController>& iController);
    void CreatePerformanceOptionsTab(TSharedPtr<FOdysseyPainterEditorData>& iData, TSharedPtr<FOdysseyPainterEditorController>& iController);
    //void CreateUndoHistoryTab(TSharedPtr<FOdysseyPainterEditorData>& iData, TSharedPtr<FOdysseyPainterEditorController>& iController);

protected:
	virtual void RegisterTabSpawners(const TSharedRef< class FTabManager >& iTabManager, TSharedRef<FWorkspaceItem>& iWorkspaceMenuCategoryRef);
	virtual void UnregisterTabSpawners(const TSharedRef< class FTabManager >& iTabManager);

private:
	// Spawner callbacks
	// Callback for spawning the Brush Selector tab.
	TSharedRef<SDockTab> HandleTabSpawnerSpawnBrushSelector(const FSpawnTabArgs& iArgs);
	// Callback for spawning the Mesh Selector tab.
	TSharedRef<SDockTab> HandleTabSpawnerSpawnMeshSelector(const FSpawnTabArgs& iArgs);
	// Callback for spawning the Brush Parameters tab.
	TSharedRef<SDockTab> HandleTabSpawnerSpawnBrushExposedParameters(const FSpawnTabArgs& iArgs);
	// Callback for spawning the Viewport tab.
	TSharedRef<SDockTab> HandleTabSpawnerSpawnViewport(const FSpawnTabArgs& iArgs);
	// Callback for spawning the ColorSelector tab.
	TSharedRef<SDockTab> HandleTabSpawnerSpawnColorSelector(const FSpawnTabArgs& iArgs);
	// Callback for spawning the ColorSelector tab.
	TSharedRef<SDockTab> HandleTabSpawnerSpawnColorSliders(const FSpawnTabArgs& iArgs);
	// Callback for spawning the LayerStack tab.
	TSharedRef<SDockTab> HandleTabSpawnerSpawnLayerStack(const FSpawnTabArgs& iArgs);
	// Callback for spawning the TopBar tab.
	TSharedRef<SDockTab> HandleTabSpawnerSpawnTopBar(const FSpawnTabArgs& iArgs);
	// Callback for spawning the Smoothing Options tab.
	TSharedRef<SDockTab> HandleTabSpawnerSpawnStrokeOptions(const FSpawnTabArgs& iArgs);
	// Callback for spawning the Smoothing Options tab.
	TSharedRef<SDockTab> HandleTabSpawnerSpawnPerformanceOptions(const FSpawnTabArgs& iArgs);
	// Callback for spawning the Notes tab.
	TSharedRef<SDockTab> HandleTabSpawnerSpawnNotes(const FSpawnTabArgs& iArgs);
	// Callback for spawning the Undo History tab.
	//TSharedRef<SDockTab> HandleTabSpawnerSpawnUndoHistory(const FSpawnTabArgs& iArgs);
	// Callback for spawning the Tools tab.
	TSharedRef<SDockTab> HandleTabSpawnerSpawnTools(const FSpawnTabArgs& iArgs);

protected:
	virtual void InitOdysseyPainterEditorGUI(TSharedPtr<FOdysseyPainterEditorData> iData, TSharedPtr<FOdysseyPainterEditorController> iController);

	// Internal widget creation
	virtual void CreateLayout();
	virtual void CreateWidgets(TSharedPtr<FOdysseyPainterEditorData>& iData, TSharedPtr<FOdysseyPainterEditorController>& iController);
	virtual TSharedRef<FTabManager::FSplitter>	CreateMainSection();
	virtual TSharedRef<FTabManager::FSplitter>	CreateLeftSection();
	virtual TSharedRef<FTabManager::FSplitter>	CreateRightSection();
	virtual TSharedRef<FTabManager::FSplitter>	CreateMiddleSection();

public:
	const TSharedRef<FTabManager::FLayout>& GetLayout() const;

    TSharedPtr<SOdysseySurfaceViewport>& GetViewportTab();
    TSharedPtr<SOdysseyBrushSelector>& GetBrushSelectorTab();
    TSharedPtr<SOdysseyMeshSelector>& GetMeshSelectorTab();
    TSharedPtr<SOdysseyBrushExposedParameters>& GetBrushExposedParametersTab();
    TSharedPtr<SOdysseyColorSelector>& GetColorSelectorTab();
    TSharedPtr<SOdysseyColorSliders>& GetColorSlidersTab();
    TSharedPtr<SOdysseyStrokeOptions>& GetStrokeOptionsTab();
    TSharedPtr<SOdysseyPerformanceOptions>& GetPerformanceOptionsTab();
    //TSharedPtr<SOdysseyUndoHistory>& GetUndoHistoryTab();
    TSharedPtr<SOdysseyPaintModifiers>& GetTopTab();
    TSharedPtr<SWidget>& GetToolsTab();

protected:
	void PerformanceOptions(FOdysseyPerformanceOptions* iPerformanceOptions);

private:
    /** List of open tool panels; used to ensure only one exists at any one time */
	TSharedRef<FTabManager::FLayout>            mLayout;

    TSharedPtr<SOdysseySurfaceViewport>         mViewportTab;
    TSharedPtr<SOdysseyBrushSelector>           mBrushSelectorTab;
    TSharedPtr<SOdysseyMeshSelector>            mMeshSelectorTab;
    TSharedPtr<SOdysseyBrushExposedParameters>  mBrushExposedParametersTab;
    TSharedPtr<SOdysseyColorSelector>           mColorSelectorTab;
    TSharedPtr<SOdysseyColorSliders>            mColorSlidersTab;
    TSharedPtr<SOdysseyLayerStackView>          mLayerStackTab;
    TSharedPtr<SOdysseyStrokeOptions>           mStrokeOptionsTab;
    TSharedPtr<SOdysseyPerformanceOptions>      mPerformanceOptionsTab;
    //TSharedPtr<SOdysseyUndoHistory>             mUndoHistoryTab;
    TSharedPtr<SOdysseyPaintModifiers>          mTopTab;
    TSharedPtr<SWidget>                         mToolsTab;

	FOdysseyPerformanceOptions*					mPerformanceOptions; //OWNED but created by a derived class

private:
    /** Tabs IDs */
    static const FName smViewportTabId;
    static const FName smBrushSelectorTabId;
    static const FName smMeshSelectorTabId;
    static const FName smBrushExposedParametersTabId;
    static const FName smColorSelectorTabId;
    static const FName smColorSlidersTabId;
    static const FName smLayerStackTabId;
    static const FName smTopBarTabId;
    static const FName smStrokeOptionsTabId;
    static const FName smNotesTabId;
    static const FName smUndoHistoryTabId;
    static const FName smPerformanceOptionsTabId;
    static const FName smToolsTabId;
};

