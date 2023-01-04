// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathCutTool/OdysseyPainterEditorVectorPathCutTool.h"
#include "LayerStack/OdysseyTextureLayer.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "TextureEditor/OdysseyTextureEditor.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorPathBuilder.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathCutTool::~UOdysseyPainterEditorVectorPathCutTool()
{
}

UOdysseyPainterEditorVectorPathCutTool::UOdysseyPainterEditorVectorPathCutTool()
    : Size(1.0f)
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathCutTool64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPathCutTool::Activate()
{
	//FOdysseyObjectEditorUtils::SetPropertyValue(BrushOptions, "Color", FOdysseyBrushColor(GetEditorAs<FOdysseyPainterEditor>()->PaintColor()));
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();
    FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetVectorEngine();

    vectorEngine->SetDrawingFlags( FOdysseyVectorEngine::RENDER_OBJECT_STRUCTURE );

    RedrawCurrentLayer( { { 0, 0, 0, 0 } } );
}

bool
UOdysseyPainterEditorVectorPathCutTool::CanDraw()
{
    return IsActivable();
}

bool
UOdysseyPainterEditorVectorPathCutTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetVectorEngine();
        UOdysseyVectorObject* selectedObject = vectorEngine->GetScene()->GetLastSelected();

        if ( selectedObject )
        {
            BLPoint localCoords = selectedObject->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );

            mStartCutAt.x = localCoords.x;
            mStartCutAt.y = localCoords.y;
        }
    }

    return true;
}

void
UOdysseyPainterEditorVectorPathCutTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{

}

bool
UOdysseyPainterEditorVectorPathCutTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();
    ::ULIS::FVec2D endCutAt;

    if( currentVectorLayer )
    {
        UOdysseyVectorObject* selectedObject = currentVectorLayer->GetVectorEngine()->GetScene()->GetLastSelected();

        if ( selectedObject )
        {
            BLPoint localCoords = selectedObject->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );

            endCutAt.x = localCoords.x;
            endCutAt.y = localCoords.y;

            if( selectedObject->GetClass() == UOdysseyVectorPathCubic::StaticClass() )
            {
                UOdysseyVectorPathCubic *cubicPath = static_cast<UOdysseyVectorPathCubic*>(selectedObject);

                cubicPath->Cut( mStartCutAt, endCutAt );

                cubicPath->Update();
            }

            // redraw the whole layer
            RedrawCurrentLayer( { { 0, 0, 0, 0 } } );

            return true;
        }
    }

    return false;
}

void
UOdysseyPainterEditorVectorPathCutTool::Commit()
{

}
