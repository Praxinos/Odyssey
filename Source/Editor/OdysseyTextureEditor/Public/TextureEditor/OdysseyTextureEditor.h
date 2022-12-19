// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyPainterEditor.h"
#include "Engine/Texture2D.h"
#include "OdysseyTextureEditorGUI.h"
#include "OdysseyTextureLayerStackUserData.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "Tools/RasterDrawingTool/OdysseyTextureEditorRasterDrawingTool.h"
#include "Tools/VectorDrawingTool/OdysseyTextureEditorVectorDrawingTool.h"
#include "Tools/VectorEditPathTool/OdysseyTextureEditorVectorEditPathTool.h"
#include "Tools/PaintBucketTool/OdysseyTextureEditorPaintBucketTool.h"
#include "Misc/OdysseyHandle.h"

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

public:
    virtual void InitData(UObject* iEditedObject) override;
    virtual void InitTools() override;
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;
    virtual void ExtendMenu(FToolMenuOwner iOwner, FName iMenuName) override;

public:
    // Getters
	virtual UTexture2D*				    Texture() const;
    virtual UOdysseyTextureLayerStack*	LayerStack() const;
	virtual UTexture*                   DisplayTexture() const override;
    virtual TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> GetDisplayBlock() override;
    
    virtual void				        SetTexture(UTexture2D* iTexture);
    UOdysseyTextureLayerStackUserData*       TextureUserData() const;

    virtual UOdysseyTextureEditorRasterDrawingTool* GetRasterDrawingTool() const override;
    virtual UOdysseyTextureEditorVectorDrawingTool* GetVectorDrawingTool() const override;
    virtual UOdysseyTextureEditorVectorEditPathTool* GetVectorEditPathTool() const override;
    virtual UOdysseyTextureEditorPaintBucketTool* GetPaintBucketTool() const override;

public:
    // Overrides
    virtual bool OnCloseRequested() override;

public:
    // Overrides
    virtual FOdysseyTextureEditorGUI* GetGUI() override;
    virtual TSharedPtr<FWorkspaceItem> RegisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) override;

public:
    // Events
    void OnCurrentLayerChanged(class UOdysseyLayerStack* iLayerStack);

protected:
    // FGCObject implementation
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

private:
    UTexture2D* mTexture;
	TSharedPtr<FOdysseyTextureEditorGUI> mGUI;
    
    UOdysseyTextureEditorRasterDrawingTool* mRasterDrawingTool;
    UOdysseyTextureEditorVectorDrawingTool* mVectorDrawingTool;
    UOdysseyTextureEditorVectorEditPathTool* mVectorEditPathTool;
    UOdysseyTextureEditorPaintBucketTool* mPaintBucketTool;

    TSharedPtr<IOdysseyHandle> mLayerStackPreloadHandle;
};
