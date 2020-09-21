// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "OdysseyPainterEditorController.h"

#include "IOdysseyLayer.h"
#include "OdysseyTree.h"

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
	void OnLayerStackCurrentLayerChanged(FOdysseyNTree< IOdysseyLayer* >* iCurrentLayer);
	void OnLayerStackDirty();

    void OnExportLayersAsTextures() override;
    void OnImportTexturesAsLayers() override;
	
	virtual void OnPaintEngineStrokeAbort() override;
	virtual void OnPaintEngineStrokeChanged(const TArray<::ul3::FRect>& iChangedTiles) override;
	virtual void OnPaintEngineStrokeWillEnd(const TArray<::ul3::FRect>& iChangedTiles) override;
	virtual void OnPaintEngineStrokeEnd(const TArray<::ul3::FRect>& iChangedTiles) override;

	virtual FReply OnClear() override;
	virtual FReply OnFill() override;
    virtual FReply OnUndoIliad() override;
    virtual FReply OnRedoIliad() override;
	virtual FReply OnClearUndo() override;
    virtual void   OnCreateNewLayer() override;
    virtual void   OnDuplicateCurrentLayer() override;
    virtual void   OnDeleteCurrentLayer() override;

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

