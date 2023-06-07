// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterDrawingTool/OdysseyAnimationEditorRasterDrawingTool.h"

#include "ObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "OdysseyAnimationEditorRasterDrawingTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyAnimationEditorRasterDrawingTool::~UOdysseyAnimationEditorRasterDrawingTool()
{
}

UOdysseyAnimationEditorRasterDrawingTool::UOdysseyAnimationEditorRasterDrawingTool()
{
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyAnimationEditorRasterDrawingTool::Activate()
{
	UOdysseyAnimationLayerStack::OnCurrentLayerChanged().AddUObject(this, &UOdysseyAnimationEditorRasterDrawingTool::OnCurrentLayerChanged);
	Load();
    Super::Activate();
}

void
UOdysseyAnimationEditorRasterDrawingTool::Load()
{
	//Define the new active tool based on the layer type
	UOdysseyAnimation* animation = GetAnimation();
	if (!animation)
		return;

	UOdysseyAnimationLayerImageRaster* layer = GetLayer();
	if (!layer)
		return;

	int celIndex = INDEX_NONE;
	int celFrameIndex = INDEX_NONE;
	if ( !layer->GetCellIndexAtFrame(animation->CurrentFrame, celIndex, celFrameIndex) )
		return;

	TSharedPtr<FOdysseyAnimationCell> cell = layer->GetCell(celIndex);
	if ( !cell )
		return;

	TSharedPtr<IOdysseyAnimationImageRasterEditingAbility> rasterEditableAbility = cell->GetAbility<IOdysseyAnimationImageRasterEditingAbility>();
	if ( !rasterEditableAbility )
		return;

	TSharedPtr<FOdysseyRasterBlock> rasterBlock = rasterEditableAbility->GetRasterBlock(celFrameIndex);
	mPaintEngine.RasterBlock(rasterBlock);

	if ( BrushInstance )
		BrushInstance->SetBlock(mPaintEngine.PaintBlock());

	mPaintEngine.OnPreUpdateDelegate().BindUObject(this, &UOdysseyAnimationEditorRasterDrawingTool::OnPaintEnginePreUpdate);
}

void
UOdysseyAnimationEditorRasterDrawingTool::Inactivate()
{
	UOdysseyAnimationLayerStack::OnCurrentLayerChanged().RemoveAll(this);

    Super::Inactivate(); //Commits

	Unload();
}

void
UOdysseyAnimationEditorRasterDrawingTool::Unload()
{
	mPaintEngine.OnPreUpdateDelegate().Unbind();

    //Cleanup
	mPaintEngine.RasterBlock(nullptr);

	if ( BrushInstance )
		BrushInstance->SetBlock(nullptr);
}

bool
UOdysseyAnimationEditorRasterDrawingTool::IsActivable() const
{
    if (!Super::IsActivable())
        return false; 

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetEditorAs<FOdysseyAnimationEditor>()->LayerStack());
    if (!layerStack)
        return false;

    UOdysseyLayer* currentLayer = layerStack->CurrentLayer.Get();
    if (!currentLayer)
        return false;

    return currentLayer->GetClass() == UOdysseyAnimationLayerImageRaster::StaticClass();
}

bool
UOdysseyAnimationEditorRasterDrawingTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
	if (!CanDraw())
		return false;

	UOdysseyAnimation* animation = GetAnimation();
	if (!animation)
		return false;

	UOdysseyAnimationLayerImageRaster* layer = GetLayer();
	if (!layer)
		return false;

	FInt32Range range = layer->GetFrameRange();

	if (animation->CurrentFrame < range.GetLowerBoundValue())
	{
		//Add a frame at current frame and extend it 
		TSharedPtr<FOdysseyAnimationCellImageRaster> cell = FOdysseyAnimationCellImageRaster::Create(layer, animation->Width(), animation->Height(), animation->Format());
        cell->SetLength(range.GetLowerBoundValue() - animation->CurrentFrame);

		FOdysseyAnimationCellsMutator mutator(layer);
		mutator.Add({cell}, 0);
		mutator.SetOffset(layer->GetOffset() - cell->GetLength());
		mutator.Commit();
	}
	else if (animation->CurrentFrame > range.GetUpperBoundValue())
	{
		//Add a frame at current frame and extend previous frame to it 
		TSharedPtr<FOdysseyAnimationCellImageRaster> cell = FOdysseyAnimationCellImageRaster::Create(layer, animation->Width(), animation->Height(), animation->Format());
        cell->SetLength(1);
		
		FOdysseyAnimationCellsMutator mutator(layer);

		int lastCellIndex = layer->GetCellsCount() - 1;
		if ( lastCellIndex >= 0 )
		{
			int cellLength;
			if ( layer->GetCellLength(lastCellIndex, cellLength) )
			{
				mutator.SetLength(lastCellIndex, cellLength + animation->CurrentFrame - range.GetUpperBoundValue() - 1);
			}
		}

		mutator.Add({cell});
		mutator.Commit();
	}
	else
	{
		int cellIndex;
		int cellFrameIndex;
		if(layer->GetCellIndexAtFrame(animation->CurrentFrame, cellIndex, cellFrameIndex))
		{
			if (cellIndex >= 0 && cellFrameIndex != 0)
			{
				TSharedPtr<FOdysseyAnimationCell> currentCell = layer->GetCell(cellIndex);
				TSharedPtr<IOdysseyAnimationImageRenderingAbility> imageRenderAbility = currentCell->GetAbility<IOdysseyAnimationImageRenderingAbility>();
				if ( imageRenderAbility )
				{
					//Here we need to break the instance
					//We get the render of the current frame, and create a raster cell to draw on it

					TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(animation->Width(), animation->Height(), animation->Format());
					TSharedPtr<IOdysseyImageRenderer> renderer = imageRenderAbility->BuildRenderer(cellFrameIndex, IOdysseyImageRenderer::eRenderType::Render);
					renderer->Copy(block, block->Rect(), {});
					::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(block->Format());
					ctx.Finish();

					int currentCellLength = cellFrameIndex;
					int newCellLength = currentCell->GetLength() - currentCellLength;

					TSharedPtr<FOdysseyAnimationCellImageRaster> cell = FOdysseyAnimationCellImageRaster::Create(layer, block);
					cell->SetLength(newCellLength);

					FOdysseyAnimationCellsMutator mutator(layer);
					mutator.SetLength(cellIndex, currentCellLength);
					mutator.Add({ cell }, cellIndex + 1);
					mutator.Commit();
				}
			}
		}
	}

	return UOdysseyPainterEditorRasterDrawingTool::OnMouseDown(iPointInTexture, iKey);	
}

bool
UOdysseyAnimationEditorRasterDrawingTool::CanDraw()
{	
	UOdysseyAnimationLayerImageRaster* layer = GetLayer();
	if (!layer)
		return false;

	bool isActive = UOdysseyLayerFunctionLibrary::IsLayerActivatedInStack(layer);
	bool isLocked = UOdysseyLayerFunctionLibrary::IsLayerLockedInStack(layer);
	return isActive && !isLocked;
}

void
UOdysseyAnimationEditorRasterDrawingTool::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
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

UOdysseyAnimation*
UOdysseyAnimationEditorRasterDrawingTool::GetAnimation() const
{
	return GetEditorAs<FOdysseyAnimationEditor>()->Animation();
}

UOdysseyAnimationLayerImageRaster*
UOdysseyAnimationEditorRasterDrawingTool::GetLayer() const
{
	UOdysseyAnimationLayerStack* layerstack = GetEditorAs<FOdysseyAnimationEditor>()->LayerStack();
	if ( !layerstack )
		return nullptr;
	return Cast<UOdysseyAnimationLayerImageRaster>(layerstack->CurrentLayer.Get());
}

FOdysseyBlendParameters
UOdysseyAnimationEditorRasterDrawingTool::OnPaintEnginePreUpdate(const FOdysseyBlendParameters& iBlendParameters)
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