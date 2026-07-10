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
#define MAXSUBPOINTS ( MAXSUBLINE + 1 )

FArianeSegmentCubic::FSubPoint::~FSubPoint()
{
}

FArianeSegmentCubic::FSubPoint::FSubPoint()
    : Point ( nullptr )
    , Index ( INDEX_NONE  )
{
}

FArianeSegmentCubic::FSubPoint::FSubPoint( FArianePoint* InPoint
                                         , double InT
                                         , double InRadius
                                         , int32 InIndex )
    : Point ( InPoint )
    , Position ( FVector::Zero() )
    , T ( InT )
    , Radius ( InRadius )
    , Index ( InIndex )
{
}

FArianeSegmentCubic::FSubPoint::FSubPoint( const FVector& InPosition
                                         , double InT
                                         , double InRadius
                                         , int32 InIndex )
    : Point ( nullptr )
    , Position ( InPosition )
    , T ( InT )
    , Radius ( InRadius )
    , Index ( InIndex )
{
}

FArianeSegmentCubic::FSubLine::~FSubLine()
{
}

FArianeSegmentCubic::FSubLine::FSubLine( FArianePoint* Point0
                                       , double T0
                                       , FArianePoint* Point1
                                       , double T1 )
    : Point { Point0, Point1 }
    , T { T0, T1 }
{
    Length = ( Point1->GetPosition() - Point0->GetPosition() ).Length();
}

FArianeSegmentCubic::~FArianeSegmentCubic()
{
}

FArianeSegmentCubic::FArianeSegmentCubic()
    : FArianeSegment()
    , Handle0( this, {0}, FVector::Zero() )
    , Handle1( this, {1}, FVector::Zero() )
{
}

FArianeSegmentCubic::FArianeSegmentCubic( FArianeObject* Owner
                                        , FArianeVertex* Vertex0
                                        , double Handle0X
                                        , double Handle0Y
                                        , double Handle0Z
                                        , double Handle1X
                                        , double Handle1Y
                                        , double Handle1Z
                                        , FArianeVertex* Vertex1
                                        , EArianeAllocationModel InAllocationModel )
    : FArianeSegment( Owner, Vertex0, Vertex1, InAllocationModel )
    , Handle0( this, {0}, FVector( Handle0X, Handle0Y, Handle0Z ) )
    , Handle1( this, {1}, FVector( Handle1X, Handle1Y, Handle1Z ) )
{
}

bool
FArianeSegmentCubic::HasBaseClass( uint32 BaseClassID )
{
    if( StaticClass() == BaseClassID )
    {
        return true;
    }

    return Super::HasBaseClass( BaseClassID );
}

FArianeHandleSegment*
FArianeSegmentCubic::GetHandle( FArianeVertex* Vertex )
{
    if( GetVertex(0) == Vertex ) return &Handle0;
    if( GetVertex(1) == Vertex ) return &Handle1;

    return nullptr;
}

FArianeHandleSegment*
FArianeSegmentCubic::GetHandle( uint32 Index )
{
    if( Index == 0 ) return &Handle0;
    if( Index == 1 ) return &Handle1;

    return nullptr;
}

void
FArianeSegmentCubic::Update()
{
    if( bAutoFractioned )
    {
        BuildVariable( MINRECURSE, MAXRECURSE );
    }

    UpdateBounds();

    bInvalidated = false;
}

// static
// De Casteljau algorithm. Stopping condition : dot product
void
FArianeSegmentCubic::BuildVariableAdaptive( TArray<FSubPoint>& SubPoints
                                          , int32 FromSubPointIndex
                                          , int32 ToSubPointIndex
                                          , FVector Bezier[4]
                                          , uint32 RecurseDepth
                                          , uint32 MinRecurse
                                          , uint32 MaxRecurse
                                          , uint32& InOutPointCount )
{
    FSubPoint& FromSubPoint = SubPoints[FromSubPointIndex];
    FSubPoint& ToSubPoint = SubPoints[ToSubPointIndex];
    FVector MidPoint = ::ULIS::CubicBezierPointAtParameter<FVector>( Bezier[0]
                                                                   , Bezier[1]
                                                                   , Bezier[2]
                                                                   , Bezier[3]
                                                                   , 0.5f );
    FVector ChildBezier[2][4];
    FVector StraightVectors[2] = { MidPoint - Bezier[0], MidPoint - Bezier[3] };
    double DotLimit = 0.9996f; // cos 1.62 deg
    double SegmentLength = 0.0;

    if( ( StraightVectors[0].IsNearlyZero() == false ) && ( StraightVectors[1].IsNearlyZero() == false ) )
    {
        StraightVectors[0].Normalize();
        StraightVectors[1].Normalize();

        if( ( RecurseDepth < MinRecurse ) // <--- minimum recurse level
         || ( ( StraightVectors[0].Dot(  StraightVectors[1] ) < DotLimit ) && ( RecurseDepth < MaxRecurse ) ) )
        {
            double RadiusAt = ( FromSubPoint.Radius + ToSubPoint.Radius ) * 0.5f;
            double SplitsAt = ( FromSubPoint.T      + ToSubPoint.T      ) * 0.5f;
            int32 SplitSubPointIndex = FromSubPointIndex + ( ( ToSubPointIndex - FromSubPointIndex ) >> 1 );

            memcpy( ChildBezier[0], Bezier, sizeof( ChildBezier[0] ) );
            memcpy( ChildBezier[1], Bezier, sizeof( ChildBezier[1] ) );

            // First sub-bezier from the divided parent bezier
            // Note: we always split at 0.5f. The splitsAt variable just helps setting the fromT and toT variables of the polygon cache.
            ::ULIS::CubicBezierSplitAtParameter<FVector>( &ChildBezier[0][0]
                                                        , &ChildBezier[0][1]
                                                        , &ChildBezier[0][2]
                                                        , &ChildBezier[0][3]
                                                        , 0.5f );

            SubPoints[SplitSubPointIndex] = FSubPoint( ChildBezier[0][3], SplitsAt, RadiusAt, SplitSubPointIndex );

            InOutPointCount++;

            BuildVariableAdaptive( SubPoints
                                 , FromSubPointIndex
                                 , SplitSubPointIndex
                                 , ChildBezier[0]
                                 , RecurseDepth + 1
                                 , MinRecurse
                                 , MaxRecurse
                                 , InOutPointCount );

            // Second sub-bezier from the divided parent bezier
            // Note: we always split at 0.5f. The splitsAt variable just helps setting the fromT and toT variables of the polygon cache.
            ::ULIS::CubicBezierInverseSplitAtParameter<FVector>( &ChildBezier[1][0]
                                                               , &ChildBezier[1][1]
                                                               , &ChildBezier[1][2]
                                                               , &ChildBezier[1][3]
                                                               , 0.5f );
            BuildVariableAdaptive( SubPoints
                                 , SplitSubPointIndex
                                 , ToSubPointIndex
                                 , ChildBezier[1]
                                 , RecurseDepth + 1
                                 , MinRecurse
                                 , MaxRecurse
                                 , InOutPointCount );
        }
    }
}

void
FArianeSegmentCubic::BuildVariable( uint32 MinRecurse
                                  , uint32 MaxRecurse )
{
    FArianeVertex* Vertex0 = Vertices[0].GetVertex();
    FArianeVertex* Vertex1 = Vertices[1].GetVertex();
    TArray<FSubPoint> SubPoints;
    //static std::mutex mutex;
    FVector Bezier[4] = { Vertex0->GetPosition()
                        , Handle0.GetPosition()
                        , Handle1.GetPosition()
                        , Vertex1->GetPosition() };
    FVector Min = FVector (  DBL_MAX,  DBL_MAX,  DBL_MAX );
    FVector Max = FVector ( -DBL_MAX, -DBL_MAX, -DBL_MAX );

     // Note: segment end points don't belong to the buffer
    SubPoints.SetNum( MAXSUBLINE + 1 );

    Length = 0.0f; // note: BuildVariableAdaptive will update the length

    // Reset
    Fractions.Empty();
    FractionSteps.Empty();
    FractionPoints.Empty();

    //if( mLength )
    {
        double SegmentStartRadius = Vertex0->GetRadius();
        double SegmentEndRadius = Vertex1->GetRadius();
        uint32 Point0Index = 0;
        uint32 Point1Index = MAXSUBPOINTS - 1;
        //double startU = 0.0f;
        uint32 PointCount = 2;

        Bounds = FBoxSphereBounds(ForceInit);

        //BuildOffsetCurves();
        // offset curves must be built before the polygon cache is built, as the builidng process
        // uses the offset curves to build the polygons.

        SubPoints[Point0Index] = FSubPoint( Vertex0, 0.0f, SegmentStartRadius, Point0Index );
        SubPoints[Point1Index] = FSubPoint( Vertex1, 1.0f, SegmentEndRadius  , Point1Index );

        BuildVariableAdaptive ( SubPoints
                              , Point0Index
                              , Point1Index
                              , Bezier
                              , 0
                              , MinRecurse
                              , MaxRecurse
                              , PointCount );

        FractionSteps.Reserve ( PointCount );
        FractionPoints.Reserve ( PointCount - 2 ); // 2 vertices are the endpoint ones
        Fractions.Reserve( PointCount - 1 );

        // create fractions points, ordered
        for( FSubPoint& SubPoint : SubPoints )
        {
            if( SubPoint.Index != INDEX_NONE )
            {
                FArianePoint* Point = SubPoint.Point ? SubPoint.Point
                                                     : &FractionPoints.Emplace_GetRef( SubPoint.Position );
                const FVector& Position = Point->GetPosition();

                FractionSteps.Emplace( Point, SubPoint.T, SubPoint.Radius );

                // ------ bounding -------//
                //if( Position.X < Min.X ) Min.X = Position.X;
                //if( Position.Y < Min.Y ) Min.Y = Position.Y;
                //if( Position.Z < Min.Z ) Min.Z = Position.Z;
                //if( Position.X > Max.X ) Max.X = Position.X;
                //if( Position.Y > Max.Y ) Max.Y = Position.Y;
                //if( Position.Z > Max.Z ) Max.Z = Position.Z;
                // ---------------------- //
            }
        }

        //Bounds.Origin = ( Min + Max ) * 0.5f;
        //Bounds.BoxExtent = ( Max - Bounds.Origin );
        //Bounds.SphereRadius = Bounds.BoxExtent.Length();

        for( int32 i = 0, n = 1; n < FractionSteps.Num(); i++, n++ )
        {
            FFraction& Fraction = Fractions.Emplace_GetRef( &FractionSteps[i]
                                                          , &FractionSteps[n] );

            Length += Fraction.Length;
        }
    }
}

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
    FArianeVertex* Vertex0 = Vertices[0].GetVertex();
    FArianeVertex* Vertex1 = Vertices[1].GetVertex();
    FVector DeltaNormal = Vertex1->GetNormal() - Vertex0->GetNormal();
    double DeltaRadius = Vertex1->GetRadius() - Vertex0->GetRadius();
    FVector Bezier[4] = { Vertex0->GetPosition()
                        , Handle0.GetPosition()
                        , Handle1.GetPosition()
                        , Vertex1->GetPosition() };

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

    NewSegmentVertex0->SetPosition( Bezier[0] );
    NewSegmentVertex0->SetNormal( Vertex0->GetNormal() + DeltaNormal * T0 );
    NewSegmentVertex0->SetRadius( Vertex0->GetRadius() + DeltaRadius * T0 );

    NewSegmentVertex1->SetPosition( Bezier[3] );
    NewSegmentVertex1->SetNormal( Vertex1->GetNormal() + DeltaNormal * T0 );
    NewSegmentVertex1->SetRadius( Vertex1->GetRadius() + DeltaRadius * T0 );


    return static_cast<FArianePath*>(NewSegmentOwner)->AllocCubicSegment( NewSegmentVertex0
                                                                        , Bezier[1]
                                                                        , Bezier[2]
                                                                        , NewSegmentVertex1
                                                                        , AllocationModel );
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

FVector
FArianeSegmentCubic::GetHandleVector( FArianeVertex* Vertex, bool bNormalize )
{
    return GetHandleVector( ( Vertices[0].GetVertex() == Vertex ) ? 0 : 1, bNormalize );
}

FVector
FArianeSegmentCubic::GetHandleVector( uint32 HandleID, bool bNormalize )
{
    FVector Vec = GetHandle(HandleID)->GetPosition() - GetVertex(HandleID)->GetPosition();

    if( bNormalize )
    {
        if( Vec.SquaredLength() )
        {
            Vec.Normalize();
        }
    }

    return Vec;
}
