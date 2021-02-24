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
class ODYSSEYFLIPBOOKEDITOR_API FOdysseyFlipbookEditorController :
	public FOdysseyPainterEditorController
{
public:
    // Construction / Destruction
    virtual ~FOdysseyFlipbookEditorController();
    FOdysseyFlipbookEditorController(FOdysseyFlipbookEditor* iEditor, TSharedPtr<FOdysseyFlipbookEditorGUI>& iGUI);

public:
    //Init
	void Init();
	void InitLayerStack();

protected:
    // Commands building
    virtual void BindCommands(const TSharedRef<FUICommandList>& iToolkitCommands) override;
	TSharedPtr<FExtender> CreateMenuExtenders();

public:
	void OnLayerStackCurrentLayerChanged(TSharedPtr<IOdysseyLayer> iOldValue);
	void OnLayerStackStructureChanged();
	void OnLayerStackImageResultChanged(const ::ul3::FRect& iRect);
	// void OnLayerIsLockedChanged(TSharedPtr<IOdysseyLayer> iLayer, bool iOldValue);
    // void OnLayerIsVisibleChanged(TSharedPtr<IOdysseyLayer> iLayer, bool iOldValue);
	void OnCurrentLayerIsAlphaLockedChanged(bool iOldValue);
	
	void OnTimelineScrubStarted();
	void OnTimelineScrubStopped();
	void OnTimelineCurrentKeyframeChanged(int32 iKeyframe);

	void OnFlipbookChanged();
	//void OnTimelineStructureChanged();
	
    void OnSpriteCreated(UPaperSprite* iSprite);
    void OnTextureCreated(UTexture2D* iTexture);
    void OnKeyframeRemoved(FPaperFlipbookKeyFrame& iKeyframe);
	
public:
	//Called when the texture pointer in the TextureWrapper changes
    void OnPreTextureChange(UTexture2D* iNewTexture);
    void OnPostTextureChange(UTexture2D* iOldTexture);

	virtual void OnPaintEngineStrokeWillEnd(const TArray<::ul3::FRect>& iChangedTiles);
	virtual void OnPaintEngineStrokeEnd(const TArray<::ul3::FRect>& iChangedTiles);

	/* virtual void OnPaintEnginePreviewBlockTilesChanged(const TArray<::ul3::FRect>& iChangedTiles) override;
	virtual void OnPaintEngineEditedBlockTilesWillChange(const TArray<::ul3::FRect>& iChangedTiles) override;
	virtual void OnPaintEngineEditedBlockTilesChanged(const TArray<::ul3::FRect>& iChangedTiles) override;
	virtual void OnPaintEngineStrokeAbort() override; */

	// virtual void HandleAlphaModeModifierChanged( int32 iValue ) override;

protected:
	virtual FOdysseyFlipbookEditor* GetEditor() const override;
    virtual TSharedPtr<FOdysseyPainterEditorGUI> GetGUI() override;

	bool PaintEngineIsLocked() const;

private:
	void SetTextureAtKeyframeIndex(int32 iKeyframe);
	void OnSpriteTextureChanged(UPaperSprite* iSprite, UTexture2D* iOldTexture);

private:
	FOdysseyFlipbookEditor*					mEditor;
	TSharedPtr<FOdysseyFlipbookEditorGUI>	mGUI;
	
	FDelegateHandle mOnSpriteTextureChangedHandle;
};

