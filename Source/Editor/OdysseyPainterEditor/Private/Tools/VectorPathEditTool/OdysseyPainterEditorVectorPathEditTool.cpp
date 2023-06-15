// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditTool.h"
#include "Undo/OdysseyVectorUndoPointPosition.h"
#include "Undo/OdysseyVectorUndoPathAlter.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorPathEditTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathEditTool::~UOdysseyPainterEditorVectorPathEditTool()
{
}

UOdysseyPainterEditorVectorPathEditTool::UOdysseyPainterEditorVectorPathEditTool()
    : mCubicPathHUD( FOdysseyVectorHUDPathCubic::VIEW_PATH
                   | FOdysseyVectorHUDPathCubic::VIEW_POINT )
    , mSelectionFlags ( FOdysseyVectorPath::PICK_POINT )
    , Radius(10.0f)
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.VectoEdit64");

    mPickingHUD.SetRadius( Radius );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPathEditTool::UnloadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->RemoveHUD(&mCubicPathHUD);
    iEngine->RemoveHUD(&mPickingHUD);

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

void
UOdysseyPainterEditorVectorPathEditTool::LoadVector( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    // we need the focus on the viewport for keyboard 
    mViewportWidget = GetEditorAs<FOdysseyPainterEditor>()->GetGUI()->GetViewportTab()->GetViewport()->GetViewportWidget();

    // commented out: moved to hover function
    //FSlateApplication::Get().SetKeyboardFocus( mViewportWidget.ToSharedRef() );

    iEngine->ClearHUD();
    iEngine->AddHUD(&mCubicPathHUD);
    iEngine->AddHUD(&mPickingHUD);

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnKeyDownVector( FOdysseyVectorEngine* iEngine
                                                        , FOdysseyVectorScene* iScene
                                                        , const FKey& iKey )
{
    // then detect which keys are pressed and set display mode
    if ( FSlateApplication::Get().GetModifierKeys().IsControlDown() )
    {
        mSelectionFlags = FOdysseyVectorPath::PICK_HANDLE_SEGMENT;
        mCubicPathHUD.SetDisplayMode( FOdysseyVectorHUDPathCubic::VIEW_PATH
                                    | FOdysseyVectorHUDPathCubic::VIEW_POINT
                                    | FOdysseyVectorHUDPathCubic::VIEW_HANDLE_SEGMENT );
    }

    if ( FSlateApplication::Get().GetModifierKeys().IsShiftDown() )
    {
        mSelectionFlags = FOdysseyVectorPath::PICK_HANDLE_POINT;
        mCubicPathHUD.SetDisplayMode( FOdysseyVectorHUDPathCubic::VIEW_PATH
                                    | FOdysseyVectorHUDPathCubic::VIEW_POINT
                                    | FOdysseyVectorHUDPathCubic::VIEW_HANDLE_POINT );
    }

    if ( FSlateApplication::Get().GetModifierKeys().IsAltDown() )
    {
        mSelectionFlags = FOdysseyVectorPath::PICK_POINT;
        mCubicPathHUD.SetDisplayMode( FOdysseyVectorHUDPathCubic::VIEW_PATH
                                    | FOdysseyVectorHUDPathCubic::VIEW_POINT );
    }

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );

    return false;
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnKeyUpVector( FOdysseyVectorEngine* iEngine
                                                      , FOdysseyVectorScene* iScene
                                                      , const FKey& iKey )
{
    // first reset display mode
    mSelectionFlags = FOdysseyVectorPath::PICK_POINT;
    mCubicPathHUD.SetDisplayMode( FOdysseyVectorHUDPathCubic::VIEW_PATH
                                | FOdysseyVectorHUDPathCubic::VIEW_POINT );

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );

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
                           , Radius
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

        if( selectedObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(selectedObject);

            PathPickPoint( path, mPickedPointArray, Radius, mSelectionFlags, iPointInTexture );
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
        if( mCubicPathHUD.GetDisplayMode() & FOdysseyVectorHUDPathCubic::VIEW_POINT )
        {
            OnMouseDownDeletePoint( iEngine, iScene, iPointInTexture, iKey );
        }
    }
    else
    {
        OnMouseDownPickPoint( iEngine, iScene, iPointInTexture, iKey );
    }

    iScene->Update( 0 ); // updated invalidated objects
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW | FOdysseyVectorScene::SIGNAL_OBJECT_MODIFIED );

    return true;
}

void
UOdysseyPainterEditorVectorPathEditTool::OnMouseHoverVector( FOdysseyVectorEngine* iEngine
                                                           , FOdysseyVectorScene* iScene
                                                           , const FOdysseyPoint& iPointInTexture )
{
    // we need the focus on the viewport for keyboard 
    FSlateApplication::Get().SetKeyboardFocus( mViewportWidget.ToSharedRef() );

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

    mPickingHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );

    // we also detect picking mode in hover events because we are not sure that the tool has keyboard focus.
    // It should not use too much CPU time.
    //DetectPickingMode();

    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
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

        object = handle->GetParent()->GetPath();
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

    if ( iSelectionFlags == FOdysseyVectorPath::PICK_HANDLE_POINT  )
    {
        if( iPoint->GetClass() == FOdysseyVectorVertex::StaticClass() )
        {
            FOdysseyVectorVertex* cubicVertex = static_cast<FOdysseyVectorVertex*>( iPoint );
            ::ULIS::FVec2D dif = { cubicVertex->GetX() - localCoords.x
                                 , cubicVertex->GetY() - localCoords.y };

            cubicVertex->SetRadius( dif.Distance() );

            return cubicVertex->GetBoundingBox( false );
        }
    }

    if( iSelectionFlags == FOdysseyVectorPath::PICK_HANDLE_SEGMENT )
    {
        if( iPoint->GetClass() == FOdysseyVectorHandleSegment::StaticClass() )
        {
            FOdysseyVectorHandleSegment* segmentHandle = static_cast<FOdysseyVectorHandleSegment*>( iPoint );
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segmentHandle->GetParent());

            segmentHandle->Set( iPoint->GetX() + localVector.x
                              , iPoint->GetY() + localVector.y );

            return cubicSegment->GetBoundingBox( false );
        }
    }

    if( iSelectionFlags == FOdysseyVectorPath::PICK_POINT )
    {
        // Segment handles are also included in the selection. Check we are on a vertex.
        if( iPoint->GetClass() == FOdysseyVectorVertex::StaticClass() )
        {
            FOdysseyVectorVertex* cubicVertex = static_cast<FOdysseyVectorVertex*>( iPoint );
            std::list<FOdysseyVectorSegment*> segmentList = cubicVertex->GetSegmentList();

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
    mPickingHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );

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
                        , mSelectionFlags );

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
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

bool
UOdysseyPainterEditorVectorPathEditTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                        , FOdysseyVectorScene* iScene
                                                        , const FOdysseyPoint& iPointInTexture
                                                        , const FKey& iKey )
{
    iScene->Update( 0 );
    iScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW | FOdysseyVectorScene::SIGNAL_OBJECT_MODIFIED );

    return true;
}

void
UOdysseyPainterEditorVectorPathEditTool::Commit()
{

}

void
UOdysseyPainterEditorVectorPathEditTool::PropertyChanged( const FName& iPropertyName )
{
    mPickingHUD.SetRadius( Radius );
}

#undef LOCTEXT_NAMESPACE
