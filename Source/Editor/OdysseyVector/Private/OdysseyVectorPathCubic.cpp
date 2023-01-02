#include "OdysseyVectorPathCubic.h"

UOdysseyVectorPathCubic::UOdysseyVectorPathCubic()
    : UOdysseyVectorPath()
{
    setJointMiter();
}

void
UOdysseyVectorPathCubic::Init( std::string iName )
{
    SetName( iName );
}

void
UOdysseyVectorPathCubic::setJointRadial()
{
    mJointType = JOINT_TYPE_RADIAL;
}

void
UOdysseyVectorPathCubic::setJointMiter()
{
    mJointType = JOINT_TYPE_MITER;
}

void
UOdysseyVectorPathCubic::setJointLinear()
{
    mJointType = JOINT_TYPE_LINEAR;
}

void
UOdysseyVectorPathCubic::setJointNone()
{
    mJointType = JOINT_TYPE_NONE;
}

UOdysseyVectorSegmentCubic*
UOdysseyVectorPathCubic::AppendVertex( UOdysseyVectorVertexCubic* iVertex
                                     , bool iConnect
                                     , bool iBuildSegments )
{
    UOdysseyVectorVertexCubic* lastVertex = static_cast<UOdysseyVectorVertexCubic*>( GetLastVertex() );

    mVertexList.push_back ( iVertex );

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

UOdysseyVectorObject*
UOdysseyVectorPathCubic::PickShape( double iX
                                  , double iY
                                  , double iRadius )
{
    BLPath path;
    BLPoint testPoint = { iX, iY };

/*
    for( std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        UOdysseyVectorSegmentCubic* segment = static_cast<UOdysseyVectorSegmentCubic*>(*it);
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
    for( std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        UOdysseyVectorSegmentCubic* cubicSegment = static_cast<UOdysseyVectorSegmentCubic*>(*it);

        if ( cubicSegment->Pick ( iX, iY, iRadius ) == true )
        {
            return this;
        }
    }

    return PickLoops( iX, iY, iRadius );
}

bool
UOdysseyVectorPathCubic::PickPoint( double iX
                                  , double iY
                                  , double iSelectionRadius
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
                mSelectedPointList.push_back( vertex );
            }
        }

        if( iSelectionFlags & PICK_HANDLE_POINT )
        {
            // Pick point handle
            if( ( fabs( vertex->GetX() + ( perpendicularVector.x * vertex->GetRadius() ) - iX ) <= iSelectionRadius ) &&
                ( fabs( vertex->GetY() + ( perpendicularVector.y * vertex->GetRadius() ) - iY ) <= iSelectionRadius ) )
            {
                mSelectedPointList.push_back( vertex->GetControlPoint() );

                return true;
            }

            // Pick point handle on the other side
            if( ( fabs( vertex->GetX() - ( perpendicularVector.x * vertex->GetRadius() ) - iX ) <= iSelectionRadius ) &&
                ( fabs( vertex->GetY() - ( perpendicularVector.y * vertex->GetRadius() ) - iY ) <= iSelectionRadius ) )
            {
                mSelectedPointList.push_back( vertex->GetControlPoint() );

                return true;
            }
        }
    }

    if( iSelectionFlags & PICK_HANDLE_SEGMENT )
    {
        for( std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
        {
            UOdysseyVectorSegmentCubic* segment = static_cast<UOdysseyVectorSegmentCubic*>(*it);
            UOdysseyVectorPoint* ctrlPoint0 = segment->GetControlPoint( 0 );
            UOdysseyVectorPoint* ctrlPoint1 = segment->GetControlPoint( 1 );

            if( ( fabs( ctrlPoint0->GetX() - iX ) <= iSelectionRadius ) &&
                ( fabs( ctrlPoint0->GetY() - iY ) <= iSelectionRadius ) )
            {
                mSelectedPointList.push_back( ctrlPoint0 );

                return true;
            }

            if( ( fabs( ctrlPoint1->GetX() - iX ) <= iSelectionRadius ) &&
                ( fabs( ctrlPoint1->GetY() - iY ) <= iSelectionRadius ) )
            {
                mSelectedPointList.push_back( ctrlPoint1 );

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
    BLContext& blctx = FOdysseyVectorEngine::GetBLContext();
    BLPath path;

    if ( IsLoop() && firstVertex )
    {
        blctx.setCompOp( BL_COMP_OP_SRC_COPY );
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

        blctx.setFillStyle( BLRgba32( mFillColor ) );
        blctx.fillPath( path );
    }
}

void
UOdysseyVectorPathCubic::DrawStructure( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    BLContext& blctx = FOdysseyVectorEngine::GetBLContext();
    BLPath path;
    UOdysseyVectorVertexCubic *firstVertex = static_cast<UOdysseyVectorVertexCubic*>( GetFirstVertex() );
    BLPoint localVector = mInverseWorldMatrix.mapVector ( 1.0f, 1.0f );
    ::ULIS::FVec2D factor = { localVector.x, localVector.y };
    double handleRadiusX = 6.0f * factor.x;
    double handleRadiusY = 6.0f * factor.y;
    double handleWidth = handleRadiusX * 2.0f;
    double handleHeight = handleRadiusY * 2.0f;

    if ( firstVertex )
    {
        blctx.setCompOp( BL_COMP_OP_SRC_COPY );
        /*iBLContext.setFillStyle(BLRgba32(0xFFFFFFFF));
        iBLContext.setStrokeStyle(BLRgba32(0xFF000000));*/

        for( std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
        {
            UOdysseyVectorSegmentCubic *segment = static_cast<UOdysseyVectorSegmentCubic*>(*it);
            ::ULIS::FVec2D& point0 = segment->GetPoint( 0 )->GetCoords();
            ::ULIS::FVec2D& point1 = segment->GetPoint( 1 )->GetCoords();
            ::ULIS::FVec2D& ctrlPoint0 = segment->GetControlPoint( 0 )->GetCoords();
            ::ULIS::FVec2D& ctrlPoint1 = segment->GetControlPoint( 1 )->GetCoords();
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
        blctx.setStrokeWidth( factor.Distance() );
        blctx.strokePath( path );

        for(std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it)
        {
            UOdysseyVectorSegmentCubic *segment = static_cast<UOdysseyVectorSegmentCubic*>(*it);

            segment->DrawStructure( iRoi, factor.x, factor.y );
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

        blctx.setFillStyle( BLRgba32( 0xFFFF00FF ) );
        blctx.fillRect( point->GetX() - handleRadiusX
                      , point->GetY() - handleRadiusY
                      , handleWidth
                      , handleHeight );

        blctx.setFillStyle( BLRgba32( 0xFF808080 ) );
        blctx.fillRect( point->GetX() + ctrlX - handleRadiusX
                      , point->GetY() + ctrlY - handleRadiusY
                      , handleWidth
                      , handleHeight );

        blctx.fillRect( point->GetX() - ctrlX - handleRadiusX
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

    if ( IsFilled() )
    {
        Fill( iRoi );
    }

    DrawLoops( iRoi, iFlags  );

    DrawShapeVariable( iRoi, iFlags );
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
UOdysseyVectorPathCubic::DrawJoint( UOdysseyVectorSegmentCubic* iPrevSegment
                                  , UOdysseyVectorSegmentCubic& iSegment
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
UOdysseyVectorPathCubic::DrawShapeVariable( ::ULIS::FRectD &iRoi, uint64 iFlags )
{
    BLContext& blctx = FOdysseyVectorEngine::GetBLContext();
    blctx.setCompOp( BL_COMP_OP_SRC_OVER );

    // We fill with stroke color because our curve is made of filled shapes.
    blctx.setFillStyle(BLRgba32(mStrokeColor));
    blctx.setStrokeStyle(BLRgba32(mStrokeColor));

    if( mSegmentList.size() )
    {
        for( std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
        {
            UOdysseyVectorSegmentCubic* segment = static_cast<UOdysseyVectorSegmentCubic*>(*it);
            UOdysseyVectorSegmentCubic* prevSegment = static_cast<UOdysseyVectorSegmentCubic*>( segment->GetPreviousSegment() );
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
UOdysseyVectorPathCubic::Merge( UOdysseyVectorPathCubic& iCubicPath )
{
    std::list<UOdysseyVectorSegment*>& segmentList = iCubicPath.mSegmentList;
    std::list<UOdysseyVectorVertex*>& vertexList = iCubicPath.mVertexList;

    std::map<UOdysseyVectorVertexCubic*, UOdysseyVectorVertexCubic*> lookupTable;

    for( std::list<UOdysseyVectorVertex*>::iterator it = vertexList.begin(); it != vertexList.end(); ++it )
    {
        UOdysseyVectorVertexCubic* originalVertex = static_cast<UOdysseyVectorVertexCubic*>(*it);
        UOdysseyVectorVertexCubic* newVertex = UOdysseyVectorVertexCubic::New( originalVertex->GetX()
                                                                             , originalVertex->GetY()
                                                                             , originalVertex->GetRadius() );

        lookupTable.insert( std::make_pair( originalVertex, newVertex ) );

        mVertexList.push_back ( newVertex );
    }

    for( std::list<UOdysseyVectorSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it )
    {
        UOdysseyVectorSegmentCubic* originalSegment = static_cast<UOdysseyVectorSegmentCubic*>(*it);
        UOdysseyVectorVertexCubic* vertex0 = static_cast<UOdysseyVectorVertexCubic*>( originalSegment->GetPoint(0) );
        UOdysseyVectorVertexCubic* vertex1 = static_cast<UOdysseyVectorVertexCubic*>( originalSegment->GetPoint(1) );
        UOdysseyVectorSegmentCubic* newSegment = UOdysseyVectorSegmentCubic::New( this
                                                                                , lookupTable[vertex0]
                                                                                , originalSegment->GetControlPoint(0)->GetX()
                                                                                , originalSegment->GetControlPoint(0)->GetY()
                                                                                , originalSegment->GetControlPoint(1)->GetX()
                                                                                , originalSegment->GetControlPoint(1)->GetY()
                                                                                , lookupTable[vertex1] );

        UOdysseyVectorPath::AddSegment( newSegment );

        newSegment->BuildVariable();
    }
}
