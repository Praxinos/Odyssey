// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/PaintBucketTool/OdysseyAnimationEditorPaintBucketTool.h"

#define LOCTEXT_NAMESPACE "OdysseyAnimationEditorPaintBucketTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyAnimationEditorPaintBucketTool::~UOdysseyAnimationEditorPaintBucketTool()
{
}

UOdysseyAnimationEditorPaintBucketTool::UOdysseyAnimationEditorPaintBucketTool()
    : mEditedBlock(nullptr)
{
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyAnimationEditorPaintBucketTool::Activate()
{
    UOdysseyAnimationLayerStack::OnCurrentLayerChanged().AddUObject(this, &UOdysseyAnimationEditorPaintBucketTool::OnCurrentLayerChanged);
    Load();
    Super::Activate();
}

void
UOdysseyAnimationEditorPaintBucketTool::Load()
{
	//Define the new active tool based on the layer type
	/* UOdysseyAnimationLayerImageRaster* layer = GetLayer();
	if (!layer)
		return;

	const ::ULIS::FBlock* block = layer->GetBlock();
	::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(block->Format());
	mEditedBlock = new ::ULIS::FBlock(block->Width(), block->Height(), block->Format(), nullptr, ::ULIS::FOnInvalidBlock(&UOdysseyAnimationEditorPaintBucketTool::OnEditedBlockInvalidated, static_cast<void*>(this)));
	ctx.Copy(
		*block,
		*mEditedBlock
	);
	ctx.Finish();

	mPaintEngine.Block(mEditedBlock);

	//Should be managed by the tool
	UOdysseyAnimationLayer::OnRenderImageChanged().AddUObject(this, &UOdysseyAnimationEditorPaintBucketTool::OnLayerRenderImageChanged);
	mPaintEngine.OnCommitDelegate().AddUObject(this, &UOdysseyAnimationEditorPaintBucketTool::OnPaintEngineCommit);
	mPaintEngine.OnPreUpdateDelegate().BindUObject(this, &UOdysseyAnimationEditorPaintBucketTool::OnPaintEnginePreUpdate); */
}

void
UOdysseyAnimationEditorPaintBucketTool::Inactivate()
{
	UOdysseyAnimationLayerStack::OnCurrentLayerChanged().RemoveAll(this);
    Super::Inactivate();
	Unload();
}

void
UOdysseyAnimationEditorPaintBucketTool::Unload()
{
	UOdysseyAnimationLayer::OnRenderImageChanged().RemoveAll(this);
	//mPaintEngine.OnCommitDelegate().RemoveAll(this);
	mPaintEngine.OnPreUpdateDelegate().Unbind();

    //Cleanup
	mPaintEngine.RasterBlock(nullptr);
	delete mEditedBlock;
	mEditedBlock = nullptr;
}

bool
UOdysseyAnimationEditorPaintBucketTool::IsActivable() const
{
	if (!Super::IsActivable())
        return false; 

	//Check for currentlayer
	UOdysseyAnimationLayerImageRaster* layer = GetLayer();
	if (!layer)
		return false;

	bool isActive = UOdysseyLayerFunctionLibrary::IsLayerActivatedInStack(layer);
	bool isLocked = UOdysseyLayerFunctionLibrary::IsLayerLockedInStack(layer);
    return isActive && !isLocked;
}

bool
UOdysseyAnimationEditorPaintBucketTool::CanDraw()
{	
    if (!Super::CanDraw())
        return false; 

	UOdysseyAnimationLayerImageRaster* layer = GetLayer();
	if (!layer)
		return false;

	bool isActive = UOdysseyLayerFunctionLibrary::IsLayerActivatedInStack(layer);
	bool isLocked = UOdysseyLayerFunctionLibrary::IsLayerLockedInStack(layer);
    return !isActive || isLocked;
}

void
UOdysseyAnimationEditorPaintBucketTool::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
{
	//ensure iLayerstack is the one the tool is working on
	UOdysseyAnimationLayerStack* layerstack = GetEditorAs<FOdysseyAnimationEditor>()->LayerStack();
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

UOdysseyAnimationLayerImageRaster*
UOdysseyAnimationEditorPaintBucketTool::GetLayer() const
{
	UOdysseyAnimationLayerStack* layerstack = GetEditorAs<FOdysseyAnimationEditor>()->LayerStack();
	if ( !layerstack )
		return nullptr;

	return Cast<UOdysseyAnimationLayerImageRaster>(layerstack->CurrentLayer.Get());
}

void
UOdysseyAnimationEditorPaintBucketTool::OnEditedBlockInvalidated(const ::ULIS::FBlock* iBlock, const ::ULIS::FRectI* iRects, const uint32 iNumRects, void* iInfo)
{
	//Indicate UObject system that we will change LayersHierarchy property
	//(actually the block has already changed, but there no where else to call it before here and being sure that PostChangePropertyValue will be called after)

	TArray<::ULIS::FRectI> rects(iRects, iNumRects);
	UOdysseyAnimationEditorPaintBucketTool* self = static_cast<UOdysseyAnimationEditorPaintBucketTool*>(iInfo);

	UOdysseyAnimationLayerStack* layerstack = self->GetEditorAs<FOdysseyAnimationEditor>()->LayerStack();
	if (!layerstack)
		return;

	UOdysseyAnimationLayerImageRaster* layer = self->GetLayer();
	if (!layer)
		return;
	
	//layer->SetRenderBlockOverride(self->mEditedBlock);

	/* for ( int i = 0; i < rects.Num(); i++ )
		layerstack->RenderImage(layerstack->GetSurface()->Block(), rects[i], rects[i].Position(), TArray<::ULIS::FEvent>()); */

	//layer->SetRenderBlockOverride(nullptr);

	//::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(layerstack->GetSurface()->Block()->Format());
	//ctx.Finish();

	//layerstack->GetSurface()->Invalidate(rects);
}

void
UOdysseyAnimationEditorPaintBucketTool::OnLayerRenderImageChanged(UOdysseyAnimationLayer* iLayer, const TArray<::ULIS::FRectI>& iRects)
{
	/* UOdysseyAnimationLayerImageRaster* layer = GetLayer();
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

	mPaintEngine.Reset(); */
}

void
UOdysseyAnimationEditorPaintBucketTool::OnPaintEngineCommit(const TArray<::ULIS::FRectI>& iChangedTiles)
{
	/* if ( iChangedTiles.Num() <= 0 )
		return;

	UOdysseyAnimationLayerImageRaster* layer = GetLayer();
	if (!layer)
		return;

	GEditor->BeginTransaction(TEXT("PaintEngine"), LOCTEXT("OnPaintStroke", "Paint Stroke"), nullptr);

	layer->UpdateBlock(*mEditedBlock, iChangedTiles, ::ULIS::FVec2F(0), TArray<::ULIS::FEvent>());

	GEditor->EndTransaction(); */
}

FOdysseyBlendParameters
UOdysseyAnimationEditorPaintBucketTool::OnPaintEnginePreUpdate(const FOdysseyBlendParameters& iBlendParameters)
{
	FOdysseyBlendParameters blendParameters = iBlendParameters;

	UOdysseyAnimationLayerImageRaster* layer = GetLayer();
	if (!layer)
		return blendParameters;

	if ( layer->IsAlphaLocked )
		blendParameters.AlphaMode = EOdysseyAlphaMode(::ULIS::Alpha_Back);

	return blendParameters;
}

#undef LOCTEXT_NAMESPACE
