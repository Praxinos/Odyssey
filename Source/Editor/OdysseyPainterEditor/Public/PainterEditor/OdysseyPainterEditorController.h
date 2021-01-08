// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include <ULIS3>
#include "OdysseyStylusInputSettings.h"
#include "OdysseyEventState.h"

class FOdysseyPainterEditorToolkit;
class FOdysseyPainterEditorData;
class FOdysseyPainterEditorGUI;
class UOdysseyBrush;
class UBlueprint;

/**
 * Implements an Editor Controller for textures.
 */
class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorController :
	public TSharedFromThis<FOdysseyPainterEditorController>
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorController();
    FOdysseyPainterEditorController();

public:
    // Init
	void InitOdysseyPainterEditorController(const TSharedRef<FUICommandList>& iToolkitCommands);

public:
    // Getters
	const TArray<TSharedPtr<FExtender>>& GetMenuExtenders() const;

protected:
	// Permissive Getters
	TArray<TSharedPtr<FExtender>>& GetMenuExtenders();

public:
    // Virtual Methods
    virtual TSharedPtr<FOdysseyPainterEditorData> GetData() = 0;
    virtual TSharedPtr<FOdysseyPainterEditorGUI> GetGUI() = 0;

public:
	virtual void OnPaintEnginePreviewBlockTilesChanged(const TArray<::ul3::FRect>& iChangedTiles);
	virtual void OnPaintEngineEditedBlockTilesWillChange(const TArray<::ul3::FRect>& iChangedTiles);
	virtual void OnPaintEngineEditedBlockTilesChanged(const TArray<::ul3::FRect>& iChangedTiles);
	virtual void OnPaintEngineStrokeAbort();

    virtual void OnAboutIliad();
    virtual void OnVisitPraxinosWebsite();
    virtual void OnVisitPraxinosForums();

    // Paint engine driving methods
    virtual void OnBrushSelected( UOdysseyBrush* iBrush );
    virtual void OnBrushChanged( UBlueprint* iBrush );
    virtual void OnBrushCompiled( UBlueprint* iBrush );

    //Mesh Selector
    virtual void OnMeshSelected( UStaticMesh* iMesh );
    virtual void OnMeshChanged( UBlueprint* iMesh );

    //Shortcut Actions
    virtual FReply OnClear();
    virtual FReply OnFill();
    virtual FReply OnUndoIliad();
    virtual FReply OnRedoIliad();
    virtual FReply OnClearUndo();
    virtual void   OnCreateNewLayer();
    virtual void   OnDuplicateCurrentLayer();
    virtual void   OnDeleteCurrentLayer();
    virtual void   OnExportLayersAsTextures() = 0;
    virtual void   OnImportTexturesAsLayers() = 0;
    void           OnResetViewportPosition();
    void           OnResetViewportRotation();
    void           OnRotateViewportLeft();
    void           OnRotateViewportRight();
    void           OnSetZoom(double iZoomValue);
    void           OnSetZoomFitScreen();
    void           OnZoomIn();
    void           OnZoomOut();
    void           OnRefreshBrush();
    void           OnAddBrushSize(int32 iValue);
    void           OnSetAlphaMode(::ul3::eAlphaMode iAlphaMode);
    void           OnSwitchTabletAPI();

    // Brush Handlers
    virtual void HandleBrushParameterChanged();

    // Color Handlers
	virtual void HandleViewportColorPicked(eOdysseyEventState::Type iEventState, const FVector2D& iPositionInTexture);
    // virtual void HandleSelectorColorChanged( const ::ul3::FPixelValue& iColor );
    virtual void HandlePaintColorChange( eOdysseyEventState::Type iEventState, const ::ul3::FPixelValue& iColor );

    // Modifiers Handlers
    virtual void HandleSizeModifierChanged( int32 iValue );
    virtual void HandleOpacityModifierChanged( int32 iValue );
    virtual void HandleFlowModifierChanged( int32 iValue );
    virtual void HandleBlendingModeModifierChanged( int32 iValue );
    virtual void HandleAlphaModeModifierChanged( int32 iValue );

    // Stroke Options Handlers
    virtual void HandleStrokeStepChanged( int32 iValue );
    virtual void HandleStrokeAdaptativeChanged( bool iValue );
    virtual void HandleStrokePaintOnTickChanged( bool iValue );
    virtual void HandleInterpolationTypeChanged( int32 iValue );
    virtual void HandleSmoothingMethodChanged( int32 iValue );
    virtual void HandleSmoothingStrengthChanged( int32 iValue );
    virtual void HandleSmoothingEnabledChanged( bool iValue );
    virtual void HandleSmoothingRealTimeChanged( bool iValue );
    virtual void HandleSmoothingCatchUpChanged( bool iValue );

    // Performance Handlers
    virtual void HandlePerformanceDrawBrushPreviewChanged( bool iValue );

    // Viewport (zoom/rotation/pan) Handlers
    virtual void HandleViewportParameterChanged();

protected:
    // Commands building
    virtual void BindCommands(const TSharedRef<FUICommandList>& iToolkitCommands);

private:
    TSharedPtr<FExtender> CreateMenuExtender(const TSharedRef<FUICommandList>& iToolkitCommands);
    static void FillImportExportMenu( FMenuBuilder& ioMenuBuilder, FOdysseyPainterEditorController& iOdysseyPainterEditor );
    static void FillAboutMenu( FMenuBuilder& ioMenuBuilder, FOdysseyPainterEditorController& iOdysseyPainterEditor );

    TSharedRef<SWidget> GenerateTabletAPIComboBoxItem( TSharedPtr<EOdysseyStylusInputDriver> iItem );
    void ChangeSelectionTabletAPIComboBoxItem( TSharedPtr<EOdysseyStylusInputDriver> iNewSelection, ESelectInfo::Type iSelectInfo );
    FText GetComboBoxTabletAPISelectedAsText() const;

private:
    TSharedPtr<EOdysseyStylusInputDriver>       mComboBoxTabletAPISelected;
	TArray<TSharedPtr<FExtender>>               mMenuExtenders;
};

