// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyPainterEditor.h"
#include "Engine/Texture2D.h"
#include "OdysseyTextureEditorGUI.h"
#include "OdysseyTextureAssetUserData.h"
#include "LayerStack/OdysseyTextureLayerStack.h"

/**
 * Implements an Editor for textures.
 */
class ODYSSEYTEXTUREEDITOR_API FOdysseyTextureEditor
    : public FOdysseyPainterEditor
{
public:
    // Construction / Destruction
    virtual ~FOdysseyTextureEditor();
    FOdysseyTextureEditor(); //Non Initialized constructor
    FOdysseyTextureEditor(UTexture2D* iTexture);

public:
    // Getters
	virtual UTexture2D*				    Texture() const;
    virtual UOdysseyTextureLayerStack*	LayerStack() const;
	virtual FOdysseySurfaceTexture2DEditable* DisplaySurface() const override;
    
    virtual void				        SetTexture(UTexture2D* iTexture);
    UOdysseyTextureAssetUserData*       TextureUserData() const;

public:
    // Overrides
    virtual bool OnCloseRequested() override;

public:
    // Overrides
    virtual FOdysseyTextureEditorGUI* GetGUI() override;
    virtual TSharedPtr<FWorkspaceItem> RegisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) override;

protected:
    // Listeners

    //LayerStack
    virtual void OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack);
    virtual void OnLayerRenderImageChanged(UOdysseyTextureLayer* iLayer, const TArray<::ULIS::FRectI>& iRects);
    virtual void OnLayerIsLockedChanged(UOdysseyLayer* iLayer);
    virtual void OnLayerIsActivatedChanged(UOdysseyLayer* iLayer);

    static void OnEditedBlockInvalidated(const ::ULIS::FBlock* iBlock, const ::ULIS::FRectI* iRects, const uint32 iNumRects, void* iInfo);

    //Tool
    virtual void SetSelectedToolDrawingLocked();

    //Paint Engine
    virtual void OnPaintEngineCommit(const TArray<::ULIS::FRectI>& iChangedTiles);
    virtual FOdysseyBlendParameters OnPaintEnginePreUpdate(const FOdysseyBlendParameters& iBlendParameters);
    
private:
    UTexture2D* mTexture;
	TSharedPtr<FOdysseyTextureEditorGUI> mGUI;

    //TODO: should be in the paintTool
    ::ULIS::FBlock* mEditedBlock;
};
