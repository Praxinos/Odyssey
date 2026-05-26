// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeSegmentCubic.h"
#include "ArianeVertex.h"
#include "ArianePath.h"
// ULIS headers for cubicBezier functions
#include <ULIS>

#define MINRECURSE 0
#define MAXRECURSE 7
#define MAXSUBLINE ( 1 << MAXRECURSE )

FArianeSegmentCubic::~FArianeSegmentCubic()
{
}

FArianeSegmentCubic::FArianeSegmentCubic()
    : FArianeSegment( nullptr, nullptr, nullptr )
    , Handle0( this, FVector::Zero() )
    , Handle1( this, FVector::Zero() )
{
}

FArianeSegmentCubic::FArianeSegmentCubic( FArianeObject* Owner
                                        , FArianeVertex* iVertex0
                                        , double Handle0X
                                        , double Handle0Y
                                        , double Handle0Z
                                        , double Handle1X
                                        , double Handle1Y
                                        , double Handle1Z
                                        , FArianeVertex* iVertex1 )
    : FArianeSegment( Owner, iVertex0, iVertex1 )
    , Handle0( this, FVector( Handle0X, Handle0Y, Handle0Z ) )
    , Handle1( this, FVector( Handle1X, Handle1Y, Handle1Z ) )
{
}

void
FArianeSegmentCubic::Update()
{
#ifdef unused
    FOdysseyVectorCell* cell = mOwner->GetCell();
    ::ULIS::FRectD previousBBox = mBBox;
    double xmin, ymin, xmax, ymax;

    FOdysseyVectorSegment::Update( iUpdateFlags );

    mBezier[0] = mPoint[0]->GetCoords();
    mBezier[1] = mCtrlPoint[0].GetCoords();
    mBezier[2] = mCtrlPoint[1].GetCoords();
    mBezier[3] = mPoint[1]->GetCoords();

    xmin = ULIS::FMath::Min4<double>( mBezier[0].x
                                    , mBezier[1].x
                                    , mBezier[2].x
                                    , mBezier[3].x );

    ymin = ULIS::FMath::Min4<double>( mBezier[0].y
                                    , mBezier[1].y
                                    , mBezier[2].y
                                    , mBezier[3].y );

    xmax = ULIS::FMath::Max4<double>( mBezier[0].x
                                    , mBezier[1].x
                                    , mBezier[2].x
                                    , mBezier[3].x );

    ymax = ULIS::FMath::Max4<double>( mBezier[0].y
                                    , mBezier[1].y
                                    , mBezier[2].y
                                    , mBezier[3].y );

    if( mNeedsWidth )
    {
        BuildVariable( iUpdateFlags & FOdysseyVectorObject::UPDATE_NEEDPOLYLINE ? 5 : MINRECURSE
                     , MAXRECURSE
                     , xmin
                     , ymin
                     , xmax
                     , ymax );
    }

    mBBox = ::ULIS::FRectD::FromMinMax( xmin, ymin, xmax, ymax );

    if( ( iUpdateFlags & FOdysseyVectorObject::UPDATE_NOINVALIDATERECT ) == 0 )
    {
        // auto invalidation of the region that needs to be redrawn
        if( cell )
        {
            cell->InvalidateRect( FOdysseyVector::MapRect( mOwner->GetWorldMatrix(), ( previousBBox | mBBox ) ) );
        }
    }
#endif
}

#ifdef unused
void
FArianeSegmentCubic::BuildVariable( uint32 MinRecurse
                                  , uint32 MaxRecurse )
{
    FArianeVertex* Vertex0 = Vertices[0].GetVertex();
    FArianeVertex* Vertex1 = Vertices[1].GetVertex();
    TArray<FArianePoint> SubPoints;
    TArray<FSegmentSubLine> SubLines;
    //static std::mutex mutex;

    double XMin, double YMin, double ZMin = -DBL_MAX;
    double XMax, double YMax, double ZMax =  DBL_MAX;

    SubLines.Reserve( MAXSUBLINE );
     // Note: segment end points don't belong to the buffer
    SubPoints.Reserve( MAXSUBLINE - 1 );

    Length = 0.0f; // note: BuildVariableAdaptive will update the length

    // Reset
    Fractions.Empty();
    FractionSteps.Empty();
    FractionPoints.Empty();

    //if( mLength )
    {
        double SegmentStartRadius = Vertex0->GetRadius();
        double SegmentEndRadius = Vertex1->GetRadius();
        //double startU = 0.0f;

        //BuildOffsetCurves();
        // offset curves must be built before the polygon cache is built, as the builidng process
        // uses the offset curves to build the polygons.

        BuildVariableAdaptive ( Vertices[0].GetVertex()
                              , Vertices[1].GetVertex()
                              , 0.0f
                              , 1.0f
                              , SegmentStartRadius
                              , SegmentEndRadius
                              , Bezier
                              , 0
                              , MinRecurse
                              , MaxRecurse
                              , SubPoints
                              , SubLines );

        FractionPoints = SubPoints;

        Fractions.Reserve( SubLines.Num() );

        for( FSegmentSubLine& subLine : SubLines )
        {
            FArianePoint* point0 = SubLine.point[0];
            FArianePoint* point1 = SubLine.point[1];
            double x0 = point0->GetX();
            double y0 = point0->GetY();
            double x1 = point1->GetX();
            double y1 = point1->GetY();

            // ------ bounding -------//
            if( x0 < oXmin ) oXmin = x0;
            if( y0 < oYmin ) oYmin = y0;
            if( x0 > oXmax ) oXmax = x0;
            if( y0 > oYmax ) oYmax = y0;

            if( x1 < oXmin ) oXmin = x1;
            if( y1 < oYmin ) oYmin = y1;
            if( x1 > oXmax ) oXmax = x1;
            if( y1 > oYmax ) oYmax = y1;
            // ---------------------- //

            if( subLine.point[0]->GetClass() == FArianePoint::StaticClass() )
            {
                // pointer arithmetic gives us the index directly
                uint32 index = ( subLine.point[0] - &subPointBuffer[0] );

                point0 = &mFractionPointBuffer[index];
            }

            if( subLine.point[1]->GetClass() == FArianePoint::StaticClass() )
            {
                // pointer arithmetic gives us the index directly
                uint32 index = ( subLine.point[1] - &subPointBuffer[0] );

                point1 = &mFractionPointBuffer[index];
            }

            FOdysseyVectorFraction& fraction = mFractionCache.emplace_back( point0
                                                                          , subLine.t[0]
                                                                          , point1
                                                                          , subLine.t[1]
                                                                          , subLine.length );

            startU = ThickenFraction( &fraction, startU, oXmin, oYmin, oXmax, oYmax );
        }
    }
}
#endif

FVector
FArianeSegmentCubic::GetPointAt( double T )
{
    return ::ULIS::CubicBezierPointAtParameter( Vertices[0].GetVertex()->GetPosition()
                                              , Handle0.GetPosition()
                                              , Handle1.GetPosition()
                                              , Vertices[1].GetVertex()->GetPosition()
                                              , T );
}

FArianeSegmentCubic*
FArianeSegmentCubic::Extract( FArianeObject* NewSegmentOwner
                            , FArianeVertex* NewSegmentVertex0
                            , float T0
                            , FArianeVertex* NewSegmentVertex1
                            , float T1 )
{
    FVector Bezier[4] = { Vertices[0].GetVertex()->GetPosition()
                        , Handle0.GetPosition()
                        , Handle1.GetPosition()
                        , Vertices[1].GetVertex()->GetPosition() };

    ::ULIS::CubicBezierSplitAtParameter( &Bezier[0]
                                       , &Bezier[1]
                                       , &Bezier[2]
                                       , &Bezier[3]
                                       , T1 );

    ::ULIS::CubicBezierInverseSplitAtParameter( &Bezier[0]
                                              , &Bezier[1]
                                              , &Bezier[2]
                                              , &Bezier[3]
                                              , T1 ? ( T0 / T1 ) : 0.0f );

    Vertices[0].GetVertex()->SetPosition( Bezier[0] );
    Handle0.SetPosition( Bezier[1] );
    Handle1.SetPosition( Bezier[2] );
    Vertices[1].GetVertex()->SetPosition( Bezier[3] );

    return static_cast<FArianePath*>(NewSegmentOwner)->AllocCubicSegment( NewSegmentVertex0
                                                                        , Handle0.GetPosition()
                                                                        , Handle1.GetPosition()
                                                                        , NewSegmentVertex1 );
}

FVector
FArianeSegmentCubic::GetVectorLeavingFromVertex( FArianeVertex* Vertex, bool bNormalize )
{
    if( Vertex == Vertices[0].GetVertex() )
    {
        return GetTangentVectorAt( 0.0f, bNormalize );
    }

    if( Vertex == Vertices[1].GetVertex() )
    {
        return -GetTangentVectorAt( 1.0f, bNormalize );
    }

    return FVector::Zero();
}

FVector
FArianeSegmentCubic::GetTangentVectorAt( double T, bool bNormalize )
{
    FVector Tangent = ::ULIS::CubicBezierTangentAtParameter( Vertices[0].GetVertex()->GetPosition()
                                                           , Handle0.GetPosition()
                                                           , Handle1.GetPosition()
                                                           , Vertices[1].GetVertex()->GetPosition()
                                                           , T );

    if( bNormalize )
    {
        Tangent.Normalize();
    }

    return Tangent;
}
