// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorSceneScaleTool/OdysseyPainterEditorVectorSceneScaleTool.h"
#include "LayerStack/OdysseyTextureLayer.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "TextureEditor/OdysseyTextureEditor.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorPathBuilder.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorSceneScaleTool::~UOdysseyPainterEditorVectorSceneScaleTool()
{
}

UOdysseyPainterEditorVectorSceneScaleTool::UOdysseyPainterEditorVectorSceneScaleTool()
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.SceneScaleTool64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorSceneScaleTool::Activate()
{
	//FOdysseyObjectEditorUtils::SetPropertyValue(BrushOptions, "Color", FOdysseyBrushColor(GetEditorAs<FOdysseyPainterEditor>()->PaintColor()));
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();
    FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();

    vectorEngine->SetDrawingFlags( 0 );

    currentVectorLayer->RenderImageChanged(false);
}

bool
UOdysseyPainterEditorVectorSceneScaleTool::CanDraw()
{
    return IsActivable();
}

bool
UOdysseyPainterEditorVectorSceneScaleTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        UOdysseyVectorRoot* scene = currentVectorLayer->GetScene();
        BLPoint localCoords = scene->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );

        mDownWorldMouseX = iPointInTexture.x;
        mDownWorldMouseY = iPointInTexture.y;

        mDownLocalMouseX = localCoords.x;
        mDownLocalMouseY = localCoords.y;

        mOldLocalMouseX = localCoords.x;
        mOldLocalMouseY = localCoords.y;
    }

    return true;
}

void
UOdysseyPainterEditorVectorSceneScaleTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        UOdysseyVectorRoot* scene = currentVectorLayer->GetScene();
        BLPoint localCoords = scene->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
        ::ULIS::FVec2D dif = { localCoords.x - mOldLocalMouseX
                             , localCoords.y - mOldLocalMouseY };
        double factor = ( 1.0f + ( dif.x / (double) layerStack->GetTexture()->GetSizeX() ) );

        scene->Scale( scene->GetScalingX() * factor
                    , scene->GetScalingY() * factor );

        scene->UpdateMatrix();

        BLPoint worldCoords = scene->GetWorldMatrix().mapPoint( mDownLocalMouseX, mDownLocalMouseY );

        scene->Translate( scene->GetTranslationX() - ( worldCoords.x - mDownWorldMouseX )
                        , scene->GetTranslationY() - ( worldCoords.y - mDownWorldMouseY ) );

        scene->UpdateMatrix();

        mOldLocalMouseX = localCoords.x;
        mOldLocalMouseY = localCoords.y;

        // redraw the whole image
        currentVectorLayer->RenderImageChanged(true);
    }
}

bool
UOdysseyPainterEditorVectorSceneScaleTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    if( currentVectorLayer )
    {
        currentVectorLayer->RenderImageChanged(false);
        return true;
    }

    return false;
}

void
UOdysseyPainterEditorVectorSceneScaleTool::Commit()
{

}
