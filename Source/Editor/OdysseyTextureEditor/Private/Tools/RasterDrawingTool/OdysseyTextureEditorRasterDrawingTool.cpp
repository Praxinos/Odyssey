// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterDrawingTool/OdysseyTextureEditorRasterDrawingTool.h"

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

	TSharedPtr<FOdysseyRasterBlock> rasterBlock = layer->GetRasterBlock();
	mPaintEngine.RasterBlock(rasterBlock);

	if ( BrushInstance )
		BrushInstance->SetBlock(mPaintEngine.PaintBlock());

	//Should be managed by the tool
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

    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    if (!layerStack)
        return false;

    UOdysseyLayer* currentLayer = layerStack->CurrentLayer.Get();
    if (!currentLayer)
        return false;

    return currentLayer->GetClass() == UOdysseyTextureLayerImageRaster::StaticClass();
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