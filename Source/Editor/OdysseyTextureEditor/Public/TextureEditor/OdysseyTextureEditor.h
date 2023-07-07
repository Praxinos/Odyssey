// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyPainterEditor.h"
#include "Engine/Texture2D.h"
#include "OdysseyTextureEditorGUI.h"
#include "OdysseyTextureLayerStackUserData.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "Tools/RasterDrawingTool/OdysseyTextureEditorRasterDrawingTool.h"
#include "Tools/VectorPrimitiveDrawingTool/OdysseyTextureEditorVectorPrimitiveDrawingTool.h"
#include "Tools/VectorPathDrawingTool/OdysseyTextureEditorVectorPathDrawingTool.h"
#include "Tools/VectorPathEditTool/OdysseyTextureEditorVectorPathEditTool.h"
#include "Tools/VectorPathCutTool/OdysseyTextureEditorVectorPathCutTool.h"
#include "Tools/VectorPickTool/OdysseyTextureEditorVectorPickTool.h"
#include "Tools/VectorObjectMoveTool/OdysseyTextureEditorVectorObjectMoveTool.h"
#include "Tools/VectorObjectRotateTool/OdysseyTextureEditorVectorObjectRotateTool.h"
#include "Tools/VectorObjectScaleTool/OdysseyTextureEditorVectorObjectScaleTool.h"
#include "Tools/VectorSceneScaleTool/OdysseyTextureEditorVectorSceneScaleTool.h"
#include "Tools/VectorScenePanTool/OdysseyTextureEditorVectorScenePanTool.h"
#include "Tools/VectorEraserTool/OdysseyTextureEditorVectorEraserTool.h"
#include "Tools/VectorPathPushTool/OdysseyTextureEditorVectorPathPushTool.h"
#include "Tools/VectorPathWidthTool/OdysseyTextureEditorVectorPathWidthTool.h"
#include "Tools/VectorPathSmoothTool/OdysseyTextureEditorVectorPathSmoothTool.h"
#include "Tools/VectorPathStitchTool/OdysseyTextureEditorVectorPathStitchTool.h"
#include "Tools/PaintBucketTool/OdysseyTextureEditorPaintBucketTool.h"
#include "Tools/ColorPickerTool/OdysseyTextureEditorColorPickerTool.h"
#include "Tools/VectorGridTool/OdysseyTextureEditorVectorGridTool.h"
#include "Tools/VectorTransformTool/OdysseyTextureEditorVectorTransformTool.h"
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
    virtual UOdysseyTextureEditorVectorPrimitiveDrawingTool* GetVectorPrimitiveDrawingTool() const override;
    virtual UOdysseyTextureEditorVectorPathDrawingTool* GetVectorPathDrawingTool() const override;
    virtual UOdysseyTextureEditorVectorPathEditTool* GetVectorPathEditTool() const override;
    virtual UOdysseyTextureEditorVectorPathCutTool* GetVectorPathCutTool() const override;
    virtual UOdysseyTextureEditorVectorPickTool* GetVectorPickTool() const override;
    virtual UOdysseyTextureEditorVectorObjectMoveTool* GetVectorObjectMoveTool() const override;
    virtual UOdysseyTextureEditorVectorObjectRotateTool* GetVectorObjectRotateTool() const override;
    virtual UOdysseyTextureEditorVectorObjectScaleTool* GetVectorObjectScaleTool() const override;
    virtual UOdysseyTextureEditorVectorSceneScaleTool* GetVectorSceneScaleTool() const override;
    virtual UOdysseyTextureEditorVectorScenePanTool* GetVectorScenePanTool() const override;
    virtual UOdysseyTextureEditorVectorEraserTool* GetVectorEraserTool() const override;
    virtual UOdysseyTextureEditorVectorPathPushTool* GetVectorPathPushTool() const override;
    virtual UOdysseyTextureEditorVectorPathWidthTool* GetVectorPathWidthTool() const override;
    virtual UOdysseyTextureEditorVectorPathSmoothTool* GetVectorPathSmoothTool() const override;
    virtual UOdysseyTextureEditorVectorPathStitchTool* GetVectorPathStitchTool() const override;
    virtual UOdysseyTextureEditorPaintBucketTool* GetPaintBucketTool() const override;
    virtual UOdysseyTextureEditorColorPickerTool* GetColorPickerTool() const override;
    virtual UOdysseyTextureEditorVectorGridTool* GetVectorGridTool() const override;
    virtual UOdysseyTextureEditorVectorTransformTool* GetVectorTransformTool() const override;

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

public:
    //Common Actions
    virtual void Clear() override;

protected:
    // FGCObject implementation
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

private:
    UTexture2D* mTexture;
	TSharedPtr<FOdysseyTextureEditorGUI> mGUI;
    
    UOdysseyTextureEditorRasterDrawingTool* mRasterDrawingTool;
    UOdysseyTextureEditorVectorPrimitiveDrawingTool* mVectorPrimitiveDrawingTool;
    UOdysseyTextureEditorVectorPathDrawingTool* mVectorPathDrawingTool;
    UOdysseyTextureEditorVectorPathEditTool* mVectorPathEditTool;
    UOdysseyTextureEditorVectorPathCutTool* mVectorPathCutTool;
    UOdysseyTextureEditorVectorPickTool* mVectorPickTool;
    UOdysseyTextureEditorVectorObjectMoveTool* mVectorObjectMoveTool;
    UOdysseyTextureEditorVectorObjectRotateTool* mVectorObjectRotateTool;
    UOdysseyTextureEditorVectorObjectScaleTool* mVectorObjectScaleTool;
    UOdysseyTextureEditorVectorSceneScaleTool* mVectorSceneScaleTool;
    UOdysseyTextureEditorVectorScenePanTool* mVectorScenePanTool;
    UOdysseyTextureEditorVectorEraserTool* mVectorEraserTool;
    UOdysseyTextureEditorVectorPathPushTool* mVectorPathPushTool;
    UOdysseyTextureEditorVectorPathWidthTool* mVectorPathWidthTool;
    UOdysseyTextureEditorVectorPathSmoothTool* mVectorPathSmoothTool;
    UOdysseyTextureEditorVectorPathStitchTool* mVectorPathStitchTool;
    UOdysseyTextureEditorPaintBucketTool* mPaintBucketTool;
    UOdysseyTextureEditorColorPickerTool* mColorPickerTool;
    UOdysseyTextureEditorVectorGridTool* mVectorGridTool;
    UOdysseyTextureEditorVectorTransformTool* mVectorTransformTool;

    TSharedPtr<IOdysseyHandle> mLayerStackPreloadHandle;
};
