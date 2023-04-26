// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/PaintBucketTool/OdysseyTextureEditorPaintBucketTool.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorPaintBucketTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyTextureEditorPaintBucketTool::~UOdysseyTextureEditorPaintBucketTool()
{
}

UOdysseyTextureEditorPaintBucketTool::UOdysseyTextureEditorPaintBucketTool()
{
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyTextureEditorPaintBucketTool::Activate()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    UOdysseyTextureLayerStack::OnCurrentLayerChanged().AddUObject( this, &UOdysseyTextureEditorPaintBucketTool::OnCurrentLayerChanged );

    Load();

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        UOdysseyPainterEditorPaintBucketTool::Activate( vectorEngine, vectorScene );

        currentVectorLayer->RenderImageChanged(false);
    }
}

void
UOdysseyTextureEditorPaintBucketTool::Load()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageRaster* currentLayerRaster = Cast<UOdysseyTextureLayerImageRaster>(layerStack->CurrentLayer.Get());

	//Define the new active tool based on the layer type
	if (!currentLayerRaster)
		return;

	TSharedPtr<FOdysseyRasterBlock> rasterBlock = currentLayerRaster->GetRasterBlock();
	mPaintEngine.RasterBlock(rasterBlock);

	//Should be managed by the tool
	mPaintEngine.OnPreUpdateDelegate().BindUObject(this, &UOdysseyTextureEditorPaintBucketTool::OnPaintEnginePreUpdate);
}

void
UOdysseyTextureEditorPaintBucketTool::Inactivate()
{
	UOdysseyTextureLayerStack::OnCurrentLayerChanged().RemoveAll(this);
    Super::Inactivate();
}

void
UOdysseyTextureEditorPaintBucketTool::Unload()
{
	mPaintEngine.OnPreUpdateDelegate().Unbind();

    //Cleanup
	mPaintEngine.RasterBlock(nullptr);
}

bool
UOdysseyTextureEditorPaintBucketTool::IsActivable() const
{
    if (!Super::IsActivable())
        return false; 

    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    if (!layerStack)
        return false;

    UOdysseyLayer* currentLayer = layerStack->CurrentLayer.Get();
    if (!currentLayer)
        return false;

    return      currentLayer->GetClass() == UOdysseyTextureLayerImageRaster::StaticClass()
            ||  currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass();
}

void
UOdysseyTextureEditorPaintBucketTool::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
{
	//ensure iLayerstack is the one the tool is working on
	UOdysseyTextureLayerStack* layerstack = GetEditorAs<FOdysseyTextureEditor>()->LayerStack();
	if ( !iLayerStack || !layerstack || layerstack != iLayerStack )
		return;

	//If not activable => Inactivate
	if (!IsActivable())
	{
		Inactivate(); //close the tool
		return;
	}

	//Reload the tool to edit the new layer
	Unload();
	Load();
}

bool
UOdysseyTextureEditorPaintBucketTool::OnMouseDown( const FOdysseyPoint& iPointInTexture
                                                  , const FKey& iKey )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());
    UOdysseyTextureLayerImageRaster* currentRasterLayer = Cast<UOdysseyTextureLayerImageRaster>(layerStack->CurrentLayer.Get());
    bool ret = false;

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("PaintBucketTool", "Bucket Tool"));

    if( currentRasterLayer )
    {
        ret = UOdysseyPainterEditorPaintBucketTool::OnMouseDown( currentRasterLayer->GetRasterBlock()->GetBlock(), iPointInTexture, iKey );
    }

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();
        FOdysseyVectorUndo* undo = nullptr;

        ret = UOdysseyPainterEditorPaintBucketTool::OnMouseDown( vectorEngine, vectorScene, &undo, iPointInTexture, iKey );

        if( undo )
        {
            GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
            // All the delegates for undos are added here for easier maintainability
            undo->mRefreshDelegate.AddUObject( currentVectorLayer, &UOdysseyTextureLayerImageVector::OnRefresh );
        }

        currentVectorLayer->RenderImageChanged(false);
    }

    GEditor->EndTransaction();

    return ret;
}

void
UOdysseyTextureEditorPaintBucketTool::OnMouseHover( const FOdysseyPoint& iPointInTexture )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        UOdysseyPainterEditorPaintBucketTool::OnMouseHover( vectorEngine, vectorScene, iPointInTexture );

        currentVectorLayer->RenderImageChanged(true);
    }
}

void
UOdysseyTextureEditorPaintBucketTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        UOdysseyPainterEditorPaintBucketTool::OnMouseDrag( vectorEngine, vectorScene, iPointInTexture );

        currentVectorLayer->RenderImageChanged(true);
    }
}

bool
UOdysseyTextureEditorPaintBucketTool::OnMouseUp( const FOdysseyPoint& iPointInTexture
                                                , const FKey& iKey )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());
    bool ret = false;

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("PaintBucketTool", "Bucket Tool"));

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();
        FOdysseyVectorUndo* undo = nullptr;

        ret = UOdysseyPainterEditorPaintBucketTool::OnMouseUp( vectorEngine, vectorScene, &undo, iPointInTexture, iKey );

        if( undo )
        {
            GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
            // All the delegates for undos are added here for easier maintainability
            undo->mRefreshDelegate.AddUObject( currentVectorLayer, &UOdysseyTextureLayerImageVector::OnRefresh );
        }

        currentVectorLayer->RenderImageChanged(false);
    }

    GEditor->EndTransaction();

    return ret;
}

FOdysseyBlendParameters
UOdysseyTextureEditorPaintBucketTool::OnPaintEnginePreUpdate(const FOdysseyBlendParameters& iBlendParameters)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageRaster* currentLayerRaster = Cast<UOdysseyTextureLayerImageRaster>(layerStack->CurrentLayer.Get());
	FOdysseyBlendParameters blendParameters = iBlendParameters;

	if (!currentLayerRaster)
		return blendParameters;

	if ( currentLayerRaster->IsAlphaLocked )
		blendParameters.AlphaMode = EOdysseyAlphaMode(::ULIS::Alpha_Back);

	return blendParameters;
}

#undef LOCTEXT_NAMESPACE
