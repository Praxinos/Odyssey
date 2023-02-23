// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathKnotTool/OdysseyPainterEditorVectorPathKnotTool.h"
#include "LayerStack/OdysseyTextureLayer.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "TextureEditor/OdysseyTextureEditor.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorPathBuilder.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathKnotTool::~UOdysseyPainterEditorVectorPathKnotTool()
{
}

UOdysseyPainterEditorVectorPathKnotTool::UOdysseyPainterEditorVectorPathKnotTool()
    : Radius(20.0f)
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathKnotTool64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPathKnotTool::Activate()
{
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    if(currentVectorLayer)
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();

        vectorEngine->ClearHUD();

        currentVectorLayer->RenderImageChanged(false);
    }
}

bool
UOdysseyPainterEditorVectorPathKnotTool::CanDraw()
{
    return IsActivable();
}

bool
UOdysseyPainterEditorVectorPathKnotTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        ::ULIS::FRectD roi = { iPointInTexture.x - Radius, iPointInTexture.y - Radius, Radius * 2, Radius * 2 };
        std::vector<UOdysseyVectorPoint*> pickedPointArray;
        UOdysseyVectorSegment* createdSegment = nullptr;
        UOdysseyVectorSegment* removedSegment = nullptr;

        pickedPointArray.reserve(500); // crashes if I don't reserve. I don't know why.
/*
        vectorEngine->UseMaskImage();
        vectorEngine->GetBLContext()->setFillAlpha( 0.0f );
        vectorEngine->GetBLContext()->clearAll();
        vectorEngine->GetBLContext()->setFillAlpha( 1.0f );
        vectorEngine->GetBLContext()->fillCircle( iPointInTexture.x, iPointInTexture.y, Radius );
        vectorEngine->UseColorImage();
*/
        vectorEngine->PickPoints( currentVectorLayer->GetScene()
                                , iPointInTexture.x
                                , iPointInTexture.y
                                , Radius
                                , pickedPointArray );

        if( pickedPointArray.size() >= 2 )
        {
            UOdysseyVectorVertex* vertexA = Cast<UOdysseyVectorVertex>( pickedPointArray[0] );
            UOdysseyVectorVertex* vertexB = Cast<UOdysseyVectorVertex>( pickedPointArray[1] );

            if( ( vertexA->GetSegmentCount() == 1 ) && ( vertexB->GetSegmentCount() == 1 ) )
            {
                if( vertexA->GetPath() != vertexB->GetPath() )
                {
/*
                    UOdysseyVectorPath* newPath = Cast<UOdysseyVectorPath>( vertexB->GetPath()->Copy() );
*/

                    // TODO: remove vertexB->GetPath() from selected objects.
                    vertexB->GetPath()->GetParent()->RemoveChild( vertexB->GetPath() );

                    vertexB->GetPath()->SwitchSpace( *vertexA->GetPath() );

                    vertexA->GetPath()->Merge( vertexB->GetPath() );
                }

                vectorEngine->Knot( vertexA, vertexB, &createdSegment, &removedSegment, true );
            }
        }

        currentVectorLayer->RenderImageChanged(false);
    }

    return true;
}

void
UOdysseyPainterEditorVectorPathKnotTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        ::ULIS::FRectD roi = { iPointInTexture.x - Radius, iPointInTexture.y - Radius, Radius * 2, Radius * 2 };
/*
        vectorEngine->UseMaskImage();
        vectorEngine->GetBLContext()->setFillAlpha( 0.0f );
        vectorEngine->GetBLContext()->clearAll();
        vectorEngine->GetBLContext()->setFillAlpha( 1.0f );
        vectorEngine->GetBLContext()->fillCircle( iPointInTexture.x, iPointInTexture.y, Radius );
        vectorEngine->UseColorImage();
*/
        /*vectorEngine->Knot( roi );*/

        currentVectorLayer->RenderImageChanged(/* { rect }, */true);
    }
}

bool
UOdysseyPainterEditorVectorPathKnotTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();

        // redraw the whole layer
        currentVectorLayer->RenderImageChanged(false);
    }

    return false;
}

void
UOdysseyPainterEditorVectorPathKnotTool::Commit()
{

}
