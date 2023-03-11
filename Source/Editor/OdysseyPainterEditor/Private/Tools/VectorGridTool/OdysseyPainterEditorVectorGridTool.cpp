// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorGridTool/OdysseyPainterEditorVectorGridTool.h"
#include "LayerStack/OdysseyTextureLayer.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "TextureEditor/OdysseyTextureEditor.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorGridTool::~UOdysseyPainterEditorVectorGridTool()
{
    delete mGridHUD;
}

UOdysseyPainterEditorVectorGridTool::UOdysseyPainterEditorVectorGridTool()
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Grid64");

    mGridHUD = new FOdysseyVectorHUDGrid( );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorGridTool::Activate()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();

        mGridHUD->UpdateSelectionBox( *currentVectorLayer->GetScene() );

        vectorEngine->ClearHUD( );
        vectorEngine->AddHUD( mGridHUD );

        currentVectorLayer->RenderImageChanged(false);
    }
}

bool
UOdysseyPainterEditorVectorGridTool::CanDraw()
{
    return IsActivable();
}

bool
UOdysseyPainterEditorVectorGridTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FSelectionBox& selectionBox = mGridHUD->GetSelectionBox();

        if( selectionBox.space )
        {

        }
    }

    return true;
}

void
UOdysseyPainterEditorVectorGridTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FSelectionBox& selectionBox = mGridHUD->GetSelectionBox();

        if ( selectionBox.space )
        {

            currentVectorLayer->RenderImageChanged(true);
            //RedrawCurrentLayer( { /*beforeBBox | selectedObject->GetBBox( true )*/{ 0, 0, 0, 0 } } );
        }
    }
}

bool
UOdysseyPainterEditorVectorGridTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);

        currentVectorLayer->RenderImageChanged(false);

        return true;
    }

    return false;
}

void
UOdysseyPainterEditorVectorGridTool::Commit()
{

}
