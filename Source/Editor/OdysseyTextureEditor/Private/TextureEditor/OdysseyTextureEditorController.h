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
    void BindCommands(const TSharedRef<FUICommandList>& iToolkitCommands);
	TSharedPtr<FExtender> CreateMenuExtenders(const TSharedRef<FUICommandList>& iToolkitCommands);
    static void FillImportExportMenu( FMenuBuilder& ioMenuBuilder, FOdysseyTextureEditorController& iOdysseyTextureEditor );

public:
	void OnLayerStackCurrentLayerChanged(FOdysseyNTree< IOdysseyLayer* >* iCurrentLayer);
	void OnLayerStackDirty();

    void OnExportLayersAsTextures();
    void OnImportTexturesAsLayers();

    // Performance Handlers
    void HandlePerformanceLiveUpdateChanged( bool iValue );

	virtual void OnPaintEngineStrokeChanged(const TArray<::ul3::FRect>& iChangedTiles) override;
	virtual void OnPaintEngineStrokeWillEnd(const TArray<::ul3::FRect>& iChangedTiles) override;
	virtual void OnPaintEngineStrokeEnd(const TArray<::ul3::FRect>& iChangedTiles) override;
	virtual void OnPaintEngineStrokeAbort() override;

    virtual void UndoIliad() override;
    virtual void RedoIliad() override;

	virtual FReply OnClear() override;
	virtual FReply OnFill() override;
	virtual FReply OnClearUndo() override;

protected:
	virtual TSharedPtr<FOdysseyPainterEditorData> GetData() override;
    virtual TSharedPtr<FOdysseyPainterEditorGUI> GetGUI() override;

private:
	TSharedPtr<FOdysseyTextureEditorData>		mData;
	TSharedPtr<FOdysseyTextureEditorGUI>		mGUI;
};

