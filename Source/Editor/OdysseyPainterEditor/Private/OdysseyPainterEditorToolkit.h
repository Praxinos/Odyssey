// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/GCObject.h"
#include "Input/Reply.h"
#include "Widgets/SWidget.h"
#include "EditorUndoClient.h"
#include "Toolkits/IToolkitHost.h"
#include "Interfaces/IOdysseyPainterEditorToolkit.h"
#include "IDetailsView.h"
#include "OdysseyPainterEditorSettings.h"
#include "OdysseyUndoHistory.h"
#include "Widgets/Colors/SColorPicker.h"
#include <memory>
#include "ScopedTransaction.h"
#include "OdysseySurface.h"
#include "Mesh/SOdysseyMeshSelector.h"
#include "Brush/SOdysseyBrushSelector.h"
#include "Brush/SOdysseyBrushExposedParameters.h"
#include "Color/SOdysseyColorSelector.h"
#include "Color/SOdysseyColorSliders.h"
#include "UndoHistory/SOdysseyUndoHistory.h"
#include "SOdysseyStrokeOptions.h"
#include "SOdysseyPerformanceOptions.h"
#include "SOdysseyPaintModifiers.h"
#include "OdysseyPaintEngine.h"
#include "OdysseyLayerStack.h"
#include "Widgets/Text/SMultiLineEditableText.h"
#include "LayerStack/SOdysseyLayerStackView.h"
#include <ULIS_CCOLOR>

class UOdysseyBrush;
class UOdysseyBrushAssetBase;
class SDockableTab;
class STextBlock;
class SOdysseySurfaceViewport;
class UFactory;
class UTexture;

/**
 * Implements an Editor toolkit for textures.
 */
class FOdysseyPainterEditorToolkit
    : public IOdysseyPainterEditorToolkit
    , public FEditorUndoClient
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorToolkit( );
    FOdysseyPainterEditorToolkit();

public:
    // Initialization
    void InitOdysseyPainterEditor( const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UTexture2D* iTexture );

public:
    // FAssetEditorToolkit interface
    virtual void RegisterTabSpawners( const TSharedRef<class FTabManager>& TabManager ) override;
    virtual void UnregisterTabSpawners( const TSharedRef<class FTabManager>& TabManager ) override;

public:
    // IToolkit interface
    virtual FText GetBaseToolkitName() const override;
    virtual FName GetToolkitFName() const override;
    virtual FLinearColor GetWorldCentricTabColorScale() const override;
    virtual FString GetWorldCentricTabPrefix() const override;

protected:
    // FEditorUndoClient interface
    virtual void PostUndo( bool bSuccess ) override;
    virtual void PostRedo( bool bSuccess ) override;

protected:
    //FAssetEditorToolkit override
    virtual void SaveAsset_Execute() override;
    virtual bool OnRequestClose() override;

protected:
    // Commands building
    void BindCommands( );
    void InitializeExtenders();
    void SetupOdysseyPainterEditorMenu( TSharedPtr< FExtender > Extender, FOdysseyPainterEditorToolkit& OdysseyPainterEditor);
    static void FillImportExportMenu( FMenuBuilder& MenuBuilder, FOdysseyPainterEditorToolkit& Kismet );
    static void FillAboutMenu( FMenuBuilder& MenuBuilder, FOdysseyPainterEditorToolkit& Kismet );
    void AddMenuEntry(FMenuBarBuilder& MenuBuilder);
    void FillSubmenu(FMenuBuilder& MenuBuilder);
    void OnExportLayersAsTextures();
    void OnImportTexturesAsLayers();
    void OnAboutIliad();
    void OnVisitPraxinosWebsite();
    void OnVisitPraxinosForums();

protected:
    // Internal widget creation
    void CreateViewportTab();
    void CreateBrushSelectorTab();
    void CreateMeshSelectorTab();
    void CreateBrushExposedParametersTab();
    void CreateLayerStackTab();
    void CreateColorSelectorTab();
    void CreateColorSlidersTab();
    void CreateTopTab();
    void CreateToolsTab();
    void CreateStrokeOptionsTab();
    void CreatePerformanceOptionsTab();
    void CreateUndoHistoryTab();


protected:
    // Paint engine driving methods
    void OnBrushSelected( UOdysseyBrush* iBrush );
    void OnBrushChanged( UBlueprint* iBrush );
    void OnBrushCompiled( UBlueprint* iBrush );
    virtual FOdysseyPaintEngine* PaintEngine() override;
    virtual FOdysseyLayerStack* LayerStack() override;

    //Mesh Selector
    void OnMeshSelected( UStaticMesh* iMesh );
    void OnMeshChanged( UBlueprint* iMesh );

    //TODO: Get rid of this when we'll have a toolbox widget
    FReply OnClearCurrentLayer();
    FReply OnFillCurrentLayer();

    virtual void SetColor( const ::ULIS::CColor& iColor ) override;

public:
    // Transaction ( Undo / Redo ) methods overrides
    void BeginTransaction(const FText& SessionName) override;
    void MarkTransactionAsDirty() override;
    void EndTransaction() override;


public: 
    void SetTextureDirty( bool ibTextureDirty ); //Set bIsTextureDirty, prompting, or not, the option to save before closing the editor

private:
    // Brush Handlers
    void  HandleBrushParameterChanged();

    // Color Handlers
    void  HandleSelectorColorChanged( const ::ULIS::CColor& iColor );
    void  HandleSlidersColorChanged( const ::ULIS::CColor& iColor );

    // Modifiers Handlers
    void  HandleSizeModifierChanged( int32 iValue );
    void  HandleOpacityModifierChanged( int32 iValue );
    void  HandleFlowModifierChanged( int32 iValue );
    void  HandleBlendingModeModifierChanged( int32 iValue );
    void  HandleAlphaModeModifierChanged( int32 iValue );

    // Stroke Options Handlers
    void  HandleStrokeStepChanged           (   int32   iValue  );
    void  HandleStrokeAdaptativeChanged     (   bool    iValue  );
    void  HandleStrokePaintOnTickChanged    (   bool    iValue  );
    void  HandleInterpolationTypeChanged    (   int32   iValue  );
    void  HandleSmoothingMethodChanged      (   int32   iValue  );
    void  HandleSmoothingStrengthChanged    (   int32   iValue  );
    void  HandleSmoothingEnabledChanged     (   bool    iValue  );
    void  HandleSmoothingRealTimeChanged    (   bool    iValue  );
    void  HandleSmoothingCatchUpChanged     (   bool    iValue  );

    // Performance Handlers
    void  HandlePerformanceLiveUpdateChanged(   bool    iValue  );

private:
    // Spawner callbacks
    // Callback for spawning the Brush Selector tab.
    TSharedRef<SDockTab> HandleTabSpawnerSpawnBrushSelector( const FSpawnTabArgs& Args );
    // Callback for spawning the Mesh Selector tab.
    TSharedRef<SDockTab> HandleTabSpawnerSpawnMeshSelector( const FSpawnTabArgs& Args );
    // Callback for spawning the Brush Exposed Parameters tab.
    TSharedRef<SDockTab> HandleTabSpawnerSpawnBrushExposedParameters( const FSpawnTabArgs& Args );
    // Callback for spawning the Viewport tab.
    TSharedRef<SDockTab> HandleTabSpawnerSpawnViewport( const FSpawnTabArgs& Args );
    // Callback for spawning the ColorSelector tab.
    TSharedRef<SDockTab> HandleTabSpawnerSpawnColorSelector(const FSpawnTabArgs& Args);
    // Callback for spawning the ColorSelector tab.
    TSharedRef<SDockTab> HandleTabSpawnerSpawnColorSliders(const FSpawnTabArgs& Args);
    // Callback for spawning the LayerStack tab.
    TSharedRef<SDockTab> HandleTabSpawnerSpawnLayerStack(const FSpawnTabArgs& Args);
    // Callback for spawning the BotBar tab.
    TSharedRef<SDockTab> HandleTabSpawnerSpawnBotBar(const FSpawnTabArgs& Args);
    // Callback for spawning the BotBar tab.
    TSharedRef<SDockTab> HandleTabSpawnerSpawnTopBar(const FSpawnTabArgs& Args);
    // Callback for spawning the Smoothing Options tab.
    TSharedRef<SDockTab> HandleTabSpawnerSpawnStrokeOptions(const FSpawnTabArgs& Args);
    // Callback for spawning the Smoothing Options tab.
    TSharedRef<SDockTab> HandleTabSpawnerSpawnPerformanceOptions(const FSpawnTabArgs& Args);
    // Callback for spawning the Notes tab.
    TSharedRef<SDockTab> HandleTabSpawnerSpawnNotes(const FSpawnTabArgs& Args);
    // Callback for spawning the Undo History tab.
    TSharedRef<SDockTab> HandleTabSpawnerSpawnUndoHistory(const FSpawnTabArgs& Args);
    // Callback for spawning the Tools tab.
    TSharedRef<SDockTab> HandleTabSpawnerSpawnTools(const FSpawnTabArgs& Args);

private:
    /** List of open tool panels; used to ensure only one exists at any one time */
    TSharedPtr<SOdysseySurfaceViewport>         ViewportTab;
    TSharedPtr<SOdysseyBrushSelector>           BrushSelectorTab;
    TSharedPtr<SOdysseyMeshSelector>            MeshSelectorTab;
    TSharedPtr<SOdysseyBrushExposedParameters>  BrushExposedParametersTab;
    TSharedPtr<SOdysseyColorSelector>           ColorSelectorTab;
    TSharedPtr<SOdysseyColorSliders>            ColorSlidersTab;
    TSharedPtr<SOdysseyLayerStackView>          LayerStackTab;
    TSharedPtr<SOdysseyStrokeOptions>           StrokeOptionsTab;
    TSharedPtr<SOdysseyPerformanceOptions>      PerformanceOptionsTab;
    TSharedPtr<SOdysseyUndoHistory>             UndoHistoryTab;
    TSharedPtr<SOdysseyPaintModifiers>          TopTab;
    TSharedPtr<SWidget>                         ToolsTab;


    /** The current transaction for undo/redo */
    FScopedTransaction* scopedTransaction;
    FOdysseyUndoHistory UndoHistory;
    bool bManipulationDirtiedSomething;

    /** Marker for closed state, avoid multiple prompting */
    bool bEditorMarkedAsClosed;

    /** Painting */
    UTexture2D*                 texture;
    FOdysseySurface*            displaySurface;
    FOdysseyBlock*              textureContentsBackup;
    TextureMipGenSettings       textureMipGenBackup;
    TextureCompressionSettings  textureCompressionBackup;
    TextureGroup                textureGroupBackup;
    FOdysseyPaintEngine         paintEngine;        // Owned        // Used by SOdysseyLayerStack
    FOdysseyLayerStack          layer_stack;        // Owned        // Used by Viewport
    UOdysseyBrush*              brush;              // NOT Owned
    UOdysseyBrushAssetBase*     brush_instance;     // Owned        // Used by PaintEngine and Brush Exposed Parameters and Brush Preview
    bool                        bLiveUpdateEnabled;
    FOdysseyLiveUpdateInfo      liveUpdateInfo;

    bool                        bIsTextureDirty;

private:
    /** Tabs IDs */
    static const FName ViewportTabId;
    static const FName BrushSelectorTabId;
    static const FName MeshSelectorTabId;
    static const FName BrushExposedParametersTabId;
    static const FName ColorSelectorTabId;
    static const FName ColorSlidersTabId;
    static const FName LayerStackTabId;
    static const FName BotBarTabId;
    static const FName TopBarTabId;
    static const FName StrokeOptionsTabId;
    static const FName NotesTabId;
    static const FName UndoHistoryTabId;
    static const FName PerformanceOptionsTabId;
    static const FName ToolsTabId;
};

