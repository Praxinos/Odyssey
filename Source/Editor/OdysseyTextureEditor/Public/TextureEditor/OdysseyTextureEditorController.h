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
class ODYSSEYTEXTUREEDITOR_API FOdysseyTextureEditorController :
	public FOdysseyPainterEditorController
{
public:
    // Construction / Destruction
    virtual ~FOdysseyTextureEditorController();
    FOdysseyTextureEditorController(FOdysseyTextureEditor* iEditor, TSharedPtr<FOdysseyTextureEditorGUI>& iGUI);

public:
    //Init
	void Init();

protected:
    // Commands building
    virtual void BindCommands(const TSharedRef<FUICommandList>& iToolkitCommands) override;

public:
    //Called when the texture pointer in the TextureWrapper changes
    void OnPreTextureChange(UTexture2D* iNewTexture);
    void OnPostTextureChange(UTexture2D* iOldTexture);

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

	virtual void   OnCreateNewLayer() override;
    virtual void   OnDuplicateCurrentLayer() override;
    virtual void   OnDeleteCurrentLayer() override;

    // virtual void HandleAlphaModeModifierChanged( int32 iValue ) override;

protected:
	virtual FOdysseyTextureEditor* GetEditor() override;
    virtual TSharedPtr<FOdysseyPainterEditorGUI> GetGUI() override;

    /* virtual void OnBrushSelected( UOdysseyBrush* iBrush ) override;
    virtual void OnBrushChanged( UBlueprint* iBrush ) override;
    virtual void OnBrushCompiled( UBlueprint* iBrush ) override; */

private:
	FOdysseyTextureEditor*		mEditor;
	TSharedPtr<FOdysseyTextureEditorGUI>		mGUI;
};

