// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathStitchTool/OdysseyPainterEditorVectorPathStitchTool.h"
#include "Tools/VectorPathStitchTool/OdysseyPainterEditorVectorPathStitchToolHUD.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorPathStitchTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathStitchTool::~UOdysseyPainterEditorVectorPathStitchTool()
{
    delete mPathStitchHUD;
}

UOdysseyPainterEditorVectorPathStitchTool::UOdysseyPainterEditorVectorPathStitchTool()
    : PickingRadius(20.0f)
    , RestrictToSelection( false )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathKnotTool64");

    mPathStitchHUD = new FOdysseyPainterEditorVectorPathStitchToolHUD( this );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPathStitchTool::Load()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

            LoadVector( vectorEngine, vectorScene );
        }
    }
}

void
UOdysseyPainterEditorVectorPathStitchTool::Unload()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

            UnloadVector( vectorEngine, vectorScene );
        }
    }
}

bool
UOdysseyPainterEditorVectorPathStitchTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
}

void
UOdysseyPainterEditorVectorPathStitchTool::UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->RemoveHUD( mPathStitchHUD );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorPathStitchTool::LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->ClearHUD();
    iEngine->AddHUD( mPathStitchHUD );

    iEngine->ResetHUD();

    // redetect paintgroups cycles in case the path drawing tool is not set to do so
    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorPathStitchTool::OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

            return OnMouseDownVector( vectorEngine, vectorScene, iPointInTexture,iKey );
        }
    }

    return false;
}

bool
UOdysseyPainterEditorVectorPathStitchTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey )
{
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
    std::vector<FOdysseyVectorPoint*>& pickedPointArray = mPathStitchHUD->GetPickedPointArray();

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
    if( pickedPointArray.size() > 1 )
    {
        FOdysseyVectorVertex* vertexA = static_cast<FOdysseyVectorVertex*>( pickedPointArray[0] );
        FOdysseyVectorVertex* vertexB = static_cast<FOdysseyVectorVertex*>( pickedPointArray[1] );
        FOdysseyVectorPath* mergedPath = nullptr;

        if( ( vertexA->GetSegmentCount() == 1 ) && ( vertexB->GetSegmentCount() == 1 ) )
        {
            if( vertexA->GetPath() != vertexB->GetPath() )
            {
                // TODO: remove vertexB->GetPath() from selected objects.
                mergedPath = vertexB->GetPath();

                vertexB->GetPath()->GetParent()->RemoveChild( mergedPath );
                vertexA->GetPath()->Merge( mergedPath, mergedVertexArray, mergedSegmentArray );
                // update the pointer with the newly created vertex's. Note, Merge alters the original vertex's ID.
                vertexB = mergedVertexArray[vertexB->GetID()];

                iScene->Unselect( mergedPath );

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
    }

    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mPathStitchHUD->Reset( iScene ); // rebuilds QuadTree after path alter.

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED
                   | FOdysseyVectorEngine::SIGNAL_INTERACTIVE );

    return true;
}

void
UOdysseyPainterEditorVectorPathStitchTool::OnMouseHover( const FOdysseyPoint& iPointInTexture )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

            OnMouseHoverVector( vectorEngine, vectorScene, iPointInTexture );
        }
    }
}

void
UOdysseyPainterEditorVectorPathStitchTool::OnMouseHoverVector( FOdysseyVectorEngine* iEngine
                                                           , FOdysseyVectorScene* iScene
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
    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_INTERACTIVE );
}

void
UOdysseyPainterEditorVectorPathStitchTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

            OnMouseDragVector( vectorEngine, vectorScene, iPointInTexture );
        }
    }
}

void
UOdysseyPainterEditorVectorPathStitchTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FOdysseyPoint& iPointInTexture )
{
    mPathStitchHUD->SetPosition( iPointInTexture.x, iPointInTexture.y );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_INTERACTIVE );
}

bool
UOdysseyPainterEditorVectorPathStitchTool::OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

            return OnMouseUpVector( vectorEngine, vectorScene, iPointInTexture, iKey );
        }
    }

    return false;
}

bool
UOdysseyPainterEditorVectorPathStitchTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey )
{
    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );

    return false;
}

void
UOdysseyPainterEditorVectorPathStitchTool::Commit()
{
}

void
UOdysseyPainterEditorVectorPathStitchTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    // redraw
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

            PropertyChangedVector( vectorEngine, vectorScene, PropertyChangedEvent.GetPropertyName() );
        }
    }
}

void
UOdysseyPainterEditorVectorPathStitchTool::PropertyChangedVector( FOdysseyVectorEngine* iEngine
                                                                , FOdysseyVectorScene* iScene
                                                                , const FName& iPropertyName)
{
    if( iPropertyName == "RestrictToSelection" )
    {
        iEngine->ResetHUD(); // rebuild the quad tree
    }

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

#undef LOCTEXT_NAMESPACE
