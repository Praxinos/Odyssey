#include "OdysseyVectorPathCubic.h"

FOdysseyVectorPathCubic::FOdysseyVectorPathCubic()
    : FOdysseyVectorPath()
{

}

bool
FOdysseyVectorPathCubic::HasBaseClass( uint32 iBaseClassID )
{
    if( mStaticClass == iBaseClassID )
    {
        return true;
    }

    return FOdysseyVectorPath::HasBaseClass( iBaseClassID );
}

void
FOdysseyVectorPathCubic::Init( std::string iName )
{
    SetName( iName );
}

FOdysseyVectorSegmentCubic*
FOdysseyVectorPathCubic::AppendVertex( FOdysseyVectorVertexCubic* iVertex
                                     , bool iConnect
                                     , bool iBuildSegments )
{
    FOdysseyVectorVertexCubic* lastVertex = static_cast<FOdysseyVectorVertexCubic*>( GetLastVertex() );

    AddVertex ( iVertex );

    if ( iConnect == true )
    {
        // lastPoint is NULL if this is the first point added
        if( lastVertex )
        {
            FOdysseyVectorSegmentCubic* segment = FOdysseyVectorSegmentCubic::New( this, lastVertex, iVertex );

            AddSegment( segment );

            if( iBuildSegments == true )
            {
                iVertex->BuildSegments();
            }

            return segment;
        }
    }

    return nullptr;
}

// Note: callbak returns true to end immediately, false to keep tracing
static bool
TraceLine( int32 iX0, int32 iY0, double iT0
         , int32 iX1, int32 iY1, double iT1
         , std::function<bool(int32 iX, int32 iY, double iT)> iCallback )
{
    int32  dx  = ( iX1 - iX0 );
    uint32 ddx = abs ( dx );
    int32  dy  = ( iY1 - iY0 );
    uint32 ddy = abs ( dy );
    double dt  = ( iT1 - iT0 );
    int32  dd  = ( ddx > ddy ) ? ddx : ddy;
    int32  px  = ( dx > 0 ) ? 1 : -1;
    int32  py  = ( dy > 0 ) ? 1 : -1;
    double pt  = ( dd ) ? dt / dd : 0.0f;
    int32  x   = iX0;
    int32  y   = iY0;
    double t   = iT0;
    uint32 cumul = 0;

    if ( ddx > ddy )
    {
        for ( uint32 i = 0; i <= ddx; i++ )
        {
            if( i == ddx ) t = iT1; // to address imprecision, we set the exact value on the last loop

            // return as soon as a point is detected inside the mask
            if ( iCallback( x, y, t ) == true ) {
                return true;
            }

            cumul += ddy;
            x     += px;
            t     += pt;

            if ( cumul >= ddx )
            {
                cumul -= ddx;
                y     += py;
            }
        }
    }
    else
    {
        for ( uint32 i = 0; i <= ddy; i++ )
        {
            if( i == ddy ) t = iT1; // to address imprecision, we set the exact value on the last loop

            // return as soon as a point is detected inside the mask
            if ( iCallback( x, y, t ) == true ) {
                return true;
            }

            cumul += ddx;
            y     += py;
            t     += pt;

            if ( cumul >= ddy )
            {
                cumul -= ddy;
                x     += px;
            }
        }
    }

    return false;
}

bool
FOdysseyVectorPathCubic::Erase( ::ULIS::FRectD &iRoi )
{
    BLContext* blctx = GetScene()->GetEngine()->GetBLContext();
    BLImage* blimg = blctx->targetImage(); // the mask image must be selected by the vector engine at this point
    BLImageData imageData;
    std::vector<FOdysseyVectorSegmentCubic*> newSegmentArray;
    std::vector<FOdysseyVectorSegmentCubic*> oldSegmentArray;
    std::vector<FOdysseyVectorVertexCubic*> newVertexArray;
    std::vector<FOdysseyVectorVertexCubic*> oldVertexArray;
    std::list<FOdysseyVectorVertex*> vertexList = mVertexList; // work on a copy, for removal

    blimg->getData( &imageData );

    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(*it);
        std::vector<FPolygon>& polygonCache = cubicSegment->GetPolygonCache();
        ::ULIS::FVec2D& firstCoords = cubicSegment->GetVertex(0)->GetCoords( nullptr );
        BLPoint firstAt = mWorldMatrix.mapPoint( firstCoords.x, firstCoords.y );
        double subVertexT[2] = { 0.0f, 0.0f };
        uint32 subVertexCount = 0;
        int32 currentPixelValue;
        std::vector<FOdysseyVectorSegmentCubic*> subSegmentArray;
        bool hasHit = false;

        for( uint32 i = 0; i < polygonCache.size(); i++ )
        {
            BLPoint p0 = mWorldMatrix.mapPoint( polygonCache[i].lineVertex[0].x, polygonCache[i].lineVertex[0].y );
            BLPoint p1 = mWorldMatrix.mapPoint( polygonCache[i].lineVertex[1].x, polygonCache[i].lineVertex[1].y );

            TraceLine( p0.x, p0.y, polygonCache[i].fromT
                     , p1.x, p1.y, polygonCache[i].toT
                     , [cubicSegment
                     , &imageData
                     , &currentPixelValue
                     , &subVertexT
                     , &subVertexCount
                     , &subSegmentArray
                     , &newVertexArray
                     , &hasHit]( int32 iX, int32 iY, double iT) -> bool
                       {
                           if( ( iX >= 0 && iX < imageData.size.w )
                            && ( iY >= 0 && iY < imageData.size.h ) )
                           {
                               uint8 *pixel = static_cast<uint8*>(imageData.pixelData);
                               uint32 offset = ( iY * imageData.size.w ) + iX;
                               int32 pixelValue = pixel[offset];

                               if( pixelValue == 255 ) hasHit = true;

                               if( iT == 0.0f )
                               {
                                   if( pixelValue == 0 )
                                   {
                                       subVertexT[subVertexCount++] = iT;
                                   }

                                   currentPixelValue = pixelValue;
                               }

                               if( ( iT > 0.0f ) && ( iT < 1.0f ) )
                               {
                                   if( (int32) abs(currentPixelValue - pixelValue) == (int32) 255 )
                                   {
                                       subVertexT[subVertexCount++] = iT;

                                       currentPixelValue = pixelValue;
                                   }
                               }

                               if( iT == 1.0f )
                               {
                                   if( pixelValue == 0 )
                                   {
                                       subVertexT[subVertexCount++] = iT;
                                   }

                                   currentPixelValue = pixelValue;
                               }

                               if( hasHit )
                               {
                                   if( subVertexCount == 2 )
                                   {
                                       if( fabs( subVertexT[0] - subVertexT[1]) < 1.0f )
                                       {
                                           subSegmentArray.push_back( cubicSegment->Sample(subVertexT[0]
                                                                    , subVertexT[1]
                                                                    , newVertexArray ) );

                                           subVertexCount = 0;
                                       }
                                   }
                               }
                           }

                           // keep tracing the line
                           return false;
                       });
        }

        if( hasHit )
        {
            // won't insert anything if no subsegment were created
            if( subSegmentArray.size() )
            {
                newSegmentArray.insert( newSegmentArray.end(), subSegmentArray.begin(), subSegmentArray.end() );
            }

            // in case of a hit, old segment is deleted no matter what.
            oldSegmentArray.push_back( cubicSegment );
        }
    }

    for( int i = 0; i < oldSegmentArray.size(); i++ )
    {
        this->RemoveSegment( oldSegmentArray[i] );
    }

    for( int i = 0; i < newVertexArray.size(); i++ )
    {
        this->AddVertex( newVertexArray[i] );
    }

    for( int i = 0; i < newSegmentArray.size(); i++ )
    {
        this->AddSegment( newSegmentArray[i] );

        /*newSegmentArray[i]->Invalidate();*/
    }

    // remove orphaned vertices
    for( std::list<FOdysseyVectorVertex*>::iterator it = vertexList.begin(); it != vertexList.end(); ++it )
    {
        FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(*it);

        if( vertex->GetSegmentCount() == 0 )
        {
            oldVertexArray.push_back( static_cast<FOdysseyVectorVertexCubic*>(vertex) );

            this->RemoveVertex( vertex );
        }
    }

    Invalidate();

    return ( mSegmentList.size() == 0 ) ? true : false;
}

bool
FOdysseyVectorPathCubic::PickShape( ::ULIS::FRectD &iRoi, uint32 iSelectionFlags )
{
    BLContext* blctx = GetScene()->GetEngine()->GetBLContext();

    if( iSelectionFlags & PICK_MATH_BASED )
    {
        for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
        {
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(*it);
 
            if( cubicSegment->Pick( iRoi.x, iRoi.y, 0.0f ) )
            {
                return true;
            }
        }
    }

    if ( iSelectionFlags & PICK_MASK_BASED )
    {
        BLImage* blimg = blctx->targetImage(); // the mask image must be selected by the vector engine at this point
        BLImageData imageData;

        blimg->getData( &imageData );

        for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
        {
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(*it);
            std::vector<FPolygon>& polygonCache = cubicSegment->GetPolygonCache();

            for( uint32 i = 0; i < polygonCache.size(); i++ )
            {
                BLPoint p0 = mWorldMatrix.mapPoint( polygonCache[i].lineVertex[0].x, polygonCache[i].lineVertex[0].y );
                BLPoint p1 = mWorldMatrix.mapPoint( polygonCache[i].lineVertex[1].x, polygonCache[i].lineVertex[1].y );
                bool pointHitMask = TraceLine( p0.x, p0.y, 0.0f
                                             , p1.x, p1.y, 0.0f
                                             , [&imageData]( int32 iX, int32 iY, double iT)
                                               {
                                                    if( ( iX >= 0 && iX < imageData.size.w )
                                                     && ( iY >= 0 && iY < imageData.size.h ) )
                                                    {
                                                        uint8 *pixel = static_cast<uint8*>(imageData.pixelData);
                                                        uint32 offset = ( iY * imageData.size.w ) + iX;

                                                        return ( pixel[offset] ) ? true : false;
                                                    }

                                                    return false;
                                               });

                if( pointHitMask )
                {
                    return true;
                }
            }
        }
    }

    return false;
}

bool
FOdysseyVectorPathCubic::PickPoint( double iX
                                  , double iY
                                  , double iSelectionRadius
                                  , std::vector<FOdysseyVectorPoint*>& oPickedPointArray
                                  , uint64 iSelectionFlags )
{
    for(std::list<FOdysseyVectorVertex*>::iterator it = mVertexList.begin(); it != mVertexList.end(); ++it)
    {
        FOdysseyVectorVertexCubic* vertex = static_cast<FOdysseyVectorVertexCubic*>(*it);
        ::ULIS::FVec2D perpendicularVector = vertex->GetPerpendicularVector( true );

        if ( iSelectionFlags & PICK_POINT )
        {
            if( ( fabs( vertex->GetX() - iX ) <= iSelectionRadius ) &&
                ( fabs( vertex->GetY() - iY ) <= iSelectionRadius ) )
            {
                oPickedPointArray.push_back( vertex );
            }
        }

        if( iSelectionFlags & PICK_HANDLE_POINT )
        {
            // Pick point handle
            if( ( fabs( vertex->GetX() + ( perpendicularVector.x * vertex->GetRadius() ) - iX ) <= iSelectionRadius ) &&
                ( fabs( vertex->GetY() + ( perpendicularVector.y * vertex->GetRadius() ) - iY ) <= iSelectionRadius ) )
            {
                oPickedPointArray.push_back( vertex->GetHandle() );

                return true;
            }

            // Pick point handle on the other side
            if( ( fabs( vertex->GetX() - ( perpendicularVector.x * vertex->GetRadius() ) - iX ) <= iSelectionRadius ) &&
                ( fabs( vertex->GetY() - ( perpendicularVector.y * vertex->GetRadius() ) - iY ) <= iSelectionRadius ) )
            {
                oPickedPointArray.push_back( vertex->GetHandle() );

                return true;
            }
        }
    }

    if( iSelectionFlags & PICK_HANDLE_SEGMENT )
    {
        for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
        {
            FOdysseyVectorSegmentCubic* segment = static_cast<FOdysseyVectorSegmentCubic*>(*it);
            FOdysseyVectorPoint* ctrlPoint0 = segment->GetHandle( 0 );
            FOdysseyVectorPoint* ctrlPoint1 = segment->GetHandle( 1 );

            if( ( fabs( ctrlPoint0->GetX() - iX ) <= iSelectionRadius ) &&
                ( fabs( ctrlPoint0->GetY() - iY ) <= iSelectionRadius ) )
            {
                oPickedPointArray.push_back( ctrlPoint0 );

                return true;
            }

            if( ( fabs( ctrlPoint1->GetX() - iX ) <= iSelectionRadius ) &&
                ( fabs( ctrlPoint1->GetY() - iY ) <= iSelectionRadius ) )
            {
                oPickedPointArray.push_back( ctrlPoint1 );

                return true;
            }
        }
    }

    return false;
}

void
FOdysseyVectorPathCubic::Unselect( FOdysseyVectorVertex *iVertex )
{
    if ( iVertex == NULL )
    {
        mSelectedPointList.clear();
    }
    else
    {
        mSelectedPointList.remove( iVertex );
    }
}

void
FOdysseyVectorPathCubic::Cut( ::ULIS::FVec2D& linePoint0
                            , ::ULIS::FVec2D& linePoint1
                            , std::vector<FOdysseyVectorVertexCubic*>& oNewVertexArray
                            , std::vector<FOdysseyVectorSegmentCubic*>& oNewSegmentArray
                            , std::vector<FOdysseyVectorSegmentCubic*>& oOldSegmentArray )
{
    // let's work on a copy as we are going to remove items in the original list
    std::list<FOdysseyVectorSegment*> tmpSegmentList = mSegmentList;

    while( tmpSegmentList.size () )
    {
        FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(tmpSegmentList.back());

        if ( cubicSegment->Cut ( linePoint0, linePoint1, oNewVertexArray, oNewSegmentArray ) )
        {
            /*mSegmentList.remove ( cubicSegment );*/ // commented out: this is in the cut func

            cubicSegment->Invalidate();

            oOldSegmentArray.push_back( cubicSegment );
        }

        tmpSegmentList.pop_back ();
    }
}

void
FOdysseyVectorPathCubic::Fill( ::ULIS::FRectD& iRoi )
{
    FOdysseyVectorVertexCubic *firstVertex = static_cast<FOdysseyVectorVertexCubic*>( GetFirstVertex() );


    if ( firstVertex )
    {
        BLContext* blctx = GetScene()->GetEngine()->GetBLContext();
        BLPath path;
        BLRgba32 blFillColor;
        FColor& fillColor = mFillBucket.GetColor();

        blFillColor.r = fillColor.R;
        blFillColor.g = fillColor.G;
        blFillColor.b = fillColor.B;
        blFillColor.a = fillColor.A;

        blctx->setCompOp( BL_COMP_OP_SRC_COPY );

        path.moveTo( firstVertex->GetX(), firstVertex->GetY() );

        for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
        {
            FOdysseyVectorSegmentCubic *segment = static_cast<FOdysseyVectorSegmentCubic*>(*it);
            ::ULIS::FVec2D& point0 = segment->GetVertex(0)->GetCoords( nullptr );
            ::ULIS::FVec2D& point1 = segment->GetVertex(1)->GetCoords( nullptr );
            ::ULIS::FVec2D& ctrlPoint0 = segment->GetHandle(0)->GetCoords();
            ::ULIS::FVec2D& ctrlPoint1 = segment->GetHandle(1)->GetCoords();

            path.cubicTo( ctrlPoint0.x
                        , ctrlPoint0.y
                        , ctrlPoint1.x
                        , ctrlPoint1.y
                        , point1.x
                        , point1.y );
        }

        blctx->setFillStyle( blFillColor );
        blctx->fillPath( path );
    }
}

void
FOdysseyVectorPathCubic::DrawShape( ::ULIS::FRectD &iRoi, uint64 iFlags )
{

    if ( mPathParam.Filled )
    {
        Fill( iRoi );
    }

    DrawShapeVariable( iRoi, iFlags );
}

void
FOdysseyVectorPathCubic::DrawShapeVariable( ::ULIS::FRectD &iRoi, uint64 iFlags )
{
    BLContext* blctx = GetScene()->GetEngine()->GetBLContext();
    BLRgba32 strokeColor;

    // Note: Blend2D color format is 0xAARRGGBB
    strokeColor.r = mObjectParam.Foreground.B;
    strokeColor.g = mObjectParam.Foreground.G;
    strokeColor.b = mObjectParam.Foreground.R;
    strokeColor.a = mObjectParam.Foreground.A;

    blctx->setCompOp( BL_COMP_OP_SRC_OVER );

    // We fill with stroke color because our curve is made of filled shapes.
    blctx->setFillRule( BL_FILL_RULE_NON_ZERO );
    blctx->setFillStyle( BLRgba32( strokeColor ) );
    blctx->setStrokeStyle( BLRgba32( strokeColor ) );

    if( mSegmentList.size() )
    {
        for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
        {
            FOdysseyVectorSegmentCubic* segment = static_cast<FOdysseyVectorSegmentCubic*>(*it);
            FOdysseyVectorVertex* vertex0 = segment->GetVertex(0);

            ::ULIS::FRectD clip = iRoi & segment->GetBoundingBox( false );

            if( ( iRoi.Area() == 0.0f ) || clip.Area() )
            {
                segment->Draw( iRoi );
            }
        }

        for( std::list<FOdysseyVectorVertex*>::iterator it = mVertexList.begin(); it != mVertexList.end(); ++it )
        {
            FOdysseyVectorVertexCubic* cubicVertex = static_cast<FOdysseyVectorVertexCubic*>(*it);

            DrawJoint( cubicVertex, iRoi, iFlags );
        }
    }
}

void
FOdysseyVectorPathCubic::Mirror( bool iMirrorX, bool iMirrorY )
{
    double factorX = ( iMirrorX ) ? -1.0f : 1.0f;
    double factorY = ( iMirrorY ) ? -1.0f : 1.0f;

    for( std::list<FOdysseyVectorVertex*>::iterator it = mVertexList.begin(); it != mVertexList.end(); ++it )
    {
        FOdysseyVectorVertexCubic* cubicPoint = static_cast<FOdysseyVectorVertexCubic*>(*it);

        cubicPoint->Set( cubicPoint->GetX() * factorX, cubicPoint->GetY() * factorY );
    }

    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(*it);
        FOdysseyVectorHandleSegment* ctrlPoint0 = static_cast<FOdysseyVectorHandleSegment*>(cubicSegment->GetHandle(0));
        FOdysseyVectorHandleSegment* ctrlPoint1 = static_cast<FOdysseyVectorHandleSegment*>(cubicSegment->GetHandle(1));

        ctrlPoint0->Set( ctrlPoint0->GetX() * factorX, ctrlPoint0->GetY() * factorY );
        ctrlPoint1->Set( ctrlPoint1->GetX() * factorX, ctrlPoint1->GetY() * factorY );
    }

    /*Update();*/
}

FOdysseyVectorObject*
FOdysseyVectorPathCubic::CopyShape()
{
    FOdysseyVectorPathCubic* cubicPathCopy = new FOdysseyVectorPathCubic();
    std::map<FOdysseyVectorVertexCubic*, FOdysseyVectorVertexCubic*> lookupTable;

    for( std::list<FOdysseyVectorVertex*>::iterator it = mVertexList.begin(); it != mVertexList.end(); ++it )
    {
        FOdysseyVectorVertexCubic* originalVertex = static_cast<FOdysseyVectorVertexCubic*>(*it);
        FOdysseyVectorVertexCubic* newVertex = FOdysseyVectorVertexCubic::New( originalVertex->GetX()
                                                                             , originalVertex->GetY()
                                                                             , originalVertex->GetRadius() );

        lookupTable.insert( std::make_pair( originalVertex, newVertex ) );

        cubicPathCopy->AddVertex( newVertex );
    }

    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegmentCubic* originalSegment = static_cast<FOdysseyVectorSegmentCubic*>(*it);
        FOdysseyVectorVertexCubic* vertex0 = static_cast<FOdysseyVectorVertexCubic*>( originalSegment->GetPoint(0) );
        FOdysseyVectorVertexCubic* vertex1 = static_cast<FOdysseyVectorVertexCubic*>( originalSegment->GetPoint(1) );
        FOdysseyVectorSegmentCubic* newSegment = FOdysseyVectorSegmentCubic::New( cubicPathCopy
                                                                                , lookupTable[vertex0]
                                                                                , originalSegment->GetHandle(0)->GetX()
                                                                                , originalSegment->GetHandle(0)->GetY()
                                                                                , originalSegment->GetHandle(1)->GetX()
                                                                                , originalSegment->GetHandle(1)->GetY()
                                                                                , lookupTable[vertex1] );

        cubicPathCopy->AddSegment( newSegment );

        newSegment->BuildVariable();
    }

    return static_cast<FOdysseyVectorObject*>( cubicPathCopy );
}

void
FOdysseyVectorPathCubic::Merge( FOdysseyVectorPath* iPath )
{
    FOdysseyVectorPathCubic* cubicPath = static_cast<FOdysseyVectorPathCubic*>(iPath);
    std::list<FOdysseyVectorSegment*> segmentList = cubicPath->mSegmentList; // work on a copy because of removal during iteration
    std::list<FOdysseyVectorVertex*> vertexList = cubicPath->mVertexList; // work on a copy because of removal during iteration

    iPath->SwitchSpace( *this );

    for( std::list<FOdysseyVectorSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it )
    {
        FOdysseyVectorSegmentCubic* segment = static_cast<FOdysseyVectorSegmentCubic*>(*it);

        iPath->RemoveSegment( segment );
    }

    for( std::list<FOdysseyVectorVertex*>::iterator it = vertexList.begin(); it != vertexList.end(); ++it )
    {
        FOdysseyVectorVertexCubic* vertex = static_cast<FOdysseyVectorVertexCubic*>(*it);

        iPath->RemoveVertex( vertex );
        this->AddVertex( vertex );
    }

    for( std::list<FOdysseyVectorSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it )
    {
        FOdysseyVectorSegmentCubic* segment = static_cast<FOdysseyVectorSegmentCubic*>(*it);

        AddSegment( segment );
    }
}

uint32
FOdysseyVectorPathCubic::GetType()
{
    return FOdysseyVectorObject::VECTORPATHCUBICTYPE;
}

void
FOdysseyVectorPathCubic::SwitchSpace( FOdysseyVectorObject& iNewSpace )
{
    BLMatrix2D& newSpaceInverseWorldMatrix = iNewSpace.GetInverseWorldMatrix();
    BLMatrix2D& newSpaceWorldMatrix = iNewSpace.GetWorldMatrix();

    for( std::list<FOdysseyVectorVertex*>::iterator it = mVertexList.begin(); it != mVertexList.end(); ++it )
    {
        FOdysseyVectorVertexCubic* cubicVertex = static_cast<FOdysseyVectorVertexCubic*>(*it);
        ::ULIS::FVec2D& point = cubicVertex->GetCoords( nullptr );
        BLPoint worldPt = mWorldMatrix.mapPoint( point.x, point.y );
        BLPoint wordlVec = mWorldMatrix.mapVector( 0.70710678118f * cubicVertex->GetRadius()
                                                 , 0.70710678118f * cubicVertex->GetRadius() );
        BLPoint localPt = newSpaceInverseWorldMatrix.mapPoint( worldPt );
        BLPoint localVec = newSpaceInverseWorldMatrix.mapVector( wordlVec );
        ::ULIS::FVec2D vec = { localVec.x, localVec.y };

        point.x = localPt.x;
        point.y = localPt.y;

        cubicVertex->SetRadius( vec.Distance() );
    }

    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(*it);
        ::ULIS::FVec2D& ctrlPoint0 = cubicSegment->GetHandle(0)->GetCoords();
        ::ULIS::FVec2D& ctrlPoint1 = cubicSegment->GetHandle(1)->GetCoords();
        BLPoint pt;

        pt = newSpaceInverseWorldMatrix.mapPoint( mWorldMatrix.mapPoint( ctrlPoint0.x, ctrlPoint0.y ) );

        ctrlPoint0.x = pt.x;
        ctrlPoint0.y = pt.y;

        pt = newSpaceInverseWorldMatrix.mapPoint( mWorldMatrix.mapPoint( ctrlPoint1.x, ctrlPoint1.y ) );

        ctrlPoint1.x = pt.x;
        ctrlPoint1.y = pt.y;

        InvalidateSegment( cubicSegment );
    }
}
