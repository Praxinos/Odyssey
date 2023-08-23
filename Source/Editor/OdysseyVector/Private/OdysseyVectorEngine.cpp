#include "OdysseyVectorEngine.h"

FOdysseyVectorEngine::~FOdysseyVectorEngine()
{
    mBLContext->end();
}

FOdysseyVectorEngine::FOdysseyVectorEngine( FOdysseyVectorScene* iScene, double iWidth, double iHeight )
    : FOdysseyVectorObject( "Engine" )
    , mSelectionSpace( nullptr )
    , mInvalidTileMap( 64, iWidth, iHeight )
{
    BLContextCreateInfo createInfo {};

    // Configure the number of threads to use.
    createInfo.threadCount = 8;

    mBLContext = new BLContext();
    mBLImage = new BLImage( iWidth, iHeight, BL_FORMAT_PRGB32 );
    mBLMask  = new BLImage( iWidth, iHeight, BL_FORMAT_A8 );

    /*mScene = NewObject<FOdysseyVectorScene>();
    mScene->Init("Vector Scene");*/

    SetScene( iScene );

    mBLContext->begin( *mBLImage, createInfo );
    //UseColorImage();
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

BLContext*
FOdysseyVectorEngine::GetBLContext()
{
    return mBLContext;
}

BLImage*
FOdysseyVectorEngine::GetBLImage()
{
    return mBLImage;
}

BLImage*
FOdysseyVectorEngine::GetBLMask()
{
    return mBLMask;
}

void
FOdysseyVectorEngine::GetColorImageSize( ::ULIS::FRectI& oImageRegion )
{
    uint32 width, height;

    GetColorImageSize( &width, &height );

    oImageRegion.x = 0;
    oImageRegion.y = 0;
    oImageRegion.w = width;
    oImageRegion.h = height;
}

void
FOdysseyVectorEngine::GetColorImageSize( uint32* iW, uint32* iH )
{
    BLImageData imageData;

    mBLImage->getData( &imageData );

    *iW = (uint32) imageData.size.w;
    *iH = (uint32) imageData.size.h;
}

void
FOdysseyVectorEngine::GetColorImagePixelValue( uint32 iX, uint32 iY, uint8* oR, uint8* oG, uint8* oB, uint8* oA )
{
    BLImageData imageData;

    mBLImage->getData( &imageData );

    if( ( iX >= 0 ) && ( iX < static_cast<uint32>(imageData.size.w) )
     && ( iY >= 0 ) && ( iY < static_cast<uint32>(imageData.size.h) ) ) 
    {
        switch( imageData.format )
        {
            case BL_FORMAT_PRGB32:
            {
                uint32 offset = ( iY * imageData.stride ) + ( iX * sizeof( uint32 ) );

                uint8 *imgBuffer =  &((uint8*)imageData.pixelData)[offset];

                *oB = imgBuffer[0];
                *oG = imgBuffer[1];
                *oR = imgBuffer[2];
                *oA = imgBuffer[3];
            }
            break;

            default:
            break;
        }
    }
}

FColor
FOdysseyVectorEngine::GetColorImagePixelValue( uint32 iX, uint32 iY )
{
    uint8 R, G, B, A;

    GetColorImagePixelValue( iX, iY, &R, &G, &B, &A );

    return FColor( R, G, B, A ); //::ULIS::FColor::RGBA8( R, G, B, A );
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
FOdysseyVectorEngine::RenderHUD( /*FOdysseyVectorScene* iScene */ )
{
    std::list<FOdysseyVectorObject*> selectedObjectList = mScene->GetSelectedObjectList();

    mBLContext->save();
    mBLContext->resetMatrix();

    for( std::list<FOdysseyVectorHUD*>::iterator hit = GetHUDList().begin(); hit != GetHUDList().end(); ++hit )
    {
        FOdysseyVectorHUD *hud = (*hit);

        hud->Draw( mScene, 0 );
    }

    mBLContext->restore();

    mBLContext->flush(BL_CONTEXT_FLUSH_SYNC);
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
FOdysseyVectorEngine::UpdateShape( uint32 iUpdateFlags )
{
    // Blend2D part
   /* BLContextCreateInfo createInfo{};*/

    // Configure the number of threads to use.
    /*createInfo.threadCount = 1;*/

    /*if( iRegion.Area() != 0 )
    {
        mBLContext->clipToRect( iRegion.x, iRegion.y, iRegion.w, iRegion.h );
    }*/

    //mBLContext->begin( *mBLImage );

    if( mInvalidationFlags )
    {
        mScene->Draw( 0 );
    /*
        mBLContext->save();
        mBLContext->resetMatrix();
    UE_LOG(LogTemp, Warning, TEXT("Some warning message %d %d %d %d"), mRoi.x, mRoi.y, mRoi.w, mRoi.h );
        mBLContext->setStrokeStyle(BLRgba32(0xFF0000FF));
        mBLContext->setStrokeWidth(2.0f);
        mBLContext->strokeRect(mRoi.x,mRoi.y,mRoi.w,mRoi.h);
        mBLContext->restore();
    */
        //RenderHUD( iScene );

        //mBLContext->restoreClipping();

        //mBLMask->swap(*mBLImage);

        //mBLContext->blitImage( BLPoint(0,0), *mBLMask );

        mBLContext->flush(BL_CONTEXT_FLUSH_SYNC);
    }

    //mBLContext->end();
    if( ( iUpdateFlags & FOdysseyVectorObject::KEEPINVALIDATED ) == 0 )
    {
        mInvalidationFlags = 0;
    }
}

void
FOdysseyVectorEngine::ClearMask()
{
    UseMaskImage();

    mBLContext->save();
    mBLContext->resetMatrix();

    mBLContext->setCompOp( BL_COMP_OP_SRC_COPY );
    mBLContext->setFillAlpha( 0.0f );
    mBLContext->clearAll();
    mBLContext->flush( BL_CONTEXT_FLUSH_SYNC );

    mBLContext->restore();

    UseColorImage();
}

::ULIS::FRectD
FOdysseyVectorEngine::GenerateCircleMask( double iX, double iY, double iRadius )
{
    UseMaskImage();

    mBLContext->save();
    mBLContext->resetMatrix();

    mBLContext->setCompOp( BL_COMP_OP_SRC_COPY );
    mBLContext->setFillAlpha( 1.0f );
    mBLContext->fillCircle( iX, iY, iRadius );
    mBLContext->flush( BL_CONTEXT_FLUSH_SYNC );

    mBLContext->restore();

    UseColorImage();

    return ::ULIS::FRectD::FromMinMax( iX - iRadius, iY - iRadius
                                     , iX + iRadius, iY + iRadius );
}

::ULIS::FRectD
FOdysseyVectorEngine::GenerateRectangleMask( const ::ULIS::FRectD& iRect )
{
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

    UseColorImage();

    return iRect;
}

::ULIS::FRectD
FOdysseyVectorEngine::GenerateFreehandMask( std::vector<::ULIS::FVec2D>& iPointArray )
{
    BLPath path;
    ::ULIS::FRectD rect = { 0, 0, 0, 0 };

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

    UseColorImage();

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

    // Note: this will be refactored in case a child is erased and a parent should as well be erased. We'll see.
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
                                                                                            ,  knotVertex ),
                                                               new FOdysseyVectorSegmentCubic( cubicPath
                                                                                            ,  knotVertex
                                                                                            ,  nextVertex ) };

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
    if( ( iObject->GetClass() == FOdysseyVectorPath::StaticClass()      )
     || ( iObject->GetClass() == FOdysseyVectorPath::StaticClass() ) )
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
}

void
FOdysseyVectorEngine::UseColorImage()
{
    mBLContext->end();
    mBLContext->begin( *mBLImage );
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
    OnSignalDelegate().Broadcast( mScene, iSignalFlags );
}
