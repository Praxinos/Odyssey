// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathStitchTool/OdysseyPainterEditorVectorPathStitchTool.h"
#include "Tools/VectorPathStitchTool/OdysseyPainterEditorVectorPathStitchToolHUD.h"
#include "PainterEditor/OdysseyPainterEditor.h"
#include "OdysseyMediaVector.h"

#include "OdysseyMediaVector.h"
#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorPathStitchTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathStitchTool::~UOdysseyPainterEditorVectorPathStitchTool()
{
}

UOdysseyPainterEditorVectorPathStitchTool::UOdysseyPainterEditorVectorPathStitchTool()
    : UOdysseyPainterEditorVectorBaseTool( new FOdysseyPainterEditorVectorPathStitchToolHUD( this ) )
    , PickingRadius(20.0f)
//    , RestrictToSelection( false )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathKnotTool64");

    mPathStitchHUD = static_cast<FOdysseyPainterEditorVectorPathStitchToolHUD*>( mBaseHUD );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

bool
UOdysseyPainterEditorVectorPathStitchTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
}

uint64
UOdysseyPainterEditorVectorPathStitchTool::LoadVector( FOdysseyVectorScene* iScene )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorPathStitchTool::UnloadVector( FOdysseyVectorScene* iScene )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorPathStitchTool::OnMouseDownVector( FOdysseyVectorScene* iScene
                                                            , const FOdysseyPoint& iPointInTexture
                                                            , const FKey& iKey )
{
    // Left mouse button clicked (Note: do not use iPointInTexture.keysDown.Find() in Down & Up events)
    if( iKey == EKeys::LeftMouseButton )
    {
        FOdysseyVectorEngine* iEngine = iScene->GetEngine();
        ::ULIS::FRectD roi = { iPointInTexture.x - PickingRadius, iPointInTexture.y - PickingRadius, PickingRadius * 2, PickingRadius * 2 };
        FOdysseyVectorVertex* knotVertex;
        // for undos
        std::vector<FOdysseyVectorPath*> addedPathArray; // stays empty
        std::vector<FOdysseyVectorVertex*> addedVertexArray;
        std::vector<FOdysseyVectorSegment*> addedSegmentArray;
        std::vector<FOdysseyVectorPath*> removedPathArray; // receives the merged path if any
        std::vector<FOdysseyVectorSegment*> removedSegmentArray;
        std::vector<FOdysseyVectorVertex*> removedVertexArray;
        std::vector<FOdysseyVectorSegment*> mergedSegmentArray;
        std::vector<FOdysseyVectorVertex*> mergedVertexArray;
        FOdysseyVectorVertex** stitchableVertex = mPathStitchHUD->GetStitchableVertices();

        //pickedPointArray.reserve(500); // crashes if I don't reserve. I don't know why.

    /*
        iEngine->PickPoints( iScene
                           , false
                           , iPointInTexture.x
                           , iPointInTexture.y
                           , PickingRadius
                           , pickedPointArray
                           , FOdysseyVectorPath::PICK_POINT );
    */
        if( stitchableVertex[0] && stitchableVertex[1] )
        {
            FOdysseyVectorVertex* vertexA = static_cast<FOdysseyVectorVertex*>( stitchableVertex[0] );
            FOdysseyVectorVertex* vertexB = static_cast<FOdysseyVectorVertex*>( stitchableVertex[1] );
            FOdysseyVectorPath* mergedPath = nullptr;

            // TODO: remove vertexB->GetPath() from selected objects.
            if( vertexA->GetPath() != vertexB->GetPath() )
            {
                mergedPath = vertexB->GetPath();

                vertexB->GetPath()->GetParent()->RemoveChild( mergedPath );
                vertexA->GetPath()->Merge( mergedPath, mergedVertexArray, mergedSegmentArray );


                // update the pointer with the newly created vertex's. Note, Merge alters the original vertex's ID.
                vertexB = mergedVertexArray[vertexB->GetID()];

                if( mergedPath->IsSelected() )
                {
                    iScene->Unselect( mergedPath );

                    iScene->Select( vertexA->GetPath() );
                }

                removedPathArray.push_back( mergedPath );
            }

            knotVertex = iEngine->Stitch( vertexA, vertexB, addedSegmentArray, removedSegmentArray, true );

            if( knotVertex )
            {
                addedVertexArray.push_back( knotVertex );
                removedVertexArray.push_back( vertexA );
                removedVertexArray.push_back( vertexB );

                // needed for valid GUndo pointer
                GEditor->BeginTransaction(LOCTEXT("VectorPathStitchTool","Vector Path Stitch Tool"));
                if( GUndo )
                {
                    FOdysseyVectorUndo *undo = new FOdysseyVectorUndoPathStitch( iScene
                                                                                , removedPathArray
                                                                                , removedVertexArray
                                                                                , removedSegmentArray
                                                                                , addedPathArray
                                                                                , addedVertexArray
                                                                                , addedSegmentArray
                                                                                , mergedVertexArray
                                                                                , mergedSegmentArray );

                    GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
                }
                GEditor->EndTransaction();
            }
        }

        iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

        mPathStitchHUD->Reset( iScene ); // rebuilds QuadTree after path alter.
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
         | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

uint64
UOdysseyPainterEditorVectorPathStitchTool::OnMouseHoverVector( FOdysseyVectorScene* iScene
                                                             , const FOdysseyPoint& iPointInTexture )
{
    double diameter = PickingRadius * 2.0f;
    ::ULIS::FRectI rect = { (int)iPointInTexture.x - (int)PickingRadius
                          , (int)iPointInTexture.y - (int)PickingRadius
                          , (int)diameter
                          , (int)diameter };

    mPathStitchHUD->SetPosition( iPointInTexture.x, iPointInTexture.y );


/*
    if( rect.x < 0 ) rect.x = 0;
    if( rect.y < 0 ) rect.y = 0;

    rect = rect & layerStack->GetSurface()->Block()->Rect();

    if( rect.Area() )
    {*/
    /*}*/
    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

uint64
UOdysseyPainterEditorVectorPathStitchTool::OnMouseDragVector( FOdysseyVectorScene* iScene
                                                            , const FOdysseyPoint& iPointInTexture )
{
    // Left mouse button clicked
    if( iPointInTexture.keysDown.Find( EKeys::LeftMouseButton ) != INDEX_NONE )
    {
        mPathStitchHUD->SetPosition( iPointInTexture.x, iPointInTexture.y );
    }

    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
         | FOdysseyVectorEngine::SIGNAL_INTERACTIVE;
}

uint64
UOdysseyPainterEditorVectorPathStitchTool::OnMouseUpVector( FOdysseyVectorScene* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey )
{
    return FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

uint64
UOdysseyPainterEditorVectorPathStitchTool::PropertyChangedVector( FOdysseyVectorScene* iScene
                                                                , const FName& iPropertyName )
{
    FOdysseyVectorEngine* iEngine = iScene->GetEngine();

    if( iPropertyName == "RestrictToSelection" )
    {
        iEngine->ResetHUD(); // rebuild the quad tree
    }

    return UOdysseyPainterEditorVectorBaseTool::PropertyChangedVector( iScene, iPropertyName )
         | FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;
}

#undef LOCTEXT_NAMESPACE
