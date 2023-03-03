#include "OdysseyVectorPath.h"

void UOdysseyVectorPath::Init( std::string iName )
{
    setJointMiter();
    SetName( iName );
}

void
UOdysseyVectorPath::setJointRadial()
{
    mJointType = JOINT_TYPE_RADIAL;
}

void
UOdysseyVectorPath::setJointMiter()
{
    mJointType = JOINT_TYPE_MITER;
}

void
UOdysseyVectorPath::setJointLinear()
{
    mJointType = JOINT_TYPE_LINEAR;
}

void
UOdysseyVectorPath::setJointNone()
{
    mJointType = JOINT_TYPE_NONE;
}

std::list<UOdysseyVectorPoint*>&
UOdysseyVectorPath::GetSelectedPointList()
{
    return mSelectedPointList;
}

UOdysseyVectorSegment*
UOdysseyVectorPath::AppendVertex( UOdysseyVectorVertex* iPoint, UOdysseyVectorVertex* iPreviousPoint )
{
    AddVertex( iPoint );

    if ( iPreviousPoint )
    {
        if ( iPreviousPoint->GetSegmentCount() < 2 )
        {
            AddSegment( UOdysseyVectorSegment::New( this, iPreviousPoint, iPoint ) );
        }
    }

    return NULL;
}

/*void
UOdysseyVectorPath::AddLoop( UOdysseyVectorLoop* iLoop )
{
    mLoopList.push_back( iLoop );

    iLoop->Attach();

    iLoop->SetParent ( this );

    printf("%s: Adding loop\n", __func__ );
}*/

/*void
UOdysseyVectorPath::RemoveLoop( UOdysseyVectorLoop* iLoop )
{
    mLoopList.remove( iLoop );

    iLoop->Detach();

    iLoop->SetParent ( nullptr );

    printf("%s: Removing loop\n", __func__ );
}*/

UOdysseyVectorObject*
UOdysseyVectorPath::CopyShape()
{
    return nullptr;
}

void
UOdysseyVectorPath::UpdateBBox()
{
    double x1 = DBL_MAX, y1 = DBL_MAX, x2 = -DBL_MAX, y2 = -DBL_MAX;

    for( std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        UOdysseyVectorSegment* segment = static_cast<UOdysseyVectorSegment*>(*it);
        ::ULIS::FRectD& coords = segment->GetBoundingBox();
        double rx1 = coords.x, ry1 = coords.y, rx2 = coords.x + coords.w, ry2 = coords.y + coords.h;

        if ( rx1 < x1 ) x1 = rx1;
        if ( ry1 < y1 ) y1 = ry1;
        if ( rx2 > x2 ) x2 = rx2;
        if ( ry2 > y2 ) y2 = ry2;
    }

    mBBox = ::ULIS::TRectangle<double>::FromMinMax( x1, y1, x2, y2 );
}

void
UOdysseyVectorPath::UpdateShape( uint32 iUpdateFlags )
{
    // update segments
    for ( std::list<UOdysseyVectorSegment*>::iterator it = mInvalidatedSegmentList.begin(); it != mInvalidatedSegmentList.end(); ++it )
    {
        UOdysseyVectorSegment* segment = static_cast<UOdysseyVectorSegment*>(*it);

        segment->Update();
    }

    mInvalidatedSegmentList.clear();

    // then update Loops
/*
    for ( std::list<UOdysseyVectorLoop*>::iterator it = mInvalidatedLoopList.begin(); it != mInvalidatedLoopList.end(); ++it )
    {
        UOdysseyVectorLoop* loop = static_cast<UOdysseyVectorLoop*>(*it);

        loop->Update();
    }

    mInvalidatedLoopList.clear();
*/
    UpdateBBox();
}

void
UOdysseyVectorPath::InvalidateSegment( UOdysseyVectorSegment* iSegment )
{
    mInvalidatedSegmentList.push_back( iSegment );

    Invalidate();
}

/*void
UOdysseyVectorPath::InvalidateLoop( UOdysseyVectorLoop* iLoop )
{
    mInvalidatedLoopList.push_back ( iLoop );

    Invalidate();
}*/

/*
void
UOdysseyVectorPath::DrawLoops( ::ULIS::FRectD &iRoi, uint64 iFlags )
{
    for( std::list<UOdysseyVectorLoop*>::iterator it = mLoopList.begin(); it != mLoopList.end(); ++it )
    {
        UOdysseyVectorLoop* loop = static_cast<UOdysseyVectorLoop*>(*it);

        //if ( loop->IsFilled() == true )
        //{
            loop->DrawShape( iRoi, iFlags );
        //}
    }
}
*/

/*
UOdysseyVectorObject*
UOdysseyVectorPath::PickLoops( double iX, double iY, double iRadius )
{
    for( std::list<UOdysseyVectorLoop*>::iterator it = mLoopList.begin(); it != mLoopList.end(); ++it )
    {
        UOdysseyVectorLoop* loop = static_cast<UOdysseyVectorLoop*>(*it);

        if ( loop->PickShape( iX, iY, iRadius ) )
        {
            return loop;
        }
    }

    return nullptr;
}
*/

/*
UOdysseyVectorLoop*
UOdysseyVectorPath::GetLoopByID( uint64 iID )
{
    for( std::list<UOdysseyVectorLoop*>::iterator it = mLoopList.begin(); it != mLoopList.end(); ++it )
    {
        UOdysseyVectorLoop* loop = static_cast<UOdysseyVectorLoop*>(*it);

        if ( iID == loop->GetID() )
        {
            return loop;
        }
    }

    return nullptr;
}
*/

void
UOdysseyVectorPath::AddVertex( UOdysseyVectorVertex* iVertex )
{
    mVertexList.push_back( iVertex );

    iVertex->SetPath( this );
}

void
UOdysseyVectorPath::RemoveVertex( UOdysseyVectorVertex* iVertex )
{
    mVertexList.remove( iVertex );
}

void
UOdysseyVectorPath::AddSegment( UOdysseyVectorSegment* iSegment )
{
    mSegmentList.push_back( iSegment );

    iSegment->SetPath( this );

    Cast<UOdysseyVectorVertex>(iSegment->GetPoint(0))->AddSegment( iSegment );
    Cast<UOdysseyVectorVertex>(iSegment->GetPoint(1))->AddSegment( iSegment );
}

void
UOdysseyVectorPath::Clear()
{
    for( std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != --mSegmentList.end(); ++it )
    {
        UOdysseyVectorSegment *segment = (*it);

        Cast<UOdysseyVectorVertex>(segment->GetPoint(0))->RemoveSegment(segment);
        Cast<UOdysseyVectorVertex>(segment->GetPoint(1))->RemoveSegment(segment);
        /*RemoveSegment( segment );*/ // this alters the list, hence the loop and leads to a crash
    }

    mSegmentList.clear();
}

void
UOdysseyVectorPath::RemoveSegment( UOdysseyVectorSegment* iSegment )
{
    mSegmentList.remove( iSegment );

    Cast<UOdysseyVectorVertex>(iSegment->GetPoint(0))->RemoveSegment( iSegment );
    Cast<UOdysseyVectorVertex>(iSegment->GetPoint(1))->RemoveSegment( iSegment );
}

std::list<UOdysseyVectorSegment*>&
UOdysseyVectorPath::GetSegmentList()
{
    return mSegmentList;
}

std::list<UOdysseyVectorVertex*>&
UOdysseyVectorPath::GetVertexList()
{
    return mVertexList;
}

UOdysseyVectorSegment*
UOdysseyVectorPath::GetLastSegment()
{
    if( mSegmentList.size() == 0 ) return nullptr;

    return mSegmentList.back();
}

UOdysseyVectorSegment*
UOdysseyVectorPath::GetFirstSegment()
{
    if( mSegmentList.size() == 0 ) return nullptr;

    return mSegmentList.front();
}

UOdysseyVectorVertex*
UOdysseyVectorPath::GetLastVertex()
{
    if( mVertexList.size() == 0 ) return nullptr;

    return mVertexList.back();
}

/*
bool
UOdysseyVectorPath::IsLoop()
{
    if ( mVertexList.size() )
    {
        if ( mVertexList.size() == mSegmentList.size() )
        {
            return true;
        }
    }

    return false;
}
*/

UOdysseyVectorVertex*
UOdysseyVectorPath::GetFirstVertex()
{
    if( mVertexList.size() == 0 ) return nullptr;

    return mVertexList.front();
}

void
UOdysseyVectorPath::DrawStructure( ::ULIS::FRectD& iRoi )
{
    BLContext* blctx = GetRoot()->GetEngine()->GetBLContext();

    blctx->setStrokeStyle( BLRgba32( 0xFF00FF00 ) );
    blctx->setStrokeWidth(1.0f);

    for(std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it)
    {
        UOdysseyVectorSegment *segment = (*it);

        segment->DrawStructure( iRoi );
    }
}

void
UOdysseyVectorPath::DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    for(std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it)
    {
        UOdysseyVectorSegment *segment = (*it);

        segment->Draw( iRoi );
    }

    /*DrawLoops( iRoi, iFlags );*/
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
_drawMiterJoint( UOdysseyVectorPath* iPath
               , ::ULIS::FVec2D& iOrigin
               , ::ULIS::FVec2D& iVector0
               , ::ULIS::FVec2D& iVector1
               , double iRadius
               , double iMiterLimit )
{
    ::ULIS::FVec2D parallelVec0 = - iVector0;
    ::ULIS::FVec2D parallelVec1 = - iVector1;
    ::ULIS::FVec2D perpendicularVec0 = {   parallelVec0.y, - parallelVec0.x };
    ::ULIS::FVec2D perpendicularVec1 = { - parallelVec1.y,   parallelVec1.x };
    ::ULIS::FVec2D edge0Point = iOrigin + ( perpendicularVec0 * iRadius );
    ::ULIS::FVec2D edge1Point = iOrigin + ( perpendicularVec1 * iRadius );
    ::ULIS::FVec2D shortestTest = edge1Point - edge0Point;
    BLContext* blctx = iPath->GetRoot()->GetEngine()->GetBLContext();
    ::ULIS::FVec2D intersectionPoint;

    blctx->setStrokeWidth( 1.0f );

    // Find on which side should the joint be drawn by comparing the directions of our vectors
    if ( shortestTest.DotProduct( parallelVec0 ) < 0 )
    {
        ::ULIS::FVec2D tmp = perpendicularVec0;
        perpendicularVec0 = -tmp;

                       tmp = perpendicularVec1;
        perpendicularVec1 = -tmp;

        edge0Point = iOrigin + ( perpendicularVec0 * iRadius );
        edge1Point = iOrigin + ( perpendicularVec1 * iRadius );
    }

    if( iRadius )
    {
        if ( intersectLine( edge0Point
                          , parallelVec0
                          , edge1Point
                          , parallelVec1
                          , intersectionPoint ) == true )
        {
            ::ULIS::FVec2D originToIntersection = intersectionPoint - iOrigin;

            double miterRatio = originToIntersection.Distance() / iRadius;

            if ( miterRatio < iMiterLimit )
            {
                BLPoint vertex[4];
                vertex[0].x = iOrigin.x;
                vertex[0].y = iOrigin.y;

                vertex[1].x = edge0Point.x;
                vertex[1].y = edge0Point.y;

                vertex[2].x = intersectionPoint.x;
                vertex[2].y = intersectionPoint.y;

                vertex[3].x = edge1Point.x;
                vertex[3].y = edge1Point.y;

                blctx->strokeLine( vertex[0].x, vertex[0].y, vertex[1].x, vertex[1].y );
                blctx->strokeLine( vertex[0].x, vertex[0].y, vertex[3].x, vertex[3].y );

                //blctx->strokePolygon( vertex, 4 );
                blctx->fillPolygon( vertex, 4 );
            }
            else
            {
                BLPoint vertex[5];
                vertex[0].x = iOrigin.x;
                vertex[0].y = iOrigin.y;

                vertex[1].x = edge0Point.x;
                vertex[1].y = edge0Point.y;

                vertex[2].x = edge0Point.x + ( parallelVec0.x * iMiterLimit * iRadius );
                vertex[2].y = edge0Point.y + ( parallelVec0.y * iMiterLimit * iRadius );

                vertex[3].x = edge1Point.x + ( parallelVec1.x * iMiterLimit * iRadius );
                vertex[3].y = edge1Point.y + ( parallelVec1.y * iMiterLimit * iRadius );

                vertex[4].x = edge1Point.x;
                vertex[4].y = edge1Point.y;

                blctx->strokeLine( vertex[0].x, vertex[0].y, vertex[1].x, vertex[1].y );
                blctx->strokeLine( vertex[0].x, vertex[0].y, vertex[4].x, vertex[4].y );

                //blctx->strokePolygon( vertex, 5 );
                blctx->fillPolygon( vertex, 5 );
            }
        }
    }
}

static void
_drawRadialJoint( UOdysseyVectorPath* iPath
                , ::ULIS::FVec2D& iOrigin
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
    BLContext* blctx = iPath->GetRoot()->GetEngine()->GetBLContext();

    blctx->setStrokeWidth( 1.0f );

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

        blctx->strokeLine( vertex[0].x, vertex[0].y, vertex[1].x, vertex[1].y );
        blctx->strokeLine( vertex[0].x, vertex[0].y, vertex[2].x, vertex[2].y );

        //blctx->strokePolygon( vertex , 3 );
        blctx->fillPolygon( vertex, 3 );

        currPerpendicularVec = interpolatedVector;
    }
}

static void
_drawLinearJoint( UOdysseyVectorPath* iPath
                , ::ULIS::FVec2D& iOrigin
                , ::ULIS::FVec2D& iPrevSegmentVector
                , ::ULIS::FVec2D& iSegmentVector
                , double iRadius )
{
    ::ULIS::FVec2D currPerpendicularVec = { iSegmentVector.y    , - iSegmentVector.x     };
    ::ULIS::FVec2D prevPerpendicularVec = { iPrevSegmentVector.y, - iPrevSegmentVector.x };
    ::ULIS::FVec2D edgePrevPoint = iOrigin + ( prevPerpendicularVec * iRadius );
    ::ULIS::FVec2D edgePoint = iOrigin + ( currPerpendicularVec * iRadius );
    ::ULIS::FVec2D shortestTest = edgePoint - edgePrevPoint;
    BLContext* blctx = iPath->GetRoot()->GetEngine()->GetBLContext();
    BLPoint vertex[3];

    blctx->setStrokeWidth( 1.0f );

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

    blctx->strokeLine( vertex[0].x, vertex[0].y, vertex[1].x, vertex[1].y );
    blctx->strokeLine( vertex[0].x, vertex[0].y, vertex[2].x, vertex[2].y );

    //blctx->strokePolygon( vertex, 3 );
    blctx->fillPolygon( vertex, 3 );
}

void
UOdysseyVectorPath::DrawJoint( UOdysseyVectorVertex* iVertex, ::ULIS::FRectD &iRoi, uint64 iFlags )
{
    UOdysseyVectorSegment* segment0 = iVertex->GetFirstSegment();
    UOdysseyVectorSegment* segment1 = iVertex->GetLastSegment();
    double vertexRadius = iVertex->GetRadius();

    if ( ( segment0 && segment1 ) && ( segment0 != segment1 ) )
    {
        ::ULIS::FVec2D segment0Vector = iVertex->GetVectorOnSegment(segment0, false);
        ::ULIS::FVec2D segment1Vector = iVertex->GetVectorOnSegment(segment1, false);
        ::ULIS::FVec2D& origin = iVertex->GetCoords();

        if ( segment0Vector.DistanceSquared() && segment1Vector.DistanceSquared() )
        {
            segment0Vector.Normalize();
            segment1Vector.Normalize();

            // if the dot product equals to 1.0f, then the point is perfectly smooth, hence there is no need for joints.
            if ( segment0Vector.DotProduct(segment1Vector) < 1.0f )
            {
                switch ( mJointType )
                {
                    case JOINT_TYPE_LINEAR :
                        _drawLinearJoint ( this, origin, segment0Vector, segment1Vector, vertexRadius );
                    break;

                    case JOINT_TYPE_MITER :
                        _drawMiterJoint  ( this, origin, segment0Vector, segment1Vector, vertexRadius,  4.0f );
                    break;

                    case JOINT_TYPE_RADIAL :
                        _drawRadialJoint ( this, origin, segment0Vector, segment1Vector, vertexRadius, 24.0f );
                    break;

                    default:
                    break;
                }
            }
            else
            {

            }
        }
    }
}
