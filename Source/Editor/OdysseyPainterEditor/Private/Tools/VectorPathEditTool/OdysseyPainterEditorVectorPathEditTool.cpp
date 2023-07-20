// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditTool.h"
#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditToolHUD.h"
#include "Undo/OdysseyVectorUndoPointPosition.h"
#include "Undo/OdysseyVectorUndoPathAlter.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorPathEditTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathEditTool::~UOdysseyPainterEditorVectorPathEditTool()
{
}

UOdysseyPainterEditorVectorPathEditTool::UOdysseyPainterEditorVectorPathEditTool()
    : mPickingFlags ( FOdysseyVectorPath::PICK_POINT )
    , PickingRadius(10.0f)
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.VectoEdit64");

    mPathEditHUD = new FOdysseyPainterEditorVectorPathEditToolHUD( this );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPathEditTool::Load()
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    UOdysseyPainterEditorVectorPathEditTool::LoadVector( vectorEngine, vectorScene );
}

void
UOdysseyPainterEditorVectorPathEditTool::Unload()
{
    
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    UOdysseyPainterEditorVectorPathEditTool::UnloadVector( vectorEngine, vectorScene );
}

bool
UOdysseyPainterEditorVectorPathEditTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
}

void
UOdysseyPainterEditorVectorPathEditTool::UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->RemoveHUD( mPathEditHUD );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorPathEditTool::LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    TSharedPtr< SViewport > viewportWidget; // to force keyboard focus on mouse hover.
                                            // Prevents the user from having to click at least once in the viewport.
    // we need the focus on the viewport for keyboard 
    viewportWidget = GetEditor()->GetGUI()->GetViewportTab()->GetViewport()->GetViewportWidget();

    // we need the focus on the viewport for keyboard 
    FSlateApplication::Get().SetKeyboardFocus( viewportWidget );

    iEngine->ClearHUD();
    iEngine->AddHUD( mPathEditHUD );

    iEngine->ResetHUD();

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnKeyDown( const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return false;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return false;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

    // then detect which keys are pressed and set display mode
    if ( FSlateApplication::Get().GetModifierKeys().IsControlDown() )
    {
        mPickingFlags = FOdysseyVectorPath::PICK_HANDLE_SEGMENT;
    }

    if ( FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
    {
        mPickingFlags = FOdysseyVectorPath::PICK_HANDLE_POINT;
    }

    if ( FSlateApplication::Get().GetModifierKeys().IsAltDown() )
    {
        mPickingFlags = FOdysseyVectorPath::PICK_POINT;
    }

    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );

    return false;
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnKeyUp( const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return false;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return false;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

    // first reset display mode
    mPickingFlags = FOdysseyVectorPath::PICK_POINT;

    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );

    return false;
}

static FOdysseyVectorVertex*
GetStitchingVertex( FOdysseyVectorVertex* iVertex
                  , FOdysseyVectorSegment* iSegment
                  , std::vector<FOdysseyVectorPoint*>& iPickedPointArray
                  , ::ULIS::FVec2D& oHandle )
{
    FOdysseyVectorVertex *currentVertex = iVertex;
    FOdysseyVectorSegment *currentSegment = iSegment;

    do
    {
        // return the nextVertex if it is not marked for deletion. Then it will be stitched with its counterpart, if any.
        if( std::find( iPickedPointArray.begin(), iPickedPointArray.end(), currentVertex ) == iPickedPointArray.end() )
        {
            if( currentSegment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
            {
                FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>( currentSegment );

                oHandle = cubicSegment->GetHandle( currentVertex )->GetCoords();
            }

            return currentVertex;
        }

        // move to the next segment
        currentSegment = currentVertex->GetOtherSegment( currentSegment );
        currentVertex = currentSegment ? currentSegment->GetOtherVertex( currentVertex ) : nullptr;

    } while( ( currentVertex ) && ( currentVertex != iVertex ) );
                                  // loop detection

    return nullptr;
}

static void
PathDeletePoint( FOdysseyVectorPath* iPath
               , std::vector<FOdysseyVectorVertex*>& iRemovedVertexArray
               , std::vector<FOdysseyVectorSegment*>& iRemovedSegmentArray
               , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray
               , double iSelectionRadius
               , const FOdysseyPoint& iPointInTexture )
{
    BLPoint localCoords = iPath->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
    std::vector<FOdysseyVectorPoint*> pickedPointArray;
    std::vector<FOdysseyVectorPoint*> extendedPointArray;
    std::vector<FStitchingPair> stitchingPairArray;

    pickedPointArray.reserve( 10 );
    stitchingPairArray.reserve( 10 );
    extendedPointArray.reserve( 10 );

    iPath->PickPoint( iPointInTexture.x, iPointInTexture.y, iSelectionRadius, pickedPointArray, FOdysseyVectorPath::PICK_POINT );

    extendedPointArray = pickedPointArray;

    // first step
    // Build stitching pairs by finding a vertex that is not doomed for deletion on both sides.
    for( int i = 0; i < pickedPointArray.size(); i++ )
    {
        FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>( pickedPointArray[i] );
        std::list<FOdysseyVectorSegment*>& segmentList = vertex->GetSegmentList();
        uint32 segmentCount = segmentList.size();

        if( segmentCount )
        {
            ::ULIS::FVec2D handle[2];
            FOdysseyVectorSegment* segment0 = segmentList.front();
            FOdysseyVectorSegment* segment1 = ( segmentCount > 1  ) ? segmentList.back() : nullptr;
            FOdysseyVectorVertex* stitchingVertex[2] = { segment0 ? GetStitchingVertex( segment0->GetOtherVertex(vertex)
                                                                                      , segment0
                                                                                      , pickedPointArray
                                                                                      , handle[0] ) : nullptr
                                                       , segment1 ? GetStitchingVertex( segment1->GetOtherVertex(vertex)
                                                                                      , segment1
                                                                                      , pickedPointArray
                                                                                      , handle[1] ) : nullptr };

            if( stitchingVertex[0] == stitchingVertex[1] ) // e.g loops
            {
                if( stitchingVertex[0] )
                {
                    // for vertices that were not picked but that cannot be stitched, delete them as well.
                    extendedPointArray.push_back( stitchingVertex[0] );
                }
            }
            else // stitchingVertex[0] != stitchingVertex[1]
            {
                if ( stitchingVertex[0] && stitchingVertex[1] )
                {
                    FStitchingPair stitchingPair = FStitchingPair( stitchingVertex[0], handle[0]
                                                                 , stitchingVertex[1], handle[1] );

                    if( std::find( stitchingPairArray.begin(), stitchingPairArray.end(), stitchingPair ) == stitchingPairArray.end() )
                    {
                        stitchingPairArray.push_back( stitchingPair );
                    }
                }
                else
                {
                    // for vertices that were not picked but that cannot be stitched, delete them as well.
                    extendedPointArray.push_back( stitchingVertex[0] ? stitchingVertex[0] : stitchingVertex[1] );
                }
            }
        }
        else
        {
        }
    }

    // second step
    // the actual deletion
    for( int i = 0; i < extendedPointArray.size(); i++ )
    {
        FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>( extendedPointArray[i] );
        // Note: work on a copy of the list, because deletion will alter the segment list
        std::list<FOdysseyVectorSegment*> segmentList = vertex->GetSegmentList();

        for( std::list<FOdysseyVectorSegment*>::iterator sit = segmentList.begin(); sit != segmentList.end(); ++sit )
        {
            FOdysseyVectorSegment* segment = *sit;

            iPath->RemoveSegment( segment );
            // for undoing
            iRemovedSegmentArray.push_back( segment );
        }

        iPath->RemoveVertex( vertex );
        // for undoing
        iRemovedVertexArray.push_back( vertex );  
    }

    // thid step
    // stitch
    for( int i = 0; i < stitchingPairArray.size(); i++ )
    {
        FOdysseyVectorVertex* stitchingVertex0 = stitchingPairArray[i].vertex[0];
        FOdysseyVectorVertex* stitchingVertex1 = stitchingPairArray[i].vertex[1];
        ::ULIS::FVec2D handle0 = stitchingPairArray[i].handle[0];
        ::ULIS::FVec2D handle1 = stitchingPairArray[i].handle[1];
        FOdysseyVectorSegmentCubic* stitchedSegment = new FOdysseyVectorSegmentCubic( iPath
                                                                                    , stitchingVertex0
                                                                                    , handle0.x
                                                                                    , handle0.y
                                                                                    , handle1.x
                                                                                    , handle1.y
                                                                                    , stitchingVertex1 );


        iPath->AddSegment( stitchedSegment );
        // for undoing
        iAddedSegmentArray.push_back( stitchedSegment );
    }

    iPath->InvalidateAllSegments();
}

void
UOdysseyPainterEditorVectorPathEditTool::OnMouseDownDeletePoint( FOdysseyVectorEngine* iEngine
                                                               , FOdysseyVectorScene* iScene
                                                               , const FOdysseyPoint& iPointInTexture
                                                               , const FKey& iKey )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();
    std::vector<FOdysseyVectorPath*> removedPathArray;
    std::vector<FOdysseyVectorVertex*> removedVertexArray;
    std::vector<FOdysseyVectorSegment*> removedSegmentArray;
    std::vector<FOdysseyVectorPath*> addedPathArray;
    std::vector<FOdysseyVectorVertex*> addedVertexArray; // not filled, here just for the undo record
    std::vector<FOdysseyVectorSegment*> addedSegmentArray;

    removedPathArray.reserve( 10 );
    removedVertexArray.reserve( 10 );
    removedSegmentArray.reserve( 10 );
    addedSegmentArray.reserve( 10 );

    for( std::list<FOdysseyVectorObject*>::iterator it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
    {
        FOdysseyVectorObject* selectedObject  = *it;

        if( selectedObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(selectedObject);

            PathDeletePoint( path
                           , removedVertexArray
                           , removedSegmentArray
                           , addedSegmentArray
                           , PickingRadius
                           , iPointInTexture );

            // remove path if empty
            if( path->GetSegmentList().size() == 0 )
            {
                path->GetParent()->RemoveChild( path );
                // for undoing
                removedPathArray.push_back( path );
            }
        }
    }

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("VectorPathEditTool","Vector Path Edit Tool"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoPathAlter( iScene
                                                                  , removedPathArray
                                                                  , removedVertexArray
                                                                  , removedSegmentArray
                                                                  , addedPathArray
                                                                  , addedVertexArray
                                                                  , addedSegmentArray );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();
}

static void
PathPickPoint( FOdysseyVectorPath* iPath
             , std::vector<FOdysseyVectorPoint*>& iPickedPointArray
             , double iSelectionRadius
             , uint64 iSelectionFlags
             , const FOdysseyPoint& iPointInTexture )
{
    BLPoint localCoords = iPath->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );
    uint32 pointCount = iPickedPointArray.size();

    iPath->PickPoint( iPointInTexture.x, iPointInTexture.y, iSelectionRadius, iPickedPointArray, iSelectionFlags );

    // Control point must move with the point. Store it in the mPickedPointArray as well
    if( iSelectionFlags == FOdysseyVectorPath::PICK_POINT )
    {
        for( int i = pointCount; i < iPickedPointArray.size(); i++ )
        {
            if( iPickedPointArray[i]->GetClass() == FOdysseyVectorVertex::StaticClass() )
            {
                FOdysseyVectorVertex* cubicVertex = static_cast<FOdysseyVectorVertex*>( iPickedPointArray[i] );
                std::list<FOdysseyVectorSegment*> segmentList = cubicVertex->GetSegmentList();

                for( std::list<FOdysseyVectorSegment*>::iterator segit = segmentList.begin(); segit != segmentList.end(); ++segit )
                {
                    FOdysseyVectorSegment* segment = static_cast<FOdysseyVectorSegment*>(*segit);

                    if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
                    {
                        FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);
                        FOdysseyVectorHandleSegment* handle = ( cubicVertex == cubicSegment->GetVertex( 0 ) ) ? cubicSegment->GetHandle( 0 ) :
                                                                                                                cubicSegment->GetHandle( 1 );

                        iPickedPointArray.push_back( handle );
                    }
                }
            }
        }
    }

    //TODO: should be called in RemoveVertex() / AddVertex()
    iPath->Invalidate();
}

static void
GroupPaintPickPoint( FOdysseyVectorGroupPaint* iPaintGroup
                   , std::vector<FOdysseyVectorPoint*>& iPickedPointArray
                   , double iSelectionRadius
                   , uint64 iSelectionFlags
                   , const FOdysseyPoint& iPointInTexture )
{
    std::list<FOdysseyVectorObject*>& childrenList = iPaintGroup->GetChildrenList();
    std::list<FOdysseyVectorObject*>::iterator it;

    for( it = childrenList.begin(); it != childrenList.end(); ++it )
    {
        FOdysseyVectorObject* childObject = *it;

        if( childObject->GetClass() == FOdysseyVectorPath::StaticClass() )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(childObject);

            PathPickPoint( path, iPickedPointArray, iSelectionRadius, iSelectionFlags, iPointInTexture );
        }
    }
}


void
UOdysseyPainterEditorVectorPathEditTool::OnMouseDownPickPoint( FOdysseyVectorEngine* iEngine
                                                             , FOdysseyVectorScene* iScene
                                                             , const FOdysseyPoint& iPointInTexture
                                                             , const FKey& iKey )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();

    mPickedPointArray.clear();

    for( std::list<FOdysseyVectorObject*>::iterator it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
    {
        FOdysseyVectorObject* selectedObject  = *it;

        if( selectedObject->GetClass() == FOdysseyVectorPath::StaticClass() )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(selectedObject);

            PathPickPoint( path, mPickedPointArray, PickingRadius, mPickingFlags, iPointInTexture );
        }

        if( selectedObject->GetClass() == FOdysseyVectorGroupPaint::StaticClass() )
        {
            FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(selectedObject);

            GroupPaintPickPoint( paintGroup, mPickedPointArray, PickingRadius, mPickingFlags, iPointInTexture );
        }
    }

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("VectorPathEditTool","Vector Path Edit Tool"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoPointPosition( iScene, mPickedPointArray );

        GUndo->StoreUndo( this, TUniquePtr<FOdysseyVectorUndo>(undo) );
    }
    GEditor->EndTransaction();
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FOdysseyPoint& iPointInTexture
                                                          , const FKey& iKey )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();

    mOldPointInTexture = ::ULIS::FVec2D( iPointInTexture.x, iPointInTexture.y );

    if( FSlateApplication::Get().GetModifierKeys().IsAltDown() )
    {
        if( mPickingFlags & FOdysseyVectorPath::PICK_POINT )
        {
            OnMouseDownDeletePoint( iEngine, iScene, iPointInTexture, iKey );
        }
    }
    else
    {
        OnMouseDownPickPoint( iEngine, iScene, iPointInTexture, iKey );
    }

    iScene->Update( 0 ); // updated invalidated objects

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW 
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );

    return true;
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return false;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return false;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    bool ret = UOdysseyPainterEditorVectorPathEditTool::OnMouseDownVector( vectorEngine, vectorScene, iPointInTexture,iKey  );
    GEditor->EndTransaction();
    return ret;
}

void
UOdysseyPainterEditorVectorPathEditTool::OnMouseHoverVector( FOdysseyVectorEngine* iEngine
                                                           , FOdysseyVectorScene* iScene
                                                           , const FOdysseyPoint& iPointInTexture )
{
    // we need the focus on the viewport for keyboard 
    //FSlateApplication::Get().SetKeyboardFocus( mViewportWidget.ToSharedRef() );

/*
    ::ULIS::FRectI formerRegion = ::ULIS::FRectI( iPointInTexture.x - iPointInTexture.deltaPosition.X - Radius
                                                , iPointInTexture.y - iPointInTexture.deltaPosition.Y - Radius
                                                , Radius * 2
                                                , Radius * 2 );
    ::ULIS::FRectI currentRegion = ::ULIS::FRectI( iPointInTexture.x - Radius
                                                 , iPointInTexture.y - Radius
                                                 , Radius * 2
                                                 , Radius * 2 );
    ::ULIS::FRectI finalRegion = currentRegion | formerRegion;
*/

    mPathEditHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

    // we also detect picking mode in hover events because we are not sure that the tool has keyboard focus.
    // It should not use too much CPU time.
    //DetectPickingMode();

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorPathEditTool::OnMouseHover( const FOdysseyPoint& iPointInTexture )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    UOdysseyPainterEditorVectorPathEditTool::OnMouseHoverVector( vectorEngine, vectorScene, iPointInTexture );
}

static FOdysseyVectorObject*
GetPointParentObject( FOdysseyVectorPoint *iPoint )
{
    FOdysseyVectorObject* object = nullptr;

    if( iPoint->GetClass() == FOdysseyVectorVertex::StaticClass() )
    {
        FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(iPoint);

        object = vertex->GetPath();
    }

    if( iPoint->GetClass() == FOdysseyVectorHandleSegment::StaticClass() )
    {
        FOdysseyVectorHandleSegment* handle = static_cast<FOdysseyVectorHandleSegment*>(iPoint);

        object = handle->GetOwner()->GetPath();
    }

    return object;
}

static ::ULIS::FRectD
DragPoint( FOdysseyVectorPoint *iPoint
         , double iWorldX
         , double iWorldY
         , double iDeltaX
         , double iDeltaY
         , uint64 iSelectionFlags )
{
    FOdysseyVectorObject* object = GetPointParentObject( iPoint );
    BLPoint localCoords = object->GetInverseWorldMatrix().mapPoint( iWorldX, iWorldY );
    BLPoint localVector = object->GetInverseWorldMatrix().mapVector( iDeltaX, iDeltaY );

    if( iSelectionFlags == FOdysseyVectorPath::PICK_HANDLE_POINT  )
    {
        FOdysseyVectorVertex* cubicVertex = static_cast<FOdysseyVectorVertex*>( iPoint );
        ::ULIS::FVec2D dif = { cubicVertex->GetX() - localCoords.x
                             , cubicVertex->GetY() - localCoords.y };

        cubicVertex->SetRadius( dif.Distance() );

        return cubicVertex->GetBoundingBox( false );
    }

    if( ( iSelectionFlags == FOdysseyVectorPath::PICK_POINT          )
     || ( iSelectionFlags == FOdysseyVectorPath::PICK_HANDLE_SEGMENT ) )
    {
        if( iPoint->GetClass() == FOdysseyVectorHandleSegment::StaticClass() )
        {
            FOdysseyVectorHandleSegment* segmentHandle = static_cast<FOdysseyVectorHandleSegment*>( iPoint );
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segmentHandle->GetOwner());

            segmentHandle->Set( iPoint->GetX() + localVector.x
                              , iPoint->GetY() + localVector.y );

            return cubicSegment->GetBoundingBox( false );
        }

        if( iPoint->GetClass() == FOdysseyVectorVertex::StaticClass() )
        {
            FOdysseyVectorVertex* cubicVertex = static_cast<FOdysseyVectorVertex*>( iPoint );

            cubicVertex->Set( iPoint->GetX() + localVector.x
                            , iPoint->GetY() + localVector.y );

            return cubicVertex->GetBoundingBox( false );
        }
    }

    return { 0, 0, 0, 0 };
}

void
UOdysseyPainterEditorVectorPathEditTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                          , FOdysseyVectorScene* iScene
                                                          , const FOdysseyPoint& iPointInTexture )
{
    FOdysseyVectorObject* selectedObject = iScene->GetLastSelected();
/*
    static ::ULIS::FRectI oldInvalidatedArea = { 0, 0, 0, 0 };
    ::ULIS::FRectD localInvalidatedArea = { 0, 0, 0, 0 };
    ::ULIS::FRectI invalidatedArea;
    ::ULIS::FRectI totalInvalidatedArea;
    BLPoint worldAreaP1;
    BLPoint worldAreaP2;

    bool inited = false;
*/
    mPathEditHUD->SetCursorPosition( iPointInTexture.x, iPointInTexture.y );

    for( int i = 0; i < mPickedPointArray.size(); i++ )
    {
        FOdysseyVectorPoint *selectedPoint = mPickedPointArray[i];
        ::ULIS::FRectD rect;

        rect = DragPoint( selectedPoint
                        , iPointInTexture.x
                        , iPointInTexture.y
                        // we don't use iPointInTexture.deltaPosition because for some reason,
                        // the readings are not good when a key is pressed.
                        , iPointInTexture.x - mOldPointInTexture.x
                        , iPointInTexture.y - mOldPointInTexture.y
                        , mPickingFlags );

        //localInvalidatedArea = ( inited == false ) ? rect : localInvalidatedArea | rect;

        //inited = true;
    }
/*
    worldAreaP1 = cubicPath->GetWorldMatrix().mapPoint( localInvalidatedArea.x, localInvalidatedArea.y );
    worldAreaP2 = cubicPath->GetWorldMatrix().mapPoint( localInvalidatedArea.x + localInvalidatedArea.w
                                                      , localInvalidatedArea.y + localInvalidatedArea.h );

    invalidatedArea = ::ULIS::FRectI::FromMinMax( ::ULIS::FMath::Min(worldAreaP1.x,worldAreaP2.x)
                                                , ::ULIS::FMath::Min(worldAreaP1.y,worldAreaP2.y)
                                                , ::ULIS::FMath::Max(worldAreaP1.x,worldAreaP2.x)
                                                , ::ULIS::FMath::Max(worldAreaP1.y,worldAreaP2.y) );

    totalInvalidatedArea = invalidatedArea | oldInvalidatedArea;

    oldInvalidatedArea = invalidatedArea;
*/
    mOldPointInTexture = ::ULIS::FVec2D( iPointInTexture.x, iPointInTexture.y );

    iScene->Update( FOdysseyVectorObject::FREQUENTUPDATES | FOdysseyVectorObject::KEEPINVALIDATED );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorPathEditTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    UOdysseyPainterEditorVectorPathEditTool::OnMouseDragVector( vectorEngine, vectorScene, iPointInTexture );
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                        , FOdysseyVectorScene* iScene
                                                        , const FOdysseyPoint& iPointInTexture
                                                        , const FKey& iKey )
{
    iScene->Update( 0 );

    iEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                   | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );

    return true;
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return false;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return false;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
    return UOdysseyPainterEditorVectorPathEditTool::OnMouseUpVector( vectorEngine, vectorScene, iPointInTexture, iKey );
}

void
UOdysseyPainterEditorVectorPathEditTool::Commit()
{

}

uint64
UOdysseyPainterEditorVectorPathEditTool::GetPickingFlags()
{
    return mPickingFlags;
}

void
UOdysseyPainterEditorVectorPathEditTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;
    
    PropertyChanged( PropertyChangedEvent.GetPropertyName() );

    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

    vectorEngine->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorPathEditTool::PropertyChanged( const FName& iPropertyName )
{
}

#undef LOCTEXT_NAMESPACE
