#include "OdysseyVectorPath.h"

FOdysseyVectorPath::~FOdysseyVectorPath()
{
   // TODO: free vertices and segments.
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
FOdysseyVectorPath::Init( const FString& iName )
{
    SetName( iName );
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


void
FOdysseyVectorPath::FlipHorizontal()
{
    for( std::list<FOdysseyVectorVertex*>::iterator it = mVertexList.begin(); it != mVertexList.end(); ++it )
    {
        FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(*it);

        vertex->SetX( -vertex->GetX() );
    }

    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegment* segment = static_cast<FOdysseyVectorSegment*>(*it);

        if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
        {
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);
            FOdysseyVectorHandleSegment* handle0 = cubicSegment->GetHandle(0);
            FOdysseyVectorHandleSegment* handle1 = cubicSegment->GetHandle(1);

            handle0->SetX( -handle0->GetX() );
            handle1->SetX( -handle1->GetX() );
        }
    }
};

void
FOdysseyVectorPath::FlipVertical()
{
    for( std::list<FOdysseyVectorVertex*>::iterator it = mVertexList.begin(); it != mVertexList.end(); ++it )
    {
        FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(*it);

        vertex->SetY( -vertex->GetY() );
    }

    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegment* segment = static_cast<FOdysseyVectorSegment*>(*it);

        if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
        {
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);
            FOdysseyVectorHandleSegment* handle0 = cubicSegment->GetHandle(0);
            FOdysseyVectorHandleSegment* handle1 = cubicSegment->GetHandle(1);

            handle0->SetY( -handle0->GetY() );
            handle1->SetY( -handle1->GetY() );
        }
    }
};

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

FOdysseyVectorSegment*
FOdysseyVectorPath::AppendVertex( FOdysseyVectorVertex* iPoint, FOdysseyVectorVertex* iPreviousPoint )
{
    AddVertex( iPoint );

    if ( iPreviousPoint )
    {
        if ( iPreviousPoint->GetSegmentCount() < 2 )
        {
            AddSegment( new FOdysseyVectorSegment( this, iPreviousPoint, iPoint ) );
        }
    }

    return NULL;
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

FOdysseyVectorObject*
FOdysseyVectorPath::CopyShape()
{
    return nullptr;
}

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
FOdysseyVectorPath::RemoveVertex( FOdysseyVectorVertex* iVertex )
{
    mVertexList.remove( iVertex );
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
FOdysseyVectorPath::Clear()
{
    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != --mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegment *segment = (*it);

        segment->GetVertex(0)->RemoveSegment(segment);
        segment->GetVertex(1)->RemoveSegment(segment);
        /*RemoveSegment( segment );*/ // this alters the list, hence the loop and leads to a crash
    }

    mSegmentList.clear();
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

void
FOdysseyVectorPath::DrawShape( uint64 iFlags )
{
    for(std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it)
    {
        FOdysseyVectorSegment *segment = (*it);

        segment->Draw();
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
