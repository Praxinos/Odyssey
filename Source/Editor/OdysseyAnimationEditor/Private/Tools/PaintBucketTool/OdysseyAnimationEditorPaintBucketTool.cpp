// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/PaintBucketTool/OdysseyAnimationEditorPaintBucketTool.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "Abilities/IOdysseyAnimationImageRasterEditingAbility.h"

#define LOCTEXT_NAMESPACE "OdysseyAnimationEditorPaintBucketTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyAnimationEditorPaintBucketTool::~UOdysseyAnimationEditorPaintBucketTool()
{
}

UOdysseyAnimationEditorPaintBucketTool::UOdysseyAnimationEditorPaintBucketTool()
{
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyAnimationEditorPaintBucketTool::Activate()
{
    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetEditorAs<FOdysseyAnimationEditor>()->LayerStack());
    UOdysseyAnimationLayerImageVector* currentVectorLayer = Cast<UOdysseyAnimationLayerImageVector>(layerStack->CurrentLayer.Get());

    UOdysseyAnimationLayerStack::OnCurrentLayerChanged().AddUObject( this, &UOdysseyAnimationEditorPaintBucketTool::OnCurrentLayerChanged );

    Load();

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        UOdysseyPainterEditorPaintBucketTool::ActivateVector( vectorEngine, vectorScene );
    }
}

void
UOdysseyAnimationEditorPaintBucketTool::Load()
{
	UOdysseyAnimation* animation = GetAnimation();
	if (!animation)
		return;

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetEditorAs<FOdysseyAnimationEditor>()->LayerStack());
    UOdysseyAnimationLayerImageRaster* currentLayerRaster = Cast<UOdysseyAnimationLayerImageRaster>(layerStack->CurrentLayer.Get());

	//Define the new active tool based on the layer type
	if (!currentLayerRaster)
		return;

    int celIndex = INDEX_NONE;
    int celFrameIndex = INDEX_NONE;
    if ( !currentLayerRaster->GetCellIndexAtFrame(animation->CurrentFrame, celIndex, celFrameIndex) )
        return;

    TSharedPtr<FOdysseyAnimationCell> cell = currentLayerRaster->GetCell(celIndex);
    if ( !cell )
        return;
    
    TSharedPtr<IOdysseyAnimationImageRasterEditingAbility> rasterEditableAbility = cell->GetAbility<IOdysseyAnimationImageRasterEditingAbility>();
    if ( !rasterEditableAbility )
        return;

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = rasterEditableAbility->GetRasterBlock(celFrameIndex);
	mPaintEngine.RasterBlock(rasterBlock);

	//Should be managed by the tool
	mPaintEngine.OnPreUpdateDelegate().BindUObject(this, &UOdysseyAnimationEditorPaintBucketTool::OnPaintEnginePreUpdate);
}

void
UOdysseyAnimationEditorPaintBucketTool::Inactivate()
{
	UOdysseyAnimationLayerStack::OnCurrentLayerChanged().RemoveAll(this);
    Super::Inactivate();
}

void
UOdysseyAnimationEditorPaintBucketTool::Unload()
{
	mPaintEngine.OnPreUpdateDelegate().Unbind();

    //Cleanup
	mPaintEngine.RasterBlock(nullptr);
}

bool
UOdysseyAnimationEditorPaintBucketTool::IsActivable() const
{
    if (!Super::IsActivable())
        return false; 

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetEditorAs<FOdysseyAnimationEditor>()->LayerStack());
    if (!layerStack)
        return false;

    UOdysseyLayer* currentLayer = layerStack->CurrentLayer.Get();
    if (!currentLayer)
        return false;

    return      currentLayer->GetClass() == UOdysseyAnimationLayerImageRaster::StaticClass()
            ||  currentLayer->GetClass() == UOdysseyAnimationLayerImageVector::StaticClass();
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

UOdysseyAnimation*
UOdysseyAnimationEditorPaintBucketTool::GetAnimation() const
{
	return GetEditorAs<FOdysseyAnimationEditor>()->Animation();
}

bool
UOdysseyAnimationEditorPaintBucketTool::OnMouseDown( const FOdysseyPoint& iPointInTexture
                                                  , const FKey& iKey )
{
    UOdysseyAnimation* animation = GetAnimation();
    if ( !animation )
        return false;

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetEditorAs<FOdysseyAnimationEditor>()->LayerStack());
    UOdysseyAnimationLayerImageVector* currentVectorLayer = Cast<UOdysseyAnimationLayerImageVector>(layerStack->CurrentLayer.Get());
    UOdysseyAnimationLayerImageRaster* currentRasterLayer = Cast<UOdysseyAnimationLayerImageRaster>(layerStack->CurrentLayer.Get());
    bool ret = false;

    if( currentRasterLayer )
    {
        int celIndex = INDEX_NONE;
        int celFrameIndex = INDEX_NONE;
        if ( !currentRasterLayer->GetCellIndexAtFrame(animation->CurrentFrame, celIndex, celFrameIndex) )
            return ret;

        TSharedPtr<FOdysseyAnimationCell> cell = currentRasterLayer->GetCell(celIndex);
        if ( !cell )
            return ret;

        TSharedPtr<IOdysseyAnimationImageRasterEditingAbility> rasterEditableAbility = cell->GetAbility<IOdysseyAnimationImageRasterEditingAbility>();
        if ( !rasterEditableAbility )
            return ret;

        TSharedPtr<FOdysseyRasterBlock> rasterBlock = rasterEditableAbility->GetRasterBlock(celFrameIndex);
        ret = UOdysseyPainterEditorPaintBucketTool::OnMouseDownRaster(rasterBlock->GetBlock(), iPointInTexture, iKey );
    }

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        ret = UOdysseyPainterEditorPaintBucketTool::OnMouseDownVector( vectorEngine, vectorScene, iPointInTexture, iKey );
    }

    return ret;
}

void
UOdysseyAnimationEditorPaintBucketTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    UOdysseyAnimation* animation = GetAnimation();
    if ( !animation )
        return;

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetEditorAs<FOdysseyAnimationEditor>()->LayerStack());
    UOdysseyAnimationLayerImageVector* currentVectorLayer = Cast<UOdysseyAnimationLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        TSharedPtr<IOdysseyAnimationImageRenderingAbility> imageRenderAbility = currentVectorLayer->GetAbility<IOdysseyAnimationImageRenderingAbility>();
        if ( imageRenderAbility )
        {
            FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
            FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

            UOdysseyPainterEditorPaintBucketTool::OnMouseDragVector(vectorEngine, vectorScene, iPointInTexture);

            
            imageRenderAbility->Changed(imageRenderAbility->GetId(), imageRenderAbility->BuildRenderer(animation->CurrentFrame, false)->GetRects());
        }
    }
}

bool
UOdysseyAnimationEditorPaintBucketTool::OnMouseUp( const FOdysseyPoint& iPointInTexture
                                                , const FKey& iKey )
{
    UOdysseyAnimation* animation = GetAnimation();
    if ( !animation )
        return false;

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetEditorAs<FOdysseyAnimationEditor>()->LayerStack());
    UOdysseyAnimationLayerImageVector* currentVectorLayer = Cast<UOdysseyAnimationLayerImageVector>(layerStack->CurrentLayer.Get());
    bool ret = false;

    if( currentVectorLayer )
    {
        TSharedPtr<IOdysseyAnimationImageRenderingAbility> imageRenderAbility = currentVectorLayer->GetAbility<IOdysseyAnimationImageRenderingAbility>();
        if ( imageRenderAbility )
        {
            FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
            FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

            ret = UOdysseyPainterEditorPaintBucketTool::OnMouseUpVector(vectorEngine, vectorScene, nullptr, iPointInTexture, iKey);

            imageRenderAbility->Commited(imageRenderAbility->GetId(), imageRenderAbility->BuildRenderer(animation->CurrentFrame, false)->GetRects());
        }
    }

    return ret;
}

FOdysseyBlendParameters
UOdysseyAnimationEditorPaintBucketTool::OnPaintEnginePreUpdate(const FOdysseyBlendParameters& iBlendParameters)
{
    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetEditorAs<FOdysseyAnimationEditor>()->LayerStack());
    UOdysseyAnimationLayerImageRaster* currentLayerRaster = Cast<UOdysseyAnimationLayerImageRaster>(layerStack->CurrentLayer.Get());
	FOdysseyBlendParameters blendParameters = iBlendParameters;

	if (!currentLayerRaster)
		return blendParameters;

	if ( currentLayerRaster->IsAlphaLocked )
		blendParameters.AlphaMode = EOdysseyAlphaMode(::ULIS::Alpha_Back);

	return blendParameters;
}

#undef LOCTEXT_NAMESPACE
