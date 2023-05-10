// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathKnotTool/OdysseyPainterEditorVectorPathKnotTool.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathKnotTool::~UOdysseyPainterEditorVectorPathKnotTool()
{
}

UOdysseyPainterEditorVectorPathKnotTool::UOdysseyPainterEditorVectorPathKnotTool()
    : mPickingHUD()
    , Radius(20.0f)
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathKnotTool64");

    mPickingHUD.SetRadius( Radius );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPathKnotTool::ActivateVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->ClearHUD();
    iEngine->AddHUD(&mPickingHUD);
}

bool
UOdysseyPainterEditorVectorPathKnotTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , FOdysseyVectorUndo** iUndo
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey )
{
    ::ULIS::FRectD roi = { iPointInTexture.x - Radius, iPointInTexture.y - Radius, Radius * 2, Radius * 2 };
    std::vector<FOdysseyVectorPoint*> pickedPointArray;
    std::vector<FOdysseyVectorVertex*> mergedVertexArray;
    std::vector<FOdysseyVectorSegment*> mergedSegmentArray;
    std::vector<FOdysseyVectorVertex*> addedVertexArray;
    std::vector<FOdysseyVectorSegment*> addedSegmentArray;
    std::vector<FOdysseyVectorSegment*> removedSegmentArray;
    std::vector<FOdysseyVectorVertex*> removedVertexArray;
    FOdysseyVectorVertex* knotVertex;

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
        FOdysseyVectorPath* mergedPath = nullptr;

        if( ( vertexA->GetSegmentCount() == 1 ) && ( vertexB->GetSegmentCount() == 1 ) )
        {
            if( vertexA->GetPath() != vertexB->GetPath() )
            {
                std::vector<FOdysseyVectorVertex*> vertexLookup;
/*
                FOdysseyVectorPath* newPath = static_cast<FOdysseyVectorPath*>( vertexB->GetPath()->Copy() );
*/

                // TODO: remove vertexB->GetPath() from selected objects.
                mergedPath = vertexB->GetPath();

                vertexB->GetPath()->GetParent()->RemoveChild( mergedPath );
                vertexA->GetPath()->Merge( mergedPath, vertexLookup, mergedVertexArray, mergedSegmentArray );
                // update the pointer with the newly created vertex's. Note, Merge alters the original vertex's ID.
                vertexB = vertexLookup[vertexB->GetID()];

                iScene->Unselect( mergedPath );
            }

            knotVertex = iEngine->Knot( vertexA, vertexB, addedSegmentArray, removedSegmentArray, true );

            if( knotVertex )
            {
                addedVertexArray.push_back( knotVertex );
                removedVertexArray.push_back( vertexA );
                removedVertexArray.push_back( vertexB );

                // BeginTransaction() must be called for GUndo to have a value. Please do it in the caller function.
                if( iUndo && GUndo )
                {
                    (*iUndo) = new FOdysseyVectorUndoKnot( iScene
                                                         , vertexA->GetPath()
                                                         , removedVertexArray
                                                         , removedSegmentArray
                                                         , addedVertexArray
                                                         , addedSegmentArray
                                                         , mergedPath
                                                         , mergedVertexArray
                                                         , mergedSegmentArray );

                    GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(*iUndo) );
                }
            }

            iScene->Update( 0 );
        }
    }

    return true;
}

void
UOdysseyPainterEditorVectorPathKnotTool::OnMouseHoverVector( FOdysseyVectorEngine* iEngine
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
UOdysseyPainterEditorVectorPathKnotTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FOdysseyPoint& iPointInTexture )
{
    mPickingHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );
}

bool
UOdysseyPainterEditorVectorPathKnotTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
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
