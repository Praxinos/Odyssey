// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyPainterEditorController.h"

#include "SOdysseyFlipbookTimelineViewEvents.h"

#include "IOdysseyLayer.h"
#include "OdysseyTreeShared.h"

class FOdysseyFlipbookEditorToolkit;
class FOdysseyFlipbookEditorData;
class FOdysseyFlipbookEditorGUI;
class FOdysseyImageLayer;

/**
 * Implements an Editor Controller for Flipbooks.
 */
class FOdysseyFlipbookEditorController :
	public FOdysseyPainterEditorController
{
public:
    // Construction / Destruction
    virtual ~FOdysseyFlipbookEditorController();
    FOdysseyFlipbookEditorController(TSharedPtr<FOdysseyFlipbookEditorData>& iData, TSharedPtr<FOdysseyFlipbookEditorGUI>& iGUI);

public:
    //Init
	void Init(const TSharedRef<FUICommandList>& iToolkitCommands);
	void InitLayerStack();

protected:
    // Commands building
    virtual void BindCommands(const TSharedRef<FUICommandList>& iToolkitCommands) override;
	TSharedPtr<FExtender> CreateMenuExtenders(const TSharedRef<FUICommandList>& iToolkitCommands);

public:
	void OnLayerStackCurrentLayerChanged(TSharedPtr<IOdysseyLayer> iOldValue);
	void OnLayerStackStructureChanged();
	void OnLayerStackImageResultChanged();
	void OnLayerIsLockedChanged(TSharedPtr<IOdysseyLayer> iLayer, bool iOldValue);
    void OnLayerIsVisibleChanged(TSharedPtr<IOdysseyLayer> iLayer, bool iOldValue);
	void OnCurrentLayerIsAlphaLockedChanged(bool iOldValue);
	
	void OnTimelineScrubStarted();
	void OnTimelineScrubStopped();
	void OnTimelineCurrentKeyframeChanged(int32 iKeyframe);

	void OnFlipbookChanged();
	//void OnTimelineStructureChanged();

	FOnSpriteCreated& OnSpriteCreated();
	FOnTextureCreated& OnTextureCreated();
	FOnKeyframeRemoved& OnKeyframeRemoved();
	
public:
	virtual void OnPaintEnginePreviewBlockTilesChanged(const TArray<::ul3::FRect>& iChangedTiles) override;
	virtual void OnPaintEngineEditedBlockTilesWillChange(const TArray<::ul3::FRect>& iChangedTiles) override;
	virtual void OnPaintEngineEditedBlockTilesChanged(const TArray<::ul3::FRect>& iChangedTiles) override;
	virtual void OnPaintEngineStrokeAbort() override;

	virtual FReply OnClear() override;
	virtual FReply OnFill() override;
    virtual FReply OnUndoIliad() override;
    virtual FReply OnRedoIliad() override;
	virtual FReply OnClearUndo() override;
    virtual void   OnCreateNewLayer() override;
    virtual void   OnDuplicateCurrentLayer() override;
    virtual void   OnDeleteCurrentLayer() override;

    void           OnExportLayersAsTextures() override;
    void           OnImportTexturesAsLayers() override;

	virtual void HandleAlphaModeModifierChanged( int32 iValue ) override;

protected:
	virtual TSharedPtr<FOdysseyPainterEditorData> GetData() override;
    virtual TSharedPtr<FOdysseyPainterEditorGUI> GetGUI() override;

private:
	void SetTextureAtKeyframeIndex(int32 iKeyframe);
	void OnSpriteTextureChanged(UPaperSprite* iSprite, UTexture2D* iOldTexture);

private:
	TSharedPtr<FOdysseyFlipbookEditorData>		mData;
	TSharedPtr<FOdysseyFlipbookEditorGUI>		mGUI;

	FOnSpriteCreated mOnSpriteCreated;
	FOnTextureCreated mOnTextureCreated;
    FOnKeyframeRemoved mOnKeyframeRemoved;
	
	FDelegateHandle mOnSpriteTextureChangedHandle;
};

