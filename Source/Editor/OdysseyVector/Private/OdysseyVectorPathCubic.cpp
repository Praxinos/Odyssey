#include "OdysseyVectorPathCubic.h"

FOdysseyVectorPathCubic::FOdysseyVectorPathCubic( const FString& iName )
    : FOdysseyVectorPath( iName )
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
FOdysseyVectorPathCubic::Init( FString& iName )
{
    SetName( iName );
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
FOdysseyVectorPathCubic::Erase( ::ULIS::FRectD &iRoi
                              , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                              , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray
                              , std::vector<FOdysseyVectorVertex*>& iRemovedVertexArray
                              , std::vector<FOdysseyVectorSegment*>& iRemovedSegmentArray )
{
    BLContext* blctx = GetScene()->GetEngine()->GetBLContext();
    BLImage* blimg = blctx->targetImage(); // the mask image must be selected by the vector engine at this point
    BLImageData imageData;
    std::vector<FOdysseyVectorSegment*> newSegmentArray;
    std::vector<FOdysseyVectorSegment*> oldSegmentArray;
    std::vector<FOdysseyVectorVertex*> newVertexArray;
    std::vector<FOdysseyVectorVertex*> oldVertexArray;
    std::list<FOdysseyVectorVertex*> vertexList = mVertexList; // work on a copy, for removal

    blimg->getData( &imageData );

    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(*it);
        std::vector<FPolygon>& polygonCache = cubicSegment->GetPolygonCache();
        ::ULIS::FVec2D& firstCoords = cubicSegment->GetVertex(0)->GetCoords();
        BLPoint firstAt = mWorldMatrix.mapPoint( firstCoords.x, firstCoords.y );
        std::vector<double> subVertexT;
        int32 previousPixelValue;
        std::vector<FOdysseyVectorSegment*> subSegmentArray;
        bool hasHit = false;

        for( uint32 i = 0; i < polygonCache.size(); i++ )
        {
            BLPoint p0 = mWorldMatrix.mapPoint( polygonCache[i].lineVertex[0].x, polygonCache[i].lineVertex[0].y );
            BLPoint p1 = mWorldMatrix.mapPoint( polygonCache[i].lineVertex[1].x, polygonCache[i].lineVertex[1].y );

            TraceLine( p0.x, p0.y, polygonCache[i].fromT
                     , p1.x, p1.y, polygonCache[i].toT
                     , [cubicSegment
                     , &imageData
                     , &previousPixelValue
                     , &subVertexT
                     , &subSegmentArray
                     , &newVertexArray
                     , &hasHit]( int32 iX, int32 iY, double iT) -> bool
                       {
                           /*if( ( iX >= 0 && iX < imageData.size.w )
                            && ( iY >= 0 && iY < imageData.size.h ) )
                           {*/
                               uint8 *pixel = static_cast<uint8*>(imageData.pixelData);
                               uint32 offset = ( iY * imageData.size.w ) + iX;
                               int32 pixelValue = ( ( iX >= 0 && iX < imageData.size.w )
                                                 && ( iY >= 0 && iY < imageData.size.h ) ) ? pixel[offset] : 0;

                               if( pixelValue == 255 ) hasHit = true;

                               if( iT == 0.0f )
                               {
                                   if( pixelValue == 0 )
                                   {
                                       subVertexT.push_back( iT );
                                   }

                                   previousPixelValue = pixelValue;
                               }

                               if( ( iT > 0.0f ) && ( iT < 1.0f ) )
                               {
                                   if( (int32) abs( pixelValue - previousPixelValue ) == 255 )
                                   {
                                       subVertexT.push_back( iT );

                                       previousPixelValue = pixelValue;
                                   }
                               }

                               if( iT == 1.0f )
                               {
                                   if( pixelValue == 0 )
                                   {
                                       subVertexT.push_back( iT );
                                   }
                               }

                               if( subVertexT.size() == 2 )
                               {
                                   uint32 tCount = subVertexT.size();
                                   double t0 = subVertexT[0];
                                   double t1 = subVertexT[1];

                                    if( fabs( subVertexT[0] - subVertexT[1]) < 1.0f )
                                    {
                                        subSegmentArray.push_back( cubicSegment->Sample( t0, t1, newVertexArray ) );
                                    }

                                   subVertexT.clear();
                               }
                           /*}*/

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

        newSegmentArray[i]->Invalidate();
    }

    // remove orphaned vertices
    for( std::list<FOdysseyVectorVertex*>::iterator it = vertexList.begin(); it != vertexList.end(); ++it )
    {
        FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(*it);

        if( vertex->GetSegmentCount() == 0 )
        {
            oldVertexArray.push_back( static_cast<FOdysseyVectorVertex*>(vertex) );

            this->RemoveVertex( vertex );
        }
    }

    Invalidate();

    iAddedVertexArray.insert( iAddedVertexArray.end(), newVertexArray.begin(), newVertexArray.end() );
    iAddedSegmentArray.insert( iAddedSegmentArray.end(), newSegmentArray.begin(), newSegmentArray.end() );
    iRemovedVertexArray.insert( iRemovedVertexArray.end(), oldVertexArray.begin(), oldVertexArray.end() );
    iRemovedSegmentArray.insert( iRemovedSegmentArray.end(), oldSegmentArray.begin(), oldSegmentArray.end() );

    return ( mSegmentList.size() == 0 ) ? true : false;
}

bool
FOdysseyVectorPathCubic::PickShape( ::ULIS::FRectD &iRoi, uint32 iSelectionFlags )
{
    BLContext* blctx = GetScene()->GetEngine()->GetBLContext();

    if( iSelectionFlags & PICK_MATH_BASED )
    {
        BLPoint pt = mInverseWorldMatrix.mapPoint( iRoi.x, iRoi.y );

        for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
        {
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(*it);
 
            if( cubicSegment->Pick( pt.x, pt.y, 0.0f ) )
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
FOdysseyVectorPathCubic::PickPoint( double iWorldX
                                  , double iWorldY
                                  , double iSelectionRadius
                                  , std::vector<FOdysseyVectorPoint*>& oPickedPointArray
                                  , uint64 iSelectionFlags )
{
    for(std::list<FOdysseyVectorVertex*>::iterator it = mVertexList.begin(); it != mVertexList.end(); ++it)
    {
        FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(*it);
        ::ULIS::FVec2D perpendicularVector = FOdysseyVectorPathCubic::GetPerpendicularVector( vertex, true );
        BLPoint worldPerpendicularVector = mWorldMatrix.mapVector( perpendicularVector.x * vertex->GetRadius()
                                                                 , perpendicularVector.y * vertex->GetRadius() );

        // Pick vertex
        if ( iSelectionFlags & PICK_POINT )
        {
            ::ULIS::FVec2D& localCoords = vertex->GetCoords();
            // convert vertex coordinates to world coordinates. Easier to detect collision inside the picking circle.
            BLPoint worldCoords = mWorldMatrix.mapPoint( localCoords.x, localCoords.y );
            ::ULIS::FVec2D dif = ::ULIS::FVec2D( worldCoords.x - iWorldX, worldCoords.y - iWorldY );

            if( dif.Distance() <= iSelectionRadius )
            {
                oPickedPointArray.push_back( vertex );
            }
        }

        // Pick vertex handle
        if( iSelectionFlags & PICK_HANDLE_POINT )
        {
            ::ULIS::FVec2D& localCoords = vertex->GetCoords();
            // convert vertex coordinates to world coordinates. Easier to detect collision inside the picking circle.
            BLPoint worldCoords = mWorldMatrix.mapPoint( localCoords.x, localCoords.y );
            // There are 2 point handles, compute both
            ::ULIS::FVec2D dif0 = ::ULIS::FVec2D( worldCoords.x + worldPerpendicularVector.x - iWorldX
                                                , worldCoords.y + worldPerpendicularVector.y - iWorldY );
            ::ULIS::FVec2D dif1 = ::ULIS::FVec2D( worldCoords.x - worldPerpendicularVector.x - iWorldX
                                                , worldCoords.y - worldPerpendicularVector.y - iWorldY );

            if( dif0.Distance() <= iSelectionRadius )
            {
                oPickedPointArray.push_back( vertex );

                return true;
            }

            if( dif1.Distance() <= iSelectionRadius )
            {
                oPickedPointArray.push_back( vertex );

                return true;
            }
        }
    }

    // Pick segment handles
    if( iSelectionFlags & PICK_HANDLE_SEGMENT )
    {
        for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
        {
            // TODO: hit-test with segment's bounding box.

            FOdysseyVectorSegmentCubic* segment = static_cast<FOdysseyVectorSegmentCubic*>(*it);
            FOdysseyVectorHandleSegment* handle0 = segment->GetHandle(0);
            FOdysseyVectorHandleSegment* handle1 = segment->GetHandle(1);
            ::ULIS::FVec2D& handle0LocalCoords = handle0->GetCoords();
            ::ULIS::FVec2D& handle1LocalCoords = handle1->GetCoords();
            // convert handles coordinates to world coordinates. Easier to detect collision inside the picking circle.
            BLPoint handle0WorldCoords = mWorldMatrix.mapPoint( handle0LocalCoords.x, handle0LocalCoords.y );
            BLPoint handle1WorldCoords = mWorldMatrix.mapPoint( handle1LocalCoords.x, handle1LocalCoords.y );
            ::ULIS::FVec2D dif0 = ::ULIS::FVec2D( handle0WorldCoords.x - iWorldX, handle0WorldCoords.y - iWorldY );
            ::ULIS::FVec2D dif1 = ::ULIS::FVec2D( handle1WorldCoords.x - iWorldX, handle1WorldCoords.y - iWorldY );

            if( dif0.Distance() <= iSelectionRadius )
            {
                oPickedPointArray.push_back( handle0 );

                return true;
            }

            if( dif1.Distance() <= iSelectionRadius )
            {
                oPickedPointArray.push_back( handle1 );

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
                            , std::vector<FOdysseyVectorVertex*>& oNewVertexArray
                            , std::vector<FOdysseyVectorSegment*>& oNewSegmentArray
                            , std::vector<FOdysseyVectorSegment*>& oOldSegmentArray )
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
FOdysseyVectorPathCubic::Fill()
{
    FOdysseyVectorVertex *firstVertex = static_cast<FOdysseyVectorVertex*>( GetFirstVertex() );

    if ( firstVertex )
    {
        BLContext* blctx = GetScene()->GetEngine()->GetBLContext();
        FColor& fillColor = mFillBucket.GetColor();
        BLRgba32 blFillColor;
        BLPath path;

        blFillColor.setR( fillColor.R );
        blFillColor.setG( fillColor.G );
        blFillColor.setB( fillColor.B );
        blFillColor.setA( fillColor.A );

        if( mSegmentList.size() == mVertexList.size() )
        {
            FOdysseyVectorSegmentCubic *segment = static_cast<FOdysseyVectorSegmentCubic*>(GetFirstSegment());
            FOdysseyVectorVertex* vertex = firstVertex;

            path.moveTo( firstVertex->GetX(), firstVertex->GetY() );

            do
            {
                FOdysseyVectorVertex* nextVertex = segment->GetOtherVertex( vertex );
                FOdysseyVectorSegment* nextSegment = nextVertex->GetOtherSegment( segment );
                ::ULIS::FVec2D& point1 = nextVertex->GetCoords();
                ::ULIS::FVec2D& handle0 = segment->GetHandle( vertex     )->GetCoords();
                ::ULIS::FVec2D& handle1 = segment->GetHandle( nextVertex )->GetCoords();

                path.cubicTo( handle0.x
                            , handle0.y
                            , handle1.x
                            , handle1.y
                            , point1.x
                            , point1.y );

                vertex = nextVertex;
                segment = static_cast<FOdysseyVectorSegmentCubic*>(nextSegment);
            }
            while( segment && ( vertex != firstVertex ) );
        }

        blctx->setFillStyle( blFillColor );
        blctx->fillPath( path );
    }
}

void
FOdysseyVectorPathCubic::DrawShape( uint64 iFlags )
{

    if ( mPathParam.Filled )
    {
        // TODO: precompute the filling (build the BLPath )
        Fill();
    }

    DrawShapeVariable( iFlags );
}

void
FOdysseyVectorPathCubic::DrawShapeVariable( uint64 iFlags )
{
    BLContext* blctx = GetScene()->GetEngine()->GetBLContext();
    BLRgba32 strokeColor;

    // Note: Blend2D color format is 0xAARRGGBB
    strokeColor.setR( mObjectParam.Foreground.B );
    strokeColor.setG( mObjectParam.Foreground.G );
    strokeColor.setB( mObjectParam.Foreground.R );
    strokeColor.setA( mObjectParam.Foreground.A );

    if( mSegmentList.size() )
    {
        if( iFlags & FOdysseyVectorObject::DRAWSTRUCTURE )
        {
    /*                    BLRgba32 wireframeColor;

            // Note: Blend2D color format is 0xAARRGGBB
            wireframeColor.r = mObjectParam.WireframeColor.B;
            wireframeColor.g = mObjectParam.WireframeColor.G;
            wireframeColor.b = mObjectParam.WireframeColor.R;
            wireframeColor.a = mObjectParam.WireframeColor.A;
    */
            blctx->save();
            blctx->resetMatrix();
            blctx->setStrokeWidth( 1.0f );
            blctx->setStrokeStyle( BLRgba32( 0xFFFFFFFF ) );

            for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
            {
                FOdysseyVectorSegmentCubic* segment = static_cast<FOdysseyVectorSegmentCubic*>(*it);
                FOdysseyVectorVertex* vertex0 = segment->GetVertex(0);

                segment->DrawStructure( this, true );
            }

            blctx->restore();
        }
        else
        {
            // We fill with stroke color because our curve is made of filled shapes.
            //blctx->setFillRule( BL_FILL_RULE_NON_ZERO );
            blctx->setFillRule( BL_FILL_RULE_EVEN_ODD );
            blctx->setFillStyle( BLRgba32( strokeColor ) );
            blctx->setStrokeStyle( BLRgba32( strokeColor ) );

            for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
            {
                FOdysseyVectorSegmentCubic* segment = static_cast<FOdysseyVectorSegmentCubic*>(*it);
                FOdysseyVectorVertex* vertex0 = segment->GetVertex(0);

                segment->Draw();
            }

            for( std::list<FOdysseyVectorVertex*>::iterator it = mVertexList.begin(); it != mVertexList.end(); ++it )
            {
                FOdysseyVectorVertex* cubicVertex = static_cast<FOdysseyVectorVertex*>(*it);

                DrawJoint( cubicVertex, iFlags );
            }
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
        FOdysseyVectorVertex* cubicPoint = static_cast<FOdysseyVectorVertex*>(*it);

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
    FOdysseyVectorPathCubic* cubicPathCopy = new FOdysseyVectorPathCubic( FString("Cubic Path") );
    std::map<FOdysseyVectorVertex*, FOdysseyVectorVertex*> lookupTable;

    for( std::list<FOdysseyVectorVertex*>::iterator it = mVertexList.begin(); it != mVertexList.end(); ++it )
    {
        FOdysseyVectorVertex* originalVertex = static_cast<FOdysseyVectorVertex*>(*it);
        FOdysseyVectorVertex* newVertex = new FOdysseyVectorVertex( cubicPathCopy
                                                                  , originalVertex->GetX()
                                                                  , originalVertex->GetY()
                                                                  , originalVertex->GetRadius() );

        lookupTable.insert( std::make_pair( originalVertex, newVertex ) );

        cubicPathCopy->AddVertex( newVertex );
    }

    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegmentCubic* originalSegment = static_cast<FOdysseyVectorSegmentCubic*>(*it);
        FOdysseyVectorVertex* vertex0 = static_cast<FOdysseyVectorVertex*>( originalSegment->GetPoint(0) );
        FOdysseyVectorVertex* vertex1 = static_cast<FOdysseyVectorVertex*>( originalSegment->GetPoint(1) );
        FOdysseyVectorSegmentCubic* newSegment = new FOdysseyVectorSegmentCubic( cubicPathCopy
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
FOdysseyVectorPathCubic::Merge( FOdysseyVectorPath* iMergedPath
                              , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                              , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray )
{
    std::vector<FOdysseyVectorVertex*> vertexLookup;

    Merge( iMergedPath, vertexLookup, iAddedVertexArray, iAddedSegmentArray );
}

void
FOdysseyVectorPathCubic::Merge( FOdysseyVectorPath* iMergedPath
                              , std::vector<FOdysseyVectorVertex*>& iVertexLookup
                              , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                              , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray )
{
    std::list<FOdysseyVectorSegment*>& segmentList = iMergedPath->GetSegmentList();
    std::list<FOdysseyVectorVertex*>& vertexList = iMergedPath->GetVertexList();
    BLMatrix2D& mergedPathWorldMatrix = iMergedPath->GetWorldMatrix();
    BLMatrix2D conversionMatrix;

    iVertexLookup.resize( vertexList.size() );

    if( vertexList.size() )
    {
        uint32 i = 0;

        FOdysseyVector::MatrixMultiply( mergedPathWorldMatrix, mInverseWorldMatrix, conversionMatrix );

        for( std::list<FOdysseyVectorVertex*>::iterator it = vertexList.begin(); it != vertexList.end(); ++it )
        {
            FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(*it);
            BLPoint pt = conversionMatrix.mapPoint( vertex->GetX(), vertex->GetY() );
            BLPoint rd = conversionMatrix.mapVector( 0.7071 * vertex->GetRadius(), 0.7071 * vertex->GetRadius() );
            ::ULIS::FVec2D radius = ::ULIS::FVec2D( rd.x, rd.y );
            FOdysseyVectorVertex* newVertex = new FOdysseyVectorVertex( this, pt.x, pt.y, radius.Distance() );

            iAddedVertexArray.push_back( newVertex );

            vertex->SetID( i );

            iVertexLookup[i++] = newVertex;

            AddVertex( newVertex );
        }

        for( std::list<FOdysseyVectorSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it )
        {
            FOdysseyVectorSegmentCubic* segment = static_cast<FOdysseyVectorSegmentCubic*>(*it);
            FOdysseyVectorVertex* vertex0 = segment->GetVertex(0);
            FOdysseyVectorVertex* vertex1 = segment->GetVertex(1);
            FOdysseyVectorHandleSegment* handle0 = segment->GetHandle(0);
            FOdysseyVectorHandleSegment* handle1 = segment->GetHandle(1);
            BLPoint pt[2] = { conversionMatrix.mapPoint( handle0->GetX(), handle0->GetY() )
                            , conversionMatrix.mapPoint( handle1->GetX(), handle1->GetY() ) };
            FOdysseyVectorVertex* newCubicVertex0 = static_cast<FOdysseyVectorVertex*>(iVertexLookup[vertex0->GetID()]);
            FOdysseyVectorVertex* newCubicVertex1 = static_cast<FOdysseyVectorVertex*>(iVertexLookup[vertex1->GetID()]);
            FOdysseyVectorSegmentCubic* newSegment = new FOdysseyVectorSegmentCubic( this
                                                                                   , newCubicVertex0
                                                                                   , pt[0].x
                                                                                   , pt[0].y
                                                                                   , pt[1].x
                                                                                   , pt[1].y
                                                                                   , newCubicVertex1 );
            iAddedSegmentArray.push_back( newSegment );

            AddSegment( newSegment );

            newSegment->Invalidate();
        }
    }

    Update( 0 );
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
        FOdysseyVectorVertex* cubicVertex = static_cast<FOdysseyVectorVertex*>(*it);
        ::ULIS::FVec2D& point = cubicVertex->GetCoords();
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

// static
void
FOdysseyVectorPathCubic::SmoothSegments( FOdysseyVectorVertex* iVertex, bool iBuildSegments, bool iPreserveHandleLength )
{
    ::ULIS::FVec2D perpendicularVector = iVertex->GetAverageStraightVectorOnSegment( true );

    // if the perpendicular vector is 0, use one of the segment's vector as a reference.
    if( perpendicularVector.DistanceSquared() == 0.0f && iVertex->GetFirstSegment() )
    {
        perpendicularVector = iVertex->GetFirstSegment()->GetHandleVector( iVertex, true );
        perpendicularVector = ::ULIS::FVec2D( perpendicularVector.y, -perpendicularVector.x );
    }

    SmoothSegments( iVertex, perpendicularVector, iBuildSegments, iPreserveHandleLength );
}

// static
void
FOdysseyVectorPathCubic::SmoothSegments( FOdysseyVectorVertex* iVertex, ::ULIS::FVec2D iPerpendicularVector, bool iBuildSegments, bool iPreserveHandleLength )
{
    std::list<FOdysseyVectorSegment*>& segmentList = iVertex->GetSegmentList();

    if ( iVertex->GetSegmentCount() > 1 )
    {
        for( std::list<FOdysseyVectorSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it )
        {
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(*it);

            if ( iVertex == cubicSegment->GetPoint(0) )
            {
                ::ULIS::FVec2D segmentVector = iVertex->GetVectorOnSegment( cubicSegment, true );
                double dot = iPerpendicularVector.DotProduct( segmentVector );
                ::ULIS::FVec2D tangentVector = ::ULIS::FVec2D( iPerpendicularVector.y, -iPerpendicularVector.x );
                double distance;

                // if perpendicular vector equals 0 or is orthogonal to the segment vector
                if ( tangentVector.DotProduct( segmentVector )  < 0.0f )
                {
                    tangentVector = -tangentVector;
                }

                if( iPreserveHandleLength )
                {
                    ::ULIS::FVec2D& handlePos = cubicSegment->GetHandle(0)->GetCoords();
                    ::ULIS::FVec2D handleVec = handlePos - iVertex->GetCoords();

                    distance = handleVec.Distance();
                }
                else
                {
                    distance = cubicSegment->GetStraightDistance() * 0.35f;
                }

                cubicSegment->GetHandle(0)->Set( iVertex->GetX() + ( tangentVector.x * distance ),
                                                 iVertex->GetY() + ( tangentVector.y * distance ) );
            }

            if ( iVertex == cubicSegment->GetPoint(1) )
            {
                ::ULIS::FVec2D segmentVector = iVertex->GetVectorOnSegment( cubicSegment, true );
                double dot = iPerpendicularVector.DotProduct( segmentVector );
                ::ULIS::FVec2D tangentVector = ::ULIS::FVec2D( iPerpendicularVector.y, -iPerpendicularVector.x );
                double distance;

                // if perpendicular vector equals 0 or is orthogonal to the segment vector
                if ( tangentVector.DotProduct( segmentVector ) < 0.0f )
                {
                    tangentVector = -tangentVector;
                }

                if( iPreserveHandleLength )
                {
                    ::ULIS::FVec2D& handlePos = cubicSegment->GetHandle(1)->GetCoords();
                    ::ULIS::FVec2D handleVec = handlePos - iVertex->GetCoords();

                    distance = handleVec.Distance();
                }
                else
                {
                    distance = cubicSegment->GetStraightDistance() * 0.35f;
                }

                cubicSegment->GetHandle(1)->Set( iVertex->GetX() + ( tangentVector.x * distance ),
                                                 iVertex->GetY() + ( tangentVector.y * distance ) );
            }

            if ( iBuildSegments == true )
            {
                cubicSegment->Update();
            }
            else
            {
                cubicSegment->Invalidate();
            }
        }
    }
}

// static
::ULIS::FVec2D
FOdysseyVectorPathCubic::GetPerpendicularVector( FOdysseyVectorVertex* iVertex, bool iNormalize )
{
    std::list<FOdysseyVectorSegment*>& segmentList = iVertex->GetSegmentList();
    ::ULIS::FVec2D parallel = { 0.0f, 0.0f };
    ::ULIS::FVec2D perpendicular = { 0.0f, 0.0f };

    if ( segmentList.size() )
    {
        uint32 count = 0;

        for( std::list<FOdysseyVectorSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it )
        {
            FOdysseyVectorSegmentCubic *segment = static_cast<FOdysseyVectorSegmentCubic*>(*it);
            FOdysseyVectorVertex* p0 = segment->GetVertex(0);
            FOdysseyVectorVertex* p1 = segment->GetVertex(1);
            ::ULIS::FVec2D& point0 = segment->GetVertex(0)->GetCoords();
            ::ULIS::FVec2D& point1 = segment->GetVertex(1)->GetCoords();
            ::ULIS::FVec2D& ctrlPoint0 = segment->GetHandle(0)->GetCoords();
            ::ULIS::FVec2D& ctrlPoint1 = segment->GetHandle(1)->GetCoords();

            if( iVertex == p0 )
            {
                // this is less computation-heavy
                ::ULIS::FVec2D vec = { ctrlPoint0 - point0 };
                /*::ULIS::FVec2D vec =  { CubicBezierTangentAtParameter<::ULIS::FVec2D>( point0.GetCoords()
                                                                     , ctrlPoint0.GetCoords()
                                                                     , ctrlPoint1.GetCoords()
                                                                     , point1.GetCoords()
                                                                     , 0.0f ) };*/

                if ( vec.DistanceSquared() )
                {
                    vec.Normalize();

                    parallel.x += vec.x;
                    parallel.y += vec.y;
                }
            }

            if( iVertex == p1 )
            {
                ::ULIS::FVec2D vec = { point1 - ctrlPoint1 };
                /*::ULIS::FVec2D vec =  { CubicBezierTangentAtParameter<::ULIS::FVec2D>( point0.GetCoords()
                                                                     , ctrlPoint0.GetCoords()
                                                                     , ctrlPoint1.GetCoords()
                                                                     , point1.GetCoords()
                                                                     , 1.0f ) };*/

                if ( vec.DistanceSquared() )
                {
                    vec.Normalize();

                    parallel.x += vec.x;
                    parallel.y += vec.y;
                }
            }
        }

        if ( iNormalize == true )
        {
            if ( parallel.DistanceSquared() )
            {
                parallel.Normalize();
            }

            perpendicular.x =   parallel.y;
            perpendicular.y = - parallel.x;
        }
    }

    return perpendicular;
}
