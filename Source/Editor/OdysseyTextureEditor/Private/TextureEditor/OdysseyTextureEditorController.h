// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyPainterEditorController.h"

#include "IOdysseyLayer.h"
#include "OdysseyTreeShared.h"

class FOdysseyTextureEditorToolkit;
class FOdysseyTextureEditorData;
class FOdysseyTextureEditorGUI;
class FOdysseyImageLayer;

/**
 * Implements an Editor Controller for textures.
 */
class FOdysseyTextureEditorController :
	public FOdysseyPainterEditorController
{
public:
    // Construction / Destruction
    virtual ~FOdysseyTextureEditorController();
    FOdysseyTextureEditorController(TSharedPtr<FOdysseyTextureEditorData>& iData, TSharedPtr<FOdysseyTextureEditorGUI>& iGUI);

public:
    //Init
	void Init(const TSharedRef<FUICommandList>& iToolkitCommands);

protected:
    // Commands building
    virtual void BindCommands(const TSharedRef<FUICommandList>& iToolkitCommands) override;

public:
	void OnLayerStackCurrentLayerChanged(TSharedPtr<IOdysseyLayer> iOldValue);
	void OnLayerStackStructureChanged();
	void OnLayerStackImageResultChanged();
    void OnLayerIsLockedChanged(TSharedPtr<IOdysseyLayer> iLayer, bool iOldValue);
    void OnLayerIsVisibleChanged(TSharedPtr<IOdysseyLayer> iLayer, bool iOldValue);
	void OnCurrentLayerIsAlphaLockedChanged(bool iOldValue);

    void OnExportLayersAsTextures() override;
    void OnImportTexturesAsLayers() override;
	
	virtual void OnPaintEngineStrokeAbort() override;
    virtual void OnPaintEnginePreviewBlockTilesChanged(const TArray<::ul3::FRect>& iChangedTiles) override;
    virtual void OnPaintEngineEditedBlockTilesWillChange(const TArray<::ul3::FRect>& iChangedTiles) override;
    virtual void OnPaintEngineEditedBlockTilesChanged(const TArray<::ul3::FRect>& iChangedTiles) override;

	virtual FReply OnClear() override;
	virtual FReply OnFill() override;
    virtual FReply OnUndoIliad() override;
    virtual FReply OnRedoIliad() override;
	virtual FReply OnClearUndo() override;
    virtual void   OnCreateNewLayer() override;
    virtual void   OnDuplicateCurrentLayer() override;
    virtual void   OnDeleteCurrentLayer() override;

    virtual void HandleAlphaModeModifierChanged( int32 iValue ) override;

protected:
	virtual TSharedPtr<FOdysseyPainterEditorData> GetData() override;
    virtual TSharedPtr<FOdysseyPainterEditorGUI> GetGUI() override;

    virtual void OnBrushSelected( UOdysseyBrush* iBrush ) override;
    virtual void OnBrushChanged( UBlueprint* iBrush ) override;
    virtual void OnBrushCompiled( UBlueprint* iBrush ) override;

private:
	TSharedPtr<FOdysseyTextureEditorData>		mData;
	TSharedPtr<FOdysseyTextureEditorGUI>		mGUI;
};

