// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorObjectPickTool/OdysseyPainterEditorVectorObjectPickTool.h"
#include "LayerStack/OdysseyTextureLayer.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "TextureEditor/OdysseyTextureEditor.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorPathBuilder.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorObjectPickTool::~UOdysseyPainterEditorVectorObjectPickTool()
{
    delete mSelectionHUD;
}

UOdysseyPainterEditorVectorObjectPickTool::UOdysseyPainterEditorVectorObjectPickTool()
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.ObjectPickTool64");

    mSelectionHUD = new FOdysseyVectorHUDSelection( mPointArray );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorObjectPickTool::Activate()
{
	//FOdysseyObjectEditorUtils::SetPropertyValue(BrushOptions, "Color", FOdysseyBrushColor(GetEditorAs<FOdysseyPainterEditor>()->PaintColor()));
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();
    FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();

/*
    vectorEngine->SetDrawingFlags( 0 );
*/

    vectorEngine->ClearHUD( );

    RedrawCurrentLayer( { { 0, 0, 0, 0 } } );
}

bool
UOdysseyPainterEditorVectorObjectPickTool::CanDraw()
{
    return IsActivable();
}

bool
UOdysseyPainterEditorVectorObjectPickTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    mPointArray.clear();

    if( currentVectorLayer )
    {
        ::ULIS::FVec2D point = { iPointInTexture.x, iPointInTexture.y };
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();

        mSelectionHUD->SetSelecting( true );

        vectorEngine->AddHUD( mSelectionHUD );

        mPointArray.push_back( point );
    }

    return true;
}

void
UOdysseyPainterEditorVectorObjectPickTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        ::ULIS::FVec2D point = { iPointInTexture.x, iPointInTexture.y };

        mPointArray.push_back( point );

        RedrawCurrentLayer( { { 0, 0, 0, 0 } } );
    }
}

bool
UOdysseyPainterEditorVectorObjectPickTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();

        if ( mPointArray.size() == 1 )
        {
            vectorEngine->Pick( *currentVectorLayer->GetScene(), mPointArray, UOdysseyVectorObject::PICK_POINT );
        }

        if ( mPointArray.size() > 1 )
        {
            vectorEngine->Pick( *currentVectorLayer->GetScene(), mPointArray, UOdysseyVectorObject::PICK_FREEHAND );
        }

        mSelectionHUD->SetSelecting( false );

        RedrawCurrentLayer( { { 0, 0, 0, 0 } } );

        return true;
    }



    return false;
}

void
UOdysseyPainterEditorVectorObjectPickTool::Commit()
{

}
