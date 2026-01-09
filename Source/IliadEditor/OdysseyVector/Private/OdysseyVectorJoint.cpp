// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyVectorJoint.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVector.h"

FOdysseyVectorJoint::~FOdysseyVectorJoint()
{
}

FOdysseyVectorJoint::FOdysseyVectorJoint( FOdysseyVectorVertex* iVertex )
    : mVertex( iVertex )
    , mLength( 0.0f )
    , mTextureStartU ( 0.0f )
    , mTextureEndU ( 0.0f )
{
}

// https://gamedev.net/forums/topic/647810-intersection-point-of-two-vectors/5094071/
static bool intersectLine( const ::ULIS::FVec2D& iOrigin0
                         , const ::ULIS::FVec2D& iDirection0
                         , const ::ULIS::FVec2D& iOrigin1
                         , const ::ULIS::FVec2D& iDirection1
                         , ::ULIS::FVec2D& oOut ) {
    ::ULIS::FVec2D c = iOrigin0 - iOrigin1;
    double cross = ( iDirection0.y * iDirection1.x ) - ( iDirection0.x * iDirection1.y );

    if ( cross )
    {
        double t = ( ( c.x * iDirection1.y ) - ( c.y * iDirection1.x ) ) / cross;

        oOut = iOrigin0 + ( iDirection0 * t );

        return true;
    }

    return false;
}

void
FOdysseyVectorJoint::SetTextureU( double iTextureStartU, double iTextureEndU )
{
    mTextureStartU = iTextureStartU;
    mTextureEndU = iTextureEndU;
}

double
FOdysseyVectorJoint::GetTextureStartU()
{
    return mTextureStartU;
}

double
FOdysseyVectorJoint::GetTextureEndU()
{
    return mTextureEndU;
}

double
FOdysseyVectorJoint::GetLength()
{
    return mLength;
}

void
FOdysseyVectorJoint::Draw( BLContext* iBLContext
                         , FOdysseyVectorEngine* iVectorEngine
                         , double iStartU
                         , double iEndU
                         , double iCombinedOpacity
                         , uint64 iDrawingFlags )
{
    FOdysseyVectorPath* path = mVertex->GetOwnerAsPath();
    const BLMatrix2D worldMatrix = iBLContext->user_transform();
    FOdysseyVectorBrush& brush = path->GetBrush();
    FColor foregroundColor = path->GetForegroundColor();
    double difU = iEndU - iStartU;
    bool skipFirstHorizontalLine = false; // to prevent overlapping of polygons,
                                          // which would be visible when opacity < 1.0f.

    if( brush.pixels ) // use our own routines
    {
        for( int i = 0; i < mPolygonCache.size(); i++ )
        {
            FOdysseyVectorPolygon3* polygon = &mPolygonCache[i];
            uint64 polygonDrawingFlags = 0;
            double polyU[3];

            //polygonDrawingFlags |= skipFirstHorizontalLine ? FPolygonDrawingFlags::SKIPFIRSTHLINE : 0;
            polygonDrawingFlags |= brush.ColorFromBrush    ? 0 : FPolygonDrawingFlags::BRUSHALPHAONLY;
            polygonDrawingFlags |= brush.BilinearFiltering ? FPolygonDrawingFlags::BILINEARFILTERING : 0;

            for( uint32 j = 0; j < polygon->pointCount; j++ )
            {
                polyU[j] = iStartU + ( polygon->U[j] * difU );
            }

            iVectorEngine->FillTriangle( iBLContext
                                       , polygon->point
                                       , polyU
                                       , polygon->V
                                       , iCombinedOpacity
                                       , foregroundColor
                                       , (int8*) brush.pixels
                                       , brush.width
                                       , brush.height
                                       , brush.bitsPerPixel
                                       , polygonDrawingFlags );
        }
    }
    else //otherwise use Blend2D's
    {
        if( mPolygonCache.size() )
        {
            // for filled overlaps
            iBLContext->set_fill_rule( BL_FILL_RULE_NON_ZERO );
            BLPath joint;

            joint.move_to( mPolygonCache[0].point[0].x, mPolygonCache[0].point[0].y );
            joint.line_to( mPolygonCache[0].point[1].x, mPolygonCache[0].point[1].y );

            for( int i = 0; i < mPolygonCache.size(); i++ )
            {
                joint.line_to( mPolygonCache[i].point[2].x, mPolygonCache[i].point[2].y );
            }

            joint.close();

            iBLContext->fill_path( joint );
        }

        //if( mPolygonCache.size() == 0 )
        {
            ::ULIS::FVec2D& vertexCoords =  mVertex->GetCoords();
            BLPoint vertexWorldCoords = worldMatrix.map_point( vertexCoords.x, vertexCoords.y );

            // this is to prevent a thin line between polygons because BLend2D draw them at sub-pixel level and this
            // might create a thin line between the polygons. So we draw a 1-3 pixel line at the edges.
            iBLContext->save();
            iBLContext->reset_transform();
            iBLContext->set_stroke_width( 2.0f );  // 1.0 is not enough due to antialiasing
            iBLContext->stroke_line( BLPoint( vertexWorldCoords.x, vertexWorldCoords.y )
                                           , worldMatrix.map_point( mNextEdgePoint[0].x, mNextEdgePoint[0].y ) );
            iBLContext->stroke_line( BLPoint( vertexWorldCoords.x, vertexWorldCoords.y )
                                           , worldMatrix.map_point( mNextEdgePoint[1].x, mNextEdgePoint[1].y ) );
            iBLContext->stroke_line( BLPoint( vertexWorldCoords.x, vertexWorldCoords.y )
                                           , worldMatrix.map_point( mPrevEdgePoint[0].x, mPrevEdgePoint[0].y ) );
            iBLContext->stroke_line( BLPoint( vertexWorldCoords.x, vertexWorldCoords.y )
                                           , worldMatrix.map_point( mPrevEdgePoint[1].x, mPrevEdgePoint[1].y ) );
            iBLContext->restore();
        }
    }
}

void
FOdysseyVectorJoint::MakeNone()
{
    mPolygonCache.resize(0);
}

/* Detailed comment 1

Note: when building the joint, we have to connect both
ending of segments connected to the vertex. However, keep
in mind a segment could be oriented differently from the other

Regular:

      segment0       vertex        segment1
----------------------� �------------------------
     v0 -----> v1      *       v0 -----> v1
----------------------. .------------------------

here, for segment0

for example, we could have the vertex being the ending vertex for each segment

      segment0       vertex        segment1
----------------------� �------------------------
     v0 -----> v1      *       v1 <----- v0
----------------------. .------------------------

so we have to find the correct "edge point" (either "�" or "." on the figure above)
and this is performed by this part of the code :

    double cross = FOdysseyVector::Cross2D( -parallelVec0, parallelVec1 );
    uint32 side = cross < 0.0f ? 0 : 1;
    double prevSegmentT = mVertex->GetT( iPrevSegment ); // retrieve vertex position in the segment (either 0.0 or 1.0)
    double nextSegmentT = mVertex->GetT( iNextSegment ); // retrieve vertex position in the segment (either 0.0 or 1.0)
    static uint32 rightOrientation[2] = { 0, 1 }; // indexes.
    static uint32 wrongOrientation[2] = { 1, 0 }; // indexes
    uint32 *edge0Side = prevSegmentT == 1.0f ? rightOrientation : wrongOrientation; // retrieve the correct indexes
    uint32 *edge1Side = nextSegmentT == 0.0f ? rightOrientation : wrongOrientation; // retrieve the correct indexes
    // then edge0Side[side] and edge1Side[side] will retrieve the correct side
    ::ULIS::FVec2D edge0Point = iPrevSegment->GetOffsetPoint( edge0Side[side], prevSegmentT );
    ::ULIS::FVec2D edge1Point = iNextSegment->GetOffsetPoint( edge1Side[side], nextSegmentT );
*/
double
FOdysseyVectorJoint::GetEdgePoints( FOdysseyVectorSegment* iPrevSegment
                                  , FOdysseyVectorSegment* iNextSegment
                                  , ::ULIS::FVec2D iPrevEdgePoint[2]
                                  , ::ULIS::FVec2D iNextEdgePoint[2] )
{
    ::ULIS::FVec2D parallelVec0 = mVertex->GetVectorOnSegment( iPrevSegment, true );
    ::ULIS::FVec2D parallelVec1 = mVertex->GetVectorOnSegment( iNextSegment, true );
    // check on which sie should the joint be
    double cross = FOdysseyVector::Cross2D( parallelVec0, parallelVec1 );
    uint32 positiveSide = cross > 0.0f ? 0 : 1;
    uint32 negativeSide = cross > 0.0f ? 1 : 0;
    double prevSegmentT = mVertex->GetIndex( iPrevSegment );
    double nextSegmentT = mVertex->GetIndex( iNextSegment );
    static uint32 rightOrientation[2] = { 0, 1 };
    static uint32 wrongOrientation[2] = { 1, 0 };
    uint32 *prevEdgePointSide = prevSegmentT == 1.0f ? rightOrientation : wrongOrientation;
    uint32 *nextEdgePointSide = nextSegmentT == 0.0f ? rightOrientation : wrongOrientation;

    iPrevEdgePoint[0] = iPrevSegment->GetOffsetPoint( prevEdgePointSide[positiveSide], prevSegmentT );
    iNextEdgePoint[0] = iNextSegment->GetOffsetPoint( nextEdgePointSide[positiveSide], nextSegmentT );
    iPrevEdgePoint[1] = iPrevSegment->GetOffsetPoint( prevEdgePointSide[negativeSide], prevSegmentT );
    iNextEdgePoint[1] = iNextSegment->GetOffsetPoint( nextEdgePointSide[negativeSide], nextSegmentT );

    return cross;
}

void
FOdysseyVectorJoint::MakeMiter( FOdysseyVectorSegment* iPrevSegment
                              , FOdysseyVectorSegment* iNextSegment
                              , const ::ULIS::FVec2D& iPrevEdgePoint
                              , const ::ULIS::FVec2D& iNextEdgePoint
                              , uint32 iSide )
{
    ::ULIS::FVec2D& origin = mVertex->GetCoords();
    double radius = mVertex->GetRadius();
    double miterLimit = mVertex->GetOwnerAsPath()->GetMiterLimit();
    // needs to be normalized for use in the intersectLine function call
    ::ULIS::FVec2D parallelVec0 = mVertex->GetVectorOnSegment( iPrevSegment, true );
    ::ULIS::FVec2D parallelVec1 = mVertex->GetVectorOnSegment( iNextSegment, true );
    ::ULIS::FVec2D perpendicularVec0;
    ::ULIS::FVec2D perpendicularVec1;
    ::ULIS::FVec2D intersectionPoint;

    perpendicularVec0 = iPrevEdgePoint - origin;
    perpendicularVec1 = iNextEdgePoint - origin;

    if( perpendicularVec0.DistanceSquared() )
    {
        perpendicularVec0.Normalize();
    }

    if( perpendicularVec1.DistanceSquared() )
    {
        perpendicularVec1.Normalize();
    }

    if( radius )
    {
        if ( intersectLine( iPrevEdgePoint
                          , parallelVec0
                          , iNextEdgePoint
                          , parallelVec1
                          , intersectionPoint ) == true )
        {
            ::ULIS::FVec2D originToIntersection = intersectionPoint - origin;
            double miterRatio = originToIntersection.Distance() / radius;

            mPolygonCache.resize(1);

            if ( miterRatio < miterLimit )
            {
                // 2 triangles
                mPolygonCache.resize(2);

                // first triangle
                mPolygonCache[0].point[0].x = origin.x;
                mPolygonCache[0].point[0].y = origin.y;
                mPolygonCache[0].U[0] = 0.5f;
                mPolygonCache[0].V[0] = 0.5f;

                mPolygonCache[0].point[1].x = iPrevEdgePoint.x;
                mPolygonCache[0].point[1].y = iPrevEdgePoint.y;
                mPolygonCache[0].U[1] = 0.0f;
                mPolygonCache[0].V[1] = (double) iSide;

                mPolygonCache[0].point[2].x = intersectionPoint.x;
                mPolygonCache[0].point[2].y = intersectionPoint.y;
                mPolygonCache[0].U[2] = 0.5f;
                mPolygonCache[0].V[2] = (double) iSide;

                //mPolygonCache[0].pointCount = 3;

                // second triangle
                mPolygonCache[1].point[0].x = origin.x;
                mPolygonCache[1].point[0].y = origin.y;
                mPolygonCache[1].U[0] = 0.5f;
                mPolygonCache[1].V[0] = 0.5f;

                mPolygonCache[1].point[1].x = intersectionPoint.x;
                mPolygonCache[1].point[1].y = intersectionPoint.y;
                mPolygonCache[1].U[1] = 0.5f;
                mPolygonCache[1].V[1] = (double) iSide;

                mPolygonCache[1].point[2].x = iNextEdgePoint.x;
                mPolygonCache[1].point[2].y = iNextEdgePoint.y;
                mPolygonCache[1].U[2] = 1.0f;
                mPolygonCache[1].V[2] = (double) iSide;

                //mPolygonCache[1].pointCount = 3;
            }
            else
            {
                // 3 triangles
                mPolygonCache.resize(3);

                // first triangle
                mPolygonCache[0].point[0].x = origin.x;
                mPolygonCache[0].point[0].y = origin.y;
                mPolygonCache[0].U[0] = 0.5f;
                mPolygonCache[0].V[0] = 0.5f;

                mPolygonCache[0].point[1].x = iPrevEdgePoint.x;
                mPolygonCache[0].point[1].y = iPrevEdgePoint.y;
                mPolygonCache[0].U[1] = 0.0f;
                mPolygonCache[0].V[1] = (double) iSide;

                mPolygonCache[0].point[2].x = iPrevEdgePoint.x - ( parallelVec0.x * miterLimit * radius );
                mPolygonCache[0].point[2].y = iPrevEdgePoint.y - ( parallelVec0.y * miterLimit * radius );
                mPolygonCache[0].U[2] = 0.33f;
                mPolygonCache[0].V[2] = (double) iSide;

                // third triangle
                mPolygonCache[2].point[0].x = origin.x;
                mPolygonCache[2].point[0].y = origin.y;
                mPolygonCache[2].U[0] = 0.5f;
                mPolygonCache[2].V[0] = 0.5f;

                mPolygonCache[2].point[1].x = iNextEdgePoint.x - ( parallelVec1.x * miterLimit * radius );
                mPolygonCache[2].point[1].y = iNextEdgePoint.y - ( parallelVec1.y * miterLimit * radius );
                mPolygonCache[2].U[1] = 0.66f;
                mPolygonCache[2].V[1] = (double) iSide;

                mPolygonCache[2].point[2].x = iNextEdgePoint.x;
                mPolygonCache[2].point[2].y = iNextEdgePoint.y;
                mPolygonCache[2].U[2] = 1.0f;
                mPolygonCache[2].V[2] = (double) iSide;

                // middle triangle
                mPolygonCache[1].point[0].x = origin.x;
                mPolygonCache[1].point[0].y = origin.y;
                mPolygonCache[1].U[0] = 0.5f;
                mPolygonCache[1].V[0] = 0.5f;

                mPolygonCache[1].point[1].x = mPolygonCache[0].point[2].x;
                mPolygonCache[1].point[1].y = mPolygonCache[0].point[2].y;
                mPolygonCache[1].U[1] = mPolygonCache[0].U[2];
                mPolygonCache[1].V[1] = mPolygonCache[0].V[2];

                mPolygonCache[1].point[2].x = mPolygonCache[2].point[1].x;
                mPolygonCache[1].point[2].y = mPolygonCache[2].point[1].y;
                mPolygonCache[1].U[2] = mPolygonCache[2].U[1];
                mPolygonCache[1].V[2] = mPolygonCache[2].V[1];

                // default pointCount might have been altered, reset it
                //mPolygonCache[0].pointCount = 3;
            }
        }
    }

    mLength = ::ULIS::FVec2D( iNextEdgePoint - iPrevEdgePoint ).Distance();
}

void
FOdysseyVectorJoint::MakeRadial( FOdysseyVectorSegment* iPrevSegment
                               , FOdysseyVectorSegment* iNextSegment
                               , const ::ULIS::FVec2D& iPrevEdgePoint
                               , const ::ULIS::FVec2D& iNextEdgePoint
                               , uint32 iSide )
{
    ::ULIS::FVec2D& origin = mVertex->GetCoords();
    double radius = mVertex->GetRadius();
    ::ULIS::FVec2D perpVec0 = iPrevEdgePoint - origin;
    ::ULIS::FVec2D perpVec1 = iNextEdgePoint - origin;
    ::ULIS::FVec2D perpendicularVec0 = perpVec0.DistanceSquared() ? perpVec0.Normalize() : perpVec0;
    ::ULIS::FVec2D perpendicularVec1 = perpVec1.DistanceSquared() ? perpVec1.Normalize() : perpVec1;
    double angle = acos( std::clamp<double>( perpendicularVec0.DotProduct( perpendicularVec1 ), -1.0f, 1.0f ) );
    static const int steps = 24;
    double a = angle / steps;
    double stepU = 1.0f / steps;
    double U = 0.0f;
    double cosa = cos(a);
    double sina = sin(a);

    mPolygonCache.resize(steps);

    for ( uint32 i = 0; i < steps; i++ )
    {
        // https://matthew-brett.github.io/teaching/rotation_2d.html
        // https://stackoverflow.com/questions/11773889/how-to-calculate-a-vector-from-an-angle-with-another-vector-in-2d
                                                 // counterclockwise
        ::ULIS::FVec2D interpolatedVector = iSide ? ::ULIS::FVec2D( (  perpendicularVec0.x * cosa ) - ( perpendicularVec0.y * sina )
                                                                  , (  perpendicularVec0.x * sina ) + ( perpendicularVec0.y * cosa ) )
                                                  // clockwise
                                                  : ::ULIS::FVec2D( (  perpendicularVec0.x * cosa ) + ( perpendicularVec0.y * sina )
                                                                  , ( -perpendicularVec0.x * sina ) + ( perpendicularVec0.y * cosa ) );

        // start drawing triangles at origin
        mPolygonCache[i].point[0].x = ( origin.x );
        mPolygonCache[i].point[0].y = ( origin.y );
        mPolygonCache[i].U[0] = 0.5f;
        mPolygonCache[i].V[0] = 0.5f;

        mPolygonCache[i].point[1].x = mPolygonCache[i].point[0].x + ( perpendicularVec0.x * radius );
        mPolygonCache[i].point[1].y = mPolygonCache[i].point[0].y + ( perpendicularVec0.y * radius );
        mPolygonCache[i].U[1] = U;
        mPolygonCache[i].V[1] = (double) iSide;

        mPolygonCache[i].point[2].x = mPolygonCache[i].point[0].x + ( interpolatedVector.x * radius );
        mPolygonCache[i].point[2].y = mPolygonCache[i].point[0].y + ( interpolatedVector.y * radius );
        mPolygonCache[i].U[2] = U + stepU;
        mPolygonCache[i].V[2] = (double) iSide;

        // default pointCount for joint's polygons is 5. Set it to 3.
        //mPolygonCache[i].pointCount = 3;

        U += stepU;

        perpendicularVec0 = interpolatedVector;
    }

    mLength = angle * radius;
}

void
FOdysseyVectorJoint::MakeLinear( FOdysseyVectorSegment* iPrevSegment
                               , FOdysseyVectorSegment* iNextSegment
                               , const ::ULIS::FVec2D& iPrevEdgePoint
                               , const ::ULIS::FVec2D& iNextEdgePoint
                               , uint32 iSide )
{
    ::ULIS::FVec2D& origin = mVertex->GetCoords();
    ::ULIS::FVec2D prevPerpendicularVec = iPrevEdgePoint - origin;
    ::ULIS::FVec2D nextPerpendicularVec = iNextEdgePoint - origin;
    double radius = mVertex->GetRadius();

    if( prevPerpendicularVec.DistanceSquared() )
    {
        prevPerpendicularVec.Normalize();
    }

    if( nextPerpendicularVec.DistanceSquared() )
    {
        nextPerpendicularVec.Normalize();
    }

    mPolygonCache.resize(1);

    mPolygonCache[0].point[0].x = ( origin.x );
    mPolygonCache[0].point[0].y = ( origin.y );
    mPolygonCache[0].U[0] = 0.5f;
    mPolygonCache[0].V[0] = 0.5f;

    mPolygonCache[0].point[1].x = mPolygonCache[0].point[0].x + ( prevPerpendicularVec.x * radius );
    mPolygonCache[0].point[1].y = mPolygonCache[0].point[0].y + ( prevPerpendicularVec.y * radius );
    mPolygonCache[0].U[1] = 0.0f;
    mPolygonCache[0].V[1] = (double) iSide;

    mPolygonCache[0].point[2].x = mPolygonCache[0].point[0].x + ( nextPerpendicularVec.x * radius );
    mPolygonCache[0].point[2].y = mPolygonCache[0].point[0].y + ( nextPerpendicularVec.y * radius );
    mPolygonCache[0].U[2] = 1.0f;
    mPolygonCache[0].V[2] = (double) iSide;

    // default pointCount for joint's polygons is 5. Set it to 3.
    //mPolygonCache[0].pointCount = 3;

    mLength = ::ULIS::FVec2D( iNextEdgePoint - iPrevEdgePoint ).Distance();
}

std::vector<FOdysseyVectorPolygon3>&
FOdysseyVectorJoint::GetPolygonCache()
{
    return mPolygonCache;
}

void
FOdysseyVectorJoint::Make( FOdysseyVectorSegment* iPrevSegment
                         , FOdysseyVectorSegment* iNextSegment )
{
    FOdysseyVectorPath* path = mVertex->GetOwnerAsPath();

    // reset polygon array
    MakeNone();

    if( iPrevSegment && iNextSegment )
    {
        double cross = GetEdgePoints( iPrevSegment
                                    , iNextSegment
                                    , mPrevEdgePoint
                                    , mNextEdgePoint );

        if( fabs( cross ) > 0.00001f )
        {
            switch( path->GetJointType() )
            {
                case eVectorPathJointType::Linear :
                    MakeLinear( iPrevSegment
                              , iNextSegment
                              , mPrevEdgePoint[0]
                              , mNextEdgePoint[0]
                              , cross > 0.0f ? 0 : 1 );
                break;

                case eVectorPathJointType::Miter :
                    MakeMiter( iPrevSegment
                             , iNextSegment
                             , mPrevEdgePoint[0]
                             , mNextEdgePoint[0]
                             , cross > 0.0f ? 0 : 1 );
                break;

                case eVectorPathJointType::Radial :
                    MakeRadial( iPrevSegment
                              , iNextSegment
                              , mPrevEdgePoint[0]
                              , mNextEdgePoint[0]
                              , cross > 0.0f ? 0 : 1 );
                break;

                default:
                break;
            }
        }
    }

    UpdateBBox();
}

void
FOdysseyVectorJoint::ResetBBox()
{
    ::ULIS::FVec2D& vertexCoords = mVertex->GetCoords();
    double radius = mVertex->GetRadius();
    double xmin = vertexCoords.x - radius
         , ymin = vertexCoords.y - radius
         , xmax = vertexCoords.x + radius
         , ymax = vertexCoords.y + radius;

    mBBox = ::ULIS::FRectD::FromMinMax( xmin, ymin, xmax, ymax );
}

void
FOdysseyVectorJoint::UpdateBBox()
{
    ::ULIS::FVec2D& vertexCoords = mVertex->GetCoords();
    FOdysseyVectorPath* path = mVertex->GetOwnerAsPath();
    double radius = mVertex->GetRadius();
    double xmin = vertexCoords.x - radius
         , ymin = vertexCoords.y - radius
         , xmax = vertexCoords.x + radius
         , ymax = vertexCoords.y + radius;

    switch( path->GetJointType() )
    {
        case eVectorPathJointType::Miter :
            // basically this is needed for miter joints.
            // For other types of joint, defining the min max is easier.
            for( int i = 0; i < mPolygonCache.size(); i++ )
            {
                for( int j = 0; j < 3; j++ )
                {
                    if( mPolygonCache[i].point[j].x < xmin ) xmin = mPolygonCache[i].point[j].x;
                    if( mPolygonCache[i].point[j].y < ymin ) ymin = mPolygonCache[i].point[j].y;
                    if( mPolygonCache[i].point[j].x > xmax ) xmax = mPolygonCache[i].point[j].x;
                    if( mPolygonCache[i].point[j].y > ymax ) ymax = mPolygonCache[i].point[j].y;
                }
            }
        break;

        default:
        break;
    }

    mBBox = ::ULIS::FRectD::FromMinMax( xmin, ymin, xmax, ymax );
}

::ULIS::FRectD
FOdysseyVectorJoint::GetBBox( bool iWorld )
{
    if( iWorld )
    {
        BLMatrix2D& worldMatrix = mVertex->GetOwner()->GetWorldMatrix();
        BLPoint pt[4] = { worldMatrix.map_point( mBBox.x          , mBBox.y           )
                        , worldMatrix.map_point( mBBox.x + mBBox.w, mBBox.y           )
                        , worldMatrix.map_point( mBBox.x + mBBox.w, mBBox.y + mBBox.h )
                        , worldMatrix.map_point( mBBox.x          , mBBox.y + mBBox.h ) };
        double xmin = ::ULIS::FMath::Min4( pt[0].x, pt[1].x, pt[2].x, pt[3].x )
             , ymin = ::ULIS::FMath::Min4( pt[0].y, pt[1].y, pt[2].y, pt[3].y )
             , xmax = ::ULIS::FMath::Max4( pt[0].x, pt[1].x, pt[2].x, pt[3].x )
             , ymax = ::ULIS::FMath::Max4( pt[0].y, pt[1].y, pt[2].y, pt[3].y );

        return ::ULIS::FRectD::FromMinMax( xmin, ymin, xmax, ymax );
    }

    return mBBox;
}
