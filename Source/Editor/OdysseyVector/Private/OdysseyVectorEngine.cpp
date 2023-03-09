#include "OdysseyVectorEngine.h"

FOdysseyVectorEngine::~FOdysseyVectorEngine()
{
    mBLContext->end();
}

FOdysseyVectorEngine::FOdysseyVectorEngine( double iWidth, double iHeight )
    : mDrawingFlags( 0 )
    , mSelectionSpace( nullptr )
{
    mBLContext = new BLContext();
    mBLImage = new BLImage( iWidth, iHeight, BL_FORMAT_PRGB32 );
    mBLMask  = new BLImage( iWidth, iHeight, BL_FORMAT_A8 );

    /*mScene = NewObject<UOdysseyVectorScene>();
    mScene->Init("Vector Scene");*/

    mBLContext->begin( *mBLImage );
}

void
FOdysseyVectorEngine::SetSelectionSpace( UOdysseyVectorGroup* iSelectionSpace )
{
   mSelectionSpace = iSelectionSpace;
}

UOdysseyVectorGroup*
FOdysseyVectorEngine::GetSelectionSpace()
{
    return mSelectionSpace;
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

::ULIS::FRectD&
FOdysseyVectorEngine::GetInvalidateRegion()
{
    return mRoi;
}

void
FOdysseyVectorEngine::GetColorImagePixelValue( uint32 iX, uint32 iY, uint8 *oR, uint8* oG, uint8 *oB, uint8 *oA )
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

                *oR = imgBuffer[0];
                *oG = imgBuffer[1];
                *oB = imgBuffer[2];
                *oA = imgBuffer[3];
            }
            break;

            default:
            break;
        }
    }
}

::ULIS::FColor
FOdysseyVectorEngine::GetColorImagePixelValue( uint32 iX, uint32 iY )
{
    uint8 R, G, B, A;

    GetColorImagePixelValue( iX, iY, &R, &G, &B, &A );

    return ::ULIS::FColor::RGBA8( R, G, B, A );
}

void
FOdysseyVectorEngine::RenderHUD( UOdysseyVectorScene& iScene )
{
    std::list<UOdysseyVectorObject*> selectedObjectList = iScene.GetSelectedObjectList();

    mBLContext->save();
    mBLContext->resetMatrix();

    for( std::list<FOdysseyVectorHUD*>::iterator hit = mHUDList.begin(); hit != mHUDList.end(); ++hit )
    {
        FOdysseyVectorHUD *hud = (*hit);

            hud->Draw( iScene, mRoi, 0 );
    }

    mBLContext->restore();
}

void
FOdysseyVectorEngine::SetDrawingFlags( uint64 iDrawingFlags )
{
    mDrawingFlags = iDrawingFlags;
}

void
FOdysseyVectorEngine::Render( UOdysseyVectorScene& iScene )
{
    static ::ULIS::FRectD zeroRectangle;
    // Blend2D part
   /* BLContextCreateInfo createInfo{};*/

    // Configure the number of threads to use.
    /*createInfo.threadCount = 1;*/

    mBLContext->setFillStyle( BLRgba32(0xFFFFFFFF) );

    if ( mRoi != zeroRectangle )
    {
        mBLContext->fillRect( mRoi.x, mRoi.y, mRoi.w, mRoi.h );

        // view the updated zone ( testing purpose only )
        /*blctx.setStrokeStyle(BLRgba32(0xFFFF0000));
        blctx.setStrokeWidth(1.0f);
        blctx.strokeRect( mRoi.x, mRoi.y, mRoi.w, mRoi.h );*/
    }
    else
    {
        mBLContext->fillAll();
        //blctx.clearAll();
    }

    iScene.Draw( mRoi, mDrawingFlags );

    RenderHUD( iScene );

/*
    BLImageData imgData;
    BLImageData mskData;

    mBLMask->getData( &mskData );
    mBLImage->getData( &imgData );

    for ( int i = 0; i < imgData.size.h; i++ ) {
        for ( int j = 0; j < imgData.size.w; j++ ) {
            uint32 offset = (i*imgData.size.w)+j;

            if( ((uint8*)mskData.pixelData)[offset] )
                ((uint32*)imgData.pixelData)[offset] = 0xFFFF0000;

        }
    }
*/

    // Reset region of interest after each draw
    memset ( &mRoi, 0, sizeof ( mRoi ) );

    mBLContext->flush(BL_CONTEXT_FLUSH_SYNC);
}

::ULIS::FRectD
FOdysseyVectorEngine::GenerateMask( std::vector<::ULIS::FVec2D>& iPointArray )
{
    BLPath path;
    ::ULIS::FRectD rect = { 0, 0, 0, 0 };

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
    }

    /*blctx.setFillStyle( BLRgba32(0xFFFFFFFF) );*/
    mBLContext->setFillAlpha(1.0f);
    mBLContext->fillPath( path );

    return rect;
}

void
FOdysseyVectorEngine::RecursiveErase( UOdysseyVectorObject& iObj
                                    , std::vector<UOdysseyVectorObject*>& iErasedObjectArray
                                    , ::ULIS::FRectD &iRoi
                                    , bool iSelectedOnly )
{
    for( std::list<UOdysseyVectorObject*>::iterator it = iObj.GetChildrenList().begin(); it != iObj.GetChildrenList().end(); ++it )
    {
        UOdysseyVectorObject *child = (*it);

        RecursiveErase( *child, iErasedObjectArray, iRoi, iSelectedOnly );
    }

    if( iSelectedOnly == true )
    {
        if( iObj.IsSelected() == true )
        {
            if( iObj.Erase( iRoi ) )
            {
                iErasedObjectArray.push_back( &iObj );
            }
        }
    }
    else
    {
        if( iObj.Erase( iRoi ) )
        {
            iErasedObjectArray.push_back( &iObj );
        }
    }
}

void
FOdysseyVectorEngine::Erase( UOdysseyVectorScene& iScene
                           , ::ULIS::FRectD &iRoi
                           , bool iSelectedOnly )
{
    std::vector<UOdysseyVectorObject*> erasedObjectArray;

    RecursiveErase( iScene, erasedObjectArray, iRoi, iSelectedOnly  );

    // Note: this will be refactored in case a child is erased and a parent should as well be erased. We'll see.
    for( int i = 0; i < erasedObjectArray.size(); i++ )
    {
        if( erasedObjectArray[i]->GetChildrenList().size() == 0 )
        {
            erasedObjectArray[i]->GetParent()->RemoveChild( erasedObjectArray[i] );
        }
    }
}

static void
RecursivePickPoints( UOdysseyVectorObject* iObject
                   , double iX
                   , double iY
                   , double iRadius
                   , std::vector<FOdysseyVectorPoint*>& oPickedPointArray )
{
    UOdysseyVectorPath* path = Cast<UOdysseyVectorPath>(iObject);

    if( path )
    {
        BLPoint localVector = iObject->GetInverseWorldMatrix().mapVector( 0.7071f, 0.7071f );
        ::ULIS::FVec2D factor = { localVector.x * iRadius, localVector.y * iRadius };
        double localRadius = factor.Distance();
        BLPoint localPoint = iObject->GetInverseWorldMatrix().mapPoint( iX, iY );
        ::ULIS::FRectD pathBBox = path->GetBBox( false );

        pathBBox.x -=   localRadius;
        pathBBox.y -=   localRadius;
        pathBBox.w += ( localRadius * 2 );
        pathBBox.h += ( localRadius * 2 );

        if( pathBBox.HitTest( ::ULIS::FVec2D( localPoint.x, localPoint.y ) ) == true )
        {
            path->PickPoint( localPoint.x, localPoint.y, localRadius, oPickedPointArray, UOdysseyVectorPath::PICK_POINT );
        }
    }

    for( std::list<UOdysseyVectorObject*>::iterator it = iObject->GetChildrenList().begin(); it != iObject->GetChildrenList().end(); ++it )
    {
        UOdysseyVectorObject* child = (*it);

        RecursivePickPoints( child, iX, iY, iRadius, oPickedPointArray );
    }
}

void
FOdysseyVectorEngine::PickPoints( UOdysseyVectorScene* iScene
                                , double iX
                                , double iY
                                , double iRadius
                                , std::vector<FOdysseyVectorPoint*>& oPickedPointArray )
{
    RecursivePickPoints( iScene, iX, iY, iRadius, oPickedPointArray );
}

bool
FOdysseyVectorEngine::Knot( FOdysseyVectorVertex* iVertexA
                          , FOdysseyVectorVertex* iVertexB
                          , FOdysseyVectorSegment** oCreatedSegment
                          , FOdysseyVectorSegment** oRemovedSegment
                          , bool iSmooth )
{
    if( ( iVertexA->GetSegmentCount() == 1 )
     && ( iVertexB->GetSegmentCount() == 1 )
     && ( iVertexA->GetPath() == iVertexB->GetPath() ) )
    {
        ::ULIS::FVec2D& vertexACoords = iVertexA->GetCoords( nullptr );
        ::ULIS::FVec2D& vertexBCoords = iVertexB->GetCoords( nullptr );
        ::ULIS::FVec2D averageCoords = ( vertexACoords + vertexBCoords ) * 0.5f;
        double vertexARadius = iVertexA->GetRadius();
        double vertexBRadius = iVertexB->GetRadius();
        double averageRadius = ( vertexARadius + vertexBRadius ) * 0.5f;
        UOdysseyVectorPath* path = iVertexA->GetPath();

        if( path->GetClass() == UOdysseyVectorPathCubic::StaticClass() )
        {
            UOdysseyVectorPathCubic* cubicPath = Cast<UOdysseyVectorPathCubic>(path);
            FOdysseyVectorSegmentCubic* firstCubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(iVertexB->GetFirstSegment());

            if( firstCubicSegment )
            {
                FOdysseyVectorVertexCubic* cubicVertex0 = static_cast<FOdysseyVectorVertexCubic*>(firstCubicSegment->GetVertex(0));
                uint32 knotVertexIndex = ( cubicVertex0 == iVertexB ) ? 0 : 1;
                uint32 nextVertexIndex = ( cubicVertex0 == iVertexB ) ? 1 : 0;
                ::ULIS::FVec2D& knotCtrlPointCoords = firstCubicSegment->GetHandle(knotVertexIndex)->GetCoords();
                ::ULIS::FVec2D& nextCtrlPointCoords = firstCubicSegment->GetHandle(nextVertexIndex)->GetCoords();
                FOdysseyVectorVertexCubic* knotVertex = static_cast<FOdysseyVectorVertexCubic*>(iVertexA);
                FOdysseyVectorVertexCubic* nextVertex = static_cast<FOdysseyVectorVertexCubic*>(firstCubicSegment->GetVertex( nextVertexIndex ));
                FOdysseyVectorSegmentCubic* newCubicSegment;

                path->RemoveSegment( firstCubicSegment );
                path->RemoveVertex( iVertexB );

                knotVertex->SetX( averageCoords.x );
                knotVertex->SetY( averageCoords.y );
                knotVertex->SetRadius( averageRadius );

                newCubicSegment = FOdysseyVectorSegmentCubic::New( cubicPath
                                                                ,  knotVertex
                                                                ,  knotCtrlPointCoords.x
                                                                ,  knotCtrlPointCoords.y
                                                                ,  nextCtrlPointCoords.x
                                                                ,  nextCtrlPointCoords.y
                                                                ,  nextVertex );

                path->AddSegment( newCubicSegment );

                /*if( iSmooth )
                {
                    knotVertex->SmoothSegments( false );
                }*/

                *oCreatedSegment = newCubicSegment;
                *oRemovedSegment = firstCubicSegment;

                iVertexA->GetFirstSegment()->Invalidate();

                path->Invalidate();

                return true;
            }
        }
    }

    return false;
}

static void
RecursivePickSegments( UOdysseyVectorObject* iObject
                     , double iX
                     , double iY
                     , double iRadius
                     , std::vector<FOdysseyVectorSegment*>& oPickedSegmentArray
                     , std::vector<double>& oDistanceArray )
{
    UOdysseyVectorPath* path = Cast<UOdysseyVectorPath>(iObject);

    if( path )
    {
        BLPoint localVector = iObject->GetInverseWorldMatrix().mapVector( 0.7071f, 0.7071f );
        ::ULIS::FVec2D factor = { localVector.x * iRadius, localVector.y * iRadius };
        double localRadius = factor.Distance();
        BLPoint localPoint = iObject->GetInverseWorldMatrix().mapPoint( iX, iY );
        ::ULIS::FRectD pathBBox = path->GetBBox( false );

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
                ::ULIS::FRectD segmentBBox = segment->GetBoundingBox();

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
                        oDistanceArray.push_back( smallestDistance );
                    } 
                }
            }
        }
    }

    for( std::list<UOdysseyVectorObject*>::iterator it = iObject->GetChildrenList().begin(); it != iObject->GetChildrenList().end(); ++it )
    {
        UOdysseyVectorObject* child = (*it);

        RecursivePickSegments( child, iX, iY, iRadius, oPickedSegmentArray, oDistanceArray );
    }
}

void
FOdysseyVectorEngine::PickSegments( UOdysseyVectorScene* iScene
                                  , double iX
                                  , double iY
                                  , double iRadius
                                  , std::vector<FOdysseyVectorSegment*>& oPickedSegmentArray
                                  , std::vector<double>& oDistance )
{
    RecursivePickSegments( iScene, iX, iY, iRadius, oPickedSegmentArray, oDistance );
}

// static
void
FOdysseyVectorEngine::RecursivePick( UOdysseyVectorGroup* iSelectionSpace
                                   , UOdysseyVectorObject& iObj
                                   , std::vector<UOdysseyVectorObject*>& iSelectedObjectArray
                                   , ::ULIS::FRectD& iRoi
                                   , uint32 iSelectionFlags )
{
    UOdysseyVectorObject* pickedObject = ( &iObj != iSelectionSpace ) ? iObj.Pick( iSelectionSpace, iRoi, iSelectionFlags ) : nullptr;

    for( std::list<UOdysseyVectorObject*>::iterator it = iObj.GetChildrenList().begin(); it != iObj.GetChildrenList().end(); ++it )
    {
        UOdysseyVectorObject* child = (*it);

        RecursivePick( iSelectionSpace, *child, iSelectedObjectArray, iRoi, iSelectionFlags );
    }

    if( pickedObject )
    {
        iSelectedObjectArray.push_back( pickedObject );
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
FOdysseyVectorEngine::Pick( UOdysseyVectorScene& iScene, std::vector<::ULIS::FVec2D>& iPointArray, uint32 iSelectionFlags )
{
    ::ULIS::FRectD roi;
    std::vector<UOdysseyVectorObject*> pickedObjectArray;

    if( iSelectionFlags & UOdysseyVectorObject::PICK_MASK_BASED )
    {
        UseMaskImage();

        roi = GenerateMask( iPointArray );
    }
    else
    {
        if( iPointArray.size() )
        {
            roi.x = iPointArray[0].x;
            roi.y = iPointArray[0].y;
        } 
    }

    mBLContext->flush(BL_CONTEXT_FLUSH_SYNC);

    // deselect all
    iScene.ClearSelection();

    RecursivePick( mSelectionSpace, iScene, pickedObjectArray, roi, iSelectionFlags );

    for ( int i = 0; i < pickedObjectArray.size(); i++ )
    {
        iScene.Select( pickedObjectArray[i] );
    }

    if( iSelectionFlags & UOdysseyVectorObject::PICK_MASK_BASED )
    {
        UseColorImage();
    }
}

void
FOdysseyVectorEngine::InvalidateRegion( double x, double y, double w, double h )
{
    mRoi.x = x;
    mRoi.y = y;
    mRoi.w = w;
    mRoi.h = h;
}

void
FOdysseyVectorEngine::InvalidateRegion( ::ULIS::FRectD& iRegion )
{
    mRoi.x = iRegion.x;
    mRoi.y = iRegion.y;
    mRoi.w = iRegion.w;
    mRoi.h = iRegion.h;
}

void
FOdysseyVectorEngine::InvalidateRegion( ::ULIS::FRectI& iRegion )
{
    mRoi.x = iRegion.x;
    mRoi.y = iRegion.y;
    mRoi.w = iRegion.w;
    mRoi.h = iRegion.h;
}

void FOdysseyVectorEngine::AddHUD( FOdysseyVectorHUD* iHUDObject )
{
    mHUDList.push_back( iHUDObject );
}

void FOdysseyVectorEngine::RemoveHUD( FOdysseyVectorHUD* iHUDObject )
{
    mHUDList.remove( iHUDObject );
}

void FOdysseyVectorEngine::ClearHUD()
{
    mHUDList.clear();
}
