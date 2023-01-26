// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/PaintBucketTool/OdysseyTextureEditorPaintBucketTool.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorPaintBucketTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyTextureEditorPaintBucketTool::~UOdysseyTextureEditorPaintBucketTool()
{
}

UOdysseyTextureEditorPaintBucketTool::UOdysseyTextureEditorPaintBucketTool()
    : mEditedBlock(nullptr)
{
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyTextureEditorPaintBucketTool::Activate()
{
    UOdysseyTextureLayerStack::OnCurrentLayerChanged().AddUObject(this, &UOdysseyTextureEditorPaintBucketTool::OnCurrentLayerChanged);
    Load();
    Super::Activate();
}

void
UOdysseyTextureEditorPaintBucketTool::Load()
{
	//Define the new active tool based on the layer type
	UOdysseyTextureLayerImageRaster* layer = GetLayer();
	if (!layer)
		return;

	UOdysseyRasterBlock* rasterBlock = layer->GetRasterBlock();
	mPaintEngine.RasterBlock(rasterBlock);

	//Should be managed by the tool
	mPaintEngine.OnPreUpdateDelegate().BindUObject(this, &UOdysseyTextureEditorPaintBucketTool::OnPaintEnginePreUpdate);
}

void
UOdysseyTextureEditorPaintBucketTool::Inactivate()
{
	UOdysseyTextureLayerStack::OnCurrentLayerChanged().RemoveAll(this);
    Super::Inactivate();
	Unload();
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

	//Check for currentlayer
	UOdysseyTextureLayerImageRaster* layer = GetLayer();
	if (!layer)
		return false;

	bool isActive = UOdysseyLayerFunctionLibrary::IsLayerActivatedInStack(layer);
	bool isLocked = UOdysseyLayerFunctionLibrary::IsLayerLockedInStack(layer);
    return isActive && !isLocked;
}

bool
UOdysseyTextureEditorPaintBucketTool::CanDraw()
{	
    if (!Super::CanDraw())
        return false; 

	UOdysseyTextureLayerImageRaster* layer = GetLayer();
	if (!layer)
		return false;

	bool isActive = UOdysseyLayerFunctionLibrary::IsLayerActivatedInStack(layer);
	bool isLocked = UOdysseyLayerFunctionLibrary::IsLayerLockedInStack(layer);
    return !isActive || isLocked;
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

UOdysseyTextureLayerImageRaster*
UOdysseyTextureEditorPaintBucketTool::GetLayer() const
{
	UOdysseyTextureLayerStack* layerstack = GetEditorAs<FOdysseyTextureEditor>()->LayerStack();
	if ( !layerstack )
		return nullptr;

	return Cast<UOdysseyTextureLayerImageRaster>(layerstack->CurrentLayer.Get());
}

FOdysseyBlendParameters
UOdysseyTextureEditorPaintBucketTool::OnPaintEnginePreUpdate(const FOdysseyBlendParameters& iBlendParameters)
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
