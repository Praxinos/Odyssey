#include "OdysseyVectorEngine.h"
//#include <future>

FOdysseyVectorEngine::~FOdysseyVectorEngine()
{
    mBLContext->end();

    delete mBLImage;
}

FOdysseyVectorEngine::FOdysseyVectorEngine( FOdysseyVectorScene* iScene, double iWidth, double iHeight )
    : FOdysseyVectorObject( "Engine" )
    , mSelectionSpace( nullptr )
    , mInvalidTileMap( 64, iWidth, iHeight )
    , mWidth( iWidth )
    , mHeight( iHeight )
{
    BLContextCreateInfo createInfo {};

    // Configure the number of threads to use.
    mProcessorCount = FPlatformMisc::NumberOfCoresIncludingHyperthreads();

    createInfo.threadCount = mProcessorCount;

    // create an empty default image. An image is always needed for matrix operations
    mDefaultBLImage.createFromData( mWidth
                                  , mHeight
                                  , BL_FORMAT_PRGB32
                                  , nullptr
                                  , 0
                                  , nullptr
                                  , nullptr );

    mBLContext = new BLContext();
    mBLMask  = new BLImage( iWidth, iHeight, BL_FORMAT_A8 );

    UseImage( &mDefaultBLImage );

    /*mScene = NewObject<FOdysseyVectorScene>();
    mScene->Init("Vector Scene");*/

    SetScene( iScene );

    mBLContext->begin( *mBLImage, createInfo );
    //UseColorImage();

    mHorizontalLineBuffer.resize( iHeight );
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
FOdysseyVectorEngine::GetBLImage()
{
    return mBLImage;
}

BLContext*
FOdysseyVectorEngine::GetBLContext()
{
    return mBLContext;
}

BLImage*
FOdysseyVectorEngine::GetBLMask()
{
    return mBLMask;
}

uint32
FOdysseyVectorEngine::GetWidth()
{
    return mWidth;
}

uint32
FOdysseyVectorEngine::GetHeight()
{
    return mHeight;
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
FOdysseyVectorEngine::RenderHUD( BLImage* iBLImage/*FOdysseyVectorScene* iScene */ )
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyVectorEngine::RenderHUD);
    std::list<FOdysseyVectorObject*> selectedObjectList = mScene->GetSelectedObjectList();

    UseImage( iBLImage );

    mBLContext->save();
    mBLContext->resetMatrix();
    //mBLContext->setCompOp( BL_COMP_OP_SRC_COPY );
    //mBLContext->setFillAlpha( 0.0f );
    mBLContext->clearAll();

    for( std::list<FOdysseyVectorHUD*>::iterator hit = GetHUDList().begin(); hit != GetHUDList().end(); ++hit )
    {
        FOdysseyVectorHUD *hud = (*hit);

        hud->Draw( mScene, 0 );
    }

    mBLContext->restore();

    mBLContext->flush(BL_CONTEXT_FLUSH_SYNC);

    UseImage( &mDefaultBLImage );
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
FOdysseyVectorEngine::Render( BLImage* iBLImage, uint64 iDrawingFlags )
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyVectorEngine::Render);
    UseImage( iBLImage );

    if( mInvalidationFlags )
    {
        mScene->Draw( iDrawingFlags );
        mBLContext->flush(BL_CONTEXT_FLUSH_SYNC);
    }

    mInvalidationFlags = 0;

    UseImage( &mDefaultBLImage );
}

void
FOdysseyVectorEngine::ClearMask()
{
    BLImage* currentImage = GetBLImage();

    UseMaskImage();

    mBLContext->save();
    mBLContext->resetMatrix();

    mBLContext->setCompOp( BL_COMP_OP_SRC_COPY );
    mBLContext->setFillAlpha( 0.0f );
    mBLContext->clearAll();
    mBLContext->flush( BL_CONTEXT_FLUSH_SYNC );

    mBLContext->restore();

    UseImage( currentImage );
}

::ULIS::FRectD
FOdysseyVectorEngine::GenerateCircleMask( double iX, double iY, double iRadius )
{
    BLImage* currentImage = GetBLImage();

    UseMaskImage();

    mBLContext->save();
    mBLContext->resetMatrix();

    mBLContext->setCompOp( BL_COMP_OP_SRC_COPY );
    mBLContext->setFillAlpha( 1.0f );
    mBLContext->fillCircle( iX, iY, iRadius );
    mBLContext->flush( BL_CONTEXT_FLUSH_SYNC );

    mBLContext->restore();

    UseImage( currentImage );

    return ::ULIS::FRectD::FromMinMax( iX - iRadius, iY - iRadius
                                     , iX + iRadius, iY + iRadius );
}

::ULIS::FRectD
FOdysseyVectorEngine::GenerateRectangleMask( const ::ULIS::FRectD& iRect )
{
    BLImage* currentImage = GetBLImage();

    UseMaskImage();

    mBLContext->save();
    mBLContext->resetMatrix();

    mBLContext->setCompOp( BL_COMP_OP_SRC_COPY );
    mBLContext->setFillAlpha( 0.0f );
    mBLContext->clearAll();
    mBLContext->setFillAlpha( 1.0f );
    mBLContext->fillRect( iRect.x, iRect.y, iRect.w, iRect.h );
    mBLContext->flush( BL_CONTEXT_FLUSH_SYNC );

    mBLContext->restore();

    UseImage( currentImage );

    return iRect;
}

::ULIS::FRectD
FOdysseyVectorEngine::GenerateFreehandMask( std::vector<::ULIS::FVec2D>& iPointArray )
{
    BLImage* currentImage = GetBLImage();
    ::ULIS::FRectD rect = { 0, 0, 0, 0 };
    BLPath path;

    UseMaskImage();

    mBLContext->save();
    mBLContext->resetMatrix();
    mBLContext->setCompOp(BL_COMP_OP_SRC_COPY);
    /*blctx.setFillStyle( BLRgba32(0x00000000) );*/
    mBLContext->setFillAlpha(0.0f);
    mBLContext->clearAll();

    if( iPointArray.size() )
    {
        double x1 = iPointArray[0].x, y1 = iPointArray[0].y
             , x2 = iPointArray[0].x, y2 = iPointArray[0].y;

        path.moveTo( iPointArray[0].x, iPointArray[0].y );

        for( uint32 i = 1; i < iPointArray.size(); i++ )
        {
            path.lineTo( iPointArray[i].x, iPointArray[i].y );

            if( iPointArray[i].x < x1 )
            {
                x1 = iPointArray[i].x;
            }

            if( iPointArray[i].y < y1 )
            {
                y1 = iPointArray[i].y;
            }

            if( iPointArray[i].x > x2 )
            {
                x2 = iPointArray[i].x;
            }

            if( iPointArray[i].y > y2 )
            {
                y2 = iPointArray[i].y;
            }
        }

        rect = ::ULIS::FRectD::FromMinMax( x1, y1, x2, y2 );

        mBLContext->setFillAlpha(1.0f);
        mBLContext->fillPath( path );
    }

    mBLContext->flush(BL_CONTEXT_FLUSH_SYNC);
    mBLContext->restore();

    UseImage( currentImage );

    return rect;
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
FOdysseyVectorEngine::UseMaskImage()
{
    mBLContext->end();
    mBLContext->begin( *mBLMask );
}

void
FOdysseyVectorEngine::UseImage( BLImage* iImage )
{
    mBLContext->end();
    mBLContext->begin(*iImage);

    mBLImage = iImage;
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
                                , double iV1 )
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
            if( ( y >= 0 ) && ( y < (int32) mHeight ) )
            {
                uint32 offset = ( y * mWidth ) + x;

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
            if( ( y >= 0x00 ) && ( y < (int32) mHeight ) )
            {
                uint32 offset = ( y * mWidth ) + x;

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
#define GETPIXEL(PIXELS,WIDTH,HEIGHT,BITSPERPIXEL,U,V,R,G,B,A)             \
    switch ( BITSPERPIXEL )                                                \
    {                                                                      \
        case 32 :                                                          \
        {                                                                  \
            unsigned char (*PIXELS32)[4] = ( unsigned char (*)[4]) PIXELS; \
            int32 TEXU = U * ( WIDTH  - 1 );                               \
            int32 TEXV = V * ( HEIGHT - 1 );                               \
            uint32 TEXOFFSET = ( TEXV * WIDTH ) + TEXU;                    \
                                                                           \
            B = PIXELS32[TEXOFFSET][0];                                    \
            G = PIXELS32[TEXOFFSET][1];                                    \
            R = PIXELS32[TEXOFFSET][2];                                    \
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
                                          , int8*  iPixelData
                                          , int32  iBitsPerPixel
                                          // Temp
                                          , int8*  iBrushPixelData
                                          , uint32 iBrushWidth
                                          , uint32 iBrushHeight
                                          , int32  iBrushBitsPerPixel )
{
    FHorizontalLine *hline = &mHorizontalLineBuffer[iLineNumber];
    int32 x0 = hline->x0,
          x1 = hline->x1;
    double u0 = hline->u0;
    double v0 = hline->v0;
    int32 dx = x1 - x0, ddx = abs ( dx );
    int32 x = x0;
    double du  = hline->u1 - hline->u0, pu = ( ddx ) ? ( du / ddx ) : 0.0f;
    double dv  = hline->v1 - hline->v0, pv = ( ddx ) ? ( dv / ddx ) : 0.0f;
    long  px = ( dx > 0 ) ? 1 : -1;
    double u = u0;
    double v = v0;
    double opacityFactor = iOpacity / 255.0f;

    uint32 offset = ( iLineNumber * mWidth );

    // Commented out: we don't drow from edge-to-edge, we stop 1 pixel before to prevent overlapping,
    // which would lead to double stroke and would produce artefact when alpha is semi-transparent.
    //for( int i = 0; i <= ddx; i++ )
    for( int i = 0; i < ddx; i++ )
    {
        if( ( x >= 0 ) && ( x < (int32) mWidth ) )
        {
            uint32 aoffset = offset + x;
            unsigned char BA = 255, BR = 0, BG = 0, BB = 0;

            if( iBrushPixelData && iBrushWidth && iBrushHeight )
            {
                GETPIXEL( iBrushPixelData
                        , iBrushWidth
                        , iBrushHeight
                        , iBrushBitsPerPixel
                        , u
                        , v
                        , BR
                        , BG
                        , BB
                        , BA );
            }

            switch ( iBitsPerPixel )
            {
                case 32 :
                {
                    unsigned char (*srcimg)[4] = ( unsigned char (*)[4]) iPixelData;

                    if( BA )
                    {
                        double alpha = (double) BA * opacityFactor;
                        double invAlpha = 1.0f - alpha;

                        srcimg[aoffset][0] = /*BB*/( invAlpha * srcimg[aoffset][0] ) + ( BB * alpha );
                        srcimg[aoffset][1] = /*BG*/( invAlpha * srcimg[aoffset][1] ) + ( BG * alpha );
                        srcimg[aoffset][2] = /*BR*/( invAlpha * srcimg[aoffset][2] ) + ( BR * alpha );
                        srcimg[aoffset][3] = /*BA*/( invAlpha * srcimg[aoffset][3] ) + ( BA * alpha );
                    }
                }
                break;

                default :
                break;
            }
        }

        x += px;
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
FOdysseyVectorEngine::DrawQuad( ::ULIS::FVec2I iPoint[4]
                              , double iU[4]
                              , double iV[4]
                              , double iOpacity
                              , int8*  iPixelData
                              , int32  iBitsPerPixel
                              // temp
                              , int8*  iBrushPixelData
                              , uint32 iBrushWidth
                              , uint32 iBrushHeight
                              , int32  iBrushBitsPerPixel )
{
    int32 ymin = iPoint[0].y,
          ymax = ymin;

    for( int i = 0; i < 4; i++ )
    {
        uint32 n = ( i + 1 ) % 4;

        if ( iPoint[i].y < ymin ) ymin = iPoint[i].y;
        if ( iPoint[i].y > ymax ) ymax = iPoint[i].y;

        // always draw in the same direction (left to right ) to avoid bad overlapping
        if( iPoint[i].x < iPoint[n].x )
        {
            TraceLine ( iPoint[i].x, iPoint[i].y, iU[i], iV[i]
                      , iPoint[n].x, iPoint[n].y, iU[n], iV[n] );
        }
        else
        {
            TraceLine ( iPoint[n].x, iPoint[n].y, iU[n], iV[n]
                      , iPoint[i].x, iPoint[i].y, iU[i], iV[i] );
        }
    }

    if ( ymin <  0               ) ymin = 0;
    if ( ymin >= (int32) mHeight ) ymin = (int32) mHeight - 1;
    if ( ymax <  0               ) ymax = 0;
    if ( ymax >= (int32) mHeight ) ymax = (int32) mHeight - 1;

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
}
