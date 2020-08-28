// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "OdysseyPainterEditorController.h"

#include "SOdysseyFlipbookTimelineViewEvents.h"

#include "IOdysseyLayer.h"
#include "OdysseyTree.h"

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
	void OnLayerStackCurrentLayerChanged(FOdysseyNTree< IOdysseyLayer* >* iCurrentLayer);
	
	void OnTimelineScrubStarted();
	void OnTimelineScrubStopped();
	void OnTimelineCurrentKeyframeChanged(int32 iKeyframe);

	void OnFlipbookChanged();
	//void OnTimelineStructureChanged();

	FOnSpriteCreated& OnSpriteCreated();
	FOnTextureCreated& OnTextureCreated();
	FOnKeyframeRemoved& OnKeyframeRemoved();
	
public:
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

