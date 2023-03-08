#include "OdysseyVectorPathCubic.h"

UOdysseyVectorPathCubic::UOdysseyVectorPathCubic()
    : UOdysseyVectorPath()
    , Filled( false )
{

}

bool
UOdysseyVectorPathCubic::IsFilled()
{
    return Filled;
}

void
UOdysseyVectorPathCubic::SetFilled( bool iIsFilled )
{
    Filled = iIsFilled;
}

void
UOdysseyVectorPathCubic::Init( std::string iName )
{
    SetName( iName );
}

UOdysseyVectorSegmentCubic*
UOdysseyVectorPathCubic::AppendVertex( UOdysseyVectorVertexCubic* iVertex
                                     , bool iConnect
                                     , bool iBuildSegments )
{
    UOdysseyVectorVertexCubic* lastVertex = static_cast<UOdysseyVectorVertexCubic*>( GetLastVertex() );

    AddVertex ( iVertex );

    if ( iConnect == true )
    {
        // lastPoint is NULL if this is the first point added
        if( lastVertex )
        {
            UOdysseyVectorSegmentCubic* segment = UOdysseyVectorSegmentCubic::New( this, lastVertex, iVertex );

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
UOdysseyVectorPathCubic::Erase( ::ULIS::FRectD &iRoi )
{
    BLContext* blctx = GetRoot()->GetEngine()->GetBLContext();
    BLImage* blimg = blctx->targetImage(); // the mask image must be selected by the vector engine at this point
    BLImageData imageData;
    std::vector<UOdysseyVectorSegmentCubic*> newSegmentArray;
    std::vector<UOdysseyVectorSegmentCubic*> oldSegmentArray;
    std::vector<UOdysseyVectorVertexCubic*> newVertexArray;

    blimg->getData( &imageData );

    for( std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        UOdysseyVectorSegmentCubic* cubicSegment = static_cast<UOdysseyVectorSegmentCubic*>(*it);
        std::vector<FPolygon>& polygonCache = cubicSegment->GetPolygonCache();
        ::ULIS::FVec2D& firstCoords = cubicSegment->GetPoint(0)->GetCoords();
        BLPoint firstAt = mWorldMatrix.mapPoint( firstCoords.x, firstCoords.y );
        double subVertexT[2] = { 0.0f, 0.0f };
        uint32 subVertexCount = 0;
        int32 currentPixelValue;
        std::vector<UOdysseyVectorSegmentCubic*> subSegmentArray;
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

                               if( subVertexCount == 2 )
                               {
                                   subSegmentArray.push_back( cubicSegment->Sample( subVertexT[0], 1.0f
                                                                                  , subVertexT[1], 1.0f, newVertexArray ) );
                                   subVertexCount = 0;
                               }
                           }

                           // keep tracing the line
                           return false;
                       });
        }

        if( hasHit == true )
        {
            // won't insert anything if no subsegment were created
            newSegmentArray.insert( newSegmentArray.end(), subSegmentArray.begin(), subSegmentArray.end() );
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

        newSegmentArray[i]->Update();
    }

    return ( mSegmentList.size() == 0 ) ? true : false;
}

bool
UOdysseyVectorPathCubic::PickShape( ::ULIS::FRectD &iRoi, uint32 iSelectionFlags )
{
    BLContext* blctx = GetRoot()->GetEngine()->GetBLContext();

    if( iSelectionFlags & PICK_MATH_BASED )
    {
        for( std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
        {
            UOdysseyVectorSegmentCubic* cubicSegment = static_cast<UOdysseyVectorSegmentCubic*>(*it);
 
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

        for( std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
        {
            UOdysseyVectorSegmentCubic* cubicSegment = static_cast<UOdysseyVectorSegmentCubic*>(*it);
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
UOdysseyVectorPathCubic::PickPoint( double iX
                                  , double iY
                                  , double iSelectionRadius
                                  , std::vector<FOdysseyVectorPoint*>& oPickedPointArray
                                  , uint64 iSelectionFlags )
{
    for(std::list<UOdysseyVectorVertex*>::iterator it = mVertexList.begin(); it != mVertexList.end(); ++it)
    {
        UOdysseyVectorVertexCubic* vertex = static_cast<UOdysseyVectorVertexCubic*>(*it);
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
                oPickedPointArray.push_back( vertex->GetControlPoint() );

                return true;
            }

            // Pick point handle on the other side
            if( ( fabs( vertex->GetX() - ( perpendicularVector.x * vertex->GetRadius() ) - iX ) <= iSelectionRadius ) &&
                ( fabs( vertex->GetY() - ( perpendicularVector.y * vertex->GetRadius() ) - iY ) <= iSelectionRadius ) )
            {
                oPickedPointArray.push_back( vertex->GetControlPoint() );

                return true;
            }
        }
    }

    if( iSelectionFlags & PICK_HANDLE_SEGMENT )
    {
        for( std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
        {
            UOdysseyVectorSegmentCubic* segment = static_cast<UOdysseyVectorSegmentCubic*>(*it);
            FOdysseyVectorPoint* ctrlPoint0 = segment->GetControlPoint( 0 );
            FOdysseyVectorPoint* ctrlPoint1 = segment->GetControlPoint( 1 );

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
UOdysseyVectorPathCubic::Unselect( UOdysseyVectorVertex *iVertex )
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
UOdysseyVectorPathCubic::Cut( ::ULIS::FVec2D& linePoint0
                            , ::ULIS::FVec2D& linePoint1 )
{
    // let's work on a copy as we are going to remove items in the original list
    std::list<UOdysseyVectorSegment*> tmpSegmentList = mSegmentList;

    while( tmpSegmentList.size () )
    {
        UOdysseyVectorSegmentCubic* cubicSegment = static_cast<UOdysseyVectorSegmentCubic*>(tmpSegmentList.back());

        if ( cubicSegment->Cut ( linePoint0, linePoint1 ) )
        {
            /*mSegmentList.remove ( cubicSegment );*/ // commented out: this is in the cut func

            cubicSegment->Invalidate();
        }

        tmpSegmentList.pop_back ();
    }
}

void
UOdysseyVectorPathCubic::Fill( ::ULIS::FRectD& iRoi )
{
    UOdysseyVectorVertexCubic *firstVertex = static_cast<UOdysseyVectorVertexCubic*>( GetFirstVertex() );


    if ( /*IsLoop() &&*/ firstVertex )
    {
        BLContext* blctx = GetRoot()->GetEngine()->GetBLContext();
        BLPath path;
        BLRgba32 fillColor;

        fillColor.r = Background.R;
        fillColor.g = Background.G;
        fillColor.b = Background.B;
        fillColor.a = Background.A;

        blctx->setCompOp( BL_COMP_OP_SRC_COPY );
        /*iBLContext.setFillStyle(BLRgba32(0xFFFFFFFF));
        iBLContext.setStrokeStyle(BLRgba32(0xFF000000));*/

        path.moveTo( firstVertex->GetX(), firstVertex->GetY() );

        for( std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
        {
            UOdysseyVectorSegmentCubic *segment = static_cast<UOdysseyVectorSegmentCubic*>(*it);
            ::ULIS::FVec2D& point0 = segment->GetPoint(0)->GetCoords();
            ::ULIS::FVec2D& point1 = segment->GetPoint(1)->GetCoords();
            ::ULIS::FVec2D& ctrlPoint0 = segment->GetControlPoint(0)->GetCoords();
            ::ULIS::FVec2D& ctrlPoint1 = segment->GetControlPoint(1)->GetCoords();

            path.cubicTo( ctrlPoint0.x
                        , ctrlPoint0.y
                        , ctrlPoint1.x
                        , ctrlPoint1.y
                        , point1.x
                        , point1.y );
        }

        blctx->setFillStyle( fillColor );
        blctx->fillPath( path );
    }
}

void
UOdysseyVectorPathCubic::DrawStructure( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    BLContext* blctx = GetRoot()->GetEngine()->GetBLContext();
    BLPath path;
    UOdysseyVectorVertexCubic *firstVertex = static_cast<UOdysseyVectorVertexCubic*>( GetFirstVertex() );
    BLPoint localVector = mInverseWorldMatrix.mapVector ( 0.7071f, 0.7071f );
    ::ULIS::FVec2D factor = { localVector.x, localVector.y };
    double handleRadiusX = 6.0f * factor.x;
    double handleRadiusY = 6.0f * factor.y;
    double handleWidth = handleRadiusX * 2.0f;
    double handleHeight = handleRadiusY * 2.0f;

    if ( firstVertex )
    {
        blctx->setCompOp( BL_COMP_OP_SRC_COPY );

        for(std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it)
        {
            UOdysseyVectorSegmentCubic *segment = static_cast<UOdysseyVectorSegmentCubic*>(*it);

            segment->DrawStructure( this, iRoi, factor.x, factor.y );
        }
    }

    // Points and Point size handles
    for(std::list<UOdysseyVectorVertex*>::iterator it = mVertexList.begin(); it != mVertexList.end(); ++it)
    {
        UOdysseyVectorVertexCubic *point = static_cast<UOdysseyVectorVertexCubic*>(*it);
        ::ULIS::FVec2D perpendicular = point->GetPerpendicularVector( true );
        double pointRadius = point->GetRadius();
        double ctrlX = ( perpendicular.x * pointRadius );
        double ctrlY = ( perpendicular.y * pointRadius );

        blctx->setFillStyle( BLRgba32( 0xFFFF00FF ) );
        blctx->fillRect( point->GetX() - handleRadiusX
                       , point->GetY() - handleRadiusY
                       , handleWidth
                       , handleHeight );

        blctx->setFillStyle( BLRgba32( 0xFF808080 ) );
        blctx->fillRect( point->GetX() + ctrlX - handleRadiusX
                       , point->GetY() + ctrlY - handleRadiusY
                       , handleWidth
                       , handleHeight );

        blctx->fillRect( point->GetX() - ctrlX - handleRadiusX
                       , point->GetY() - ctrlY - handleRadiusY
                       , handleWidth
                       , handleHeight );
    }
}

void
UOdysseyVectorPathCubic::DrawShape( ::ULIS::FRectD &iRoi, uint64 iFlags )
{
/*
    BLPath path;

    iBLContext.setCompOp(BL_COMP_OP_SRC_COPY);

    iBLContext.setStrokeStyle( BLRgba32( mStrokeColor ) );
    iBLContext.setStrokeWidth( 20 );

    path.clear();

    for( std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        UOdysseyVectorSegmentCubic *segment = static_cast<UOdysseyVectorSegmentCubic*>(*it);
        bool isStandalone = ( segment->GetPoint(0).GetSegmentCount() == 1 );

        DrawSegment( path, *segment, isStandalone );
    }

    iBLContext.strokePath( path );
*/

    if ( Filled )
    {
        Fill( iRoi );
    }

    DrawShapeVariable( iRoi, iFlags );
}

void
UOdysseyVectorPathCubic::DrawShapeVariable( ::ULIS::FRectD &iRoi, uint64 iFlags )
{
    BLContext* blctx = GetRoot()->GetEngine()->GetBLContext();
    BLRgba32 strokeColor;

    // Note: Blend2D color format is 0xAARRGGBB
    strokeColor.r = Foreground.B;
    strokeColor.g = Foreground.G;
    strokeColor.b = Foreground.R;
    strokeColor.a = Foreground.A;

    blctx->setCompOp( BL_COMP_OP_SRC_OVER );

    // We fill with stroke color because our curve is made of filled shapes.
    blctx->setFillRule( BL_FILL_RULE_NON_ZERO );
    blctx->setFillStyle( BLRgba32( strokeColor ) );
    blctx->setStrokeStyle( BLRgba32( strokeColor ) );

    if( mSegmentList.size() )
    {
        for( std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
        {
            UOdysseyVectorSegmentCubic* segment = static_cast<UOdysseyVectorSegmentCubic*>(*it);
            UOdysseyVectorVertex* vertex0 = Cast<UOdysseyVectorVertex>(segment->GetPoint(0));

            ::ULIS::FRectD clip = iRoi & segment->GetBoundingBox();

            if( ( iRoi.Area() == 0.0f ) || clip.Area() )
            {
                segment->Draw( this, iRoi );
            }
        }

        for( std::list<UOdysseyVectorVertex*>::iterator it = mVertexList.begin(); it != mVertexList.end(); ++it )
        {
            UOdysseyVectorVertexCubic* cubicVertex = static_cast<UOdysseyVectorVertexCubic*>(*it);

            DrawJoint( cubicVertex, iRoi, iFlags );
        }
    }
}

void
UOdysseyVectorPathCubic::Mirror( bool iMirrorX, bool iMirrorY )
{
    double factorX = ( iMirrorX ) ? -1.0f : 1.0f;
    double factorY = ( iMirrorY ) ? -1.0f : 1.0f;

    for( std::list<UOdysseyVectorVertex*>::iterator it = mVertexList.begin(); it != mVertexList.end(); ++it )
    {
        UOdysseyVectorVertexCubic* cubicPoint = static_cast<UOdysseyVectorVertexCubic*>(*it);

        cubicPoint->Set( cubicPoint->GetX() * factorX, cubicPoint->GetY() * factorY );
    }

    for( std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        UOdysseyVectorSegmentCubic* cubicSegment = static_cast<UOdysseyVectorSegmentCubic*>(*it);
        UOdysseyVectorHandleSegment* ctrlPoint0 = static_cast<UOdysseyVectorHandleSegment*>(cubicSegment->GetControlPoint(0));
        UOdysseyVectorHandleSegment* ctrlPoint1 = static_cast<UOdysseyVectorHandleSegment*>(cubicSegment->GetControlPoint(1));

        ctrlPoint0->Set( ctrlPoint0->GetX() * factorX, ctrlPoint0->GetY() * factorY );
        ctrlPoint1->Set( ctrlPoint1->GetX() * factorX, ctrlPoint1->GetY() * factorY );
    }

    /*Update();*/
}

UOdysseyVectorObject*
UOdysseyVectorPathCubic::CopyShape()
{
    UOdysseyVectorPathCubic* cubicPathCopy = NewObject<UOdysseyVectorPathCubic>();
    std::map<UOdysseyVectorVertexCubic*, UOdysseyVectorVertexCubic*> lookupTable;

    for( std::list<UOdysseyVectorVertex*>::iterator it = mVertexList.begin(); it != mVertexList.end(); ++it )
    {
        UOdysseyVectorVertexCubic* originalVertex = static_cast<UOdysseyVectorVertexCubic*>(*it);
        UOdysseyVectorVertexCubic* newVertex = UOdysseyVectorVertexCubic::New( originalVertex->GetX()
                                                                             , originalVertex->GetY()
                                                                             , originalVertex->GetRadius() );

        lookupTable.insert( std::make_pair( originalVertex, newVertex ) );

        cubicPathCopy->AddVertex( newVertex );
    }

    for( std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        UOdysseyVectorSegmentCubic* originalSegment = static_cast<UOdysseyVectorSegmentCubic*>(*it);
        UOdysseyVectorVertexCubic* vertex0 = static_cast<UOdysseyVectorVertexCubic*>( originalSegment->GetPoint(0) );
        UOdysseyVectorVertexCubic* vertex1 = static_cast<UOdysseyVectorVertexCubic*>( originalSegment->GetPoint(1) );
        UOdysseyVectorSegmentCubic* newSegment = UOdysseyVectorSegmentCubic::New( cubicPathCopy
                                                                                , lookupTable[vertex0]
                                                                                , originalSegment->GetControlPoint(0)->GetX()
                                                                                , originalSegment->GetControlPoint(0)->GetY()
                                                                                , originalSegment->GetControlPoint(1)->GetX()
                                                                                , originalSegment->GetControlPoint(1)->GetY()
                                                                                , lookupTable[vertex1] );

        cubicPathCopy->AddSegment( newSegment );

        newSegment->BuildVariable();
    }

    return static_cast<UOdysseyVectorObject*>( cubicPathCopy );
}

void
UOdysseyVectorPathCubic::Merge( UOdysseyVectorPath* iPath )
{
    UOdysseyVectorPathCubic* cubicPath = Cast<UOdysseyVectorPathCubic>(iPath);
    std::list<UOdysseyVectorSegment*> segmentList = cubicPath->mSegmentList; // work on a copy because of removal during iteration
    std::list<UOdysseyVectorVertex*> vertexList = cubicPath->mVertexList; // work on a copy because of removal during iteration

    iPath->SwitchSpace( *this );

    for( std::list<UOdysseyVectorSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it )
    {
        UOdysseyVectorSegmentCubic* segment = static_cast<UOdysseyVectorSegmentCubic*>(*it);

        iPath->RemoveSegment( segment );
    }

    for( std::list<UOdysseyVectorVertex*>::iterator it = vertexList.begin(); it != vertexList.end(); ++it )
    {
        UOdysseyVectorVertexCubic* vertex = static_cast<UOdysseyVectorVertexCubic*>(*it);

        iPath->RemoveVertex( vertex );
        this->AddVertex( vertex );
    }

    for( std::list<UOdysseyVectorSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it )
    {
        UOdysseyVectorSegmentCubic* segment = static_cast<UOdysseyVectorSegmentCubic*>(*it);

        AddSegment( segment );
    }
}

uint32
UOdysseyVectorPathCubic::GetType()
{
    return UOdysseyVectorObject::VECTORPATHCUBICTYPE;
}

void
UOdysseyVectorPathCubic::SwitchSpace( UOdysseyVectorObject& iNewSpace )
{
    BLMatrix2D& newSpaceInverseWorldMatrix = iNewSpace.GetInverseWorldMatrix();
    BLMatrix2D& newSpaceWorldMatrix = iNewSpace.GetWorldMatrix();

    for( std::list<UOdysseyVectorVertex*>::iterator it = mVertexList.begin(); it != mVertexList.end(); ++it )
    {
        UOdysseyVectorVertexCubic* cubicVertex = static_cast<UOdysseyVectorVertexCubic*>(*it);
        ::ULIS::FVec2D& point = cubicVertex->GetCoords();
        BLPoint worldPt = mWorldMatrix.mapPoint( point.x, point.y );
        BLPoint wordlVec = mWorldMatrix.mapVector( 0.70710678118f * cubicVertex->GetRadius()
                                                 , 0.70710678118f * cubicVertex->GetRadius() );
        BLPoint localPt = newSpaceInverseWorldMatrix.mapPoint( worldPt );
        BLPoint localVec = newSpaceInverseWorldMatrix.mapVector( wordlVec );
        ::ULIS::FVec2D vec = { localVec.x, localVec.y };

        point.x = localPt.x;
        point.y = localPt.y;

        cubicVertex->SetRadius( vec.Distance(), false );
    }

    for( std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        UOdysseyVectorSegmentCubic* cubicSegment = static_cast<UOdysseyVectorSegmentCubic*>(*it);
        ::ULIS::FVec2D& ctrlPoint0 = cubicSegment->GetControlPoint(0)->GetCoords();
        ::ULIS::FVec2D& ctrlPoint1 = cubicSegment->GetControlPoint(1)->GetCoords();
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
