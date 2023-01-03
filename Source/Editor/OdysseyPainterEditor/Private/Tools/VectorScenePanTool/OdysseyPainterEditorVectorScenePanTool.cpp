// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorScenePanTool/OdysseyPainterEditorVectorScenePanTool.h"
#include "LayerStack/OdysseyTextureLayer.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "TextureEditor/OdysseyTextureEditor.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorPathBuilder.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorScenePanTool::~UOdysseyPainterEditorVectorScenePanTool()
{
}

UOdysseyPainterEditorVectorScenePanTool::UOdysseyPainterEditorVectorScenePanTool()
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.ScenePanTool64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorScenePanTool::Activate()
{
	//FOdysseyObjectEditorUtils::SetPropertyValue(BrushOptions, "Color", FOdysseyBrushColor(GetEditorAs<FOdysseyPainterEditor>()->PaintColor()));
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentVectorImageLayer();
    FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetVectorEngine();

    vectorEngine->SetDrawingFlags( 0 );

    RedrawCurrentLayer( { { 0, 0, 0, 0 } } );
}

bool
UOdysseyPainterEditorVectorScenePanTool::CanDraw()
{
    return IsActivable();
}

bool
UOdysseyPainterEditorVectorScenePanTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentVectorImageLayer();

    if( currentVectorLayer )
    {
        mOldWorldMouseX = iPointInTexture.x;
        mOldWorldMouseY = iPointInTexture.y;
    }

    return true;
}

void
UOdysseyPainterEditorVectorScenePanTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentVectorImageLayer();

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetVectorEngine();
        UOdysseyVectorRoot *scene = vectorEngine->GetScene();
        double difx = iPointInTexture.x - mOldWorldMouseX;
        double dify = iPointInTexture.y - mOldWorldMouseY;

        scene->Translate( scene->GetTranslationX() + difx
                        , scene->GetTranslationY() + dify );

        scene->UpdateMatrix();

        mOldWorldMouseX = iPointInTexture.x;
        mOldWorldMouseY = iPointInTexture.y;

        // redraw the whole layer
        RedrawCurrentLayer( { { 0, 0, 0, 0 } } );
    }
}

bool
UOdysseyPainterEditorVectorScenePanTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentVectorImageLayer();

    if( currentVectorLayer )
    {

        return true;
    }

    return false;
}

void
UOdysseyPainterEditorVectorScenePanTool::Commit()
{

}
