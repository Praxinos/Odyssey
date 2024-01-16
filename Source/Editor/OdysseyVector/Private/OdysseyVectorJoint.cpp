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
FOdysseyVectorJoint::MakeMiter( FOdysseyVectorSegment* iPrevSegment
                               , FOdysseyVectorSegment* iNextSegment ) // next segment in chain
{
    ::ULIS::FVec2D& origin = mVertex->GetCoords();
    double radius = mVertex->GetRadius();
    // needs to be normalized for reuse in the intersectLine function call
    ::ULIS::FVec2D parallelVec0 = mVertex->GetVectorOnSegment( iPrevSegment, true );
    ::ULIS::FVec2D parallelVec1 = mVertex->GetVectorOnSegment( iNextSegment, true );
    // have to clamp due to imprecision of the dot product
    double cross = FOdysseyVector::Cross2D( -parallelVec0, parallelVec1 );
    uint32 side = cross < 0.0f ? 0 : 1;
    double prevSegmentT = mVertex->GetT( iPrevSegment );
    double nextSegmentT = mVertex->GetT( iNextSegment );
    ::ULIS::FVec2D edge0Point = iPrevSegment->GetOffsetPoint( prevSegmentT == 1.0f ? 0 : 1, prevSegmentT );
    ::ULIS::FVec2D edge1Point = iNextSegment->GetOffsetPoint( nextSegmentT == 0.0f ? 0 : 1, nextSegmentT );
    ::ULIS::FVec2D perpendicularVec0 = edge0Point - origin;
    ::ULIS::FVec2D perpendicularVec1 = edge1Point - origin;
    double miterLimit = mVertex->GetPath()->GetMiterLimit();
    ::ULIS::FVec2D intersectionPoint;

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
                mPolygonCache[0].U[0] = 0.5f;
                mPolygonCache[0].V[0] = 0.5f;

                mPolygonCache[0].point[1].x = edge0Point.x;
                mPolygonCache[0].point[1].y = edge0Point.y;
                mPolygonCache[0].U[1] = 0.0f;
                mPolygonCache[0].V[1] = (double) side;

                mPolygonCache[0].point[2].x = intersectionPoint.x;
                mPolygonCache[0].point[2].y = intersectionPoint.y;
                mPolygonCache[0].U[2] = 0.5f;
                mPolygonCache[0].V[2] = (double) side;

                //mPolygonCache[0].pointCount = 3;

                // second triangle
                mPolygonCache[1].point[0].x = origin.x;
                mPolygonCache[1].point[0].y = origin.y;
                mPolygonCache[1].U[0] = 0.5f;
                mPolygonCache[1].V[0] = 0.5f;

                mPolygonCache[1].point[1].x = intersectionPoint.x;
                mPolygonCache[1].point[1].y = intersectionPoint.y;
                mPolygonCache[1].U[1] = 0.5f;
                mPolygonCache[1].V[1] = (double) side;

                mPolygonCache[1].point[2].x = edge1Point.x;
                mPolygonCache[1].point[2].y = edge1Point.y;
                mPolygonCache[1].U[2] = 1.0f;
                mPolygonCache[1].V[2] = (double) side;

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

                mPolygonCache[0].point[1].x = edge0Point.x;
                mPolygonCache[0].point[1].y = edge0Point.y;
                mPolygonCache[0].U[1] = 0.0f;
                mPolygonCache[0].V[1] = (double) side;

                mPolygonCache[0].point[2].x = edge0Point.x - ( parallelVec0.x * miterLimit * radius );
                mPolygonCache[0].point[2].y = edge0Point.y - ( parallelVec0.y * miterLimit * radius );
                mPolygonCache[0].U[2] = 0.33f;
                mPolygonCache[0].V[2] = (double) side;

                // third triangle
                mPolygonCache[2].point[0].x = origin.x;
                mPolygonCache[2].point[0].y = origin.y;
                mPolygonCache[2].U[0] = 0.5f;
                mPolygonCache[2].V[0] = 0.5f;

                mPolygonCache[2].point[1].x = edge1Point.x - ( parallelVec1.x * miterLimit * radius );
                mPolygonCache[2].point[1].y = edge1Point.y - ( parallelVec1.y * miterLimit * radius );
                mPolygonCache[2].U[1] = 0.66f;
                mPolygonCache[2].V[1] = (double) side;

                mPolygonCache[2].point[2].x = edge1Point.x;
                mPolygonCache[2].point[2].y = edge1Point.y;
                mPolygonCache[2].U[2] = 1.0f;
                mPolygonCache[2].V[2] = (double) side;

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

    mLength = ::ULIS::FVec2D( edge1Point - edge0Point ).Distance();
}

void
FOdysseyVectorJoint::MakeRadial( FOdysseyVectorSegment* iPrevSegment
                               , FOdysseyVectorSegment* iNextSegment ) // next segment in chain
{
    ::ULIS::FVec2D& origin = mVertex->GetCoords();
    double radius = mVertex->GetRadius();
    ::ULIS::FVec2D parallelVec0 = mVertex->GetVectorOnSegment( iPrevSegment, false );
    ::ULIS::FVec2D parallelVec1 = mVertex->GetVectorOnSegment( iNextSegment, false );
    // have to clamp due to imprecision of the dot product
    double cross = FOdysseyVector::Cross2D( -parallelVec0, parallelVec1 );
    uint32 side = cross < 0.0f ? 0 : 1;
    ::ULIS::FVec2D edge0Point = iPrevSegment->GetOffsetPoint( side, mVertex->GetT( iPrevSegment ) );
    ::ULIS::FVec2D edge1Point = iNextSegment->GetOffsetPoint( side, mVertex->GetT( iNextSegment ) );
    ::ULIS::FVec2D perpVec0 = edge0Point - origin;
    ::ULIS::FVec2D perpVec1 = edge1Point - origin;
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
        ::ULIS::FVec2D interpolatedVector = side ? ::ULIS::FVec2D( (  perpendicularVec0.x * cosa ) - ( perpendicularVec0.y * sina )
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
        mPolygonCache[i].V[1] = (double) side;

        mPolygonCache[i].point[2].x = mPolygonCache[i].point[0].x + ( interpolatedVector.x * radius );
        mPolygonCache[i].point[2].y = mPolygonCache[i].point[0].y + ( interpolatedVector.y * radius );
        mPolygonCache[i].U[2] = U + stepU;
        mPolygonCache[i].V[2] = (double) side;

        // default pointCount for joint's polygons is 5. Set it to 3.
        //mPolygonCache[i].pointCount = 3;

        U += stepU;

        perpendicularVec0 = interpolatedVector;
    }

    mLength = angle * radius;
}

void
FOdysseyVectorJoint::MakeLinear( FOdysseyVectorSegment* iPrevSegment
                               , FOdysseyVectorSegment* iNextSegment ) // next segment in chain
{
    ::ULIS::FVec2D& origin = mVertex->GetCoords();
    double radius = mVertex->GetRadius();
    ::ULIS::FVec2D parallelVec0 = mVertex->GetVectorOnSegment( iPrevSegment, false );
    ::ULIS::FVec2D parallelVec1 = mVertex->GetVectorOnSegment( iNextSegment, false );
    // have to clamp due to imprecision of the dot product
    double cross = FOdysseyVector::Cross2D( -parallelVec0, parallelVec1 );
    uint32 side = cross < 0.0f ? 0 : 1;
    ::ULIS::FVec2D edge0Point = iPrevSegment->GetOffsetPoint( side, mVertex->GetT( iPrevSegment ) );
    ::ULIS::FVec2D edge1Point = iNextSegment->GetOffsetPoint( side, mVertex->GetT( iNextSegment ) );
    ::ULIS::FVec2D perpendicularVec0 = edge0Point - origin;
    ::ULIS::FVec2D perpendicularVec1 = edge1Point - origin;

    if( perpendicularVec0.DistanceSquared() )
    {
        perpendicularVec0.Normalize();
    }

    if( perpendicularVec1.DistanceSquared() )
    {
        perpendicularVec1.Normalize();
    }

    mPolygonCache.resize(1);

    mPolygonCache[0].point[0].x = ( origin.x );
    mPolygonCache[0].point[0].y = ( origin.y );
    mPolygonCache[0].U[0] = 0.5f;
    mPolygonCache[0].V[0] = 0.5f;

    mPolygonCache[0].point[1].x = mPolygonCache[0].point[0].x + ( perpendicularVec0.x * radius );
    mPolygonCache[0].point[1].y = mPolygonCache[0].point[0].y + ( perpendicularVec0.y * radius );
    mPolygonCache[0].U[1] = 0.0f;
    mPolygonCache[0].V[1] = (double) side;

    mPolygonCache[0].point[2].x = mPolygonCache[0].point[0].x + ( perpendicularVec1.x * radius );
    mPolygonCache[0].point[2].y = mPolygonCache[0].point[0].y + ( perpendicularVec1.y * radius );
    mPolygonCache[0].U[2] = 1.0f;
    mPolygonCache[0].V[2] = (double) side;

    // default pointCount for joint's polygons is 5. Set it to 3.
    //mPolygonCache[0].pointCount = 3;

    mLength = ::ULIS::FVec2D( edge1Point - edge0Point ).Distance();
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
    FOdysseyVectorPath* path = mVertex->GetPath();

    // reset polygon array
    MakeNone();

    if( iPrevSegment && iNextSegment )
    {
        switch( path->GetJointType() )
        {
            case eJointType::Linear :
                MakeLinear( iPrevSegment, iNextSegment );
            break;

            case eJointType::Miter :
                MakeMiter( iPrevSegment, iNextSegment );
            break;

            case eJointType::Radial :
                MakeRadial( iPrevSegment, iNextSegment );
            break;

            default:
            break;
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
