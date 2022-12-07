#include "OdysseyVectorPathCubic.h"

FOdysseyVectorPathCubic::FOdysseyVectorPathCubic()
    : FOdysseyVectorPath()
{
    setJointMiter();
}

FOdysseyVectorPathCubic::FOdysseyVectorPathCubic( std::string iName )
    : FOdysseyVectorPath( iName )
{

}

void
FOdysseyVectorPathCubic::setJointRadial()
{
    mJointType = JOINT_TYPE_RADIAL;
}

void
FOdysseyVectorPathCubic::setJointMiter()
{
    mJointType = JOINT_TYPE_MITER;
}

void
FOdysseyVectorPathCubic::setJointLinear()
{
    mJointType = JOINT_TYPE_LINEAR;
}

void
FOdysseyVectorPathCubic::setJointNone()
{
    mJointType = JOINT_TYPE_NONE;
}

FOdysseyVectorSegmentCubic*
FOdysseyVectorPathCubic::AppendPoint( FOdysseyVectorPointCubic* iPoint
                             , bool iConnect
                             , bool iBuildSegments )
{
    FOdysseyVectorPointCubic* lastPoint = static_cast<FOdysseyVectorPointCubic*>( GetLastPoint() );

    mPointList.push_back ( iPoint );

    if ( iConnect == true )
    {
        // lastPoint is NULL if this is the first point added
        if( lastPoint )
        {
            FOdysseyVectorSegmentCubic* segment = new FOdysseyVectorSegmentCubic( *this, lastPoint, iPoint );

            AddSegment( segment );

            if( iBuildSegments == true )
            {
                iPoint->BuildSegments();
            }

            return segment;
        }
    }

    return nullptr;
}

FOdysseyVectorObject*
FOdysseyVectorPathCubic::PickShape( double iX
                                  , double iY
                                  , double iRadius )
{
    BLPath path;
    BLPoint testPoint = { iX, iY };

/*
    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegmentCubic* segment = static_cast<FOdysseyVectorSegmentCubic*>(*it);
        ::ULIS::FVec2D& point0 = segment->GetPoint(0).GetCoords();
        ::ULIS::FVec2D& point1 = segment->GetPoint(1).GetCoords();
        ::ULIS::FVec2D& ctrlPoint0 = segment->GetControlPoint(0).GetCoords();
        ::ULIS::FVec2D& ctrlPoint1 = segment->GetControlPoint(1).GetCoords();

        path.moveTo( point0.x, point0.y );
        path.cubicTo( ctrlPoint0.x
                    , ctrlPoint0.y
                    , ctrlPoint1.x
                    , ctrlPoint1.y
                    , point1.x
                    , point1.y );
    }

    if ( path.hitTest( testPoint, BL_FILL_RULE_NON_ZERO ) == BL_HIT_TEST_IN )
    {
        return true;
    }
*/
    // Pick inside the polygons that makes the segment
    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(*it);

        if ( cubicSegment->Pick ( iX, iY, iRadius ) == true )
        {
            return this;
        }
    }

    return PickLoops( iX, iY, iRadius );
}

bool
FOdysseyVectorPathCubic::PickPoint( double iX
                           , double iY
                           , double iSelectionRadius
                           , uint64 iSelectionFlags )
{
    for(std::list<FOdysseyVectorPoint*>::iterator it = mPointList.begin(); it != mPointList.end(); ++it)
    {
        FOdysseyVectorPointCubic* point = static_cast<FOdysseyVectorPointCubic*>(*it);
        ::ULIS::FVec2D perpendicularVector = point->GetPerpendicularVector( true );

        if ( iSelectionFlags & PICK_POINT )
        {
            if( ( fabs( point->GetX() - iX ) <= iSelectionRadius ) &&
                ( fabs( point->GetY() - iY ) <= iSelectionRadius ) )
            {
                mSelectedPointList.push_back( point );
            }
        }

        if( iSelectionFlags & PICK_HANDLE_POINT )
        {
            // Pick point handle
            if( ( fabs( point->GetX() + ( perpendicularVector.x * point->GetRadius() ) - iX ) <= iSelectionRadius ) &&
                ( fabs( point->GetY() + ( perpendicularVector.y * point->GetRadius() ) - iY ) <= iSelectionRadius ) )
            {
                mSelectedPointList.push_back( &point->GetControlPoint() );

                return true;
            }

            // Pick point handle on the other side
            if( ( fabs( point->GetX() - ( perpendicularVector.x * point->GetRadius() ) - iX ) <= iSelectionRadius ) &&
                ( fabs( point->GetY() - ( perpendicularVector.y * point->GetRadius() ) - iY ) <= iSelectionRadius ) )
            {
                mSelectedPointList.push_back( &point->GetControlPoint() );

                return true;
            }
        }
    }

    if( iSelectionFlags & PICK_HANDLE_SEGMENT )
    {
        for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
        {
            FOdysseyVectorSegmentCubic* segment = static_cast<FOdysseyVectorSegmentCubic*>(*it);
            FOdysseyVectorPoint& ctrlPoint0 = segment->GetControlPoint( 0 );
            FOdysseyVectorPoint& ctrlPoint1 = segment->GetControlPoint( 1 );

            if( ( fabs( ctrlPoint0.GetX() - iX ) <= iSelectionRadius ) &&
                ( fabs( ctrlPoint0.GetY() - iY ) <= iSelectionRadius ) )
            {
                mSelectedPointList.push_back( &ctrlPoint0 );

                return true;
            }

            if( ( fabs( ctrlPoint1.GetX() - iX ) <= iSelectionRadius ) &&
                ( fabs( ctrlPoint1.GetY() - iY ) <= iSelectionRadius ) )
            {
                mSelectedPointList.push_back( &ctrlPoint1 );

                return true;
            }
        }
    }

    return false;
}

void
FOdysseyVectorPathCubic::Unselect( FOdysseyVectorPoint *iPoint )
{
    if ( iPoint == NULL )
    {
        mSelectedPointList.clear();
    }
    else
    {
        mSelectedPointList.remove( iPoint );
    }
}

void
FOdysseyVectorPathCubic::Cut( ::ULIS::FVec2D& linePoint0
                     , ::ULIS::FVec2D& linePoint1 )
{
    // let's work on a copy as we are going to remove items in the original list
    std::list<FOdysseyVectorSegment*> tmpSegmentList = mSegmentList;

    while( tmpSegmentList.size () )
    {
        FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(tmpSegmentList.back());

        if ( cubicSegment->Cut ( linePoint0, linePoint1 ) )
        {
            /*mSegmentList.remove ( cubicSegment );*/ // commented out: this is in the cut func

            cubicSegment->Invalidate();
        }

        tmpSegmentList.pop_back ();
    }
}

void
FOdysseyVectorPathCubic::Fill( ::ULIS::FRectD& iRoi )
{
    FOdysseyVectorPointCubic *firstPoint = static_cast<FOdysseyVectorPointCubic*>( GetFirstPoint() );
    BLContext& blctx = FOdysseyVectorEngine::GetBLContext();
    BLPath path;

    if ( IsLoop() && firstPoint )
    {
        blctx.setCompOp( BL_COMP_OP_SRC_COPY );
        /*iBLContext.setFillStyle(BLRgba32(0xFFFFFFFF));
        iBLContext.setStrokeStyle(BLRgba32(0xFF000000));*/

        path.moveTo( firstPoint->GetX(), firstPoint->GetY() );

        for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
        {
            FOdysseyVectorSegmentCubic *segment = static_cast<FOdysseyVectorSegmentCubic*>(*it);
            ::ULIS::FVec2D& point0 = segment->GetPoint(0)->GetCoords();
            ::ULIS::FVec2D& point1 = segment->GetPoint(1)->GetCoords();
            ::ULIS::FVec2D& ctrlPoint0 = segment->GetControlPoint(0).GetCoords();
            ::ULIS::FVec2D& ctrlPoint1 = segment->GetControlPoint(1).GetCoords();

            path.cubicTo( ctrlPoint0.x
                        , ctrlPoint0.y
                        , ctrlPoint1.x
                        , ctrlPoint1.y
                        , point1.x
                        , point1.y );
        }

        blctx.setFillStyle( BLRgba32( mFillColor ) );
        blctx.fillPath( path );
    }
}

void
FOdysseyVectorPathCubic::DrawStructure( ::ULIS::FRectD& iRoi )
{
    BLContext& blctx = FOdysseyVectorEngine::GetBLContext();
    BLPath path;
    FOdysseyVectorPointCubic *firstPoint = static_cast<FOdysseyVectorPointCubic*>( GetFirstPoint() );
    BLPoint localVector = mInverseWorldMatrix.mapVector ( 1.0f, 0.0f );
    ::ULIS::FVec2D ulisVector = { localVector.x, localVector.y };
    double zoomFactor = ulisVector.Distance();
    double handleSize = 6 * zoomFactor;
    double handleHalfSize = handleSize * 0.5f;

    if ( firstPoint )
    {
        blctx.setCompOp( BL_COMP_OP_SRC_COPY );
        /*iBLContext.setFillStyle(BLRgba32(0xFFFFFFFF));
        iBLContext.setStrokeStyle(BLRgba32(0xFF000000));*/

        for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
        {
            FOdysseyVectorSegmentCubic *segment = static_cast<FOdysseyVectorSegmentCubic*>(*it);
            ::ULIS::FVec2D& point0 = segment->GetPoint( 0 )->GetCoords();
            ::ULIS::FVec2D& point1 = segment->GetPoint( 1 )->GetCoords();
            ::ULIS::FVec2D& ctrlPoint0 = segment->GetControlPoint( 0 ).GetCoords();
            ::ULIS::FVec2D& ctrlPoint1 = segment->GetControlPoint( 1 ).GetCoords();
/*
            printf("%f %f - %f %f - %f %f - %f %f\n", point0.x
                                                    , point0.y
                                                    , ctrlPoint0.x
                                                    , ctrlPoint0.y
                                                    , ctrlPoint1.x
                                                    , ctrlPoint1.y
                                                    , point1.x
                                                    , point1.y );
*/
/*
            path.moveTo( point0.x, point0.y );
            path.cubicTo( ctrlPoint0.x
                        , ctrlPoint0.y
                        , ctrlPoint1.x
                        , ctrlPoint1.y
                        , point1.x
                        , point1.y );
*/
        }

        blctx.setStrokeStyle( BLRgba32( 0xFF00FF00 ) );
        blctx.setStrokeWidth( zoomFactor );
        blctx.strokePath( path );

        for(std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it)
        {
            FOdysseyVectorSegmentCubic *segment = static_cast<FOdysseyVectorSegmentCubic*>(*it);

            segment->DrawStructure( iRoi, zoomFactor );
        }
    }

    // Points and Point size handles
    for(std::list<FOdysseyVectorPoint*>::iterator it = mPointList.begin(); it != mPointList.end(); ++it)
    {

        FOdysseyVectorPointCubic *point = static_cast<FOdysseyVectorPointCubic*>(*it);
        ::ULIS::FVec2D perpendicular = point->GetPerpendicularVector( true );
        double pointRadius = point->GetRadius();
        double ctrlX = ( perpendicular.x * pointRadius );
        double ctrlY = ( perpendicular.y * pointRadius );

        blctx.setFillStyle( BLRgba32( 0xFFFF00FF ) );
        blctx.fillRect( point->GetX() - handleHalfSize
                      , point->GetY() - handleHalfSize
                      , handleSize
                      , handleSize );

        blctx.setFillStyle( BLRgba32( 0xFF808080 ) );
        blctx.fillRect( point->GetX() + ctrlX - handleHalfSize
                      , point->GetY() + ctrlY - handleHalfSize
                      , handleSize
                      , handleSize );

        blctx.fillRect( point->GetX() - ctrlX - handleHalfSize
                      , point->GetY() - ctrlY - handleHalfSize
                      , handleSize
                      , handleSize );
    }
}

void
FOdysseyVectorPathCubic::DrawShape( ::ULIS::FRectD &iRoi, uint64 iFlags )
{
/*
    BLPath path;

    iBLContext.setCompOp(BL_COMP_OP_SRC_COPY);

    iBLContext.setStrokeStyle( BLRgba32( mStrokeColor ) );
    iBLContext.setStrokeWidth( 20 );

    path.clear();

    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegmentCubic *segment = static_cast<FOdysseyVectorSegmentCubic*>(*it);
        bool isStandalone = ( segment->GetPoint(0).GetSegmentCount() == 1 );

        DrawSegment( path, *segment, isStandalone );
    }

    iBLContext.strokePath( path );
*/

    if ( IsFilled() )
    {
        Fill( iRoi );
    }

    DrawLoops( iRoi, iFlags  );

    DrawShapeVariable( iRoi, iFlags );

    if( mIsSelected )
    {
        DrawStructure( iRoi );
    }
/*
        iBLContext.setStrokeWidth( 6 );
    DrawLoops( iRoi  );
*/
}

// https://gamedev.net/forums/topic/647810-intersection-point-of-two-vectors/5094071/
static bool intersectLine( ::ULIS::FVec2D& iOrigin0
                         , ::ULIS::FVec2D& iDirection0
                         , ::ULIS::FVec2D& iOrigin1
                         , ::ULIS::FVec2D& iDirection1
                         , ::ULIS::FVec2D& iOut ) {
    ::ULIS::FVec2D c = iOrigin0 - iOrigin1;
    double cross = ( iDirection0.y * iDirection1.x ) - ( iDirection0.x * iDirection1.y );

    if ( cross )
    {
        double t = ( ( c.x * iDirection1.y ) - ( c.y * iDirection1.x ) ) / cross;

        iOut = iOrigin0 + ( iDirection0 * t );

        return true;
    }

    return false;
}

static void
_drawMiterJoint( ::ULIS::FVec2D& iOrigin
               , ::ULIS::FVec2D& iPrevSegmentVector
               , ::ULIS::FVec2D& iSegmentVector
               , double iRadius
               , double iMiterLimit )
{
    ::ULIS::FVec2D currParallelVec = - iSegmentVector;
    ::ULIS::FVec2D prevParallelVec = iPrevSegmentVector;
    ::ULIS::FVec2D currPerpendicularVec = { iSegmentVector.y    , - iSegmentVector.x     };
    ::ULIS::FVec2D prevPerpendicularVec = { iPrevSegmentVector.y, - iPrevSegmentVector.x };
    ::ULIS::FVec2D edgePrevPoint = iOrigin + ( prevPerpendicularVec * iRadius );
    ::ULIS::FVec2D edgePoint = iOrigin + ( currPerpendicularVec * iRadius );
    ::ULIS::FVec2D shortestTest = edgePoint - edgePrevPoint;
    // have to clamp due to imprecision of the dot product
    double dot = std::clamp<double>( currPerpendicularVec.DotProduct( prevPerpendicularVec ), -1.0f, 1.0f );
    BLContext& blctx = FOdysseyVectorEngine::GetBLContext();

    ::ULIS::FVec2D intersectionPoint;

    // Find on which side should the joint be drawn by comparing the directions of our vectors
    if ( shortestTest.DotProduct( iPrevSegmentVector ) < 0 )
    {
        ::ULIS::FVec2D tmp = currPerpendicularVec;
        currPerpendicularVec = -tmp;

               tmp = prevPerpendicularVec;
        prevPerpendicularVec = -tmp;

        edgePrevPoint = iOrigin + ( prevPerpendicularVec * iRadius );
        edgePoint = iOrigin + ( currPerpendicularVec * iRadius );
    }

    if( iRadius )
    {
        if ( intersectLine( edgePoint
                          , currParallelVec
                          , edgePrevPoint
                          , prevParallelVec
                          , intersectionPoint ) == true )
        {
            ::ULIS::FVec2D originToIntersection = intersectionPoint - iOrigin;

            double miterRatio = originToIntersection.Distance() / iRadius;

            if ( miterRatio < iMiterLimit )
            {
                BLPoint vertex[4];
                vertex[0].x = iOrigin.x;
                vertex[0].y = iOrigin.y;

                vertex[1].x = edgePoint.x;
                vertex[1].y = edgePoint.y;

                vertex[2].x = intersectionPoint.x;
                vertex[2].y = intersectionPoint.y;

                vertex[3].x = edgePrevPoint.x;
                vertex[3].y = edgePrevPoint.y;

                blctx.strokePolygon( vertex, 4 );
                blctx.fillPolygon( vertex, 4 );
            }
            else
            {
                BLPoint vertex[5];
                vertex[0].x = iOrigin.x;
                vertex[0].y = iOrigin.y;

                vertex[1].x = edgePoint.x;
                vertex[1].y = edgePoint.y;

                vertex[2].x = edgePoint.x + ( currParallelVec.x * iMiterLimit * iRadius );
                vertex[2].y = edgePoint.y + ( currParallelVec.y * iMiterLimit * iRadius );

                vertex[3].x = edgePrevPoint.x + ( prevParallelVec.x * iMiterLimit * iRadius );
                vertex[3].y = edgePrevPoint.y + ( prevParallelVec.y * iMiterLimit * iRadius );

                vertex[4].x = edgePrevPoint.x;
                vertex[4].y = edgePrevPoint.y;

                blctx.strokePolygon( vertex, 5 );
                blctx.fillPolygon( vertex, 5 );
            }
        }
    }
}

static void
_drawRadialJoint( ::ULIS::FVec2D& iOrigin
                , ::ULIS::FVec2D& iPrevSegmentVector
                , ::ULIS::FVec2D& iSegmentVector
                , double iRadius
                , uint32 iSteps )
{
    ::ULIS::FVec2D currPerpendicularVec = { iSegmentVector.y    , - iSegmentVector.x     };
    ::ULIS::FVec2D prevPerpendicularVec = { iPrevSegmentVector.y, - iPrevSegmentVector.x };
    ::ULIS::FVec2D edgePrevPoint = iOrigin + ( prevPerpendicularVec * iRadius );
    ::ULIS::FVec2D edgePoint = iOrigin + ( currPerpendicularVec * iRadius );
    ::ULIS::FVec2D shortestTest = edgePoint - edgePrevPoint;
    // have to clamp due to imprecision of the dot product
    double dot = std::clamp<double>( currPerpendicularVec.DotProduct( prevPerpendicularVec ), -1.0f, 1.0f );
    double angle = acos( dot );
    double a = ( iSteps ) ? angle / iSteps : 0.0f;
    BLPoint vertex[3];
    BLContext& blctx = FOdysseyVectorEngine::GetBLContext();

    // Find on which side should the joint be drawn by comparing the directions of our vectors
    if ( shortestTest.DotProduct( iPrevSegmentVector ) < 0 )
    {
        ::ULIS::FVec2D tmp = currPerpendicularVec;
        currPerpendicularVec = -tmp;

               tmp = prevPerpendicularVec;
        prevPerpendicularVec = -tmp;

        a = -a;
    }

    // start drawing triangles at origin
    vertex[0].x = ( iOrigin.x );
    vertex[0].y = ( iOrigin.y );

    for ( uint32 i = 0; i < iSteps; i++ )
    {
        double cosa = cos(a);
        double sina = sin(a);
        ::ULIS::FVec2D interpolatedVector = { currPerpendicularVec.x * cosa + currPerpendicularVec.y * sina,
                                     -currPerpendicularVec.x * sina + currPerpendicularVec.y * cosa };

        vertex[1].x = vertex[0].x + ( currPerpendicularVec.x * iRadius );
        vertex[1].y = vertex[0].y + ( currPerpendicularVec.y * iRadius );

        vertex[2].x = vertex[0].x + ( interpolatedVector.x * iRadius );
        vertex[2].y = vertex[0].y + ( interpolatedVector.y * iRadius );

        blctx.strokePolygon( vertex , 3 );
        blctx.fillPolygon( vertex, 3 );

        currPerpendicularVec = interpolatedVector;
    }
}

static void
_drawLinearJoint( ::ULIS::FVec2D& iOrigin
                , ::ULIS::FVec2D& iPrevSegmentVector
                , ::ULIS::FVec2D& iSegmentVector
                , double iRadius )
{
    ::ULIS::FVec2D currPerpendicularVec = { iSegmentVector.y    , - iSegmentVector.x     };
    ::ULIS::FVec2D prevPerpendicularVec = { iPrevSegmentVector.y, - iPrevSegmentVector.x };
    ::ULIS::FVec2D edgePrevPoint = iOrigin + ( prevPerpendicularVec * iRadius );
    ::ULIS::FVec2D edgePoint = iOrigin + ( currPerpendicularVec * iRadius );
    ::ULIS::FVec2D shortestTest = edgePoint - edgePrevPoint;
    BLContext& blctx = FOdysseyVectorEngine::GetBLContext();
    BLPoint vertex[3];

    if ( shortestTest.DotProduct( iPrevSegmentVector ) < 0 )
    {
        ::ULIS::FVec2D tmp = currPerpendicularVec;

        currPerpendicularVec = -tmp;

                tmp = prevPerpendicularVec;
        prevPerpendicularVec = -tmp;
    }

    vertex[0].x = ( iOrigin.x );
    vertex[0].y = ( iOrigin.y );

    vertex[1].x = vertex[0].x + ( currPerpendicularVec.x * iRadius );
    vertex[1].y = vertex[0].y + ( currPerpendicularVec.y * iRadius );

    vertex[2].x = vertex[0].x + ( prevPerpendicularVec.x * iRadius );
    vertex[2].y = vertex[0].y + ( prevPerpendicularVec.y * iRadius );

    blctx.strokePolygon( vertex, 3 );
    blctx.fillPolygon( vertex, 3 );
}

void
FOdysseyVectorPathCubic::DrawJoint( FOdysseyVectorSegmentCubic* iPrevSegment
                           , FOdysseyVectorSegmentCubic& iSegment
                           , double iRadius )
{
    if ( iPrevSegment )
    {
        ::ULIS::FVec2D prevSegmentVector = iPrevSegment->GetVectorAtEnd( false );
        ::ULIS::FVec2D segmentVector = iSegment.GetVectorAtStart( false );
        ::ULIS::FVec2D& origin = iSegment.GetPoint(0)->GetCoords();

        if ( prevSegmentVector.DistanceSquared() && segmentVector.DistanceSquared() )
        {
            prevSegmentVector.Normalize();
            segmentVector.Normalize();

            // if the dot product equals to 1.0f, then the point is perfectly smooth, hence there is no need for joints.
            if ( prevSegmentVector.DotProduct(segmentVector) < 1.0f )
            {
                switch ( mJointType )
                {
                    case JOINT_TYPE_LINEAR :
                        _drawLinearJoint ( origin, prevSegmentVector, segmentVector, iRadius );
                    break;

                    case JOINT_TYPE_MITER :
                        _drawMiterJoint ( origin, prevSegmentVector, segmentVector, iRadius, 4.0f );
                    break;

                    case JOINT_TYPE_RADIAL :
                        _drawRadialJoint ( origin, prevSegmentVector, segmentVector, iRadius, 24 );
                    break;

                    default:
                    break;
                }
            }
        }
    }
}

void
FOdysseyVectorPathCubic::DrawShapeVariable( ::ULIS::FRectD &iRoi, uint64 iFlags )
{
    BLContext& blctx = FOdysseyVectorEngine::GetBLContext();
    blctx.setCompOp( BL_COMP_OP_SRC_OVER );

    // We fill with stroke color because our curve is made of filled shapes.
    blctx.setFillStyle(BLRgba32(mStrokeColor));
    blctx.setStrokeStyle(BLRgba32(mStrokeColor));

    if( mSegmentList.size() )
    {
        for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
        {
            FOdysseyVectorSegmentCubic* segment = static_cast<FOdysseyVectorSegmentCubic*>(*it);
            FOdysseyVectorSegmentCubic* prevSegment = static_cast<FOdysseyVectorSegmentCubic*>( segment->GetPreviousSegment() );
            double segmentStartRadius = segment->GetPoint(0)->GetRadius();
            ::ULIS::FRectD clip = iRoi & segment->GetBoundingBox();

            if( ( iRoi.Area() == 0.0f ) || clip.Area() )
            {
                segment->Draw( iRoi );
            }

            if ( prevSegment )
            {
                DrawJoint( prevSegment
                         , *segment
                         , segmentStartRadius );
            }
        }
    }
}

void
FOdysseyVectorPathCubic::Mirror( bool iMirrorX, bool iMirrorY )
{
    double factorX = ( iMirrorX ) ? -1.0f : 1.0f;
    double factorY = ( iMirrorY ) ? -1.0f : 1.0f;

    for( std::list<FOdysseyVectorPoint*>::iterator it = mPointList.begin(); it != mPointList.end(); ++it )
    {
        FOdysseyVectorPointCubic* cubicPoint = static_cast<FOdysseyVectorPointCubic*>(*it);

        cubicPoint->Set( cubicPoint->GetX() * factorX, cubicPoint->GetY() * factorY );
    }

    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(*it);
        FOdysseyVectorHandleSegment& ctrlPoint0 = static_cast<FOdysseyVectorHandleSegment&>(cubicSegment->GetControlPoint(0));
        FOdysseyVectorHandleSegment& ctrlPoint1 = static_cast<FOdysseyVectorHandleSegment&>(cubicSegment->GetControlPoint(1));

        ctrlPoint0.Set( ctrlPoint0.GetX() * factorX, ctrlPoint0.GetY() * factorY );
        ctrlPoint1.Set( ctrlPoint1.GetX() * factorX, ctrlPoint1.GetY() * factorY );
    }

    /*Update();*/
}

FOdysseyVectorObject*
FOdysseyVectorPathCubic::CopyShape()
{
    FOdysseyVectorPathCubic* cubicPathCopy = new FOdysseyVectorPathCubic ( );
    std::map<FOdysseyVectorPointCubic*, FOdysseyVectorPointCubic*> lookupTable;

    for( std::list<FOdysseyVectorPoint*>::iterator it = mPointList.begin(); it != mPointList.end(); ++it )
    {
        FOdysseyVectorPointCubic* originalPoint = static_cast<FOdysseyVectorPointCubic*>(*it);
        FOdysseyVectorPointCubic* newPoint = new FOdysseyVectorPointCubic( originalPoint->GetX()
                                                           , originalPoint->GetY()
                                                           , originalPoint->GetRadius() );

        lookupTable.insert( std::make_pair( originalPoint, newPoint ) );

        cubicPathCopy->AddPoint( newPoint );
    }

    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegmentCubic* originalSegment = static_cast<FOdysseyVectorSegmentCubic*>(*it);
        FOdysseyVectorPointCubic* point0 = static_cast<FOdysseyVectorPointCubic*>( originalSegment->GetPoint(0) );
        FOdysseyVectorPointCubic* point1 = static_cast<FOdysseyVectorPointCubic*>( originalSegment->GetPoint(1) );
        FOdysseyVectorSegmentCubic* newSegment = new FOdysseyVectorSegmentCubic( *cubicPathCopy
                                                                 , lookupTable[point0]
                                                                 , originalSegment->GetControlPoint(0).GetX()
                                                                 , originalSegment->GetControlPoint(0).GetY()
                                                                 , originalSegment->GetControlPoint(1).GetX()
                                                                 , originalSegment->GetControlPoint(1).GetY()
                                                                 , lookupTable[point1] );

        cubicPathCopy->AddSegment( newSegment );

        newSegment->BuildVariable();
    }

    return static_cast<FOdysseyVectorObject*>( cubicPathCopy );
}

void
FOdysseyVectorPathCubic::Merge( FOdysseyVectorPathCubic& iCubicPath )
{
    std::list<FOdysseyVectorSegment*>& segmentList = iCubicPath.mSegmentList;
    std::list<FOdysseyVectorPoint*>& pointList = iCubicPath.mPointList;

    std::map<FOdysseyVectorPointCubic*, FOdysseyVectorPointCubic*> lookupTable;

    for( std::list<FOdysseyVectorPoint*>::iterator it = pointList.begin(); it != pointList.end(); ++it )
    {
        FOdysseyVectorPointCubic* originalPoint = static_cast<FOdysseyVectorPointCubic*>(*it);
        FOdysseyVectorPointCubic* newPoint = new FOdysseyVectorPointCubic( originalPoint->GetX()
                                                           , originalPoint->GetY()
                                                           , originalPoint->GetRadius() );

        lookupTable.insert( std::make_pair( originalPoint, newPoint ) );

        mPointList.push_back ( newPoint );
    }

    for( std::list<FOdysseyVectorSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it )
    {
        FOdysseyVectorSegmentCubic* originalSegment = static_cast<FOdysseyVectorSegmentCubic*>(*it);
        FOdysseyVectorPointCubic* point0 = static_cast<FOdysseyVectorPointCubic*>( originalSegment->GetPoint(0) );
        FOdysseyVectorPointCubic* point1 = static_cast<FOdysseyVectorPointCubic*>( originalSegment->GetPoint(1) );
        FOdysseyVectorSegmentCubic* newSegment = new FOdysseyVectorSegmentCubic( *this
                                                                 , lookupTable[point0]
                                                                 , originalSegment->GetControlPoint(0).GetX()
                                                                 , originalSegment->GetControlPoint(0).GetY()
                                                                 , originalSegment->GetControlPoint(1).GetX()
                                                                 , originalSegment->GetControlPoint(1).GetY()
                                                                 , lookupTable[point1] );

        FOdysseyVectorPath::AddSegment( newSegment );

        newSegment->BuildVariable();
    }
}
