// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "EditorUndoClient.h"
#include "IDetailsView.h"
#include "Input/Reply.h"
#include "ScopedTransaction.h"
#include "Toolkits/IToolkitHost.h"
#include "UObject/GCObject.h"
#include "Widgets/Colors/SColorPicker.h"
#include "Widgets/SWidget.h"
#include "Widgets/Text/SMultiLineEditableText.h"

#include "Brush/SOdysseyBrushExposedParameters.h"
#include "Brush/SOdysseyBrushSelector.h"
#include "Color/SOdysseyColorSelector.h"
#include "Color/SOdysseyColorSliders.h"
#include "IOdysseyPainterEditorToolkit.h"
#include "LayerStack/SOdysseyLayerStackView.h"
#include "Mesh/SOdysseyMeshSelector.h"
#include "OdysseyLayerStack.h"
#include "OdysseyPaintEngine.h"
#include "OdysseyPainterEditorSettings.h"
#include "OdysseySurface.h"
#include "OdysseyUndoHistory.h"
#include "SOdysseyPaintModifiers.h"
#include "SOdysseyPerformanceOptions.h"
#include "SOdysseyStrokeOptions.h"
#include "UndoHistory/SOdysseyUndoHistory.h"

#include <memory>
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
    virtual ~FOdysseyPainterEditorToolkit();
    FOdysseyPainterEditorToolkit();

public:
    // Initialization
    void InitOdysseyPainterEditor( const EToolkitMode::Type iMode, const TSharedPtr<class IToolkitHost>& iInitToolkitHost, UTexture2D* iTexture );

public:
    // FAssetEditorToolkit interface
    virtual void RegisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) override;
    virtual void UnregisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) override;

public:
    // IToolkit interface
    virtual FText GetBaseToolkitName() const override;
    virtual FName GetToolkitFName() const override;
    virtual FLinearColor GetWorldCentricTabColorScale() const override;
    virtual FString GetWorldCentricTabPrefix() const override;

protected:
    // FEditorUndoClient interface
    virtual void PostUndo( bool iSuccess ) override;
    virtual void PostRedo( bool iSuccess ) override;

protected:
    //FAssetEditorToolkit override
    virtual void SaveAsset_Execute() override;
    virtual void SaveAssetAs_Execute() override;
    virtual bool OnRequestClose() override;

protected:
    // Commands building
    void BindCommands();
    void InitializeExtenders();
    void SetupOdysseyPainterEditorMenu( TSharedPtr< FExtender > ioExtender, FOdysseyPainterEditorToolkit& iOdysseyPainterEditor );
    static void FillImportExportMenu( FMenuBuilder& ioMenuBuilder, FOdysseyPainterEditorToolkit& iOdysseyPainterEditor );
    static void FillAboutMenu( FMenuBuilder& ioMenuBuilder, FOdysseyPainterEditorToolkit& iOdysseyPainterEditor );
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
    //void CreateUndoHistoryTab();

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
    FReply OnUndo();
    FReply OnRedo();
    FReply OnClearUndo();

    // debug purposes
    //FReply OnCheck();
    

    virtual void SetColor( const ::ULIS::CColor& iColor ) override;
    
private:
    void UndoIliad();
    void RedoIliad();

protected:
    // Transaction ( Undo / Redo ) methods overrides
    void BeginTransaction( const FText& iSessionName ) override;
    void MarkTransactionAsDirty() override;
    void EndTransaction() override;

public:
    void SetTextureDirty( bool iTextureDirty ); //Set bIsTextureDirty, prompting, or not, the option to save before closing the editor

private:
    // Brush Handlers
    void HandleBrushParameterChanged();

    // Color Handlers
    void HandleSelectorColorChanged( const ::ULIS::CColor& iColor );
    void HandleSlidersColorChanged( const ::ULIS::CColor& iColor );

    // Modifiers Handlers
    void HandleSizeModifierChanged( int32 iValue );
    void HandleOpacityModifierChanged( int32 iValue );
    void HandleFlowModifierChanged( int32 iValue );
    void HandleBlendingModeModifierChanged( int32 iValue );
    void HandleAlphaModeModifierChanged( int32 iValue );

    // Stroke Options Handlers
    void HandleStrokeStepChanged( int32 iValue );
    void HandleStrokeAdaptativeChanged( bool iValue );
    void HandleStrokePaintOnTickChanged( bool iValue );
    void HandleInterpolationTypeChanged( int32 iValue );
    void HandleSmoothingMethodChanged( int32 iValue );
    void HandleSmoothingStrengthChanged( int32 iValue );
    void HandleSmoothingEnabledChanged( bool iValue );
    void HandleSmoothingRealTimeChanged( bool iValue );
    void HandleSmoothingCatchUpChanged( bool iValue );

    // Performance Handlers
    void HandlePerformanceLiveUpdateChanged( bool iValue );

private:
    // Spawner callbacks
    // Callback for spawning the Brush Selector tab.
    TSharedRef<SDockTab> HandleTabSpawnerSpawnBrushSelector( const FSpawnTabArgs& iArgs );
    // Callback for spawning the Mesh Selector tab.
    TSharedRef<SDockTab> HandleTabSpawnerSpawnMeshSelector( const FSpawnTabArgs& iArgs );
    // Callback for spawning the Brush Exposed Parameters tab.
    TSharedRef<SDockTab> HandleTabSpawnerSpawnBrushExposedParameters( const FSpawnTabArgs& iArgs );
    // Callback for spawning the Viewport tab.
    TSharedRef<SDockTab> HandleTabSpawnerSpawnViewport( const FSpawnTabArgs& iArgs );
    // Callback for spawning the ColorSelector tab.
    TSharedRef<SDockTab> HandleTabSpawnerSpawnColorSelector( const FSpawnTabArgs& iArgs );
    // Callback for spawning the ColorSelector tab.
    TSharedRef<SDockTab> HandleTabSpawnerSpawnColorSliders( const FSpawnTabArgs& iArgs );
    // Callback for spawning the LayerStack tab.
    TSharedRef<SDockTab> HandleTabSpawnerSpawnLayerStack( const FSpawnTabArgs& iArgs );
    // Callback for spawning the BotBar tab.
    TSharedRef<SDockTab> HandleTabSpawnerSpawnBotBar( const FSpawnTabArgs& iArgs );
    // Callback for spawning the BotBar tab.
    TSharedRef<SDockTab> HandleTabSpawnerSpawnTopBar( const FSpawnTabArgs& iArgs );
    // Callback for spawning the Smoothing Options tab.
    TSharedRef<SDockTab> HandleTabSpawnerSpawnStrokeOptions( const FSpawnTabArgs& iArgs );
    // Callback for spawning the Smoothing Options tab.
    TSharedRef<SDockTab> HandleTabSpawnerSpawnPerformanceOptions( const FSpawnTabArgs& iArgs );
    // Callback for spawning the Notes tab.
    TSharedRef<SDockTab> HandleTabSpawnerSpawnNotes( const FSpawnTabArgs& iArgs );
    // Callback for spawning the Undo History tab.
    //TSharedRef<SDockTab> HandleTabSpawnerSpawnUndoHistory(const FSpawnTabArgs& iArgs);
    // Callback for spawning the Tools tab.
    TSharedRef<SDockTab> HandleTabSpawnerSpawnTools( const FSpawnTabArgs& iArgs );

private:
    /** List of open tool panels; used to ensure only one exists at any one time */
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

    /** The current transaction for undo/redo */
    FScopedTransaction* mScopedTransaction;
    FOdysseyUndoHistory mUndoHistory;
    bool                mIsManipulationDirtiedSomething;

    /** Marker for closed state, avoid multiple prompting */
    bool                mIsEditorMarkedAsClosed;

    /** Painting */
    UTexture2D*                 mTexture;
    FOdysseySurface*            mDisplaySurface;
    FOdysseyBlock*              mTextureContentsBackup;
    TextureMipGenSettings       mTextureMipGenBackup;
    TextureCompressionSettings  mTextureCompressionBackup;
    TextureGroup                mTextureGroupBackup;
    FOdysseyPaintEngine         mPaintEngine;        // Owned        // Used by SOdysseyLayerStack
    FOdysseyLayerStack          mLayerStack;        // Owned        // Used by Viewport
    UOdysseyBrush*              mBrush;              // NOT Owned
    UOdysseyBrushAssetBase*     mBrushInstance;     // Owned        // Used by PaintEngine and Brush Exposed Parameters and Brush Preview
    FOdysseyLiveUpdateInfo      mLiveUpdateInfo;

    bool                        mIsTextureDirty;

private:
    /** Tabs IDs */
    static const FName smViewportTabId;
    static const FName smBrushSelectorTabId;
    static const FName smMeshSelectorTabId;
    static const FName smBrushExposedParametersTabId;
    static const FName smColorSelectorTabId;
    static const FName smColorSlidersTabId;
    static const FName smLayerStackTabId;
    static const FName smBotBarTabId;
    static const FName smTopBarTabId;
    static const FName smStrokeOptionsTabId;
    static const FName smNotesTabId;
    //static const FName smUndoHistoryTabId;
    static const FName smPerformanceOptionsTabId;
    static const FName smToolsTabId;
};

