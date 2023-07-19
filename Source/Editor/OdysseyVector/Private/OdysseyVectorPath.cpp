#include "OdysseyVectorPath.h"

FOdysseyVectorPath::~FOdysseyVectorPath()
{
   // Free vertices and clear list in one go.
    mVertexList.remove_if( []( FOdysseyVectorVertex* iVertex )
                           {
                               delete iVertex;
                               return true;
                           });

   // Free segments and clear list in one go.
    mSegmentList.remove_if( []( FOdysseyVectorSegment* iSegment )
                           {
                               delete iSegment;
                               return true;
                           });
}

FOdysseyVectorPath::FOdysseyVectorPath( const FString& iName )
    : FOdysseyVectorObject( iName )
    , mPaintingCode( 0 )
{
    SetJointType( eJointType::Miter );

    mPathParam.Filled = false;
}

bool
FOdysseyVectorPath::HasBaseClass( uint32 iBaseClassID )
{
    if( mStaticClass == iBaseClassID )
    {
        return true;
    }

    return FOdysseyVectorObject::HasBaseClass( iBaseClassID );
}

void
FOdysseyVectorPath::ToVertexAndSectionArray( std::vector<FOdysseyVectorVertex*>& oVertexArray
                                           , std::vector<FOdysseyVectorSection*>& oSectionArray )
{
    if( mSegmentList.size() )
    {
        FOdysseyVectorSegment *segment = GetFirstSegment();
        FOdysseyVectorVertex* firstVertex = segment->GetVertex(0);
        FOdysseyVectorVertex* vertex = firstVertex;

        do
        {
            FOdysseyVectorVertex* nextVertex = segment->GetOtherVertex( vertex );
            FOdysseyVectorSection* section = vertex->GetSection( segment );
            FOdysseyVectorSegment *nextSegment = nextVertex->GetOtherSegment( segment );

            oVertexArray.push_back( vertex );
            oSectionArray.push_back( section );

            vertex = nextVertex;
            segment = nextSegment;
        }
        while( segment && ( vertex != firstVertex ) );
    }
}

bool
FOdysseyVectorPath::HasIntersections()
{
    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegment* segment = static_cast<FOdysseyVectorSegment*>(*it);

        if( segment->GetIntersectionVertexList().size() )
        {
            return true;
        }
    }

    return false;
}

void
FOdysseyVectorPath::SelectVertex( FOdysseyVectorVertex* iVertex )
{
    iVertex->SetSelected( true );

    mSelectedVertexList.push_back( iVertex );
}

bool
FOdysseyVectorPath::IsLoop()
{
    return ( mSegmentList.size() == mVertexList.size() ) ? true : false;
}

bool
FOdysseyVectorPath::IsFilled()
{
    return mPathParam.Filled;
}

void
FOdysseyVectorPath::SetFilled( bool iIsFilled )
{
    mPathParam.Filled = iIsFilled;
}

eJointType
FOdysseyVectorPath::GetJointType()
{
    return mPathParam.JointType;
}

void
FOdysseyVectorPath::SetJointType( eJointType iJointType )
{
    mPathParam.JointType = iJointType;
}

void
FOdysseyVectorPath::UnselectAllVertices()
{
     mSelectedVertexList.remove_if( []( FOdysseyVectorVertex* iVertex )
                                    { 
                                        iVertex->SetSelected( false );

                                        return true;
                                    } );
}

std::list<FOdysseyVectorVertex*>&
FOdysseyVectorPath::GetSelectedVertexList()
{
    return mSelectedVertexList;
}

void
FOdysseyVectorPath::SetPaintingCode( uint32 iPaintingCode )
{
    mPaintingCode = iPaintingCode;
}

uint32
FOdysseyVectorPath::GetPaintingCode()
{
    return mPaintingCode;
}

/*void
FOdysseyVectorPath::AddLoop( FOdysseyVectorCycle* iLoop )
{
    mLoopList.push_back( iLoop );

    iLoop->Attach();

    iLoop->SetParent ( this );

    printf("%s: Adding loop\n", __func__ );
}*/

/*void
FOdysseyVectorPath::RemoveLoop( FOdysseyVectorCycle* iLoop )
{
    mLoopList.remove( iLoop );

    iLoop->Detach();

    iLoop->SetParent ( nullptr );

    printf("%s: Removing loop\n", __func__ );
}*/

void
FOdysseyVectorPath::UpdateBBox()
{
    double xmin = DBL_MAX, ymin = DBL_MAX, xmax = -DBL_MAX, ymax = -DBL_MAX;
    bool hasBBox = false;

    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegment* segment = static_cast<FOdysseyVectorSegment*>(*it);
        ::ULIS::FRectD segmentBBox = segment->GetBoundingBox( false );
        double rx1 = segmentBBox.x
             , ry1 = segmentBBox.y
             , rx2 = segmentBBox.x + segmentBBox.w
             , ry2 = segmentBBox.y + segmentBBox.h;

        hasBBox = true;

        if ( rx1 < xmin ) xmin = rx1;
        if ( ry1 < ymin ) ymin = ry1;
        if ( rx2 > xmax ) xmax = rx2;
        if ( ry2 > ymax ) ymax = ry2;
    }

    // This is in case there are oprhaned vertices, which should not happen. Once we
    // are sure orphaned vertices are impossible, the loop below can be removed.
    for( std::list<FOdysseyVectorVertex*>::iterator it = mVertexList.begin(); it != mVertexList.end(); ++it )
    {
        FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(*it);
        ::ULIS::FVec2D& vertexCoords = vertex->GetCoords();

        hasBBox = true;

        if ( vertexCoords.x < xmin ) xmin = vertexCoords.x;
        if ( vertexCoords.y < ymin ) ymin = vertexCoords.y;
        if ( vertexCoords.x > xmax ) xmax = vertexCoords.x;
        if ( vertexCoords.y > ymax ) ymax = vertexCoords.y;
    }

    mBBox = ( hasBBox ) ? ::ULIS::FRectD::FromMinMax( xmin, ymin, xmax, ymax ) : ::ULIS::FRectD( 0.0f, 0.0f, 0.0f, 0.0f );
}

BLPath&
FOdysseyVectorPath::GetBLPath()
{
    return mBLPath;
}

void
FOdysseyVectorPath::UpdateShape( uint32 iUpdateFlags )
{
    mBLPath.clear();

    // update segments
    for ( std::list<FOdysseyVectorSegment*>::iterator it = mInvalidatedSegmentList.begin(); it != mInvalidatedSegmentList.end(); ++it )
    {
        FOdysseyVectorSegment* segment = static_cast<FOdysseyVectorSegment*>(*it);

        segment->Update();
    }

    mInvalidatedSegmentList.clear();

    UpdateBBox();

    // cache BL Path (for drawing structure for example)
    for ( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegment* segment = static_cast<FOdysseyVectorSegment*>(*it);
        ::ULIS::FVec2D& point0 = segment->GetVertex(0)->GetCoords();
        ::ULIS::FVec2D& point1 = segment->GetVertex(1)->GetCoords();

        if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
        {
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);
            ::ULIS::FVec2D& ctrlPoint0 = cubicSegment->GetHandle(0)->GetCoords();
            ::ULIS::FVec2D& ctrlPoint1 = cubicSegment->GetHandle(1)->GetCoords();

            mBLPath.moveTo( point0.x, point0.y );
            mBLPath.cubicTo( ctrlPoint0.x, ctrlPoint0.y
                           , ctrlPoint1.x, ctrlPoint1.y
                           , point1.x, point1.y );
        }
    }
}

void
FOdysseyVectorPath::InvalidateSegment( FOdysseyVectorSegment* iSegment )
{
    if( iSegment->IsInvalidated() == false )
    {
        mInvalidatedSegmentList.push_back( iSegment );

        Invalidate();
    }
}

void
FOdysseyVectorPath::AddVertex( FOdysseyVectorVertex* iVertex )
{
    mVertexList.push_back( iVertex );

    iVertex->SetPath( this );
}

void
FOdysseyVectorPath::UnselectVertex( FOdysseyVectorVertex* iVertex )
{
    mSelectedVertexList.remove( iVertex );

    iVertex->SetSelected( false );
}

void
FOdysseyVectorPath::RemoveVertex( FOdysseyVectorVertex* iVertex )
{
    mVertexList.remove( iVertex );

    if( iVertex->IsSelected() )
    {
        UnselectVertex( iVertex );
    }
}

void
FOdysseyVectorPath::AddSegment( FOdysseyVectorSegment* iSegment )
{
    mSegmentList.push_back( iSegment );

    iSegment->SetPath( this );

    iSegment->GetVertex(0)->AddSegment( iSegment );
    iSegment->GetVertex(1)->AddSegment( iSegment );

    Invalidate();
/*
    iSegment->GetVertex(0)->AddSection( iSegment->GetDefaultSection() );
    iSegment->GetVertex(1)->AddSection( iSegment->GetDefaultSection() );
*/
}

void
FOdysseyVectorPath::RemoveSegment( FOdysseyVectorSegment* iSegment )
{
    //iSegment->ClearIntersections(); // note: re-adds the default section

    mSegmentList.remove( iSegment );

    iSegment->GetVertex(0)->RemoveSegment( iSegment );
    iSegment->GetVertex(1)->RemoveSegment( iSegment );

    Invalidate();
/*
    iSegment->GetVertex(0)->RemoveSection(iSegment->GetDefaultSection());
    iSegment->GetVertex(1)->RemoveSection(iSegment->GetDefaultSection());
*/
}

std::list<FOdysseyVectorSegment*>&
FOdysseyVectorPath::GetSegmentList()
{
    return mSegmentList;
}

std::list<FOdysseyVectorVertex*>&
FOdysseyVectorPath::GetVertexList()
{
    return mVertexList;
}

FOdysseyVectorSegment*
FOdysseyVectorPath::GetLastSegment()
{
    if( mSegmentList.size() == 0 ) return nullptr;

    return mSegmentList.back();
}

FOdysseyVectorSegment*
FOdysseyVectorPath::GetFirstSegment()
{
    if( mSegmentList.size() == 0 ) return nullptr;

    return mSegmentList.front();
}

FOdysseyVectorVertex*
FOdysseyVectorPath::GetLastVertex()
{
    if( mVertexList.size() == 0 ) return nullptr;

    return mVertexList.back();
}

FOdysseyVectorVertex*
FOdysseyVectorPath::GetFirstVertex()
{
    if( mVertexList.size() == 0 ) return nullptr;

    return mVertexList.front();
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
_drawMiterJoint( FOdysseyVectorPath* iPath
               , ::ULIS::FVec2D& iOrigin
               , ::ULIS::FVec2D& iVector0
               , ::ULIS::FVec2D& iVector1
               , double iRadius
               , double iMiterLimit )
{
    ::ULIS::FVec2D parallelVec0 = iVector0;
    ::ULIS::FVec2D parallelVec1 = iVector1;
    ::ULIS::FVec2D perpendicularVec0 = {   parallelVec0.y, - parallelVec0.x };
    ::ULIS::FVec2D perpendicularVec1 = { - parallelVec1.y,   parallelVec1.x };
    ::ULIS::FVec2D edge0Point = iOrigin + ( perpendicularVec0 * iRadius );
    ::ULIS::FVec2D edge1Point = iOrigin + ( perpendicularVec1 * iRadius );
    ::ULIS::FVec2D shortestTest = edge0Point - edge1Point;
    // have to clamp due to imprecision of the dot product
    double dot = std::clamp<double>( shortestTest.DotProduct( parallelVec0 ), -1.0f, 1.0f );
    BLContext* blctx = iPath->GetScene()->GetEngine()->GetBLContext();

    BLMatrix2D& worldMatrix = iPath->GetWorldMatrix();
    BLPoint worldOrigin = worldMatrix.mapPoint( iOrigin.x, iOrigin.y );
    ::ULIS::FVec2D intersectionPoint;

    // Find on which side should the joint be drawn by comparing the directions of our vectors
    if ( dot < 0 )
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

                // we draw lines between the polygons to correct the artefacts, otherwise there is a thin line between the polygons
                // line stroking is done in world coordinates because we need a 1 pixel width
                blctx->save();
                blctx->resetMatrix();
                blctx->setStrokeWidth( 1.0f );
                blctx->strokeLine( worldOrigin, worldMatrix.mapPoint( vertex[1].x, vertex[1].y ) );
                blctx->strokeLine( worldOrigin, worldMatrix.mapPoint( vertex[3].x, vertex[3].y ) );
                blctx->restore();

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

                vertex[2].x = edge0Point.x - ( parallelVec0.x * iMiterLimit * iRadius );
                vertex[2].y = edge0Point.y - ( parallelVec0.y * iMiterLimit * iRadius );

                vertex[3].x = edge1Point.x - ( parallelVec1.x * iMiterLimit * iRadius );
                vertex[3].y = edge1Point.y - ( parallelVec1.y * iMiterLimit * iRadius );

                vertex[4].x = edge1Point.x;
                vertex[4].y = edge1Point.y;

                // we draw lines between the polygons to correct the artefacts, otherwise there is a thin line between the polygons
                // line stroking is done in world coordinates because we need a 1 pixel width
                blctx->save();
                blctx->resetMatrix();
                blctx->setStrokeWidth( 1.0f );
                blctx->strokeLine( worldOrigin, worldMatrix.mapPoint( vertex[1].x, vertex[1].y ) );
                blctx->strokeLine( worldOrigin, worldMatrix.mapPoint( vertex[4].x, vertex[4].y ) );
                blctx->restore();

                //blctx->strokePolygon( vertex, 5 );
                blctx->fillPolygon( vertex, 5 );
            }
        }
    }
}

static void
_drawRadialJoint( FOdysseyVectorPath* iPath
                , ::ULIS::FVec2D& iOrigin
                , ::ULIS::FVec2D& iVector0
                , ::ULIS::FVec2D& iVector1
                , double iRadius )
{
    ::ULIS::FVec2D parallelVec0 = iVector0;
    ::ULIS::FVec2D parallelVec1 = iVector1;
    ::ULIS::FVec2D perpendicularVec0 = {   parallelVec0.y, - parallelVec0.x };
    ::ULIS::FVec2D perpendicularVec1 = { - parallelVec1.y,   parallelVec1.x };
    ::ULIS::FVec2D edge0Point = iOrigin + ( perpendicularVec0 * iRadius );
    ::ULIS::FVec2D edge1Point = iOrigin + ( perpendicularVec1 * iRadius );
    ::ULIS::FVec2D shortestTest = edge0Point - edge1Point;
    // have to clamp due to imprecision of the dot product
    double dot = std::clamp<double>( shortestTest.DotProduct( parallelVec0 ), -1.0f, 1.0f );
    double angle = acos( std::clamp<double>( perpendicularVec0.DotProduct( perpendicularVec1 ), -1.0f, 1.0f ) );
    static const int steps = 24;
    double a = angle / steps;
    BLContext* blctx = iPath->GetScene()->GetEngine()->GetBLContext();
    BLMatrix2D& worldMatrix = iPath->GetWorldMatrix();
    BLPoint worldOrigin = worldMatrix.mapPoint( iOrigin.x, iOrigin.y );
    static BLPoint vertex[steps][3];

    // Find on which side should the joint be drawn by comparing the directions of our vectors
    if ( dot < 0 )
    {
        ::ULIS::FVec2D tmp = perpendicularVec0;
        perpendicularVec0 = -tmp;

        a = -a;
    }

    double cosa = cos(a);
    double sina = sin(a);

    for ( uint32 i = 0; i < steps; i++ )
    {
        // https://stackoverflow.com/questions/11773889/how-to-calculate-a-vector-from-an-angle-with-another-vector-in-2d
        ::ULIS::FVec2D interpolatedVector = { (  perpendicularVec0.x * cosa ) + ( perpendicularVec0.y * sina ),
                                              ( -perpendicularVec0.x * sina ) + ( perpendicularVec0.y * cosa ) };

        // start drawing triangles at origin
        vertex[i][0].x = ( iOrigin.x );
        vertex[i][0].y = ( iOrigin.y );

        vertex[i][1].x = vertex[i][0].x + ( perpendicularVec0.x * iRadius );
        vertex[i][1].y = vertex[i][0].y + ( perpendicularVec0.y * iRadius );

        vertex[i][2].x = vertex[i][0].x + ( interpolatedVector.x * iRadius );
        vertex[i][2].y = vertex[i][0].y + ( interpolatedVector.y * iRadius );

        //blctx->strokePolygon( vertex , 3 );
        blctx->fillPolygon( vertex[i], 3 );

        perpendicularVec0 = interpolatedVector;
    }

    // we draw lines between the polygons to correct the artefacts, otherwise there is a thin line between the polygons
    // line stroking is done in world coordinates because we need a 1 pixel width
    blctx->save();
    blctx->resetMatrix();
    blctx->setStrokeWidth( 1.0f );
    for ( uint32 i = 0; i < steps; i++ )
    {
        blctx->strokeLine( worldOrigin, worldMatrix.mapPoint( vertex[i][1].x, vertex[i][1].y ) );
        blctx->strokeLine( worldOrigin, worldMatrix.mapPoint( vertex[i][2].x, vertex[i][2].y ) );

    }
    blctx->restore();
}

static void
_drawLinearJoint( FOdysseyVectorPath* iPath
                , ::ULIS::FVec2D& iOrigin
                , ::ULIS::FVec2D& iVector0
                , ::ULIS::FVec2D& iVector1
                , double iRadius )
{
    ::ULIS::FVec2D parallelVec0 = iVector0;
    ::ULIS::FVec2D parallelVec1 = iVector1;
    ::ULIS::FVec2D perpendicularVec0 = {   parallelVec0.y, - parallelVec0.x };
    ::ULIS::FVec2D perpendicularVec1 = { - parallelVec1.y,   parallelVec1.x };
    ::ULIS::FVec2D edge0Point = iOrigin + ( perpendicularVec0 * iRadius );
    ::ULIS::FVec2D edge1Point = iOrigin + ( perpendicularVec1 * iRadius );
    ::ULIS::FVec2D shortestTest = edge0Point - edge1Point;
    // have to clamp due to imprecision of the dot product
    double dot = std::clamp<double>( shortestTest.DotProduct( parallelVec0 ), -1.0f, 1.0f );
    BLContext* blctx = iPath->GetScene()->GetEngine()->GetBLContext();
    BLPoint vertex[3];
    BLMatrix2D& worldMatrix = iPath->GetWorldMatrix();
    BLPoint worldOrigin = worldMatrix.mapPoint( iOrigin.x, iOrigin.y );

    if ( dot < 0 )
    {
        ::ULIS::FVec2D tmp = perpendicularVec0;

        perpendicularVec0 = -tmp;

                tmp = perpendicularVec1;
        perpendicularVec1 = -tmp;
    }

    vertex[0].x = ( iOrigin.x );
    vertex[0].y = ( iOrigin.y );

    vertex[1].x = vertex[0].x + ( perpendicularVec0.x * iRadius );
    vertex[1].y = vertex[0].y + ( perpendicularVec0.y * iRadius );

    vertex[2].x = vertex[0].x + ( perpendicularVec1.x * iRadius );
    vertex[2].y = vertex[0].y + ( perpendicularVec1.y * iRadius );

    // we draw lines between the polygons to correct the artefacts, otherwise there is a thin line between the polygons
    // line stroking is done in world coordinates because we need a 1 pixel width
    blctx->save();
    blctx->resetMatrix();
    blctx->setStrokeWidth( 1.0f );
    blctx->strokeLine( worldOrigin, worldMatrix.mapPoint( vertex[1].x, vertex[1].y ) );
    blctx->strokeLine( worldOrigin, worldMatrix.mapPoint( vertex[2].x, vertex[2].y ) );
    blctx->restore();

    //blctx->strokePolygon( vertex, 3 );
    blctx->fillPolygon( vertex, 3 );
}

void
FOdysseyVectorPath::InvalidateAllSegments()
{
    for(std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it)
    {
        FOdysseyVectorSegment *segment = (*it);

        segment->Invalidate();
    }
}

void
FOdysseyVectorPath::DrawJoint( FOdysseyVectorVertex* iVertex, uint64 iFlags )
{
    FOdysseyVectorSegment* segment0 = iVertex->GetFirstSegment();
    FOdysseyVectorSegment* segment1 = iVertex->GetLastSegment();
    double vertexRadius = iVertex->GetRadius();
    BLContext* blctx = GetScene()->GetEngine()->GetBLContext();

    if ( ( segment0 && segment1 ) && ( segment0 != segment1 ) )
    {
        ::ULIS::FVec2D segment0Vector = iVertex->GetVectorOnSegment(segment0, false);
        ::ULIS::FVec2D segment1Vector = iVertex->GetVectorOnSegment(segment1, false);
        ::ULIS::FVec2D& origin = iVertex->GetCoords();
        BLPoint worldOrigin = mWorldMatrix.mapPoint( origin.x, origin.y );

        if ( segment0Vector.DistanceSquared() && segment1Vector.DistanceSquared() )
        {
            ::ULIS::FVec2D perpendicularVector0, perpendicularVector1;

            segment0Vector.Normalize();
            segment1Vector.Normalize();

            perpendicularVector0 = ::ULIS::FVec2D( segment0Vector.y, -segment0Vector.x );
            perpendicularVector1 = ::ULIS::FVec2D( segment1Vector.y, -segment1Vector.x );

            blctx->save();
            blctx->resetMatrix();
            blctx->setStrokeWidth( 1.0f );
            blctx->strokeLine( worldOrigin, mWorldMatrix.mapPoint( origin.x + perpendicularVector0.x * vertexRadius
                                                                 , origin.y + perpendicularVector0.y * vertexRadius ) );
            blctx->strokeLine( worldOrigin, mWorldMatrix.mapPoint( origin.x - perpendicularVector0.x * vertexRadius
                                                                 , origin.y - perpendicularVector0.y * vertexRadius ) );
            blctx->strokeLine( worldOrigin, mWorldMatrix.mapPoint( origin.x + perpendicularVector1.x * vertexRadius
                                                                 , origin.y + perpendicularVector1.y * vertexRadius ) );
            blctx->strokeLine( worldOrigin, mWorldMatrix.mapPoint( origin.x - perpendicularVector1.x * vertexRadius
                                                                 , origin.y - perpendicularVector1.y * vertexRadius ) );
            blctx->restore();

            // if the dot product equals to 1.0f, then the point is perfectly smooth, hence there is no need for joints.
            if ( segment0Vector.DotProduct(segment1Vector) < 1.0f )
            {
                switch ( mPathParam.JointType )
                {
                    case eJointType::Linear :
                        _drawLinearJoint ( this, origin, segment0Vector, segment1Vector, vertexRadius );
                    break;

                    case eJointType::Miter :
                        _drawMiterJoint  ( this, origin, segment0Vector, segment1Vector, vertexRadius,  4.0f );
                    break;

                    case eJointType::Radial :
                        _drawRadialJoint ( this, origin, segment0Vector, segment1Vector, vertexRadius );
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

// static
::ULIS::FVec2D
FOdysseyVectorPath::GetPerpendicularVector( FOdysseyVectorVertex* iVertex, bool iNormalize )
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

///////////////////////////////////////////

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

// Pick from mask image
void
FOdysseyVectorPath::PickVertex( std::vector<FOdysseyVectorVertex*>& oPickedVertexArray )
{
    BLImage* maskImage = GetScene()->GetEngine()->GetBLMask();
    std::list<FOdysseyVectorVertex*>::iterator it;
    BLImageData imageData;

    maskImage->getData( &imageData );

    for( it = mVertexList.begin(); it != mVertexList.end(); ++it )
    {
        FOdysseyVectorVertex* vertex = (*it);
        ::ULIS::FVec2D& localCoords = vertex->GetCoords();
        // convert vertex coordinates to world coordinates. Easier to detect collision inside the picking circle.
        BLPoint worldCoords = mWorldMatrix.mapPoint( localCoords.x, localCoords.y );
        int32 x = (int32) worldCoords.x;
        int32 y = (int32) worldCoords.y;

        if( ( x >= 0 ) && ( x < imageData.size.w )
         && ( y >= 0 ) && ( y < imageData.size.h ) )
        {
            uint8 *pixel = static_cast<uint8*>( imageData.pixelData );
            uint32 offset = ( y * imageData.size.w ) + x;
            uint8 pixelValue = pixel[offset];

            if( pixelValue == 255 )
            {
                oPickedVertexArray.push_back( vertex );
            }
        }
    }
}

bool
FOdysseyVectorPath::Erase( const ::ULIS::FRectD &iRoi
                              , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                              , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray
                              , std::vector<FOdysseyVectorVertex*>& iRemovedVertexArray
                              , std::vector<FOdysseyVectorSegment*>& iRemovedSegmentArray )
{
    BLContext* blctx = GetScene()->GetEngine()->GetBLContext();
    BLImage* blimg = GetScene()->GetEngine()->GetBLMask(); // the mask image must be selected by the vector engine at this point
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
FOdysseyVectorPath::PickShape( const ::ULIS::FRectD &iRoi, uint32 iSelectionFlags )
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
        BLImage* blimg = GetScene()->GetEngine()->GetBLMask(); // the mask image must be selected by the vector engine at this point
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
FOdysseyVectorPath::PickPoint( double iWorldX
                                  , double iWorldY
                                  , double iSelectionRadius
                                  , std::vector<FOdysseyVectorPoint*>& oPickedPointArray
                                  , uint64 iSelectionFlags )
{
    for(std::list<FOdysseyVectorVertex*>::iterator it = mVertexList.begin(); it != mVertexList.end(); ++it)
    {
        FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(*it);
        ::ULIS::FVec2D perpendicularVector = FOdysseyVectorPath::GetPerpendicularVector( vertex, true );
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
FOdysseyVectorPath::Cut( const ::ULIS::FVec2D& linePoint0
                            , const ::ULIS::FVec2D& linePoint1
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
FOdysseyVectorPath::Fill()
{
    FOdysseyVectorVertex *firstVertex = static_cast<FOdysseyVectorVertex*>( GetFirstVertex() );

    if ( firstVertex )
    {
        BLContext* blctx = GetScene()->GetEngine()->GetBLContext();
        FColor& fillColor = mBackgroundBucket.GetColor();
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
FOdysseyVectorPath::DrawShape( uint64 iFlags )
{

    if ( mPathParam.Filled )
    {
        // TODO: precompute the filling (build the BLPath )
        Fill();
    }

    BLContext* blctx = GetScene()->GetEngine()->GetBLContext();
    FColor color = mForegroundBucket.GetColor();
    BLRgba32 strokeColor = BLRgba32( color.R, color.G, color.B, color.A );

    if( mSegmentList.size() )
    {
        // We fill with stroke color because our curve is made of filled shapes.
        //blctx->setFillRule( BL_FILL_RULE_NON_ZERO );
        blctx->setFillRule( BL_FILL_RULE_EVEN_ODD );
        blctx->setFillStyle( strokeColor );
        blctx->setStrokeStyle( strokeColor );

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

void
FOdysseyVectorPath::DrawStructure( FColor& iStrokeColor, double iStrokeWidth, bool iWorld )
{
    BLContext* blctx = GetScene()->GetEngine()->GetBLContext();
    BLRgba32 strokeColor = BLRgba32( iStrokeColor.R
                                   , iStrokeColor.G
                                   , iStrokeColor.B
                                   , iStrokeColor.A );

    blctx->save();

    if( iWorld )
    {
        blctx->resetMatrix();
    }

    blctx->setStrokeWidth( iStrokeWidth );
    blctx->setStrokeStyle( strokeColor );

    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegmentCubic* segment = static_cast<FOdysseyVectorSegmentCubic*>(*it);
        FOdysseyVectorVertex* vertex0 = segment->GetVertex(0);

        segment->DrawStructure( this, iWorld );
    }

    blctx->restore();
}

void
FOdysseyVectorPath::Mirror( bool iMirrorX, bool iMirrorY )
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
FOdysseyVectorPath::CopyShape()
{
    FOdysseyVectorPath* cubicPathCopy = new FOdysseyVectorPath( FString("Cubic Path") );
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
FOdysseyVectorPath::Merge( FOdysseyVectorPath* iMergedPath
                              , std::vector<FOdysseyVectorVertex*>& iAddedVertexArray
                              , std::vector<FOdysseyVectorSegment*>& iAddedSegmentArray )
{
    std::vector<FOdysseyVectorVertex*> vertexLookup;

    Merge( iMergedPath, vertexLookup, iAddedVertexArray, iAddedSegmentArray );
}

void
FOdysseyVectorPath::Merge( FOdysseyVectorPath* iMergedPath
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
FOdysseyVectorPath::GetType()
{
    return FOdysseyVectorObject::VECTORPATHTYPE;
}

void
FOdysseyVectorPath::SwitchSpace( FOdysseyVectorObject& iNewSpace )
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
FOdysseyVectorPath::SharpSegments( FOdysseyVectorVertex* iVertex, bool iBuildSegments, bool iPreserveHandleLength )
{
    std::list<FOdysseyVectorSegment*>& segmentList = iVertex->GetSegmentList();

    for( std::list<FOdysseyVectorSegment*>::iterator it = segmentList.begin(); it != segmentList.end(); ++it )
    {
        FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(*it);

        cubicSegment->GetHandle( iVertex )->Set( iVertex->GetX(), iVertex->GetY() );

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

// static
void
FOdysseyVectorPath::SmoothSegments( FOdysseyVectorVertex* iVertex, bool iBuildSegments, bool iPreserveHandleLength )
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
FOdysseyVectorPath::SmoothSegments( FOdysseyVectorVertex* iVertex, ::ULIS::FVec2D iPerpendicularVector, bool iBuildSegments, bool iPreserveHandleLength )
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
