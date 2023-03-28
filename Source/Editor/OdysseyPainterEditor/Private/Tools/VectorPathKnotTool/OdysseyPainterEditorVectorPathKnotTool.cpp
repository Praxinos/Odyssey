// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathKnotTool/OdysseyPainterEditorVectorPathKnotTool.h"
#include "LayerStack/OdysseyTextureLayer.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "TextureEditor/OdysseyTextureEditor.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathKnotTool::~UOdysseyPainterEditorVectorPathKnotTool()
{
}

UOdysseyPainterEditorVectorPathKnotTool::UOdysseyPainterEditorVectorPathKnotTool()
    : Radius(20.0f)
    , mPickingHUD()
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathKnotTool64");

    mPickingHUD.SetRadius( Radius );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPathKnotTool::Activate( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->ClearHUD();
    iEngine->AddHUD(&mPickingHUD);
}

bool
UOdysseyPainterEditorVectorPathKnotTool::OnMouseDown( FOdysseyVectorEngine* iEngine
                                                    , FOdysseyVectorScene* iScene
                                                    , const FOdysseyPoint& iPointInTexture
                                                    , const FKey& iKey )
{
    ::ULIS::FRectD roi = { iPointInTexture.x - Radius, iPointInTexture.y - Radius, Radius * 2, Radius * 2 };
    std::vector<FOdysseyVectorPoint*> pickedPointArray;
    FOdysseyVectorSegment* createdSegment = nullptr;
    FOdysseyVectorSegment* removedSegment = nullptr;

    pickedPointArray.reserve(500); // crashes if I don't reserve. I don't know why.

    iEngine->PickPoints( iScene
                       , iPointInTexture.x
                       , iPointInTexture.y
                       , Radius
                       , pickedPointArray
                       , FOdysseyVectorPath::PICK_POINT );

    if( pickedPointArray.size() >= 2 )
    {
        FOdysseyVectorVertex* vertexA = static_cast<FOdysseyVectorVertex*>( pickedPointArray[0] );
        FOdysseyVectorVertex* vertexB = static_cast<FOdysseyVectorVertex*>( pickedPointArray[1] );

        if( ( vertexA->GetSegmentCount() == 1 ) && ( vertexB->GetSegmentCount() == 1 ) )
        {
            if( vertexA->GetPath() != vertexB->GetPath() )
            {
/*
                FOdysseyVectorPath* newPath = static_cast<FOdysseyVectorPath*>( vertexB->GetPath()->Copy() );
*/

                // TODO: remove vertexB->GetPath() from selected objects.
                vertexB->GetPath()->GetParent()->RemoveChild( vertexB->GetPath() );

                vertexA->GetPath()->Merge( vertexB->GetPath() );
            }

            iEngine->Knot( vertexA, vertexB, &createdSegment, &removedSegment, true );

            iScene->Update( 0 );
        }
    }

    return true;
}

void
UOdysseyPainterEditorVectorPathKnotTool::OnMouseHover( FOdysseyVectorEngine* iEngine
                                                     , FOdysseyVectorScene* iScene
                                                     , const FOdysseyPoint& iPointInTexture )
{
    double diameter = Radius * 2.0f;
    ::ULIS::FRectI rect = { (int)iPointInTexture.x - (int)Radius
                          , (int)iPointInTexture.y - (int)Radius
                          , (int)diameter
                          , (int)diameter };

    mPickingHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );
/*
    if( rect.x < 0 ) rect.x = 0;
    if( rect.y < 0 ) rect.y = 0;

    rect = rect & layerStack->GetSurface()->Block()->Rect();

    if( rect.Area() )
    {*/
    /*}*/
}

void
UOdysseyPainterEditorVectorPathKnotTool::OnMouseDrag( FOdysseyVectorEngine* iEngine
                                                    , FOdysseyVectorScene* iScene
                                                    , const FOdysseyPoint& iPointInTexture )
{
    mPickingHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );
}

bool
UOdysseyPainterEditorVectorPathKnotTool::OnMouseUp( FOdysseyVectorEngine* iEngine
                                                  , FOdysseyVectorScene* iScene
                                                  , const FOdysseyPoint& iPointInTexture
                                                  , const FKey& iKey )
{
    return false;
}

void
UOdysseyPainterEditorVectorPathKnotTool::Commit()
{
}

void
UOdysseyPainterEditorVectorPathKnotTool::PropertyChanged( const FName& iPropertyName )
{
    mPickingHUD.SetRadius( Radius );
}
