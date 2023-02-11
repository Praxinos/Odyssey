// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorObjectMoveTool/OdysseyPainterEditorVectorObjectMoveTool.h"
#include "LayerStack/OdysseyTextureLayer.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "TextureEditor/OdysseyTextureEditor.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorPathBuilder.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorObjectMoveTool::~UOdysseyPainterEditorVectorObjectMoveTool()
{
}

UOdysseyPainterEditorVectorObjectMoveTool::UOdysseyPainterEditorVectorObjectMoveTool()
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.ObjectMoveTool64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorObjectMoveTool::Activate()
{
	//FOdysseyObjectEditorUtils::SetPropertyValue(BrushOptions, "Color", FOdysseyBrushColor(GetEditorAs<FOdysseyPainterEditor>()->PaintColor()));
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    if(currentVectorLayer)
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();

        vectorEngine->ClearHUD();

        currentVectorLayer->RenderImageChanged(false);
    }
}

bool
UOdysseyPainterEditorVectorObjectMoveTool::CanDraw()
{
    return IsActivable();
}

bool
UOdysseyPainterEditorVectorObjectMoveTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    if( currentVectorLayer )
    {
        mOldWorldMouseX = iPointInTexture.x;
        mOldWorldMouseY = iPointInTexture.y;
    }

    return true;
}

void
UOdysseyPainterEditorVectorObjectMoveTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();
    static ::ULIS::FRectI oldInvalidatedArea = { 0, 0, 0, 0 };

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        std::list<UOdysseyVectorObject*>& selectObjectList = currentVectorLayer->GetScene()->GetSelectedObjectList();
        double difx = iPointInTexture.x - mOldWorldMouseX;
        double dify = iPointInTexture.y - mOldWorldMouseY;
        ::ULIS::FRectD beforeBBox = UOdysseyVectorObject::GetBoundingBoxFromList( selectObjectList );

        for( std::list<UOdysseyVectorObject*>::iterator it = selectObjectList.begin(); it != selectObjectList.end(); ++it )
        {
            UOdysseyVectorObject* selectedObject = (*it);

            if ( selectedObject->HasSelectedParent() == false )
            {
                UOdysseyVectorObject* parentObject = selectedObject->GetParent();
                BLPoint localDif = parentObject->GetInverseWorldMatrix().mapVector( difx, dify );

                selectedObject->Translate( selectedObject->GetTranslationX() + localDif.x
                                         , selectedObject->GetTranslationY() + localDif.y );

                selectedObject->UpdateMatrix();
            }
        }

        mOldWorldMouseX = iPointInTexture.x;
        mOldWorldMouseY = iPointInTexture.y;

        currentVectorLayer->RenderImageChanged(true);
        //RedrawCurrentLayer( { { 0, 0, 0, 0 } } /*{ beforeBBox | UOdysseyVectorObject::GetBoundingBoxFromList( selectObjectList ) }*/, true );
    }
}

bool
UOdysseyPainterEditorVectorObjectMoveTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
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
UOdysseyPainterEditorVectorObjectMoveTool::Commit()
{

}
