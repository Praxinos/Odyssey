// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterDrawingTool/OdysseyTextureEditorRasterDrawingTool.h"

#include "ObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorRasterDrawingTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyTextureEditorRasterDrawingTool::~UOdysseyTextureEditorRasterDrawingTool()
{
}

UOdysseyTextureEditorRasterDrawingTool::UOdysseyTextureEditorRasterDrawingTool()
{
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyTextureEditorRasterDrawingTool::Activate()
{
	UOdysseyTextureLayerStack::OnCurrentLayerChanged().AddUObject(this, &UOdysseyTextureEditorRasterDrawingTool::OnCurrentLayerChanged);
	Load();
    Super::Activate();
}

void
UOdysseyTextureEditorRasterDrawingTool::Load()
{
	//Define the new active tool based on the layer type
	UOdysseyTextureLayerImageRaster* layer = GetLayer();
	if (!layer)
		return;

	UOdysseyRasterBlock* rasterBlock = layer->GetRasterBlock();
	mPaintEngine.RasterBlock(rasterBlock);

	if ( BrushInstance )
		BrushInstance->SetBlock(mPaintEngine.PaintBlock());

	//Should be managed by the tool
	//UOdysseyTextureLayer::OnRenderImageChanged().AddUObject(this, &UOdysseyTextureEditorRasterDrawingTool::OnLayerRenderImageChanged);
	//mPaintEngine.OnCommitDelegate().AddUObject(this, &UOdysseyTextureEditorRasterDrawingTool::OnPaintEngineCommit);
	mPaintEngine.OnPreUpdateDelegate().BindUObject(this, &UOdysseyTextureEditorRasterDrawingTool::OnPaintEnginePreUpdate);
}

void
UOdysseyTextureEditorRasterDrawingTool::Inactivate()
{
	UOdysseyTextureLayerStack::OnCurrentLayerChanged().RemoveAll(this);

    Super::Inactivate(); //Commits

	Unload();
}

void
UOdysseyTextureEditorRasterDrawingTool::Unload()
{
	//UOdysseyTextureLayer::OnRenderImageChanged().RemoveAll(this);
	//mPaintEngine.OnCommitDelegate().RemoveAll(this);
	mPaintEngine.OnPreUpdateDelegate().Unbind();

    //Cleanup
	mPaintEngine.RasterBlock(nullptr);

	if ( BrushInstance )
		BrushInstance->SetBlock(nullptr);
}

bool
UOdysseyTextureEditorRasterDrawingTool::IsActivable() const
{
	if (!Super::IsActivable())
        return false; 

	//Check for currentlayer
	UOdysseyTextureLayerImageRaster* layer = GetLayer();
	if (!layer)
		return false;

	return true;
}

bool
UOdysseyTextureEditorRasterDrawingTool::CanDraw()
{	
    if (!Super::CanDraw())
        return false; 

	UOdysseyTextureLayerImageRaster* layer = GetLayer();
	if (!layer)
		return false;

	bool isActive = UOdysseyLayerFunctionLibrary::IsLayerActivatedInStack(layer);
	bool isLocked = UOdysseyLayerFunctionLibrary::IsLayerLockedInStack(layer);
    return isActive && !isLocked;
}

void
UOdysseyTextureEditorRasterDrawingTool::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
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

UOdysseyTextureLayerImageRaster*
UOdysseyTextureEditorRasterDrawingTool::GetLayer() const
{
	UOdysseyTextureLayerStack* layerstack = GetEditorAs<FOdysseyTextureEditor>()->LayerStack();
	if ( !layerstack )
		return nullptr;
	return Cast<UOdysseyTextureLayerImageRaster>(layerstack->CurrentLayer.Get());
}

/* void
UOdysseyTextureEditorRasterDrawingTool::OnEditedBlockInvalidated(const ::ULIS::FBlock* iBlock, const ::ULIS::FRectI* iRects, const uint32 iNumRects, void* iInfo)
{
	//Indicate UObject system that we will change LayersHierarchy property
	//(actually the block has already changed, but there no where else to call it before here and being sure that PostChangePropertyValue will be called after)

	TArray<::ULIS::FRectI> rects(iRects, iNumRects);
	UOdysseyTextureEditorRasterDrawingTool* self = static_cast<UOdysseyTextureEditorRasterDrawingTool*>(iInfo);

	UOdysseyTextureLayerStack* layerstack = self->GetEditorAs<FOdysseyTextureEditor>()->LayerStack();
	if (!layerstack)
		return;

	UOdysseyTextureLayerImageRaster* layer = self->GetLayer();
	if (!layer)
		return;
	
	layer->SetRenderBlockOverride(self->mEditedBlock);

	for ( int i = 0; i < rects.Num(); i++ )
		layerstack->RenderImage(layerstack->GetSurface()->Block(), rects[i], rects[i].Position(), TArray<::ULIS::FEvent>());

	if ( layer )
		layer->SetRenderBlockOverride(nullptr);

	::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(layerstack->GetSurface()->Block()->Format());
	ctx.Finish();

	layerstack->GetSurface()->Invalidate(rects);
} */

/*
void
UOdysseyTextureEditorRasterDrawingTool::OnLayerRenderImageChanged(UOdysseyTextureLayer* iLayer, const TArray<::ULIS::FRectI>& iRects)
{
	UOdysseyTextureLayerImageRaster* layer = GetLayer();
	if (!iLayer || !layer || iLayer != layer)
		return;

	const ::ULIS::FBlock* block = layer->GetBlock();
	::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(block->Format());
	for ( int i = 0; i < iRects.Num(); i++ )
	{
		ctx.Copy(
			*block,
			*mEditedBlock,
			iRects[i],
			iRects[i].Position()
		);

		//TODO: put this Finish() outside of this loop when we can be sure all the rects don't overlap
		ctx.Finish();
	}

	mPaintEngine.Reset();
} */

/*
void
UOdysseyTextureEditorRasterDrawingTool::OnPaintEngineCommit(const TArray<::ULIS::FRectI>& iChangedTiles)
{
	if ( iChangedTiles.Num() <= 0 )
		return;

	UOdysseyTextureLayerImageRaster* layer = GetLayer();
	if (!layer)
		return;

	GEditor->BeginTransaction(TEXT("PaintEngine"), LOCTEXT("OnPaintStroke", "Paint Stroke"), nullptr);

	layer->UpdateBlock(*mEditedBlock, iChangedTiles, ::ULIS::FVec2F(0), TArray<::ULIS::FEvent>());

	GEditor->EndTransaction();
}
*/

FOdysseyBlendParameters
UOdysseyTextureEditorRasterDrawingTool::OnPaintEnginePreUpdate(const FOdysseyBlendParameters& iBlendParameters)
{
	FOdysseyBlendParameters blendParameters = iBlendParameters;

	UOdysseyTextureLayerImageRaster* layer = GetLayer();
	if (!layer)
		return blendParameters;

	if ( layer->IsAlphaLocked )
		blendParameters.AlphaMode = EOdysseyAlphaMode(::ULIS::Alpha_Back);

	return blendParameters;
}

#undef LOCTEXT_NAMESPACE