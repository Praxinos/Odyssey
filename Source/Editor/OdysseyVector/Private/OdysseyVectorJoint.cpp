#include "OdysseyVectorJoint.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorEngine.h"


FOdysseyVectorJoint::~FOdysseyVectorJoint()
{
}

FOdysseyVectorJoint::FOdysseyVectorJoint( FOdysseyVectorVertex* iVertex )
    : mVertex( iVertex )
    , mLength( 0.0f )
{
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

double
FOdysseyVectorJoint::GetLength()
{
    return mLength;
}

void
FOdysseyVectorJoint::Draw( BLContext* iBLContext
                         , double iStartU
                         , double iEndU
                         , double iCombinedOpacity
                         , uint64 iDrawingFlags )
{
    FOdysseyVectorPath* path = mVertex->GetPath();
    BLMatrix2D& worldMatrix = path->GetWorldMatrix();
    FOdysseyVectorEngine* vectorEngine = path->GetEngine();
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
            polygonDrawingFlags |= brush.ColorFromBrush   ? 0 : FPolygonDrawingFlags::BRUSHALPHAONLY;

            for( uint32 j = 0; j < polygon->pointCount; j++ )
            {
                polyU[j] = iStartU + ( polygon->U[j] * difU );
            }

            vectorEngine->FillTriangle( iBLContext
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
        for( int i = 0; i < mPolygonCache.size(); i++ )
        {
            BLPoint pt[3] = { { mPolygonCache[i].point[0].x, mPolygonCache[i].point[0].y }
                            , { mPolygonCache[i].point[1].x, mPolygonCache[i].point[1].y }
                            , { mPolygonCache[i].point[2].x, mPolygonCache[i].point[2].y } };

            iBLContext->fillPolygon( pt, mPolygonCache[i].pointCount );
        }

        // this is to prevent a thin line between polygons because BLend2D draw them at sub-pixel level and this
        // might create a thin line between the polygons. So we draw a one-pixel line at the edges.
        iBLContext->save();
        iBLContext->resetMatrix();
        iBLContext->setStrokeWidth( 1.0f );
        for ( int i = 0; i < mPolygonCache.size(); i++ )
        {
            iBLContext->strokeLine( worldMatrix.mapPoint( mPolygonCache[i].point[0].x, mPolygonCache[i].point[0].y )
                                  , worldMatrix.mapPoint( mPolygonCache[i].point[1].x, mPolygonCache[i].point[1].y ) );
            iBLContext->strokeLine( worldMatrix.mapPoint( mPolygonCache[i].point[2].x, mPolygonCache[i].point[2].y )
                                  , worldMatrix.mapPoint( mPolygonCache[i].point[0].x, mPolygonCache[i].point[0].y ) );
        }
        iBLContext->restore();
    }
}

void
FOdysseyVectorJoint::MakeNone()
{
    mPolygonCache.resize(0);
}

void
FOdysseyVectorJoint::MakeMiter( ::ULIS::FVec2D& iVector0 // previous segment in chain
                              , ::ULIS::FVec2D& iVector1 ) // next segment in chain
{
    ::ULIS::FVec2D& origin = mVertex->GetCoords();
    double radius = mVertex->GetRadius();
    double miterLimit = mVertex->GetPath()->GetMiterLimit();
    ::ULIS::FVec2D parallelVec0 = iVector0;
    ::ULIS::FVec2D parallelVec1 = iVector1;
    ::ULIS::FVec2D perpendicularVec0 = { - parallelVec0.y,   parallelVec0.x };
    ::ULIS::FVec2D perpendicularVec1 = {   parallelVec1.y, - parallelVec1.x };
    ::ULIS::FVec2D edge0Point = origin + ( perpendicularVec0 * radius );
    ::ULIS::FVec2D edge1Point = origin + ( perpendicularVec1 * radius );
    ::ULIS::FVec2D shortestTest = edge1Point - edge0Point;
    // have to clamp due to imprecision of the dot product
    double dot = std::clamp<double>( shortestTest.DotProduct( parallelVec1 ), -1.0f, 1.0f );
    ::ULIS::FVec2D intersectionPoint;
    double side = 1.0f;

    // Find on which side should the joint be drawn by comparing the directions of our vectors
    if ( dot < 0 )
    {
        ::ULIS::FVec2D tmp = perpendicularVec0;
        perpendicularVec0 = -tmp;

                       tmp = perpendicularVec1;
        perpendicularVec1 = -tmp;

        // Update
        edge0Point = origin + ( perpendicularVec0 * radius );
        edge1Point = origin + ( perpendicularVec1 * radius );

        side = -1.0f;
    }

    if( radius )
    {
        if ( intersectLine( edge0Point
                          , parallelVec0
                          , edge1Point
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
                mPolygonCache[0].U[0] = 0.0f;
                mPolygonCache[0].V[0] = 0.5f;

                mPolygonCache[0].point[1].x = edge0Point.x;
                mPolygonCache[0].point[1].y = edge0Point.y;
                mPolygonCache[0].U[1] = 0.0f;
                mPolygonCache[0].V[1] = side == 1.0f ? 1.0f : 0.0f;

                mPolygonCache[0].point[2].x = intersectionPoint.x;
                mPolygonCache[0].point[2].y = intersectionPoint.y;
                mPolygonCache[0].U[2] = 0.5f;
                mPolygonCache[0].V[2] = side == 1.0f ? 1.0f : 0.0f;

                //mPolygonCache[0].pointCount = 3;

                // second triangle
                mPolygonCache[1].point[0].x = origin.x;
                mPolygonCache[1].point[0].y = origin.y;
                mPolygonCache[1].U[0] = 0.0f;
                mPolygonCache[1].V[0] = 0.5f;

                mPolygonCache[1].point[1].x = intersectionPoint.x;
                mPolygonCache[1].point[1].y = intersectionPoint.y;
                mPolygonCache[1].U[1] = 0.5f;
                mPolygonCache[1].V[1] = side == 1.0f ? 1.0f : 0.0f;

                mPolygonCache[1].point[2].x = edge1Point.x;
                mPolygonCache[1].point[2].y = edge1Point.y;
                mPolygonCache[1].U[2] = 1.0f;
                mPolygonCache[1].V[2] = side == 1.0f ? 1.0f : 0.0f;

                //mPolygonCache[1].pointCount = 3;
            }
            else
            {
                // 3 triangles
                mPolygonCache.resize(3);

                // first triangle
                mPolygonCache[0].point[0].x = origin.x;
                mPolygonCache[0].point[0].y = origin.y;
                mPolygonCache[0].U[0] = 0.0f;
                mPolygonCache[0].V[0] = 0.5f;

                mPolygonCache[0].point[1].x = edge0Point.x;
                mPolygonCache[0].point[1].y = edge0Point.y;
                mPolygonCache[0].U[1] = 0.0f;
                mPolygonCache[0].V[1] = side == 1.0f ? 1.0f : 0.0f;

                mPolygonCache[0].point[2].x = edge0Point.x - ( parallelVec0.x * miterLimit * radius );
                mPolygonCache[0].point[2].y = edge0Point.y - ( parallelVec0.y * miterLimit * radius );
                mPolygonCache[0].U[2] = 0.33f;
                mPolygonCache[0].V[2] = side == 1.0f ? 1.0f : 0.0f;

                // third triangle
                mPolygonCache[2].point[0].x = origin.x;
                mPolygonCache[2].point[0].y = origin.y;
                mPolygonCache[2].U[0] = 0.0f;
                mPolygonCache[2].V[0] = 0.5f;

                mPolygonCache[2].point[1].x = edge1Point.x - ( parallelVec1.x * miterLimit * radius );
                mPolygonCache[2].point[1].y = edge1Point.y - ( parallelVec1.y * miterLimit * radius );
                mPolygonCache[2].U[1] = 0.66f;
                mPolygonCache[2].V[1] = side == 1.0f ? 1.0f : 0.0f;

                mPolygonCache[2].point[2].x = edge1Point.x;
                mPolygonCache[2].point[2].y = edge1Point.y;
                mPolygonCache[2].U[2] = 1.0f;
                mPolygonCache[2].V[2] = side == 1.0f ? 1.0f : 0.0f;

                // middle triangle
                mPolygonCache[1].point[0].x = origin.x;
                mPolygonCache[1].point[0].y = origin.y;
                mPolygonCache[1].U[0] = 0.0f;
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

    mLength = shortestTest.Distance();
}

void
FOdysseyVectorJoint::MakeRadial( ::ULIS::FVec2D& iVector0 // previous segment in chain
                               , ::ULIS::FVec2D& iVector1 ) // next segment in chain
{
    ::ULIS::FVec2D& origin = mVertex->GetCoords();
    double radius = mVertex->GetRadius();
    ::ULIS::FVec2D parallelVec0 = iVector0;
    ::ULIS::FVec2D parallelVec1 = iVector1;
    ::ULIS::FVec2D perpendicularVec0 = { - parallelVec0.y,   parallelVec0.x };
    ::ULIS::FVec2D perpendicularVec1 = {   parallelVec1.y, - parallelVec1.x };
    ::ULIS::FVec2D edge0Point = origin + ( perpendicularVec0 * radius );
    ::ULIS::FVec2D edge1Point = origin + ( perpendicularVec1 * radius );
    ::ULIS::FVec2D shortestTest = edge1Point - edge0Point;
    // have to clamp due to imprecision of the dot product
    double dot = std::clamp<double>( shortestTest.DotProduct( parallelVec1 ), -1.0f, 1.0f );
    double angle = acos( std::clamp<double>( perpendicularVec0.DotProduct( perpendicularVec1 ), -1.0f, 1.0f ) );
    static const int steps = 24;
    double a = angle / steps;
    double side = 1.0f;
    double stepU = 1.0f / steps;
    double U = 0.0f;

    // Find on which side should the joint be drawn by comparing the directions of our vectors
    if ( dot < 0 )
    {
        ::ULIS::FVec2D tmp = perpendicularVec0;

        perpendicularVec0 = -tmp;

                tmp = perpendicularVec1;
        perpendicularVec1 = -tmp;

        side = -1.0f;
    }

    double cosa = cos(a);
    double sina = sin(a);

    mPolygonCache.resize(steps);

    for ( uint32 i = 0; i < steps; i++ )
    {
        // https://stackoverflow.com/questions/11773889/how-to-calculate-a-vector-from-an-angle-with-another-vector-in-2d
        ::ULIS::FVec2D interpolatedVector = { (  perpendicularVec0.x * cosa ) - ( perpendicularVec0.y * sina ) * side,
                                       side * (  perpendicularVec0.x * sina ) + ( perpendicularVec0.y * cosa ) };

        // start drawing triangles at origin
        mPolygonCache[i].point[0].x = ( origin.x );
        mPolygonCache[i].point[0].y = ( origin.y );
        mPolygonCache[i].U[0] = 0.0f;
        mPolygonCache[i].V[0] = 0.5f;

        mPolygonCache[i].point[1].x = mPolygonCache[i].point[0].x + ( perpendicularVec0.x * radius );
        mPolygonCache[i].point[1].y = mPolygonCache[i].point[0].y + ( perpendicularVec0.y * radius );
        mPolygonCache[i].U[1] = U;
        mPolygonCache[i].V[1] = side == 1.0f ? 1.0f : 0.0f;

        mPolygonCache[i].point[2].x = mPolygonCache[i].point[0].x + ( interpolatedVector.x * radius );
        mPolygonCache[i].point[2].y = mPolygonCache[i].point[0].y + ( interpolatedVector.y * radius );
        mPolygonCache[i].U[2] = U + stepU;
        mPolygonCache[i].V[2] = side == 1.0f ? 1.0f : 0.0f;

        // default pointCount for joint's polygons is 5. Set it to 3.
        //mPolygonCache[i].pointCount = 3;

        U += stepU;

        perpendicularVec0 = interpolatedVector;
    }

    mLength = angle * radius;
}

void
FOdysseyVectorJoint::MakeLinear( ::ULIS::FVec2D& iVector0 // previous segment in chain
                               , ::ULIS::FVec2D& iVector1 ) // next segment in chain
{
    ::ULIS::FVec2D& origin = mVertex->GetCoords();
    double radius = mVertex->GetRadius();
    ::ULIS::FVec2D parallelVec0 = iVector0;
    ::ULIS::FVec2D parallelVec1 = iVector1;
    ::ULIS::FVec2D perpendicularVec0 = { - parallelVec0.y,   parallelVec0.x };
    ::ULIS::FVec2D perpendicularVec1 = {   parallelVec1.y, - parallelVec1.x };
    ::ULIS::FVec2D edge0Point = origin + ( perpendicularVec0 * radius );
    ::ULIS::FVec2D edge1Point = origin + ( perpendicularVec1 * radius );
    ::ULIS::FVec2D shortestTest = edge1Point - edge0Point;
    // have to clamp due to imprecision of the dot product
    double dot = std::clamp<double>( shortestTest.DotProduct( parallelVec1 ), -1.0f, 1.0f );
    double side = 1.0f;

    if ( dot < 0 )
    {
        ::ULIS::FVec2D tmp = perpendicularVec0;

        perpendicularVec0 = -tmp;

                tmp = perpendicularVec1;
        perpendicularVec1 = -tmp;

        side = -1.0f;
    }

    mPolygonCache.resize(1);

    mPolygonCache[0].point[0].x = ( origin.x );
    mPolygonCache[0].point[0].y = ( origin.y );
    mPolygonCache[0].U[0] = 0.0f;
    mPolygonCache[0].V[0] = 0.5f;

    mPolygonCache[0].point[1].x = mPolygonCache[0].point[0].x + ( perpendicularVec0.x * radius );
    mPolygonCache[0].point[1].y = mPolygonCache[0].point[0].y + ( perpendicularVec0.y * radius );
    mPolygonCache[0].U[1] = 0.0f;
    mPolygonCache[0].V[1] = side == 1.0f ? 1.0f : 0.0f;

    mPolygonCache[0].point[2].x = mPolygonCache[0].point[0].x + ( perpendicularVec1.x * radius );
    mPolygonCache[0].point[2].y = mPolygonCache[0].point[0].y + ( perpendicularVec1.y * radius );
    mPolygonCache[0].U[2] = 1.0f;
    mPolygonCache[0].V[2] = side == 1.0f ? 1.0f : 0.0f;

    // default pointCount for joint's polygons is 5. Set it to 3.
    //mPolygonCache[0].pointCount = 3;

    mLength = shortestTest.Distance();
}

std::vector<FOdysseyVectorPolygon3>&
FOdysseyVectorJoint::GetPolygonCache()
{
    return mPolygonCache;
}

void
FOdysseyVectorJoint::Make( FOdysseyVectorSegment* iCurrentSegment )
{
    FOdysseyVectorPath* path = mVertex->GetPath();

    // reset polygon array
    MakeNone();

    if( iCurrentSegment )
    {
        FOdysseyVectorSegment* prevSegment = mVertex->GetOtherSegment( iCurrentSegment );

        if( iCurrentSegment && prevSegment )
        {
            ::ULIS::FVec2D segment0Vector = mVertex->GetVectorOnSegment( iCurrentSegment, false );
            ::ULIS::FVec2D segment1Vector = mVertex->GetVectorOnSegment( prevSegment    , false );

            if( segment0Vector.DistanceSquared() )
            {
                segment0Vector.Normalize();
            }

            if( segment1Vector.DistanceSquared() )
            {
                segment1Vector.Normalize();
            }

            switch( path->GetJointType() )
            {
                case eJointType::Linear :
                    MakeLinear( segment0Vector, segment1Vector );
                break;

                case eJointType::Miter :
                    MakeMiter( segment0Vector, segment1Vector );
                break;

                case eJointType::Radial :
                    MakeRadial( segment0Vector, segment1Vector );
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
    FOdysseyVectorPath* path = mVertex->GetPath();
    double radius = mVertex->GetRadius();
    double xmin = vertexCoords.x - radius
         , ymin = vertexCoords.y - radius
         , xmax = vertexCoords.x + radius
         , ymax = vertexCoords.y + radius;

    switch( path->GetJointType() )
    {
        case eJointType::Miter :
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
        BLMatrix2D& worldMatrix = mVertex->GetPath()->GetWorldMatrix();
        BLPoint pt[4] = { worldMatrix.mapPoint( mBBox.x          , mBBox.y           )
                        , worldMatrix.mapPoint( mBBox.x + mBBox.w, mBBox.y           )
                        , worldMatrix.mapPoint( mBBox.x + mBBox.w, mBBox.y + mBBox.h )
                        , worldMatrix.mapPoint( mBBox.x          , mBBox.y + mBBox.h ) };
        double xmin = ::ULIS::FMath::Min4( pt[0].x, pt[1].x, pt[2].x, pt[3].x )
             , ymin = ::ULIS::FMath::Min4( pt[0].y, pt[1].y, pt[2].y, pt[3].y )
             , xmax = ::ULIS::FMath::Max4( pt[0].x, pt[1].x, pt[2].x, pt[3].x )
             , ymax = ::ULIS::FMath::Max4( pt[0].y, pt[1].y, pt[2].y, pt[3].y );

        return ::ULIS::FRectD::FromMinMax( xmin, ymin, xmax, ymax );
    }

    return mBBox;
}
