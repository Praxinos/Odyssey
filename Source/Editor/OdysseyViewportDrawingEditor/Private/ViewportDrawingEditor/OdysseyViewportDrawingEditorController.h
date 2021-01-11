// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include <ULIS3>
#include "OdysseyStylusInputSettings.h"

#include "OdysseyViewportDrawingEditorData.h"
#include "OdysseyViewportDrawingEditorGUI.h"

/**
 * Implements an Editor Controller for the paint in viewport.
 */
class ODYSSEYVIEWPORTDRAWINGEDITOR_API FOdysseyViewportDrawingEditorController :
	public TSharedFromThis<FOdysseyViewportDrawingEditorController>,
    public IAssetEditorInstance
{
public:
    // Construction / Destruction
    virtual ~FOdysseyViewportDrawingEditorController();
    FOdysseyViewportDrawingEditorController( TSharedPtr<FOdysseyViewportDrawingEditorData>& iData, TSharedPtr<SOdysseyViewportDrawingEditorGUI>& iGUI );

public:
    // Init
    void Init(/*const TSharedRef<FUICommandList>& iToolkitCommands*/);

    //Clean the layer stack delegates created in the controller
    void ClearLayerStackDelegates();

public:
    TSharedPtr<FOdysseyViewportDrawingEditorData> GetData();
    TSharedPtr<SOdysseyViewportDrawingEditorGUI> GetGUI();

public:
    //IAssetEditorInstance
	virtual FName GetEditorName() const;
	virtual void FocusWindow(UObject* ObjectToFocusOn = nullptr);
	virtual bool CloseWindow();
	virtual bool IsPrimaryEditor() const;
	virtual void InvokeTab(const struct FTabId& TabId);
	virtual FName GetToolbarTabId() const;
	virtual TSharedPtr<class FTabManager> GetAssociatedTabManager();
	virtual double GetLastActivationTime();
	virtual void RemoveEditingAsset(UObject* Asset);

public:
    void EdModeEnter();
    void EdModeExit();

public:
    void OnPaintEnginePreviewBlockTilesChanged(const TArray<::ul3::FRect>& iChangedTiles);
    void OnPaintEngineEditedBlockTilesWillChange(const TArray<::ul3::FRect>& iChangedTiles);
    void OnPaintEngineEditedBlockTilesChanged(const TArray<::ul3::FRect>& iChangedTiles);
    void OnPaintEngineStrokeAbort();

    void OnLayerStackCurrentLayerChanged(TSharedPtr<IOdysseyLayer> iOldValue);
    void OnLayerStackStructureChanged();
    void OnLayerStackImageResultChanged();
    void OnLayerIsLockedChanged(TSharedPtr<IOdysseyLayer> iLayer,bool iOldValue);
    void OnLayerIsVisibleChanged(TSharedPtr<IOdysseyLayer> iLayer, bool iOldValue);
    void OnCurrentLayerIsAlphaLockedChanged(bool iOldValue);

    void OnBrushSelected(UOdysseyBrush* iBrush);
    void OnBrushCompiled(UBlueprint* iBrush);
    void OnEditedTextureChanged(UTexture2D* iTexture);

    // Tools Actions
    FReply OnClear();
    FReply OnFill();
    FReply OnUndoIliad();
    FReply OnRedoIliad();
    FReply OnClearUndo();

    // Brush Handlers
    void HandleBrushParameterChanged();

    // Color Handlers
    void HandlePaintColorChange(eOdysseyEventState::Type iEventState,const ::ul3::FPixelValue& iColor);

    // Modifiers Handlers
    void HandleSizeModifierChanged(int32 iValue);
    void HandleOpacityModifierChanged(int32 iValue);
    void HandleFlowModifierChanged(int32 iValue);
    void HandleBlendingModeModifierChanged(int32 iValue);
    void HandleAlphaModeModifierChanged(int32 iValue);

    // Stroke Options Handlers
    void HandleStrokeStepChanged(int32 iValue);
    void HandleStrokeAdaptativeChanged(bool iValue);
    void HandleStrokePaintOnTickChanged(bool iValue);
    void HandleInterpolationTypeChanged(int32 iValue);
    void HandleSmoothingMethodChanged(int32 iValue);
    void HandleSmoothingStrengthChanged(int32 iValue);
    void HandleSmoothingEnabledChanged(bool iValue);
    void HandleSmoothingRealTimeChanged(bool iValue);
    void HandleSmoothingCatchUpChanged(bool iValue);

private:
    TSharedPtr<FOdysseyViewportDrawingEditorData>		mData;
    TSharedPtr<SOdysseyViewportDrawingEditorGUI>		mGUI;
};