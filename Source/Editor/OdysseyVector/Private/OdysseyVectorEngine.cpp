#include "OdysseyVectorEngine.h"
//#include <future>

FOdysseyVectorEngine::~FOdysseyVectorEngine()
{
}

FOdysseyVectorEngine::FOdysseyVectorEngine( FOdysseyVectorScene* iScene
                                          , uint32 iPreferredWidth
                                          , uint32 iPreferredHeight )
    : FOdysseyVectorObject( "Engine" )
    , mSelectionSpace( nullptr )
    , mInvalidTileMap( 64, iPreferredWidth, iPreferredHeight )
    , mPreferredWidth( iPreferredWidth )
    , mPreferredHeight( iPreferredHeight )
{
    // Configure the number of threads to use.
    mProcessorCount = FPlatformMisc::NumberOfCoresIncludingHyperthreads();

    SetScene( iScene );
}

bool
FOdysseyVectorEngine::HasBaseClass( uint32 iBaseClassID )
{
    if( mStaticClass == iBaseClassID )
    {
        return true;
    }

    return FOdysseyVectorObject::HasBaseClass( iBaseClassID );
}

void
FOdysseyVectorEngine::SetSelectionSpace( FOdysseyVectorGroup* iSelectionSpace )
{
   mSelectionSpace = iSelectionSpace;
}

FOdysseyVectorGroup*
FOdysseyVectorEngine::GetSelectionSpace()
{
    return mSelectionSpace;
}

FULISInvalidTileMap&
FOdysseyVectorEngine::GetInvalidTileMap()
{
    return mInvalidTileMap;
}

BLImage*
FOdysseyVectorEngine::GetBLMask()
{
    return mBLMask;
}

void
FOdysseyVectorEngine::SetBLMask( BLImage* iBLMask )
{
    mBLMask = iBLMask;
}

uint32
FOdysseyVectorEngine::GetPreferredWidth()
{
    return mPreferredWidth;
}

uint32
FOdysseyVectorEngine::GetPreferredHeight()
{
    return mPreferredHeight;
}

void
FOdysseyVectorEngine::SetScene( FOdysseyVectorScene* iScene )
{
    mScene = iScene;

    // todo: replace with RemoveAllChildren();
    mChildrenList.clear();
    AppendChild( iScene );

    mScene->SetEngine( this );

    ResetHUD();
}

FOdysseyVectorScene*
FOdysseyVectorEngine::GetScene()
{
    return mScene;
}

void
FOdysseyVectorEngine::RenderHUD( BLContext* iBLContext/*FOdysseyVectorScene* iScene */ )
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyVectorEngine::RenderHUD);
    std::list<FOdysseyVectorObject*> selectedObjectList = mScene->GetSelectedObjectList();

    //UseImage( iBLImage );

    iBLContext->save();
    iBLContext->resetMatrix();
    //mBLContext->setCompOp( BL_COMP_OP_SRC_COPY );
    //mBLContext->setFillAlpha( 0.0f );
    iBLContext->clearAll();

    for( FOdysseyVectorHUD *hud : GetHUDList() )
    {
        hud->Draw( iBLContext, mScene, 0 );
    }

    iBLContext->restore();

    iBLContext->flush(BL_CONTEXT_FLUSH_SYNC);

    //UseImage( &mDefaultBLImage );
}

void
FOdysseyVectorEngine::SelectAllInSelectionSpace()
{
    // TODO: set scene as the default selection space
    FOdysseyVectorGroup* selectionSpace = mSelectionSpace ? mSelectionSpace : mScene;
    std::list<FOdysseyVectorObject*>& childrenList = selectionSpace->GetChildrenList();

    mScene->ClearSelection();

    for( std::list<FOdysseyVectorObject*>::iterator it = childrenList.begin(); it != childrenList.end(); ++it )
    {
        FOdysseyVectorObject *child = (*it);

        mScene->Select( child );
    }
}

void
FOdysseyVectorEngine::Render( BLContext* iBLContext, uint64 iDrawingFlags )
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyVectorEngine::Render);
    BLImage* image = iBLContext->targetImage();
    BLImageData imageData;

    image->getData( &imageData );

    // for drawing polygones (textured)
    if( mHorizontalLineBuffer.size() != imageData.size.h )
    {
        mHorizontalLineBuffer.resize( imageData.size.h );
    }

    if( mInvalidationFlags )
    {
        mScene->Draw( iBLContext, 1.0f, iDrawingFlags );

        iBLContext->flush(BL_CONTEXT_FLUSH_SYNC);
    }

    mInvalidationFlags = 0;
}

void
FOdysseyVectorEngine::RecursiveErase( FOdysseyVectorObject* iObject
                                    , std::vector<FOdysseyVectorObject*>& iAddedObjectArray
                                    , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                                    , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray
                                    , std::vector<FOdysseyVectorObject*>& iRemovedObjectArray
                                    , std::vector<FOdysseyVectorVertex*>& iRemovedVertexArray
                                    , std::vector<FOdysseyVectorSegment*>& iRemovedSegmentArray
                                    , const ::ULIS::FRectD &iRoi
                                    , bool iSelectedOnly )
{
    for( std::list<FOdysseyVectorObject*>::iterator it = iObject->GetChildrenList().begin(); it != iObject->GetChildrenList().end(); ++it )
    {
        FOdysseyVectorObject *child = (*it);

        RecursiveErase( child
                      , iAddedObjectArray
                      , iAddedVertexArray
                      , iAddedSegmentArray
                      , iRemovedObjectArray
                      , iRemovedVertexArray
                      , iRemovedSegmentArray
                      , iRoi
                      , iSelectedOnly );
    }

    if( iObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) ) 
    {
        FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(iObject);

        if( iSelectedOnly == true )
        {
            if( path->IsSelected() == true )
            {
                if( path->Erase( iRoi, iAddedVertexArray, iAddedSegmentArray, iRemovedVertexArray, iRemovedSegmentArray ) )
                {
                    iRemovedObjectArray.push_back( path );
                }
            }
        }
        else
        {
            if( path->Erase( iRoi, iAddedVertexArray, iAddedSegmentArray, iRemovedVertexArray, iRemovedSegmentArray ) )
            {
                iRemovedObjectArray.push_back( path );
            }
        }
    }
}

void
FOdysseyVectorEngine::Erase( FOdysseyVectorScene* iScene
                           , std::vector<FOdysseyVectorObject*>& iAddedObjectArray
                           , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                           , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray
                           , std::vector<FOdysseyVectorObject*>& iRemovedObjectArray
                           , std::vector<FOdysseyVectorVertex*>& iRemovedVertexArray
                           , std::vector<FOdysseyVectorSegment*>& iRemovedSegmentArray
                           , ::ULIS::FRectD &iRoi
                           , bool iSelectedOnly )
{
    RecursiveErase( iScene
                  , iAddedObjectArray
                  , iAddedVertexArray
                  , iAddedSegmentArray
                  , iRemovedObjectArray
                  , iRemovedVertexArray
                  , iRemovedSegmentArray
                  , iRoi
                  , iSelectedOnly  );

    for( int i = 0; i < iRemovedObjectArray.size(); i++ )
    {
        if( iRemovedObjectArray[i]->GetChildrenList().size() == 0 )
        {
            iRemovedObjectArray[i]->GetParent()->RemoveChild( iRemovedObjectArray[i] );
        }
    }

    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );
}

void
FOdysseyVectorEngine::RecursiveEraseSections( FOdysseyVectorObject* iObject
                                            , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                                            , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray
                                            , std::vector<FOdysseyVectorObject*>& iRemovedObjectArray
                                            , std::vector<FOdysseyVectorVertex*>& iRemovedVertexArray
                                            , std::vector<FOdysseyVectorSegment*>& iRemovedSegmentArray
                                            , bool iSelectedOnly )
{
    for( std::list<FOdysseyVectorObject*>::iterator it = iObject->GetChildrenList().begin(); it != iObject->GetChildrenList().end(); ++it )
    {
        FOdysseyVectorObject *child = (*it);

        RecursiveEraseSections( child
                              , iAddedVertexArray
                              , iAddedSegmentArray
                              , iRemovedObjectArray
                              , iRemovedVertexArray
                              , iRemovedSegmentArray
                              , iSelectedOnly );
    }
/*
    if( iObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) ) 
    {
        FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(iObject);
        std::vector<FOdysseyVectorSegment*> removedSegmentArray;

        path->PickSegments( iRemovedObjectArray );
    }
*/
    if( iObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) ) 
    {
        FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(iObject);
        std::vector<FOdysseyVectorSection*> trimmedSectionArray;

        if( iSelectedOnly == true )
        {
            if( paintGroup->IsSelected() == true )
            {
                paintGroup->PickSectionLessPaths( iRemovedObjectArray );

                for( int i = 0; i < iRemovedObjectArray.size(); i++ )
                { 
                    iRemovedObjectArray[i]->GetParent()->RemoveChild( iRemovedObjectArray[i] );
                }

                paintGroup->PickSections( trimmedSectionArray );
                paintGroup->EraseSections( trimmedSectionArray
                                         , iRemovedVertexArray
                                         , iRemovedSegmentArray
                                         , iAddedVertexArray
                                         , iAddedSegmentArray );
            }
        }
        else
        {

            paintGroup->PickSectionLessPaths( iRemovedObjectArray );

            for( int i = 0; i < iRemovedObjectArray.size(); i++ )
            { 
                iRemovedObjectArray[i]->GetParent()->RemoveChild( iRemovedObjectArray[i] );
            }

            paintGroup->PickSections( trimmedSectionArray );
            paintGroup->EraseSections( trimmedSectionArray
                                     , iRemovedVertexArray
                                     , iRemovedSegmentArray
                                     , iAddedVertexArray
                                     , iAddedSegmentArray );
        }
    }
}

void
FOdysseyVectorEngine::EraseSections( FOdysseyVectorScene* iScene
                                   , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                                   , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray
                                   , std::vector<FOdysseyVectorObject*>& iRemovedObjectArray
                                   , std::vector<FOdysseyVectorVertex*>& iRemovedVertexArray
                                   , std::vector<FOdysseyVectorSegment*>& iRemovedSegmentArray
                                   , bool iSelectedOnly )
{
    RecursiveEraseSections( iScene
                          , iAddedVertexArray
                          , iAddedSegmentArray
                          , iRemovedObjectArray
                          , iRemovedVertexArray
                          , iRemovedSegmentArray
                          , iSelectedOnly  );

    for( int i = 0; i < iRemovedObjectArray.size(); i++ )
    {
        if( iRemovedObjectArray[i]->GetChildrenList().size() == 0 )
        {
            iRemovedObjectArray[i]->GetParent()->RemoveChild( iRemovedObjectArray[i] );
        }
    }

    iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );
}

static void
RecursivePickPoints( FOdysseyVectorObject* iObject
                   , double iWorldX
                   , double iWorldY
                   , double iRadius
                   , std::vector<FOdysseyVectorPoint*>& oPickedPointArray
                   , uint64 iPickingFlags )
{
    if( iObject->GetClass() == FOdysseyVectorPath::StaticClass() )
    {
        FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(iObject);
        BLMatrix2D& inverseWorldMatrix = iObject->GetInverseWorldMatrix();
        BLPoint localRadius = inverseWorldMatrix.mapVector( 0.7071f * iRadius, 0.7071f * iRadius );
        BLPoint localPoint = inverseWorldMatrix.mapPoint( iWorldX, iWorldY );
        ::ULIS::FRectD pathBBox = path->GetBBox( false );

        // get sure we hit the box be enlarging it with the picking circle radius value.
        // otherwise we might not be able to pick points located at the box's boundaries.
        pathBBox.x -=   localRadius.x;
        pathBBox.y -=   localRadius.y;
        pathBBox.w += ( localRadius.x * 2 );
        pathBBox.h += ( localRadius.y * 2 );

        if( pathBBox.HitTest( ::ULIS::FVec2D( localPoint.x, localPoint.y ) ) == true )
        {
            path->PickPoint( iWorldX, iWorldY, iRadius, oPickedPointArray, iPickingFlags );
        }
    }

    for( std::list<FOdysseyVectorObject*>::iterator it = iObject->GetChildrenList().begin(); it != iObject->GetChildrenList().end(); ++it )
    {
        FOdysseyVectorObject* child = (*it);

        RecursivePickPoints( child, iWorldX, iWorldY, iRadius, oPickedPointArray, iPickingFlags );
    }
}

void
FOdysseyVectorEngine::PickPoints( FOdysseyVectorScene* iScene
                                , bool iRestrictToSelection
                                , double iX
                                , double iY
                                , double iRadius
                                , std::vector<FOdysseyVectorPoint*>& oPickedPointArray
                                , uint64 iPickingFlags )
{
    if( iRestrictToSelection )
    {
        std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();

        for( std::list<FOdysseyVectorObject*>::iterator it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
        {
            FOdysseyVectorObject* selectedObject = (*it);

            if( selectedObject->HasSelectedAncestor() == false )
            {
                RecursivePickPoints( selectedObject, iX, iY, iRadius, oPickedPointArray, iPickingFlags );
            }
        }
    }
    else
    {
        RecursivePickPoints( iScene, iX, iY, iRadius, oPickedPointArray, iPickingFlags );
    }
}

FOdysseyVectorVertex*
FOdysseyVectorEngine::Stitch( FOdysseyVectorVertex* iVertexA
                            , FOdysseyVectorVertex* iVertexB
                            , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray
                            , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray
                            , bool iSmooth )
{
    if( ( iVertexA->GetSegmentCount() == 1 )
     && ( iVertexB->GetSegmentCount() == 1 )
     && ( iVertexA->GetPath() == iVertexB->GetPath() ) )
    {
        ::ULIS::FVec2D& vertexACoords = iVertexA->GetCoords();
        ::ULIS::FVec2D& vertexBCoords = iVertexB->GetCoords();
        ::ULIS::FVec2D averageCoords = ( vertexACoords + vertexBCoords ) * 0.5f;
        double vertexARadius = iVertexA->GetRadius();
        double vertexBRadius = iVertexB->GetRadius();
        double averageRadius = ( vertexARadius + vertexBRadius ) * 0.5f;
        FOdysseyVectorPath* path = iVertexA->GetPath();

        if( path->GetClass() == FOdysseyVectorPath::StaticClass() )
        {
            FOdysseyVectorPath* cubicPath = static_cast<FOdysseyVectorPath*>(path);
            FOdysseyVectorSegmentCubic* vertexBSegment = static_cast<FOdysseyVectorSegmentCubic*>(iVertexB->GetFirstSegment());
            FOdysseyVectorSegmentCubic* vertexASegment = static_cast<FOdysseyVectorSegmentCubic*>(iVertexA->GetFirstSegment());
            FOdysseyVectorVertex* prevVertex = static_cast<FOdysseyVectorVertex*>(vertexASegment->GetOtherVertex( iVertexA ));
            FOdysseyVectorVertex* nextVertex = static_cast<FOdysseyVectorVertex*>(vertexBSegment->GetOtherVertex( iVertexB ));
            FOdysseyVectorVertex* knotVertex = new FOdysseyVectorVertex( cubicPath, averageCoords.x, averageCoords.y, averageRadius );
            FOdysseyVectorSegmentCubic* newCubicSegment[2] = { new FOdysseyVectorSegmentCubic( cubicPath
                                                                                            ,  prevVertex
                                                                                            ,  knotVertex
                                                                                            ,  true ),
                                                               new FOdysseyVectorSegmentCubic( cubicPath
                                                                                            ,  knotVertex
                                                                                            ,  nextVertex
                                                                                            ,  true ) };

            newCubicSegment[0]->GetHandle(0)->Set( vertexASegment->GetHandle(prevVertex)->GetCoords() );
            newCubicSegment[0]->GetHandle(1)->Set( vertexASegment->GetHandle(iVertexA  )->GetCoords() );
            newCubicSegment[1]->GetHandle(0)->Set( vertexBSegment->GetHandle(iVertexB  )->GetCoords() );
            newCubicSegment[1]->GetHandle(1)->Set( vertexBSegment->GetHandle(nextVertex)->GetCoords() );

            path->RemoveSegment( vertexASegment );
            path->RemoveSegment( vertexBSegment );
            path->RemoveVertex( iVertexA );
            path->RemoveVertex( iVertexB );

            oRemovedSegmentArray.push_back( vertexASegment );
            oRemovedSegmentArray.push_back( vertexBSegment );

            path->AddVertex( knotVertex );
            path->AddSegment( newCubicSegment[0] );
            path->AddSegment( newCubicSegment[1] );

            oAddedSegmentArray.push_back( newCubicSegment[0] );
            oAddedSegmentArray.push_back( newCubicSegment[1] );
/*
            if( iSmooth )
            {
                FOdysseyVectorPath::SmoothSegments( knotVertex, false, true );
            }
*/
            path->InvalidateAllSegments();

            //mScene->Update( 0 );

            return knotVertex;
        }
    }

    return nullptr;
}

static void
RecursivePickSegments( FOdysseyVectorObject* iObject
                     , double iX
                     , double iY
                     , double iRadius
                     , std::vector<FOdysseyVectorSegment*>& oPickedSegmentArray
                     , std::vector<double>* oDistanceArray )
{
    if( iObject->GetClass() == FOdysseyVectorPath::StaticClass() )
    {
        FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(iObject);
        BLPoint localVector = iObject->GetInverseWorldMatrix().mapVector( 0.7071f, 0.7071f );
        ::ULIS::FVec2D factor = { localVector.x * iRadius, localVector.y * iRadius };
        double localRadius = factor.Distance();
        BLPoint localPoint = iObject->GetInverseWorldMatrix().mapPoint( iX, iY );
        ::ULIS::FRectD pathBBox = path->GetBBox( false );

        // get sure we hit the box be enlarging it with the picking circle radius value.
        // otherwise we might not be able to pick points located at the box's boundaries.
        pathBBox.x -=   localRadius;
        pathBBox.y -=   localRadius;
        pathBBox.w += ( localRadius * 2 );
        pathBBox.h += ( localRadius * 2 );

        if( pathBBox.HitTest( ::ULIS::FVec2D( localPoint.x, localPoint.y ) ) == true )
        {
            std::list<FOdysseyVectorSegment*>& segmentList = path->GetSegmentList();

            for( std::list<FOdysseyVectorSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it )
            {
                FOdysseyVectorSegment* segment = (*it);
                ::ULIS::FRectD segmentBBox = segment->GetBoundingBox( false );

                segmentBBox.x -=   localRadius;
                segmentBBox.y -=   localRadius;
                segmentBBox.w += ( localRadius * 2 );
                segmentBBox.h += ( localRadius * 2 );

                if( segmentBBox.HitTest( ::ULIS::FVec2D( localPoint.x, localPoint.y ) ) == true )
                {
                    double smallestDistance;

                    if( segment->ProximityTest( localPoint.x, localPoint.y, localRadius, smallestDistance ) )
                    {
                        oPickedSegmentArray.push_back( segment );

                        if( oDistanceArray )
                        {
                            oDistanceArray->push_back( smallestDistance );
                        }
                    } 
                }
            }
        }
    }

    for( std::list<FOdysseyVectorObject*>::iterator it = iObject->GetChildrenList().begin(); it != iObject->GetChildrenList().end(); ++it )
    {
        FOdysseyVectorObject* child = (*it);

        RecursivePickSegments( child, iX, iY, iRadius, oPickedSegmentArray, oDistanceArray );
    }
}

void
FOdysseyVectorEngine::PickSegments( FOdysseyVectorScene* iScene
                                  , bool iRestrictToSelection
                                  , double iX
                                  , double iY
                                  , double iRadius
                                  , std::vector<FOdysseyVectorSegment*>& oPickedSegmentArray
                                  , std::vector<double>* oDistance )
{
    if( iRestrictToSelection )
    {
        std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetSelectedObjectList();

        for( std::list<FOdysseyVectorObject*>::iterator it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
        {
            FOdysseyVectorObject* selectedObject = (*it);

            if( selectedObject->HasSelectedAncestor() == false )
            {
                RecursivePickSegments( selectedObject, iX, iY, iRadius, oPickedSegmentArray, oDistance );
            }
        }
    }
    else
    {
        RecursivePickSegments( iScene, iX, iY, iRadius, oPickedSegmentArray, oDistance );
    }
}

// static
void
FOdysseyVectorEngine::RecursivePick( FOdysseyVectorGroup* iSelectionSpace
                                   , FOdysseyVectorObject* iObj
                                   , std::vector<FOdysseyVectorObject*>& oSelectedObjectArray
                                   , const ::ULIS::FRectD& iRoi
                                   , uint32 iSelectionFlags )
{
    FOdysseyVectorObject* pickedObject = ( iObj != iSelectionSpace ) ? iObj->Pick( iSelectionSpace, iRoi, iSelectionFlags ) : nullptr;

    for( std::list<FOdysseyVectorObject*>::iterator it = iObj->GetChildrenList().begin(); it != iObj->GetChildrenList().end(); ++it )
    {
        FOdysseyVectorObject* child = (*it);

        RecursivePick( iSelectionSpace, child, oSelectedObjectArray, iRoi, iSelectionFlags );
    }

    if( pickedObject )
    {
        oSelectedObjectArray.push_back( pickedObject );
    }
}

void
FOdysseyVectorEngine::Pick( FOdysseyVectorScene* iScene
                          , const ::ULIS::FRectD& iRoi
                          , std::vector<FOdysseyVectorObject*>& oPickedObjectArray
                          , uint32 iSelectionFlags )
{
    RecursivePick( mSelectionSpace ? mSelectionSpace : iScene, iScene, oPickedObjectArray, iRoi, iSelectionFlags );
/*
    if( iSelectionFlags & FOdysseyVectorObject::PICK_MASK_BASED )
    {
        UseColorImage();
    }
*/
}

std::list<FOdysseyVectorHUD*>&
FOdysseyVectorEngine::GetHUDList()
{
    return mHUDList;
}

void
FOdysseyVectorEngine::AddHUD( FOdysseyVectorHUD* iHUDObject )
{
    GetHUDList().push_back( iHUDObject );
}

void
FOdysseyVectorEngine::RemoveHUD( FOdysseyVectorHUD* iHUDObject )
{
    GetHUDList().remove( iHUDObject );
}

void
FOdysseyVectorEngine::ClearHUD()
{
    GetHUDList().clear();
}

void
FOdysseyVectorEngine::ResetHUD()
{
    for( std::list<FOdysseyVectorHUD*>::iterator hit = GetHUDList().begin(); hit != GetHUDList().end(); ++hit )
    {
        FOdysseyVectorHUD *hud = (*hit);

        hud->Reset( mScene );
    }
}

FOdysseyVectorEngine::FSignalDelegate&
FOdysseyVectorEngine::OnSignalDelegate()
{
    static FSignalDelegate onSignalDelegate;

    return onSignalDelegate;
}

void
FOdysseyVectorEngine::Signal( uint64 iSignalFlags )
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyVectorEngine::Signal);
    // Force invalidation when we need redrawing
    // This should be removed once we have per-rectangle invalidation
    if( iSignalFlags & FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW )
    {
        Invalidate();
    }

    OnSignalDelegate().Broadcast( mScene, iSignalFlags );
}

void
FOdysseyVectorEngine::TraceLine ( int32 iX0
                                , int32 iY0
                                , double iU0
                                , double iV0
                                , int32 iX1
                                , int32 iY1
                                , double iU1
                                , double iV1
                                , uint32 iImageWidth
                                , uint32 iImageHeight )
{
    int32 dx  = ( iX1 - iX0 ),
          ddx = abs ( dx ),
          dy  = ( iY1 - iY0 ),
          ddy = abs ( dy ),
          dd  = ( ddx > ddy ) ? ddx : ddy;
    double du  = iU1  - iU0, pu = ( dd ) ? ( du / dd ) : 0.0f;
    double dv  = iV1  - iV0, pv = ( dd ) ? ( dv / dd ) : 0.0f;
    int px = ( dx > 0 ) ? 1 : -1, 
        py = ( dy > 0 ) ? 1 : -1;
    int32 x = iX0,
          y = iY0;
    double u = iU0;
    double v = iV0;
    int cumul = 0;

    if( ddx > ddy )
    {
        for( int i = 0; i <= ddx; i++ )
        {
            if( ( y >= 0 ) && ( y < (int32) iImageHeight ) )
            {
                uint32 offset = ( y * iImageWidth ) + x;

                if( mHorizontalLineBuffer[y].inited == 0 )
                {
                    mHorizontalLineBuffer[y].inited = 1;

                    mHorizontalLineBuffer[y].x0 = mHorizontalLineBuffer[y].x1 = x;
                    mHorizontalLineBuffer[y].u0 = mHorizontalLineBuffer[y].u1 = u;
                    mHorizontalLineBuffer[y].v0 = mHorizontalLineBuffer[y].v1 = v;
                }
                else
                {
                    if( x < mHorizontalLineBuffer[y].x0 )
                    {
                        mHorizontalLineBuffer[y].x0 = x;
                        mHorizontalLineBuffer[y].u0 = u;
                        mHorizontalLineBuffer[y].v0 = v;
                    }

                    if( x > mHorizontalLineBuffer[y].x1 )
                    {
                        mHorizontalLineBuffer[y].x1 = x;
                        mHorizontalLineBuffer[y].u1 = u;
                        mHorizontalLineBuffer[y].v1 = v;
                    }

                    mHorizontalLineBuffer[y].inited = 2;
                }
            }

            cumul += ddy;
            x     += px;
            u     += pu;
            v     += pv;

            if( cumul >= ddx )
            {
                cumul -= ddx;
                y     += py;
            }
        }
    }
    else
    {
        for( int i = 0x00; i <= ddy; i++ )
        {
            if( ( y >= 0x00 ) && ( y < (int32) iImageHeight ) )
            {
                uint32 offset = ( y * iImageWidth ) + x;

                if( mHorizontalLineBuffer[y].inited == 0 )
                {
                     mHorizontalLineBuffer[y].inited = 1;

                     mHorizontalLineBuffer[y].x0 = mHorizontalLineBuffer[y].x1 = x;
                     mHorizontalLineBuffer[y].u0 = mHorizontalLineBuffer[y].u1 = u;
                     mHorizontalLineBuffer[y].v0 = mHorizontalLineBuffer[y].v1 = v;
                }
                else
                {
                    if( x < mHorizontalLineBuffer[y].x0 )
                    {
                        mHorizontalLineBuffer[y].x0 = x;
                        mHorizontalLineBuffer[y].u0 = u;
                        mHorizontalLineBuffer[y].v0 = v;
                    }

                    if( x > mHorizontalLineBuffer[y].x1 )
                    {
                        mHorizontalLineBuffer[y].x1 = x;
                        mHorizontalLineBuffer[y].u1 = u;
                        mHorizontalLineBuffer[y].v1 = v;
                    }

                    mHorizontalLineBuffer[y].inited = 2;
                }
            }

            cumul += ddx;
            y     += py;
            u     += pu;
            v     += pv;

            if( cumul >= ddy )
            {
                cumul -= ddy;
                x     += px;
            }
        }
    }
}

// Macro for faster execution. Indeed, an inline function is not guaranteed to be inlined.
#define GETPIXEL(PIXELS,WIDTH,HEIGHT,BITSPERPIXEL,ALPHAONLY,U,V,R,G,B,A)   \
    switch ( BITSPERPIXEL )                                                \
    {                                                                      \
        case 32 :                                                          \
        {                                                                  \
            unsigned char (*PIXELS32)[4] = ( unsigned char (*)[4]) PIXELS; \
            int32 TEXU = U * ( WIDTH  - 1 );                               \
            int32 TEXV = V * ( HEIGHT - 1 );                               \
            uint32 TEXOFFSET = ( TEXV * WIDTH ) + TEXU;                    \
                                                                           \
            if( ALPHAONLY == false )                                       \
            {                                                              \
                B = PIXELS32[TEXOFFSET][0];                                \
                G = PIXELS32[TEXOFFSET][1];                                \
                R = PIXELS32[TEXOFFSET][2];                                \
            }                                                              \
                                                                           \
            A = PIXELS32[TEXOFFSET][3];                                    \
        }                                                                  \
        break;                                                             \
                                                                           \
        default :                                                          \
        break;                                                             \
    }                                                                      \


void
FOdysseyVectorEngine::TraceHorizontalLine ( int32  iLineNumber
                                          , double iOpacity
                                          , int8*  iImagePixelData
                                          , uint32 iImageWidth
                                          , uint32 iImageHeight
                                          , int32  iImageBitsPerPixel
                                          , const  FColor& iColor
                                          // Temp
                                          , int8*  iBrushPixelData
                                          , uint32 iBrushWidth
                                          , uint32 iBrushHeight
                                          , int32  iBrushBitsPerPixel
                                          , bool   iBrushAlphaOnly )
{
    FHorizontalLine *hline = &mHorizontalLineBuffer[iLineNumber];
    int32 x0 = hline->x0,
          x1 = hline->x1;
    double u0 = hline->u0;
    double v0 = hline->v0;
    int32 dx = x1 - x0;
    int32 x = x0;
    double du  = hline->u1 - hline->u0, pu = ( dx ) ? ( du / dx ) : 0.0f;
    double dv  = hline->v1 - hline->v0, pv = ( dx ) ? ( dv / dx ) : 0.0f;
    double u = u0;
    double v = v0;
    double opacityFactor = iOpacity / 255.0f;
    uint32 offset = ( iLineNumber * iImageWidth );
    int32 screenx = dx;
    unsigned char BR = iColor.R, BG = iColor.G, BB = iColor.B, BA = iColor.A;

    // Clipping. Note: x1 MUST be > 0, which is checked before the call to this function
    if( x0 < 0 )
    {
        int32 clippingW = -x0;

        x = 0;
        u  += ( clippingW * pu );
        v  += ( clippingW * pv );

        screenx  = dx - clippingW;
    }

    // Commented out: we don't drow from edge-to-edge, we stop 1 pixel before to prevent overlapping,
    // which would lead to double stroke and would produce artefact when alpha is semi-transparent.
    //for( int i = 0; i <= ddx; i++ )
    for( int i = 0; ( i < screenx ) && ( x < (int)iImageWidth /* clipping */ ); i++ )
    {
        if( ( x >= 0 ) && ( x < (int32) iImageWidth ) )
        {
            uint32 aoffset = offset + x;

            if( iBrushPixelData && iBrushWidth && iBrushHeight )
            {
                GETPIXEL( iBrushPixelData
                        , iBrushWidth
                        , iBrushHeight
                        , iBrushBitsPerPixel
                        , iBrushAlphaOnly
                        , fmod(u,1.0f) // function call might slow things (maybe not that much, as fmod is declared inline)
                        , fmod(v,1.0f) // function call might slow things (maybe not that much, as fmod is declared inline)
                        , BR
                        , BG
                        , BB
                        , BA );
            }

            switch ( iImageBitsPerPixel )
            {
                case 32 :
                {
                    unsigned char (*srcimg)[4] = ( unsigned char (*)[4]) iImagePixelData;

                    if( BA )
                    {
                        double blending = (double) BA * opacityFactor;
                        double invBlending = 1.0f - blending;
                        uint32 maxAlpha = ( uint32) srcimg[aoffset][3] + ( BA * iOpacity );

                        srcimg[aoffset][0] = /*BB*/( invBlending * srcimg[aoffset][0] ) + ( BB * blending );
                        srcimg[aoffset][1] = /*BG*/( invBlending * srcimg[aoffset][1] ) + ( BG * blending );
                        srcimg[aoffset][2] = /*BR*/( invBlending * srcimg[aoffset][2] ) + ( BR * blending );
                        srcimg[aoffset][3] = ( maxAlpha > 255 ) ? 255 : maxAlpha;
                    }
                }
                break;

                default :
                break;
            }
        }

        x ++;
        u += pu;
        v += pv;
    }
}
/*
void
FOdysseyVectorEngine::DrawQuadThread( uint32 iProcessorID
                                    , uint32 iProcessorCount
                                    , int32  iFirstLine
                                    , int32  iLastLine
                                    , double iOpacity
                                    , int8*  iPixelData
                                    , int32  iBitsPerPixel
                                    // Temp
                                    , int8*  iBrushPixelData
                                    , uint32 iBrushWidth
                                    , uint32 iBrushHeight
                                    , int32  iBrushBitsPerPixel )
{
    for( int i = iFirstLine + iProcessorID; i <= iLastLine ; i += iProcessorCount )
    {
        if( mHorizontalLineBuffer[i].inited == 2 )
        {
            TraceHorizontalLine( i
                               , iOpacity
                               , iPixelData
                               , iBitsPerPixel
                               , iBrushPixelData
                               , iBrushWidth
                               , iBrushHeight
                               , iBrushBitsPerPixel );
        }

        mHorizontalLineBuffer[i].inited = 0;
    }
}
*/

void
FOdysseyVectorEngine::DrawPolygon( ::ULIS::FVec2I* iPoint
                                 , double* iU
                                 , double* iV
                                 , uint32 pointCount
                                 , double iOpacity
                                 , int8*  iImagePixelData
                                 , uint32 iImageWidth
                                 , uint32 iImageHeight
                                 , int32  iImageBitsPerPixel
                                 , const FColor& iColor
                                 // temp
                                 , int8*  iBrushPixelData
                                 , uint32 iBrushWidth
                                 , uint32 iBrushHeight
                                 , int32  iBrushBitsPerPixel
                                 , bool   iBrushAlphaOnly )
{
    int32 ymin = iPoint[0].y,
          ymax = ymin;

    for( uint32 i = 0; i < pointCount; i++ )
    {
        uint32 n = ( i + 1 ) % pointCount;

        if ( iPoint[i].y < ymin ) ymin = iPoint[i].y;
        if ( iPoint[i].y > ymax ) ymax = iPoint[i].y;

        // always draw in the same direction (left to right ) to avoid bad overlapping
        if( iPoint[i].x < iPoint[n].x )
        {
            TraceLine ( iPoint[i].x, iPoint[i].y, iU[i], iV[i]
                      , iPoint[n].x, iPoint[n].y, iU[n], iV[n], iImageWidth, iImageHeight );
        }
        else
        {
            TraceLine ( iPoint[n].x, iPoint[n].y, iU[n], iV[n]
                      , iPoint[i].x, iPoint[i].y, iU[i], iV[i], iImageWidth, iImageHeight );
        }
    }

    if ( ymin <  0                    ) ymin = 0;
    if ( ymin >= (int32) iImageHeight ) ymin = (int32) iImageHeight - 1;
    if ( ymax <  0                    ) ymax = 0;
    if ( ymax >= (int32) iImageHeight ) ymax = (int32) iImageHeight - 1;

    if ( ymin <= ymax )
    {
/*
        FOdysseyVectorComputer& mainComputer = FOdysseyVectorComputer::GetMainComputer();

        mainComputer.Run( [ this
                          , &ymin
                          , &ymax
                          , &iOpacity
                          , &iPixelData
                          , &iBitsPerPixel
                          , &iBrushPixelData
                          , &iBrushWidth
                          , &iBrushHeight
                          , &iBrushBitsPerPixel]( uint32 iProcessorID, uint32 iProcessorCount ) -> bool
                          {
                              for( int i = ymin + iProcessorID; i <= ymax ; i += iProcessorCount )
                              {
                                  if( mHorizontalLineBuffer[i].inited == 2 )
                                  {
                                      TraceHorizontalLine( i
                                                         , iOpacity
                                                         , iPixelData
                                                         , iBitsPerPixel
                                                         , iBrushPixelData
                                                         , iBrushWidth
                                                         , iBrushHeight
                                                         , iBrushBitsPerPixel );
                                  }

                                  mHorizontalLineBuffer[i].inited = 0;

                                  if( i == ymax )
                                  {
                                      return true;
                                  }
                              }

                              return false;
                          } );
*/

/*
        std::vector<std::future<void>> threads;

//mProcessorCount = 2;

        threads.resize( mProcessorCount );

        int totalThreads = ( ymax - ymin  + 1 ) < (int) mProcessorCount ? ( ymax - ymin  + 1 ) :  (int)mProcessorCount;

        for( int32 i = 0; i < totalThreads; i++ )
        //for( uint32 i = 0; i < threads.size(); i++ )
        {
            threads[i] = std::async( std::launch::async
                                    , [ this
                                      , &ymin
                                      , &ymax
                                      , &iOpacity
                                      , &iPixelData
                                      , &iBitsPerPixel
                                      , &iBrushPixelData
                                      , &iBrushWidth
                                      , &iBrushHeight
                                      , &iBrushBitsPerPixel]( uint32 iProcessorID, uint32 iProcessorCount )
                                      {

                                          FGenericPlatformProcess::SetThreadAffinityMask( (uint64) 1 << iProcessorID );

                                          for( int i = ymin + iProcessorID; i <= ymax ; i += iProcessorCount )
                                          {
                                              if( mHorizontalLineBuffer[i].inited == 2 )
                                              {
                                                    TraceHorizontalLine( i
                                                                     , iOpacity
                                                                     , iPixelData
                                                                     , iBitsPerPixel
                                                                     , iBrushPixelData
                                                                     , iBrushWidth
                                                                     , iBrushHeight
                                                                     , iBrushBitsPerPixel );
                                              }

                                              mHorizontalLineBuffer[i].inited = 0;
                                          }
                                      }
                                  , i
                                  , mProcessorCount );
        }

        for( uint32 i = 0; i < (uint32)totalThreads; i++ )
        {
            threads[i].wait();
        }
*/
/*
        int32 lineCount = ( ymax - ymin ) + 1;

        ParallelFor( lineCount
                  , [ this
                    , &ymin
                    , &iOpacity
                    , iPixelData
                    , &iBitsPerPixel
                    , iBrushPixelData
                    , &iBrushWidth
                    , &iBrushHeight
                    , &iBrushBitsPerPixel ]( int32 iIndex )
                      {
                          int32 lineID = iIndex + ymin;

                          if( mHorizontalLineBuffer[lineID].inited == 2 )
                          {
                              TraceHorizontalLine( lineID
                                                 , iOpacity
                                                 , iPixelData
                                                 , iBitsPerPixel
                                                 , iBrushPixelData
                                                 , iBrushWidth
                                                 , iBrushHeight
                                                 , iBrushBitsPerPixel );
                          }

                          mHorizontalLineBuffer[lineID].inited = 0;
                      } );
*/

    // Single CPU version. The one that actually works.
        for ( int i = ymin; i <= ymax; i++ )
        {
            if( mHorizontalLineBuffer[i].inited == 2 )
            {
                if( mHorizontalLineBuffer[i].x1 >= 0 )
                {
                    TraceHorizontalLine( i
                                       , iOpacity
                                       , iImagePixelData
                                       , iImageWidth
                                       , iImageHeight
                                       , iImageBitsPerPixel
                                       , iColor
                                       , iBrushPixelData
                                       , iBrushWidth
                                       , iBrushHeight
                                       , iBrushBitsPerPixel
                                       , iBrushAlphaOnly );
                }
            }

            mHorizontalLineBuffer[i].inited = 0;
        }

    }
}
