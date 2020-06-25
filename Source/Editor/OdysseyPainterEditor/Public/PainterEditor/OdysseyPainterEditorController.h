// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include <ULIS3>

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
	virtual void OnPaintEngineStrokeChanged(const TArray<::ul3::FRect>& iChangedTiles);
	virtual void OnPaintEngineStrokeWillEnd(const TArray<::ul3::FRect>& iChangedTiles);
	virtual void OnPaintEngineStrokeEnd(const TArray<::ul3::FRect>& iChangedTiles);
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

    //TODO: Get rid of this when we'll have a toolbox widget
    virtual FReply OnClear();
    virtual FReply OnFill();
    virtual FReply OnUndo();
    virtual FReply OnRedo();
    virtual FReply OnClearUndo();

    // Brush Handlers
    virtual void HandleBrushParameterChanged();

    // Color Handlers
	virtual void HandleViewportColorPicked(const ::ul3::FPixelValue& iColor);
    virtual void HandleSelectorColorChanged( const ::ul3::FPixelValue& iColor );
    virtual void HandleSlidersColorChanged( const ::ul3::FPixelValue& iColor );

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

private:
    // Commands building
    void BindCommands(const TSharedRef<FUICommandList>& iToolkitCommands);
    TSharedPtr<FExtender> CreateMenuExtender(const TSharedRef<FUICommandList>& iToolkitCommands);
    static void FillImportExportMenu( FMenuBuilder& ioMenuBuilder, FOdysseyPainterEditorController& iOdysseyPainterEditor );
    static void FillAboutMenu( FMenuBuilder& ioMenuBuilder, FOdysseyPainterEditorController& iOdysseyPainterEditor );

protected:
	virtual void UndoIliad();
	virtual void RedoIliad();

private:
	TArray<TSharedPtr<FExtender>>               mMenuExtenders;
};

